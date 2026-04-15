import re
import os

def fix_file(fpath):
    with open(fpath, 'r') as f:
        c = f.read()
    orig = c
    c = re.sub(r'#if\s+defined\(HAS_[A-Z0-9_]+_SSSE3\)', '#if 1', c)
    c = re.sub(r'#ifdef\s+HAS_[A-Z0-9_]+_SSSE3', '#if 1', c)
    c = re.sub(r'#if\s+defined\(HAS_[A-Z0-9_]+_SSE2\)', '#if 1', c)
    c = re.sub(r'#ifdef\s+HAS_[A-Z0-9_]+_SSE2', '#if 1', c)
    c = re.sub(r'#if\s+defined\(HAS_[A-Z0-9_]+_SSE41\)', '#if 1', c)
    c = re.sub(r'#ifdef\s+HAS_[A-Z0-9_]+_SSE41', '#if 1', c)
    if orig != c:
        with open(fpath, 'w') as f:
            f.write(c)

for root, _, files in os.walk('source'):
    for f in files:
        if f.endswith('.cc'):
            fix_file(os.path.join(root, f))
