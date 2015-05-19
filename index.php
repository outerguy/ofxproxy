<?php
/*
index.php: HTMLを生成する
Copyright (C) 2012-2015 OFFICE OUTERGUY. All rights reserved.
mailto:contact@beatrek.com
Dual-licensed under the GNU All-Permissive License and Beatrek Origin License.
*/

require_once("./common.inc");
require_once("./client.inc");

$fiid = $_GET["help"];

// INIファイルに定義されている金融機関を整理する
$banks = array();
$creditcards = array();
$invstmts = array();
$prepaids = array();
$fis = get_fi_settings();
foreach($fis as $fi) {
	switch($fi["type"]) {
	case "BANK": // 銀行
		array_push($banks, $fi);
		break;
	case "CREDITCARD": // クレジットカード
		array_push($creditcards, $fi);
		break;
	case "INVSTMT": // 証券
		array_push($invstmts, $fi);
		break;
	case "PREPAID": // 前払式帳票
		array_push($prepaids, $fi);
		break;
	default:
		// 何もしない
		break;
	}
}

// HTMLを取得する
$html = file_get_contents(ENV_FILE_DIR_CLIENT . ENV_FILE_TEMPLATE_HTML);

$str = "";

if($fiid != "" && array_search($fiid, array_keys($fis)) !== false) {
	// ヘルプを要求されている場合、ヘルプを生成する
	$str .= "<h2 id=\"" . $fis[$fiid]["fiid"] . "\">" . $fis[$fiid]["name"] . "</h2>\r\n";
	$str .= $fis[$fiid]["help"];
	$str .= "<p><input type=\"button\" value=\"閉じる\" onclick=\"javascript: self.window.close(); return false;\" onkeypress=\"this.click();\" /></p>\r\n";

} else {
	// デバッグ機能が有効の場合、デバッグHTMLを取得する
	if(ENV_BOOL_DEBUG == true) $str .= file_get_contents(ENV_FILE_DIR_CLIENT . ENV_FILE_TEMPLATE_DEBUG);

	// ホームHTMLを取得する
	$str .= file_get_contents(ENV_FILE_DIR_CLIENT . ENV_FILE_TEMPLATE_HOME);

	// 埋め込み文字列を置換する
	$str = str_replace(array("<!--[bank]-->", "<!--[creditcard]-->", "<!--[invstmt]-->", "<!--[prepaid]-->", "<!--[filist]-->"), array(trim(get_form($banks)), trim(get_form($creditcards)), trim(get_form($invstmts)), trim(get_form($prepaids)), trim(get_filist($fis))), $str);
}

$html = str_replace("<!--[content]-->", trim($str), $html);

// レスポンスを返す
header(get_http_status(ENV_NUM_STATUS_SUCCESS));
header("Cache-Control: no-cache");
header("Pragma: no-cache");
header("Content-Type: text/html; charset=UTF-8");
header("Content-Length: " . strlen($html));
echo $html;

?>
