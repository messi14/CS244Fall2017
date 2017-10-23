<?php

#Getting post data sent from the client
$temperature = $_POST["red"];
$humidity = $_POST["IR"];

#forming a string to write to the file
$temp = $temperature.", ".$humidity."\n";

#file path where data is to be written
$file = 'data3.txt';

#open a file and write the string data to it
if($handle = fopen($file, 'a')) {
    fwrite($handle, $temp);
    fclose($handle);
    echo "SUCCESS: Data written to file.";
} else {
    echo "FAILED: Could not open file for writing.";
}
?>

