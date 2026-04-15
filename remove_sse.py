import os
import re
import sys

def process_file(filepath):
    with open(filepath, 'r') as f:
        content = f.read()

    # We need to remove SSE blocks.
    # This is quite hard with simple regex. Let's do string replacement.
    
