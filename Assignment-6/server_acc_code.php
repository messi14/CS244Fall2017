<?php

#forming a string to write to the file
$data = json_decode(file_get_contents('php://input'), true);
$data = $data["data"];
#print_r($data);

#file path where data is to be written
$file = 'acc_data.txt';

#open a file and write the string data to it
if($handle = fopen($file, 'a')) {
    fwrite($handle, $header);
    $RED =  $data["Red"];
    $IR = $data["IR"];
    $X = $data['X'];
    $Y = $data['Y'];
    $Z = $data['Z'];

    for ($i = 0; $i < count($RED); $i++) {
        $temp = $RED[$i].", ".$IR[$i].", ".$X[$i].", ".$Y[$i].", ".$Z[$i]."\n";
        fwrite($handle, $temp);
    }
    fclose($handle);
    echo "SUCCESS: Data written to file.";
} else {
    echo "FAILED: Could not open file for writing.";
}
?>

