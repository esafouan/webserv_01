<?php
  print_r($_GET);


echo "sssss";
#echo "Name: <br>";
// while(true);
 if ($_SERVER["REQUEST_METHOD"] === "GET") {
//     // Retrieve the posted data
     $name = $_POST["name"];
     $title = $_POST["title"];

//     // Process the data (e.g., print it)
//     // header("Content-Type: text/html; charset=UTF-8");
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
print_r($_POST);

// print_r($_GET);

// ?>  



