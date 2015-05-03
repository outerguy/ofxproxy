<?php
/*
OFXProxy
server.php: ログイン情報を受け取って金融機関毎の処理を実行する
Copyright (C) 2012-2015 OFFICE OUTERGUY. All rights reserved.
mailto:contact@beatrek.com
Dual-licensed under the GNU AGPLv3 and Beatrek Origin License.
*/

require_once("./common.inc");

$resp = array();
$ofxforms = array();
$debug = "";
while(list($k, $v) = each($_POST)) {
	$ofxforms[$k] = parse_param($v);
	$debug .= $k . "=" . $v . "\r\n";
	
	$p = strpos($k, "_");
	if($p !== false) {
		$ofxforms[substr($k, $p + 1)] = parse_param($v);
		$debug .= substr($k, $p + 1) . "=" . $v . "\r\n";
	}
}
$ofxforms["fiid"] = basename($ofxforms["fiid"]);
env_dlog($debug);

if(file_exists(ENV_FILE_DIR_SERVER . $ofxforms["fiid"] . ENV_FILE_EXT_INC) == true && is_readable(ENV_FILE_DIR_SERVER . $ofxforms["fiid"] . ENV_FILE_EXT_INC) == true) {
	$settings = get_fi_settings($ofxforms["fiid"]);
	$resp = require_once(ENV_FILE_DIR_SERVER . $ofxforms["fiid"] . ENV_FILE_EXT_INC);
}

$content = "";
switch($resp["status"]) {
case ENV_NUM_STATUS_SUCCESS:
	$content = $resp["ofx"];
	break;
case ENV_NUM_STATUS_ADDITION:
case ENV_NUM_STATUS_FAILURE:
case ENV_NUM_STATUS_MAINTENANCE:
case ENV_NUM_STATUS_CAUTION:
	$content = generate_html($settings, $resp);
	break;
case ENV_NUM_STATUS_NONE:
default:
	$resp["status"] = ENV_NUM_STATUS_NONE;
	break;
}

header(get_http_status($resp["status"]));
header("Cache-Control: no-cache");
header("Pragma: no-cache");

switch($resp["status"]) {
case ENV_NUM_STATUS_SUCCESS:
	header("Content-Type: application/x-ofx; charset=UTF-8");
	header("Content-Disposition: attachment; filename=\"" . $settings["fiid"] . "_" . ENV_STR_DATE_TODAY . ENV_FILE_EXT_OFX . "\"");
	break;
case ENV_NUM_STATUS_NONE:
	$content = "";
	break;
case ENV_NUM_STATUS_ADDITION:
case ENV_NUM_STATUS_FAILURE:
case ENV_NUM_STATUS_MAINTENANCE:
case ENV_NUM_STATUS_CAUTION:
default:
	header("Content-Type: text/html; charset=UTF-8");
	break;
}

$n = strlen($content);
if($n > 0) {
	header("Content-Length: " . (string)$n);
	echo $content;
}

?>
