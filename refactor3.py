import os
import re

def process_file(filepath):
    with open(filepath, 'r') as f:
        content = f.read()
    
    original = content

    # Fix syntax errors caused by previous script
    # libyuv::0 -> 0
    content = re.sub(r'libyuv::0', '0', content)
    
    # In util/cpuid.c, remove the printf lines for SSE
    content = re.sub(r'^[ \t]*printf\("Has SSE.*\\n", has_sse[0-9]*\);\r?\n', '', content, flags=re.MULTILINE)
    content = re.sub(r'^[ \t]*printf\("Has SSSE.*\\n", has_ssse[0-9]*\);\r?\n', '', content, flags=re.MULTILINE)
    
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
