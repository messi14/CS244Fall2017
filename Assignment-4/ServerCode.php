<?php

#reading json data and decoding it using json_decode function
$data = json_decode(file_get_contents('php://input'), true);
$data = $data["data"];
#print_r($data);

#file path where data is to be written
$file = 'acc_data.txt';

#open a file and write the string data to it
if($handle = fopen($file, 'a')) {
    fwrite($handle, $header);
    foreach($data as $item) { //foreach element in $arr
        $X = $item['X'];
        $Y = $item['Y'];
        $Z = $item['Z'];
        $temp =  $X.", ".$Y.", ".$Z."\n";
        fwrite($handle, $temp);
    }
    fclose($handle);
    echo "SUCCESS: Data written to file.";
} else {
    echo "FAILED: Could not open file for writing.";
}
?>