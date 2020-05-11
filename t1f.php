<?php
    
$servername = "localhost";
$username = "wwwwldpc_admin";
$password = "vectra24";
$dbname = "wwwwldpc_tcp";

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);

// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}
echo "Connected successfully";

$sql = "INSERT INTO incetray1 (data) VALUES ('2')";

if ($conn->query($sql) === TRUE) {
    echo "New record created successfully";
} else {
    echo "Error: " . $sql . "<br>" . $conn->error;
}

mysqli_close($conn);
?>
