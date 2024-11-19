#!/usr/bin/php
<?php
// Set content type for HTML response
header("Content-Type: text/html\r\n\r\n");

// Get the 'get_value' parameter from the URL query string
$get_value = isset($_GET['get_value']) ? $_GET['get_value'] : 'No GET value provided';

// Get the 'post_value' parameter from the POST request body
$post_value = isset($_POST['post_value']) ? $_POST['post_value'] : 'No POST value provided';
f
?>
<html>
<head>
    <title>PHP CGI Example</title>
</head>
<body>
    <h1>PHP CGI Script</h1>
    <p><strong>GET Value:</strong> <?php echo htmlspecialchars($get_value); ?></p>
    <p><strong>POST Value:</strong> <?php echo htmlspecialchars($post_value); ?></p>
</body>
</html>
