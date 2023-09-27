<?php
// Start or resume the session
session_start();
header_remove('Expires');
header_remove('Cache-Control');
header_remove('Pragma');
header_remove('Content-type');

// Check if the user is logged in
if(isset($_SESSION['name']) && isset($_SESSION['title'])) {
    echo ("<br>");
    // If the user is logged in, display their name and title
    echo("<h1 style=\"color: red;\">");
    echo "Welcome back, " . $_SESSION['name'] . "! Your title is: " . $_SESSION['title'];
    echo("</h1>");

} else {
    // If the user is not logged in, show the input form
    if ($_SERVER["REQUEST_METHOD"] == "POST") {
        // If the form is submitted, save the name and title in session variables and cookies
        $name = $_POST['name'];
        $title = $_POST['title'];

        // Save the values in session variables
        $_SESSION['name'] = $name;
        $_SESSION['title'] = $title;

        // Set cookies to remember the values
        setcookie('name', $name, time() + 3600); // Expires in 1 hour
        setcookie('title', $title, time() + 3600);

        // Display a message to the user
        echo "Welcome, " . $name . "! Your title is: " . $title;
    } else {
        // If the form is not submitted, show the input form
        ?>
        <!DOCTYPE html>
        <html>
        <head>
            <title>Session and Cookie Example</title>
        </head>
        <body>
            <h1>Enter your name and title:</h1>
            <form method="post" action="">
                <label for="name">Name:</label>
                <input type="text" id="name" name="name" required><br><br>
                <label for="title">Title:</label>
                <input type="text" id="title" name="title" required><br><br>
                <input type="submit" value="Submit">
            </form>
        </body>
        </html>
        <?php
    }
}
?>
