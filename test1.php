<?php
$t1 = microtime(true);
function tetst($a){
$myfile = fopen("/server/cop/data1.txt", "a") or die("Unable to open file!");
$txt = $a."\r\n";
fwrite($myfile, $txt);
fclose($myfile);
echo $a.'<br/>';
}
for ($i=0; $i <5000 ; $i++) { 
	tetst($i);
}
$t2 = microtime(true);
echo '普通函数耗时为：'.(($t2-$t1)*1000).':ms';

?>