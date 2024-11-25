#!/usr/bin/php-cgi
<?php
// Start the session
session_start();

// Set content type
header("Content-Type: text/html");

// Handle login
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $username = $_POST['username'] ?? '';
    $password = $_POST['password'] ?? '';

    // Check credentials
    if ($username === 'admin' && $password === 'admin') {
        $_SESSION['logged_in'] = true;
        echo "Login successful!";
    } else {
        echo "Invalid credentials. <a href='login.php'>Try Again</a>";
    }
    exit;
}

// Check if already logged in
if (!empty($_SESSION['logged_in'])) {
    echo "You are already logged in.";
    exit;
}

// Login form
?>
<!DOCTYPE html>
<html>
<head>
    <title>Login</title>
</head>
<body>
    <center>
        <form method="POST" action="test_post.php">
            <br>
            <br>
            <br>
            <br>
            <br>
            <br>
            <br>
            <label for="username">Username:</label>
            <input type="text" id="username" name="username" required>
            <br>
            <br>
            <label for="password">Password:</label>
            <input type="password" id="password" name="password" required>
            <br>
            <br>
            <button type="submit">Login</button>
        </form>
    </center>
    </body>
</html>
