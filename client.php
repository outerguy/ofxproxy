<?php
/*
client.php: JavaScriptを生成する
Copyright (C) 2012-2015 OFFICE OUTERGUY. All rights reserved.
mailto:contact@beatrek.com
Dual-licensed under the GNU AGPLv3 and Beatrek Origin License.
*/

require_once("./common.inc");
require_once("./client.inc");

// JavaScriptテンプレートを読み込む
$js = file_get_contents(ENV_FILE_DIR_CLIENT . ENV_FILE_TEMPLATE_JS);

// 埋め込み文字列を置換する
$js = str_replace(array("<!--[client]-->", "<!--[server]-->", "<!--[family]-->", "\"<!--[debug]-->\""), array(ENV_PRODUCT_CLIENT_VERSION, ENV_PRODUCT_VERSION, ENV_PRODUCT_FAMILY, (ENV_BOOL_DEBUG == true? "true": "false")), $js);

// レスポンスを返す
header(get_http_status(ENV_NUM_STATUS_SUCCESS));
header("Cache-Control: no-cache");
header("Pragma: no-cache");
header("Content-Type: text/javascript; charset=UTF-8");
header("Content-Length: " . strlen($js));
echo $js;

?>
