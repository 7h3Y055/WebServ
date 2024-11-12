#!/usr/bin/env python3

# CGI libraries
import cgi
import cgitb

cgitb.enable()  # Enable debugging

# Output the content type and start of HTML
# print("Content-Type: text/html\n")
print("""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Animated Butterfly</title>
    <style>
        /* Basic style for the page */
        body {
            background-color: #d9f2d9;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            overflow: hidden;
        }
        
        /* Butterfly styling */
        .butterfly {
            position: relative;
            width: 100px;
            height: 100px;
            animation: moveButterfly 6s ease-in-out infinite;
        }
        
        /* Wing styling */
        .wing {
            position: absolute;
            width: 50px;
            height: 100px;
            background-color: #FF69B4;
            border-radius: 50%;
            opacity: 0.8;
            transform-origin: bottom center;
            animation: flapWings 0.5s ease-in-out infinite alternate;
        }
        
        /* Right wing position */
        .wing.right {
            right: 0;
            transform: rotate(10deg);
        }

        /* Left wing position */
        .wing.left {
            left: 0;
            transform: rotate(-10deg);
        }

        /* Wing flapping animation */
        @keyframes flapWings {
            from { transform: rotate(-15deg); }
            to { transform: rotate(15deg); }
        }
        
        /* Butterfly movement path */
        @keyframes moveButterfly {
            0% { transform: translate(0, 0); }
            25% { transform: translate(100px, -150px); }
            50% { transform: translate(200px, 0); }
            75% { transform: translate(100px, 150px); }
            100% { transform: translate(0, 0); }
        }
    </style>
</head>
<body>
    <div class="butterfly">
        <div class="wing left"></div>
        <div class="wing right"></div>
    </div>
</body>
</html>
""")
