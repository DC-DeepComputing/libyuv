import os
import re

def process_file(filepath):
    with open(filepath, 'r') as f:
        content = f.read()
    
    original = content

    # 1. Remove kCpuHasSSE checks (entire lines in cpuid.c or cpu_id.cc that use them outside of if blocks)
    # Wait, in source/convert.cc, it's `if (TestCpuFlag(kCpuHasSSE2)) { ... }`
    # We can write a simple brace matcher to remove these if blocks.

    def remove_testcpuflag_blocks(text):
        pattern = re.compile(r'^[ \t]*if\s*\(\s*TestCpuFlag\s*\(\s*kCpuHasSSE[0-9]*\s*\)\s*\)\s*\{', re.MULTILINE)
        while True:
            m = pattern.search(text)
            if not m:
                break
            start = m.start()
            # find matching brace
            brace_count = 1
            i = m.end()
            while i < len(text) and brace_count > 0:
                if text[i] == '{':
                    brace_count += 1
                elif text[i] == '}':
                    brace_count -= 1
                i += 1
            text = text[:start] + text[i:]
        return text

    content = remove_testcpuflag_blocks(content)

    # 2. Also remove any line containing kCpuHasSSE
    # But wait, in cpu_id.cc it builds cpu_info = ... | ((...)? kCpuHasSSE2 : 0)
    # We can just remove "kCpuHasSSE\d+" from those lines, or remove the lines.
    lines = content.split('\n')
    new_lines = []
    for line in lines:
        if 'kCpuHasSSE' in line:
            # If it's a static const int definition, remove it. (Already done by previous script)
            # If it's part of an OR expression like `| kCpuHasSSE2`, we can remove that part.
            line = re.sub(r'\|\s*\(\(.*?\)\s*\?\s*kCpuHasSSE[0-9]*\s*:\s*0\)\s*', '', line)
            line = re.sub(r'kCpuHasSSE[0-9]*', '0', line)
            # if the line is just `int has_sse2 = TestCpuFlag(0);`, remove it.
            if 'has_sse' in line and 'TestCpuFlag' in line:
                continue
            if 'printf' in line and 'SSE' in line:
                continue
        new_lines.append(line)
    
    content = '\n'.join(new_lines)

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
