#!/usr/bin/php-cgi
<?php
// Set content type
header("Content-Type: text/plain");


// Print POST data
echo "POST Data:\n";
if (!empty($_POST)) {
    print_r($_POST);
} else {
    echo "No POST data received.\n";
}

