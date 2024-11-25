#!/usr/bin/env python3
# import cgi

# Ensure the correct content-type header is printed for HTML
print("Status: 200 OK")
print("Content-Type: text/html")
print("")
# HTML output with a dynamic floral design
# name =  input("")
# print(f"<h1>Hello, {name}!</h1>")
html_content = """
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Dynamic Floral Design</title>
    <style>
        body {
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background-color: #f5f5f5;
            font-family: Arial, sans-serif;
        }
        .flower {
            position: relative;
            width: 100px;
            height: 100px;
            background: #FFD700;
            border-radius: 50%;
            animation: rotate 8s linear infinite;
        }
        .petal {
            position: absolute;
            width: 60px;
            height: 20px;
            background: #FF6347;
            border-radius: 20px;
        }
        .petal:nth-child(1) { transform: rotate(0deg) translate(40px); }
        .petal:nth-child(2) { transform: rotate(45deg) translate(40px); }
        .petal:nth-child(3) { transform: rotate(90deg) translate(40px); }
        .petal:nth-child(4) { transform: rotate(135deg) translate(40px); }
        .petal:nth-child(5) { transform: rotate(180deg) translate(40px); }
        .petal:nth-child(6) { transform: rotate(225deg) translate(40px); }
        .petal:nth-child(7) { transform: rotate(270deg) translate(40px); }
        .petal:nth-child(8) { transform: rotate(315deg) translate(40px); }

        @keyframes rotate {
            0% { transform: rotate(0deg); }
            100% { transform: rotate(360deg); }
        }
    </style>
</head>
<body>
    <div class="flower">
        <div class="petal"></div>
        <div class="petal"></div>
        <div class="petal"></div>
        <div class="petal"></div>
        <div class="petal"></div>
        <div class="petal"></div>
        <div class="petal"></div>
        <div class="petal"></div>
    </div>
</body>
</html>
"""

# Print the HTML content
print(html_content)
