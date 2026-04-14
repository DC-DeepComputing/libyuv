import os
import re

directories = ['include/libyuv', 'source']
files_to_process = []
for d in directories:
    for root, _, files in os.walk(d):
        for f in files:
            if f.endswith('.h') or f.endswith('.cc'):
                files_to_process.append(os.path.join(root, f))

colors = ['ARGB', 'BGRA', 'RGBA', 'ABGR']

def replace_in_file(filepath):
    with open(filepath, 'r') as f:
        content = f.read()

    lines = content.split('\n')
    
    i = 0
    while i < len(lines):
        line = lines[i]
        # Match function definition or declaration containing ToARGB, ToBGRA, etc.
        # But exclude things like ToARGB1555 by looking ahead for specific suffixes
        match = re.search(r'^(\s*(?:LIBYUV_API\s+|static inline\s+)?(?:int|void|uint32_t)\s+\w*?To(ARGB|BGRA|RGBA|ABGR))(?:Row|Matrix|Filter|Scale|Clip|_|\()', line)
        if not match:
            i += 1
            continue
            
        color = match.group(2)
        lower_color = color.lower()
        
        c_idx = i - 1
        if c_idx >= 0 and 'LIBYUV_API' in lines[c_idx]:
            c_idx -= 1
            
        while c_idx >= 0 and lines[c_idx].strip().startswith('//'):
            for oc in colors:
                if oc != color:
                    # Replace "to oc" with "to color" in comment
                    # Also "to  oc" if there are multiple spaces
                    lines[c_idx] = re.sub(rf'\bto {oc}\b', f'to {color}', lines[c_idx], flags=re.IGNORECASE)
            c_idx -= 1

        # Find the end of the function declaration or body
        j = i
        open_braces = 0
        found_brace = False
        found_semi = False
        
        while j < len(lines):
            open_braces += lines[j].count('{')
            if '{' in lines[j]:
                found_brace = True
            open_braces -= lines[j].count('}')
            
            if ';' in lines[j] and not found_brace:
                found_semi = True
                break
                
            if found_brace and open_braces == 0:
                break
                
            j += 1

        if j >= len(lines):
            j = len(lines) - 1

        # Rename destination variables in this block
        for k in range(i, j + 1):
            for oc in colors:
                loc = oc.lower()
                if loc != lower_color:
                    lines[k] = re.sub(rf'\bdst_{loc}\b', f'dst_{lower_color}', lines[k])
                    lines[k] = re.sub(rf'\bdst_{loc}0\b', f'dst_{lower_color}0', lines[k])
                    lines[k] = re.sub(rf'\bdst_stride_{loc}\b', f'dst_stride_{lower_color}', lines[k])
                    lines[k] = re.sub(rf'\b(uint8_t\*\s*){loc}\b', rf'\1{lower_color}', lines[k])
        
        i = j + 1

    new_content = '\n'.join(lines)
    if new_content != content:
        with open(filepath, 'w') as f:
            f.write(new_content)
        print(f"Updated {filepath}")

for filepath in files_to_process:
    replace_in_file(filepath)
