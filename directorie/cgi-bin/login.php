<?php
// Start or resume the session
session_start();

// Check if a username is already set in the session
if (isset($_SESSION['username']) && !empty($_SESSION['username'])) {
    // If yes, redirect to the welcome page
    header('Location: welcome.php');
    exit();
}

// Check if the form has been submitted
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    // Validate the submitted form data (you may want to add more validation)
    $username = filter_input(INPUT_POST, 'username', FILTER_SANITIZE_STRING);
    $title = filter_input(INPUT_POST, 'title', FILTER_SANITIZE_STRING);

    // Save the username and title in the session
    $_SESSION['username'] = $username;
    $_SESSION['title'] = $title;

    // Redirect to the welcome page
    header('Location: welcome.php');
    exit();
}
?>

<!DOCTYPE html>
<html>
<head>
    <title>Login Page</title>
</head>
<body>
    <h1>Login</h1>
    <form action="login.php" method="post">
        <label for="username">Username:</label>
        <input type="text" id="username" name="username" required><br>

        <label for="title">Title:</label>
        <input type="text" id="title" name="title" required><br>

        <input type="submit" value="Login">
    </form>
</body>
</html>
