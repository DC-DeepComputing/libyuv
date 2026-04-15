import os
import re

def process_file(filepath):
    with open(filepath, 'r') as f:
        content = f.read()
    
    original = content

    # 1. Remove SSE macros from #define
    content = re.sub(r'^[ \t]*#define\s+HAS_[A-Za-z0-9_]*SSE[0-9]*[ \t]*\r?\n', '', content, flags=re.MULTILINE)
    
    # 2. Remove kCpuHasSSE checks from #if blocks
    pattern = re.compile(r'^[ \t]*if\s*\(\s*TestCpuFlag\s*\(\s*kCpuHasSSE[0-9]*\s*\)\s*\)\s*\{', re.MULTILINE)
    while True:
        m = pattern.search(content)
        if not m:
            break
        start = m.start()
        brace_count = 1
        i = m.end()
        while i < len(content) and brace_count > 0:
            if content[i] == '{': brace_count += 1
            elif content[i] == '}': brace_count -= 1
            i += 1
        content = content[:start] + content[i:]

    # 3. Clean up kCpuHasSSE constants and usages
    content = re.sub(r'^[ \t]*static\s+const\s+int\s+kCpuHasSSE[0-9]*\s*=\s*0x[0-9a-fA-F]+;\r?\n', '', content, flags=re.MULTILINE)
    content = re.sub(r'\|\s*\(\(.*?\)\s*\?\s*kCpuHasSSE[0-9]*\s*:\s*0\)\s*', '', content)
    content = re.sub(r'^[ \t]*printf\("Has SSE.*\\n",.*?\);\r?\n', '', content, flags=re.MULTILINE)
    content = re.sub(r'^[ \t]*printf\("Has SSSE.*\\n",.*?\);\r?\n', '', content, flags=re.MULTILINE)
    content = re.sub(r'^[ \t]*int\s+has_ss?e[0-9]*\s*=\s*TestCpuFlag\(kCpuHasSSE[0-9]*\);\r?\n', '', content, flags=re.MULTILINE)

    # 4. Modify vec8, vec16, vec32 in row.h to be 32 bytes
    if filepath.endswith('row.h'):
        content = re.sub(r'align\(16\)\)\s+(u?)vec8\[16\]', r'align(32)) \1vec8[32]', content)
        content = re.sub(r'vector_size\(16\)\)\s+(u?)vec8', r'vector_size(32))) \1vec8', content)
        content = re.sub(r'typedef (int8_t|uint8_t) (u?)vec8\[16\];', r'typedef \1 \2vec8[32];', content)

        content = re.sub(r'align\(16\)\)\s+(u?)vec16\[8\]', r'align(32)) \1vec16[16]', content)
        content = re.sub(r'vector_size\(16\)\)\s+(u?)vec16', r'vector_size(32))) \1vec16', content)
        content = re.sub(r'typedef (int16_t|uint16_t) (u?)vec16\[8\];', r'typedef \1 \2vec16[16];', content)

        content = re.sub(r'align\(16\)\)\s+(u?)vec32\[4\]', r'align(32)) \1vec32[8]', content)
        content = re.sub(r'vector_size\(16\)\)\s+(u?)vec32', r'vector_size(32))) \1vec32', content)
        content = re.sub(r'typedef (int32_t|uint32_t) (u?)vec32\[4\];', r'typedef \1 \2vec32[8];', content)

    # 5. Expand constants in .cc files
    def expand_braces(match):
        type_name = match.group(1) # e.g. uvec8
        inner = match.group(2)
        items = [x.strip() for x in inner.split(',') if x.strip()]
        if ('vec8' in type_name and len(items) == 16) or \
           ('vec16' in type_name and len(items) == 8) or \
           ('vec32' in type_name and len(items) == 4):
            return match.group(0).replace(inner, inner + ", " + inner)
        return match.group(0)

    content = re.sub(r'static\s+const\s+((?:u)?vec(?:8|16|32))\s+\w+\s*=\s*\{([^}]+)\}', expand_braces, content)

    # 6. Switch broadcast to movdqa in AVX code (we use vmovdqu to be safe)
    content = re.sub(r'vbroadcastf128(\s+)([^,]+),\s*%%ymm', r'vmovdqu\1\2,%%ymm', content)
    content = re.sub(r'vbroadcastf128(\s+)([^,]+),\s*ymm', r'vmovdqu\1\2,ymm', content)
    content = re.sub(r'vbroadcastss(\s+)([^,]+),\s*%%ymm', r'vmovdqu\1\2,%%ymm', content)
    content = re.sub(r'vbroadcastss(\s+)([^,]+),\s*ymm', r'vmovdqu\1\2,ymm', content)
    content = re.sub(r'vbroadcasti128(\s+)([^,]+),\s*%%ymm', r'vmovdqu\1\2,%%ymm', content)
    
    # AVX512 appropriate broadcast
    content = re.sub(r'vbroadcastf128(\s+)([^,]+),\s*%%zmm', r'vbroadcasti64x4\1\2,%%zmm', content)
    content = re.sub(r'vbroadcastf128(\s+)([^,]+),\s*zmm', r'vbroadcasti64x4\1\2,zmm', content)

    if content != original:
        with open(filepath, 'w') as f:
            f.write(content)
        print(f"Updated {filepath}")

def main():
    dirs = ['include', 'source', 'util', 'unit_test']
    for d in dirs:
        if not os.path.exists(d): continue
        for root, _, files in os.walk(d):
            for f in files:
                if f.endswith(('.h', '.cc', '.c')):
                    process_file(os.path.join(root, f))

if __name__ == '__main__':
    main()
