#!/usr/bin/php
<?php
// CGI header
header("Status: 400 Bad Request");
header("Content-Type: text/html");

// HTML content with PHP, CSS, and a floral-inspired animated design
echo <<<HTML
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Beautiful Floral Design</title>
    <style>
        body {
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background: linear-gradient(to bottom right, #e0f7fa, #ffccbc);
            font-family: Arial, sans-serif;
            overflow: hidden;
        }
        .flower-container {
            position: relative;
            width: 150px;
            height: 150px;
            display: flex;
            justify-content: center;
            align-items: center;
            animation: spin 12s linear infinite;
        }
        .petal {
            position: absolute;
            width: 50px;
            height: 50px;
            background: radial-gradient(circle, #ff8a65, #ff3d00);
            border-radius: 50%;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.2);
            opacity: 0.8;
            animation: pulse 2s infinite;
        }
        .petal:nth-child(1) { transform: rotate(0deg) translateX(80px); }
        .petal:nth-child(2) { transform: rotate(45deg) translateX(80px); }
        .petal:nth-child(3) { transform: rotate(90deg) translateX(80px); }
        .petal:nth-child(4) { transform: rotate(135deg) translateX(80px); }
        .petal:nth-child(5) { transform: rotate(180deg) translateX(80px); }
        .petal:nth-child(6) { transform: rotate(225deg) translateX(80px); }
        .petal:nth-child(7) { transform: rotate(270deg) translateX(80px); }
        .petal:nth-child(8) { transform: rotate(315deg) translateX(80px); }

        /* Animation for spinning and pulsing effect */
        @keyframes spin {
            0% { transform: rotate(0deg); }
            100% { transform: rotate(360deg); }
        }
        @keyframes pulse {
            0%, 100% { transform: scale(1); opacity: 0.8; }
            50% { transform: scale(1.2); opacity: 1; }
        }
    </style>
</head>
<body>
    <div class="flower-container">
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
HTML;
?>
