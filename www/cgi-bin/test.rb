#!/usr/bin/env ruby

require 'cgi'

cgi = CGI.new

puts cgi.header
puts "<html>"
puts "<head><title>Test CGI Script</title></head>"
puts "<body>"
puts "<h1>CGI Script Test</h1>"
puts "<p>This is a test of the web server's CGI functionality.</p>"
puts "<p>Current Time: #{Time.now}</p>"
puts "<p>Query String: #{cgi.query_string}</p>"
puts "<p>Path Info: #{ENV['PATH_INFO']}</p>"
puts "</body>"
puts "</html>"