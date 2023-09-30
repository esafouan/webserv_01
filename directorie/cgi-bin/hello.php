<?php

if ($_SERVER["REQUEST_METHOD"] === "POST") {
    $name = $_POST["name"];
    $title = $_POST["title"];

    echo "<html><body>";
    echo "<h1>Received Data:</h1>";
    echo "<p>Name: " . htmlspecialchars($name) . "</p>";
    echo "<p>Title: " . htmlspecialchars($title) . "</p>";
    echo "</body></html>";
} else if ($_SERVER["REQUEST_METHOD"] === "GET") {
    $name = $_GET["name"];
    $title = $_GET["title"];

    echo "<html><body>";
    echo "<h1>Received Data:</h1>";
    echo "<p>Name: " . htmlspecialchars($name) . "</p>";
    echo "<p>Title: " . htmlspecialchars($title) . "</p>";
    echo "</body></html>";
} else {
    // Handle other HTTP methods or provide an error message
    header("HTTP/1.1 405 Method Not Allowed");
    echo "Method not allowed";
}


 ?>  