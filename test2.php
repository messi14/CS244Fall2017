<?php

#Getting post data sent from the client
$temperature = $_POST["temperature"];
$humidity = $_POST["humidity"];
$heartRate = $_POST["heartRate"];

#forming a string to write to the file
$temp = "Data Info: \n Temperature : ".$temperature."\n Humidity : ".$humidity."\n Heart Rate : ".$heartRate;

#file path where data is to be written
$file = '/home/ubuntu/data.txt';

#open a file and write the string data to it
if($handle = fopen($file, 'w')) {
    fwrite($handle, $temp);
    fclose($handle);
    echo "SUCCESS: Data written to file.";
} else {
    echo "Could not open file for writing.";
}
?>
