#!/usr/bin/python

# Import modules for CGI handling 
import cgi, cgitb 

form = cgi.FieldStorage() 

# Get data from fields
first_name = form.getvalue('name')
last_name  = form.getvalue('title')

print ("Content-type:text/html\r\n\r\n")
print ("<html>")
print ("<head>")
print ("<title>Hello - Second CGI Program</title>")
print ("</head>")
print ("<body>")
print ("<h2>Hello %s %s</h2>"%(first_name, last_name))
print ("</body>")
print ("</html>")



