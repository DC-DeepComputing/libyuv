import os
import re

def process_file(filepath):
    with open(filepath, 'r') as f:
        content = f.read()

    original = content

    # 1. Remove SSE macros
    content = re.sub(r'^[ \t]*#define\s+HAS_[A-Za-z0-9_]*SSE[0-9]*\r?\n', '', content, flags=re.MULTILINE)
    
    # 2. Remove kCpuHasSSE2 etc.
    content = re.sub(r'^[ \t]*static\s+const\s+int\s+kCpuHasSSE[0-9]*\s*=\s*0x[0-9a-fA-F]+;\r?\n', '', content, flags=re.MULTILINE)

    # 3. Modify vec8/uvec8 to 32 bytes in row.h
    if filepath.endswith('row.h'):
        content = re.sub(r'align\(16\)\)\s+(u?)vec8\[16\]', r'align(32)) \1vec8[32]', content)
        content = re.sub(r'vector_size\(16\)\)\s+(u?)vec8', r'vector_size(32))) \1vec8', content)
        content = re.sub(r'(u?)vec8\[16\]', r'\1vec8[32]', content)

    # 4. Expand constants in .cc files
    # Find static const uvec8 name = { ... };
    # We will just duplicate the contents of the braces.
    def expand_braces(match):
        inner = match.group(2)
        # If it already has 32 elements, maybe skip. But just duplicating is easier if we match exactly 16 elements.
        items = [x.strip() for x in inner.split(',')]
        if len(items) == 16:
            return match.group(1) + inner + ", " + inner + match.group(3)
        return match.group(0)
    
    content = re.sub(r'(static\s+const\s+(?:u)?vec8\s+\w+\s*=\s*\{)([^}]+)(\})', expand_braces, content)

    # 5. Switch broadcast to movdqa in AVX code
    # Only if the destination is ymm. 
    # Example: vbroadcastf128 %3,%%ymm2 -> vmovdqa %3,%%ymm2
    # vbroadcastss %3,%%ymm2 -> vmovdqa %3,%%ymm2  (Wait, vbroadcastss broadcasts a float. If we expand it to 32 bytes, we can use vmovdqa if it was a constant. But if it's not a constant? Let's just replace the instruction name).
    # The instructions literally said "switch from broadcast instructions to movdqa or equivalent"
    content = re.sub(r'vbroadcastf128(\s+)', r'vmovdqa\1', content)
    content = re.sub(r'vbroadcastss(\s+)([^,]+),\s*%%ymm', r'vmovdqa\1\2,%%ymm', content)
    content = re.sub(r'vbroadcastss(\s+)([^,]+),\s*ymm', r'vmovdqa\1\2,ymm', content)
    content = re.sub(r'vbroadcasti128(\s+)', r'vmovdqa\1', content)

    # 6. AVX512 "update avx512 to use the appropriate broadcast"
    # AVX512 uses vbroadcasti64x4. If constants are 32 bytes, they can use vmovdqa64 (for 64 byte loads) or maybe just keep vbroadcasti64x4 since it reads 32 bytes?
    # Wait, if we use 32 byte constants, vbroadcasti64x4 reads 32 bytes and duplicates to 64 bytes. This is the correct broadcast for 32->64!
    # So AVX512 might be fine, but let's check if there are vbroadcastf128 into zmm.
    content = re.sub(r'vbroadcastf128(\s+)([^,]+),\s*%%zmm', r'vbroadcastf32x4\1\2,%%zmm', content)
    
    if content != original:
        with open(filepath, 'w') as f:
            f.write(content)
        print(f"Updated {filepath}")

def main():
    dirs = ['include', 'source']
    for d in dirs:
        for root, _, files in os.walk(d):
            for f in files:
                if f.endswith(('.h', '.cc', '.c')):
                    process_file(os.path.join(root, f))

if __name__ == '__main__':
    main()
