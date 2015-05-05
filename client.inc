<?php
/*
client.inc: 定数・共通関数を定義する
Copyright (C) 2008-2015 OFFICE OUTERGUY. All rights reserved.
mailto:contact@beatrek.com
Dual-licensed under the GNU AGPLv3 and Beatrek Origin License.
*/

define("ENV_FILE_DIR_CLIENT", "./client/");

define("ENV_FILE_TEMPLATE_HTML", "template.html");
define("ENV_FILE_TEMPLATE_JS", "template.js");
define("ENV_FILE_TEMPLATE_HOME", "home.html");
define("ENV_FILE_TEMPLATE_FORM", "form.html");
define("ENV_FILE_TEMPLATE_DEBUG", "debug.html");

define("ENV_FILE_EXT_HTML", ".html");

define("ENV_PRODUCT_CLIENT_VERSION", "1.1");

// フォームを生成する
function get_form($fis) {
	$ret = "";
	foreach($fis as $fi) {
		// フォームHTMLを取得する
		$str = file_get_contents(ENV_FILE_DIR_CLIENT . ENV_FILE_TEMPLATE_FORM);
		
		$ret .= str_replace(array("<!--[fiid]-->", "<!--[name]-->", "<!--[home]-->", "<!--[auth]-->"), array($fi["fiid"], $fi["name"], $fi["home"], trim(get_auth($fi))), $str);
	}
	return $ret;
}

// 認証情報を生成する
function get_auth($fi) {
	$ret = "";
	
	$forms = explode("|", $fi["form"]);
	foreach($forms as $form) {
		list($text, $attr) = explode("|", $fi[$form], 2);
		$attr = strtolower($attr);
		switch($attr) {
		case "text":
		case "password":
			break;
		default:
			$attr = "text";
			break;
		}
		$ret .= "<dt>" . $text . "</dt>\r\n";
		$ret .= "<dd><input type=\"" . $attr . "\" name=\"" . $fi["fiid"] . "_" . $form . "\" value=\"\" size=\"16\" maxlength=\"256\" class=\"ofxinput\" /></dd>\r\n";
	}
	
	return $ret;
}

// 金融機関一覧を生成する
function get_filist($fis) {
	$ret = "";
	foreach($fis as $fi) $ret .= "<li><label for=\"" . $fi["fiid"] . "_hide\"><input type=\"checkbox\" name=\"" . $fi["fiid"] . "_hide\" id=\"" . $fi["fiid"] . "_hide\" onchange=\"self.document.getElementById(&quot;" . $fi["fiid"] . "&quot;).style.display = (this.checked == true? &quot;none&quot;: &quot;block&quot;); save_cookie_ofxform(); self.document.getElementById(&quot;" . $fi["fiid"] . "_hide&quot;).focus();\" />" . $fi["name"] . "</label></li>\r\n";
	
	return $ret;
}

?>
