import re
import os

# Fix source/row_common.cc
with open('source/row_common.cc', 'r') as f:
    c = f.read()
c = re.sub(r'#if\s+defined\(HAS_[A-Z0-9_]+ROW_AVX2\)\s*\n\s*([A-Za-z0-9_]+Row_AVX2\([^)]+\);)\s*\n\s*#else\s*\n\s*[A-Za-z0-9_]+Row_SSE2\([^)]+\);\s*\n\s*#endif', r'\1', c)
c = re.sub(r'#if\s+defined\(HAS_[A-Z0-9_]+ROW_SSSE3\)\s*\n\s*([A-Za-z0-9_]+Row_AVX2\([^)]+\);)\s*\n\s*#else\s*\n\s*[A-Za-z0-9_]+Row_SSSE3\([^)]+\);\s*\n\s*#endif', r'\1', c)
c = re.sub(r'RGB24ToARGBRow_SSSE3', 'RGB24ToARGBRow_C', c)
c = re.sub(r'RGB24ToARGBRow_SSE2', 'RGB24ToARGBRow_C', c)
with open('source/row_common.cc', 'w') as f:
    f.write(c)

# Fix source/row_any.cc
with open('source/row_any.cc', 'r') as f:
    c = f.read()
# Remove ANY* macros that use SSE functions
c = re.sub(r'^[ \t]*ANY[0-9A-Z]*\([^,]+_SSE[0-9]*,[^,]+_SSE[0-9]*.*?\)\r?\n', '', c, flags=re.MULTILINE)
c = re.sub(r'^[ \t]*ANY[0-9A-Z]*\([^,]+_SSSE3,[^,]+_SSSE3.*?\)\r?\n', '', c, flags=re.MULTILINE)
with open('source/row_any.cc', 'w') as f:
    f.write(c)
