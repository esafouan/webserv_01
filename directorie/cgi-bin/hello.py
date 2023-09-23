# #!/usr/bin/python

# # Import modules for CGI handling 
# import cgi, cgitb 

# # Create instance of FieldStorage 
# form = cgi.FieldStorage() 

# # Get data from fields
# first_name = form.getvalue('name')
# last_name  = form.getvalue('title')

# print ("Content-type:text/html\r\n\r\n")
# print ("<html>")
# print ("<head>")
# print ("<title>Hello - Second CGI Program</title>")
# print ("</head>")
# print ("<body>")
# print ("<h2>Hello %s %s</h2>"%(first_name, last_name))
# print ("</body>")
# print ("</html>")


#!/usr/bin/python

import sys
import os
import cgi

# Set appropriate HTTP headers for image response
print("Content-type: image/jpeg")  # Set the appropriate content type (e.g., image/jpeg)

# Optionally, you can specify the Content-Length header
# Uncomment the line below and replace <size_in_bytes> with the actual size.
# print("Content-Length:", <size_in_bytes>)

# Print an empty line to separate headers from the image data
print()

# Read binary image data from stdin and write it to stdout
sys.stdout.buffer.write(sys.stdin.buffer.read())

