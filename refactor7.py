import os
import re

def process_file(filepath):
    with open(filepath, 'r') as f:
        content = f.read()
    original = content

    # 1. Expand 16-byte constants to 32-byte in row_gcc.cc / row_win.cc / row_common.cc
    def expand_vec(match):
        type_name = match.group(1)
        inner = match.group(2)
        items = [x for x in inner.split(',') if x.strip()]
        if (('vec8' in type_name and len(items) == 16) or 
            ('vec16' in type_name and len(items) == 8) or 
            ('vec32' in type_name and len(items) == 4)):
            return match.group(0).replace(inner, inner + ", " + inner)
        return match.group(0)

    content = re.sub(r'static\s+const\s+((?:u|l|ul)?vec(?:8|16|32))\s+\w+\s*=\s*\{([^}]+)\}', expand_vec, content)

    # 2. Modify row.h sizes
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

    # 3. Clean up kCpuHasSSE usages
    content = re.sub(r'^[ \t]*static\s+const\s+int\s+kCpuHasSSE[0-9]*\s*=\s*0x[0-9a-fA-F]+;\r?\n', '', content, flags=re.MULTILINE)
    content = re.sub(r'\|\s*\(\(.*?\)\s*\?\s*kCpuHasSSE[0-9]*\s*:\s*0\)\s*', '', content)
    content = re.sub(r'^[ \t]*printf\("Has SSE.*\\n",.*?\);\r?\n', '', content, flags=re.MULTILINE)
    content = re.sub(r'^[ \t]*printf\("Has SSSE.*\\n",.*?\);\r?\n', '', content, flags=re.MULTILINE)
    content = re.sub(r'^[ \t]*int\s+has_ss?e[0-9]*\s*=\s*TestCpuFlag\(kCpuHasSSE[0-9]*\);\r?\n', '', content, flags=re.MULTILINE)

    pattern = re.compile(r'^[ \t]*if\s*\(\s*TestCpuFlag\s*\(\s*kCpuHasSSE[0-9]*\s*\)\s*\)\s*\{', re.MULTILINE)
    while True:
        m = pattern.search(content)
        if not m: break
        start = m.start()
        brace_count = 1
        i = m.end()
        while i < len(content) and brace_count > 0:
            if content[i] == '{': brace_count += 1
            elif content[i] == '}': brace_count -= 1
            i += 1
        content = content[:start] + content[i:]

    # Delete any function ending with _SSE2, _SSSE3, _SSE41, _SSE42
    func_pattern = re.compile(r'^[ \t]*(?:LIBYUV_API\s+|static\s+|__inline\s+|__declspec[^{]*\s+|inline\s+|void\s+|int\s+|uint32_t\s+|float\s+|double\s+)+[A-Za-z0-9_]+_(?:SSE2|SSSE3|SSE41|SSE42)(?:_[A-Za-z0-9_]+)?\s*\(', re.MULTILINE)
    while True:
        m = func_pattern.search(content)
        if not m: break
        start = m.start()
        i = m.end()
        paren_count = 1
        while i < len(content) and paren_count > 0:
            if content[i] == '(': paren_count += 1
            elif content[i] == ')': paren_count -= 1
            i += 1
        
        while i < len(content) and content[i].isspace():
            i += 1
        
        if i < len(content) and content[i] == ';':
            content = content[:start] + content[i+1:]
        elif i < len(content) and content[i] == '{':
            brace_count = 1
            i += 1
            while i < len(content) and brace_count > 0:
                if content[i] == '{': brace_count += 1
                elif content[i] == '}': brace_count -= 1
                i += 1
            content = content[:start] + content[i:]
        else:
            nl = content.find('\n', i)
            content = content[:start] + content[nl+1:]

    # Change `#if defined(HAS_..._SSE)` to `#if 1` everywhere so constants are declared
    content = re.sub(r'#if\s+defined\(HAS_[A-Za-z0-9_]+_SSE[0-9]*\)', '#if 1', content)

    # Remove `#define HAS_..._SSE` from row.h entirely
    if filepath.endswith('row.h'):
        content = re.sub(r'^[ \t]*#define\s+HAS_[A-Za-z0-9_]*SSE[0-9]*[ \t]*\r?\n', '', content, flags=re.MULTILINE)

    # 5. Broadcast replacements
    content = re.sub(r'vbroadcastf128(\s+)([^,]+),\s*%%ymm', r'vmovdqu\1\2,%%ymm', content)
    content = re.sub(r'vbroadcastf128(\s+)([^,]+),\s*ymm', r'vmovdqu\1\2,ymm', content)
    content = re.sub(r'vbroadcastss(\s+)([^,]+),\s*%%ymm', r'vmovdqu\1\2,%%ymm', content)
    content = re.sub(r'vbroadcastss(\s+)([^,]+),\s*ymm', r'vmovdqu\1\2,ymm', content)
    content = re.sub(r'vbroadcasti128(\s+)([^,]+),\s*%%ymm', r'vmovdqu\1\2,%%ymm', content)
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
