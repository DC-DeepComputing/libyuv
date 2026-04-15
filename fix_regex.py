import os
import re

def process_file(filepath):
    with open(filepath, 'r') as f:
        content = f.read()
    original = content

    # Clean up kCpuHasSSE usages and unused variables
    content = re.sub(r'^[ \t]*int\s+has_[a-z0-9_]+\s*=\s*TestCpuFlag\([^)]*\);\r?\n', '', content, flags=re.MULTILINE)

    # Remove ALL if (TestCpuFlag(0) ...) { ... } blocks
    pattern = re.compile(r'^[ \t]*if\s*\([^{]*TestCpuFlag\s*\(\s*0\s*\)[^{]*\)\s*\{', re.MULTILINE)
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

    if content != original:
        with open(filepath, 'w') as f:
            f.write(content)
        print(f"Fixed {filepath}")

dirs = ['include', 'source', 'util', 'unit_test']
for d in dirs:
    if not os.path.exists(d): continue
    for root, _, files in os.walk(d):
        for f in files:
            if f.endswith(('.h', '.cc', '.c')):
                process_file(os.path.join(root, f))
