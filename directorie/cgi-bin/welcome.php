<?php
// Start or resume the session
session_start();

// Check if a username is set in the session
if (isset($_SESSION['username']) && !empty($_SESSION['username'])) {
    // Display a welcome message with the username and title
    echo ("<h1 style=\"color: red;\">");
    echo " Welcome back, " . htmlspecialchars($_SESSION['username']) . "! Your title is: " . htmlspecialchars($_SESSION['title']);
    echo ("</h1>");
} else {
    // Redirect to the login page if no username is set
    header('Location: login.php');
    exit(); // Ensure that the script stops here
}
?>

<!DOCTYPE html>
<html>
<head>
    <title>Welcome Page</title>
</head>
<body>
    <!-- Rest of your welcome page content can go here -->
</body>
</html>
