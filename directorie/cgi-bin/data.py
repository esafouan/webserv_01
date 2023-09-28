
#!/usr/bin/python

import sys
import os
import cgi
# Set appropriate HTTP headers for image response
print("Content-type: video/mp4")  # Set the appropriate content type (e.g., image/jpeg)

# Optionally, you can specify the Content-Length header
# Uncomment the line below and replace <size_in_bytes> with the actual size.
# print("Content-Length:", <size_in_bytes>)

# Print an empty line to separate headers from the image data
print()

# Read binary image data from stdin and write it to stdout
sys.stdout.buffer.write(sys.stdin.buffer.read())