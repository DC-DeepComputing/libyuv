import re
import os

with open('source/row_common.cc', 'r') as f:
    content = f.read()

# Replace #if defined(HAS_...) \n ..._AVX2(...) \n #else \n ..._SSE2(...) \n #endif
content = re.sub(r'#if\s+defined\(HAS_[A-Z0-9_]+ROW_AVX2\)\s*\n\s*([A-Za-z0-9_]+Row_AVX2\([^)]+\);)\s*\n\s*#else\s*\n\s*[A-Za-z0-9_]+Row_SSE2\([^)]+\);\s*\n\s*#endif', r'\1', content)
content = re.sub(r'#if\s+defined\(HAS_[A-Z0-9_]+ROW_SSSE3\)\s*\n\s*([A-Za-z0-9_]+Row_AVX2\([^)]+\);)\s*\n\s*#else\s*\n\s*[A-Za-z0-9_]+Row_SSSE3\([^)]+\);\s*\n\s*#endif', r'\1', content)

# Replace RGB24ToARGBRow_SSSE3 with RGB24ToARGBRow_C or RGB24ToARGBRow_AVX2 ?
# Let's find RGB24ToARGBRow_SSSE3
content = re.sub(r'RGB24ToARGBRow_SSSE3', 'RGB24ToARGBRow_C', content)
content = re.sub(r'RGB24ToARGBRow_SSE2', 'RGB24ToARGBRow_C', content)

with open('source/row_common.cc', 'w') as f:
    f.write(content)
