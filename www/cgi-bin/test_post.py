#!/usr/bin/env python3

import sys
import html




# Read input from stdin
input_data = sys.stdin.read().strip()

# HTML output
html_template = f"""Content-Type: text/html

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Centered Text</title>
    <style>
        body {{
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background-color: #f4f4f4;
            font-family: Arial, sans-serif;
        }}
        .center-frame {{
            border: 2px solid #333;
            border-radius: 10px;
            padding: 20px;
            background-color: white;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.2);
            max-width: 500px;
            word-wrap: break-word;
            text-align: center;
        }}
    </style>
</head>
<body>
    <div class="center-frame">
        <p>{html.escape(input_data)}</p>
    </div>
</body>
</html>
"""

# Print the HTML
print(html_template)
