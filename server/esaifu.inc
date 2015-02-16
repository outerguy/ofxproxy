<?php
/*
esaifu.inc: e-�����ӂ�HTML���OFX�t�@�C���𐶐�����
Copyright (C) 2013-2015 OFFICE OUTERGUY. All rights reserved.
mailto:contact@beatrek.com
Licensed under the GNU AGPLv3.
*/

// ���O�C�������擾����
$user = $ofxforms["user_id"];
$pass = $ofxforms["password"];
$auth = urlencode(mb_convert_encoding(urldecode($ofxforms["image_key"]), "Shift_JIS", "UTF-8"));
$sesscookie = $ofxforms["sesscookie"];
$accesskey = $ofxforms["accesskey"];

$resp = array();
$ofx = "";

if($sesscookie != "") list($cookie, $dummy) = explode("\t", sess_decode($sesscookie));
if($accesskey != "") {
	list($ssid, $method, $uri, $query, $user, $dummy) = explode("\t", sess_decode($accesskey), 6);
	$sid = (integer)$ssid;
}

// ���s���ԁi�^�C���A�E�g�j���Đݒ肷��
@set_time_limit(ENV_NUM_TIMEOUT);

if($sid == 0) {
	// �z�[����ʂ��擾����
	$method = "GET";
	$uris = parse_uri($settings["home"]);
	$query = "";
	$cookie = "";
	list($head, $body) = esaifu_http11($method, $uris, $query, $cookie);
	
	// ���_�C���N�g����
	$retry = 0;
	while($retry++ < ENV_NUM_HTTP11_RETRY) {
		$locations = parse_header($head, "location");
		if(count($locations) > 0) {
			$method = "GET";
			$uris = parse_uri($locations[0], $uris);
			$query = "";
			$cookie = esaifu_update_cookie($head, $cookie);
			list($head, $body) = esaifu_http11($method, $uris, $query, $cookie);
		}
	}
	
	// ���O�C����ʂ��擾����
	$as = parse_tag($body, "a");
	foreach($as as $a) if(parse_tag_search(parse_tag($a["innerHTML"], "img"), "alt", "������O�C���j") != -1) {
		$method = "GET";
		$uris = parse_uri($a["href"], $uris);
		$query = "";
		$cookie = esaifu_update_cookie($head, $cookie);
		list($head, $body) = esaifu_http11($method, $uris, $query, $cookie);
		break;
	}
	
	// �摜�F�؂̉摜���擾����
	$imgs = parse_tag($body, "img");
	$c = parse_tag_search($imgs, "id", "siimage");
	if($c != -1) {
		$imguris = parse_uri($imgs[$c]["src"], $uris);
		list($imghead, $imgbody) = esaifu_http11($method, $imguris, $query, $cookie, $uris["scheme"] . "://" . $uris["host"] . $uris["path"] . $query);
		$imgsrc = "data:image/png;base64," . base64_encode($imgbody);
	}
	
	// ���O�C������
	$forms = parse_tag($body, "form");
	if(count($forms) > 0) {
		$inputs = parse_tag($forms[0]["innerHTML"], "input");
		$queries = array();
		foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
		$queries["user_id"] = "user_id=" . $user;
		$queries["password"] = "password=" . $pass;
		$queries["image_key"] = "image_key=";
		//$queries["submit.x"] = "submit.x=0";
		//$queries["submit.y"] = "submit.y=0";
		
		$method = $forms[0]["method"];
		$uris = parse_uri($forms[0]["action"], $uris);
		$query = implode("&", $queries);
		$cookie = esaifu_update_cookie($head, $cookie);
	}
	
	// �Z�b�V������ޔ�����
	$sid = 1;
	$sess = sess_encode(implode("\t", array((string)$sid, $method, $uris["scheme"] . "://" . $uris["host"] . $uris["path"], $query, $user, ENV_STR_SESSION_PADDING)));
	$head = "";
	$body = "";
} else if($sid == 1) {
	// �Z�b�V�����𕜌�����
	$sid = 0;
	$uris = parse_uri($uri);
	$query = str_replace("image_key=", "image_key=" . $auth, $query);
	list($head, $body) = esaifu_http11($method, $uris, $query, $cookie);
	
	// ���_�C���N�g����
	$forms = parse_tag($body, "form");
	$c = parse_tag_search($forms, "id", "form1");
	if($c != -1) {
		$scripts = parse_tag($body, "script");
		foreach($scripts as $script) if(preg_match("/([\'\"])([^\\1]*?)\\1/i", $script["innerHTML"], $matches) > 0) {
			$forms[$c]["action"] = $matches[2];
			
			$method = $forms[$c]["method"];
			$uris = parse_uri($forms[$c]["action"], $uris);
			// $query�͈�O�̂��̂��g����
			// $query = implode("&", $queries);
			$cookie = esaifu_update_cookie($head, $cookie);
			list($head, $body) = esaifu_http11($method, $uris, $query, $cookie);
			break;
		}
	}
	
	// ���_�C���N�g����
	$retry = 0;
	while($retry++ < ENV_NUM_HTTP11_RETRY) {
		$locations = parse_header($head, "location");
		if(count($locations) > 0) {
			$method = "GET";
			$uris = parse_uri($locations[0], $uris);
			$query = "";
			$cookie = esaifu_update_cookie($head, $cookie);
			list($head, $body) = esaifu_http11($method, $uris, $query, $cookie);
		}
	}
}

if($sid > 0) {
	// �Z�b�V�����������p��
	$resp["status"] = ENV_NUM_STATUS_ADDITION;
	$resp["aid"] = "image_key";
	$resp["addition"] = $imgsrc;
	$resp["sess"] = $sess;
	$resp["sid"] = $sid;
	$resp["sesscookie"] = sess_encode(implode("\t", array($cookie, ENV_STR_SESSION_PADDING)));
	$resp["accesskey"] = sess_encode(implode("\t", array((string)$sid, $method, $uris["scheme"] . "://" . $uris["host"] . $uris["path"], $query, $user, ENV_STR_SESSION_PADDING)));
	
	$mfachallengetrnrs = "";
	$mfachallengetrnrs .= "<MFACHALLENGETRNRS>";
	$mfachallengetrnrs .= "<MFACHALLENGERS>";
	$mfachallengetrnrs .= "<MFACHALLENGE>";
	$mfachallengetrnrs .= "<MFAPHRASEID>" . $resp["aid"] . "</MFAPHRASEID>";
	$mfachallengetrnrs .= "<MFAPHRASELABEL>" . mb_convert_string($resp["addition"]) . "</MFAPHRASELABEL>";
	$mfachallengetrnrs .= "</MFACHALLENGE>";
	$mfachallengetrnrs .= "</MFACHALLENGERS>";
	$mfachallengetrnrs .= "</MFACHALLENGETRNRS>";
	
	$resp["ofx"] = generate_ofx($resp["status"], $mfachallengetrnrs, $resp["sesscookie"], $resp["accesskey"]);
} else if(strpos($body, "�����e�i���X��") !== false) {
	// �V�X�e�������e�i���X��ʂ̏ꍇ
	$resp["status"] = ENV_NUM_STATUS_MAINTENANCE;
	$resp["method"] = $method;
	$resp["uri"] = $uris["scheme"] . "://" . $uris["host"] . $uris["path"];
	$resp["query"] = $query;
	$resp["cookie"] = $cookie;
	$resp["head"] = $head;
	$resp["body"] = $body;
	$resp["ofx"] = generate_ofx($resp["status"]);
} else if(strpos($body, "�O��̂����p����") === false) {
	// ���O�C�����s�̏ꍇ
	$resp["status"] = ENV_NUM_STATUS_FAILURE;
	$resp["method"] = $method;
	$resp["uri"] = $uris["scheme"] . "://" . $uris["host"] . $uris["path"];
	$resp["query"] = $query;
	$resp["cookie"] = $cookie;
	$resp["head"] = $head;
	$resp["body"] = $body;
	$resp["ofx"] = generate_ofx($resp["status"]);
} else {
	$account = array();
	
	$account["acctname"] = $settings["name"];
	
	// �x�X�ԍ����擾����
	$account["branchid"] = "0";
	
	// �����ԍ����擾����
	$account["acctid"] = $user;
	
	// �c�����擾����
	$spans = parse_tag($body, "span");
	$c = parse_tag_search($spans, "class", "balance");
	if($c != -1) $account["balance"] = parse_amount(strip_tags($spans[$c]["innerHTML"]));
	
	// ����\����ʂ��擾����
	$lis = parse_tag($body, "li");
	$c = parse_tag_search($lis, "class", "navi_history");
	if($c != -1) {
		$as = parse_tag($lis[$c]["innerHTML"], "a");
		if(count($as) == 1) {
			$method = "GET";
			$uris = parse_uri($as[0]["href"], $uris);
			$query = "";
			$cookie = esaifu_update_cookie($head, $cookie);
			list($head, $body) = esaifu_http11($method, $uris, $query, $cookie);
		}
	}
	
	// ������ʂ��擾����
	$forms = parse_tag($body, "form");
	if(count($forms) > 0) {
		$queries = array();
		$inputs = parse_tag($forms[0]["innerHTML"], "input");
		foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
		$selects = parse_tag($forms[0]["innerHTML"], "select");
		foreach($selects as $select) {
			$options = parse_tag($select["innerHTML"], "option");
			foreach($options as $option) if($select["name"] != "" && $option["selected"] == "selected") $queries[$select["name"]] = urlencode($select["name"]) . "=" . urlencode($option["value"]);
		}
		$queries["user_year_from"] = "user_year_from=" . (string)(integer)substr(ENV_STR_DATE_PASTDAY, 0, 4);
		$queries["user_month_from"] = "user_month_from=" . (string)(integer)substr(ENV_STR_DATE_PASTDAY, 4, 2);
		$queries["user_day_from"] = "user_day_from=" . (string)(integer)substr(ENV_STR_DATE_PASTDAY, 6, 2);
		$queries["user_year_to"] = "user_year_to=" . (string)(integer)substr(ENV_STR_DATE_TODAY, 0, 4);
		$queries["user_month_to"] = "user_month_to=" . (string)(integer)substr(ENV_STR_DATE_TODAY, 4, 2);
		$queries["user_day_to"] = "user_day_to=" . (string)(integer)substr(ENV_STR_DATE_TODAY, 6, 2);
		
		$method = $forms[0]["method"];
		$uris = parse_uri($forms[0]["action"], $uris);
		$query = implode("&", $queries);
		$cookie = esaifu_update_cookie($head, $cookie);
		list($head, $body) = esaifu_http11($method, $uris, $query, $cookie);
	}
	
	// ���ׂ��擾����
	$tables = parse_tag($body, "table");
	$c = parse_tag_search($tables, "class", "generic");
	if($c != -1) {
		$tbodys = parse_tag($tables[$c]["innerHTML"], "tbody");
		if(count($tbodys) > 0) $account["details"] = esaifu_get_details($tbodys[0]["innerHTML"]);
	}
	
	// ���s���ԁi�^�C���A�E�g�j���Đݒ肷��
	@set_time_limit(ENV_NUM_TIMEOUT);
	
	// ���O�A�E�g����
	$as = parse_tag($body, "a");
	foreach($as as $a) if(parse_tag_search(parse_tag($a["innerHTML"], "img"), "alt", "���O�A�E�g") != -1) {
		$method = "GET";
		$uris = parse_uri($a["href"], $uris);
		$query = "";
		$cookie = esaifu_update_cookie($head, $cookie);
		list($head, $body) = esaifu_http11($method, $uris, $query, $cookie);
		break;
	}
	
	// ���_�C���N�g����
	$retry = 0;
	while($retry++ < ENV_NUM_HTTP11_RETRY) {
		$locations = parse_header($head, "location");
		if(count($locations) > 0) {
			$method = "GET";
			$uris = parse_uri($locations[0], $uris);
			$query = "";
			$cookie = esaifu_update_cookie($head, $cookie);
			list($head, $body) = esaifu_http11($method, $uris, $query, $cookie);
		}
	}
	
	// ���s���ԁi�^�C���A�E�g�j���Đݒ肷��
	@set_time_limit(ENV_NUM_TIMEOUT);
	
	$bankmsgsrsv1 = "";
	$bankmsgsrsv1 .= "<BANKMSGSRSV1>";
	$bankmsgsrsv1 .= "\r\n";
	
	// ���������擾����
	$bankmsgsrsv1 .= "<STMTTRNRS>";
	$bankmsgsrsv1 .= "\r\n";
	$bankmsgsrsv1 .= "<TRNUID>0</TRNUID>";
	$bankmsgsrsv1 .= "\r\n";
	$bankmsgsrsv1 .= "<STATUS><CODE>0</CODE><SEVERITY>INFO</SEVERITY></STATUS>";
	$bankmsgsrsv1 .= "\r\n";
	$bankmsgsrsv1 .= "<STMTRS>";
	$bankmsgsrsv1 .= "\r\n";
	$bankmsgsrsv1 .= "<CURDEF>" . ENV_STR_OFX_CURRENCY_JPY . "</CURDEF>";
	$bankmsgsrsv1 .= "\r\n";
	$bankmsgsrsv1 .= "<BANKACCTFROM>";
	$bankmsgsrsv1 .= "<BANKID>" . $settings["code"] . "</BANKID>";
	$bankmsgsrsv1 .= "<BRANCHID>" . $account["branchid"] . "</BRANCHID>";
	$bankmsgsrsv1 .= "<ACCTID>" . $account["acctid"] . "</ACCTID>";
	$bankmsgsrsv1 .= "<ACCTTYPE>" . ENV_STR_ACCTTYPE_CHECKING . "</ACCTTYPE>";
	$bankmsgsrsv1 .= "</BANKACCTFROM>";
	$bankmsgsrsv1 .= "\r\n";
	$bankmsgsrsv1 .= esaifu_parse_details($account);
	$bankmsgsrsv1 .= "</STMTRS>";
	$bankmsgsrsv1 .= "\r\n";
	$bankmsgsrsv1 .= "</STMTTRNRS>";
	$bankmsgsrsv1 .= "\r\n";
	
	$bankmsgsrsv1 .= "</BANKMSGSRSV1>";
	$bankmsgsrsv1 .= "\r\n";
	
	// OFX�t�@�C�����o�͂���
	$resp["status"] = ENV_NUM_STATUS_SUCCESS;
	$resp["ofx"] = generate_ofx($resp["status"], $bankmsgsrsv1);
}
return $resp;

// HTTP/1.1
function esaifu_http11($method, $uris, $query = "", $cookie = "", $referer = "") {
	$ret = "INVALID HOST";
	if(preg_match("/\.mun-prepaid\.com$/", $uris["host"]) > 0) $ret = http11(strtoupper($method), $uris["scheme"], $uris["host"], 0, $uris["path"], $query, "", $cookie, true, ENV_PRODUCT_UA, "", $referer);
	return explode("\r\n\r\n", $ret, 2);
}

function esaifu_update_cookie($head, $cookie) {
	return update_cookie(array("cookie_use_ok", "sslCookieUseOk", "vp_partner_id", "vp_session", "Nicos_pmk"), parse_header($head, "set-cookie"), $cookie);
}

function esaifu_get_details($body) {
	$rets = array();
	$i = 0;
	$trs = parse_tag($body, "tr");
	foreach($trs as $tr) {
		$tds = parse_tag($tr["innerHTML"], "td");
		if(count($tds) == 7) {
			// ���p���t���擾����
			$dt = explode(" ", $tds[1]["innerHTML"]);
			$rets[$i]["date"] = parse_date(trim(strip_tags($dt[0])));
			
			// �����p��ʁA�����p���e�A����т����ID���擾����
			$buf = trim(strip_tags($tds[2]["innerHTML"]));
			switch($buf) {
			case "�����p�i�\��j":
				$name = $buf;
				$memo = implode(ENV_CHR_CONCATENATOR, array(str_replace("�J�[�h��4�� ", "", trim(strip_tags($tds[3]["innerHTML"]))), trim(strip_tags($tds[6]["innerHTML"]))));
				break;
			case "�����p�i�m��j":
				list($name, $memo) = explode("<br>", $tds[3]["innerHTML"], 2);
				$name = trim(strip_tags($name));
				$memo = implode(ENV_CHR_CONCATENATOR, array(str_replace("�J�[�h��4�� ", "", trim(strip_tags($memo))), trim(strip_tags($tds[6]["innerHTML"]))));
				break;
			default:
				$name = trim(strip_tags($tds[3]["innerHTML"]));
				$memo = $buf;
				break;
			}
			$rets[$i]["summary"] = $name;
			$rets[$i]["memo"] = $memo;
			
			// �����p���z���擾����
			$rets[$i]["amount"] = parse_amount(strip_tags($tds[4]["innerHTML"]));
			
			$i++;
		}
	}
	
	return $rets;
}

function esaifu_parse_details($account) {
	$ret = "";
	$cds = array();
	$cd_date = "";
	$cd_num = 0;
	foreach($account["details"] as $line) {
		$cd = array();
		$cd["TRNTYPE"] = ENV_STR_OFX_TRNTYPE_DIRECTDEBIT;
		
		// ���t���擾����
		$cd["DTPOSTED"] = $line["date"];
		
		// �ʔԂ𐶐�����
		if($cd["DTPOSTED"] == $cd_date) $cd_num++; else $cd_num = 0;
		
		// �g�����U�N�V�����ԍ��𐶐�����
		$cd["FITID"] = $cd["DTPOSTED"] . "0000000" . sprintf("%05d", $cd_num);
		
		// �E�v���擾����
		$cd["NAME"] = mb_convert_string($line["summary"]);
		
		// ���z���擾����
		$cd["TRNAMT"] = parse_amount($line["amount"]);
		$cd["MEMO"] = ($line["memo"] != ""? mb_convert_string($line["memo"]): ENV_STR_OFX_MEMO);
		
		array_push($cds, $cd);
		$cd_date = $cd["DTPOSTED"];
	}
	// BANKTRANLIST
	$ret .= "<BANKTRANLIST>";
	$ret .= "\r\n";
	$ret .= "<DTSTART>" . ENV_STR_DATE_PASTDAY . ENV_STR_OFX_TZ . "</DTSTART>";
	$ret .= "<DTEND>" . ENV_STR_DATE_TODAY . ENV_STR_OFX_TZ . "</DTEND>";
	$ret .= "\r\n";
	
	foreach($cds as $cd) {
		$ret .= "<STMTTRN>";
		$ret .= "<TRNTYPE>" . $cd["TRNTYPE"] . "</TRNTYPE>";
		$ret .= "<DTPOSTED>" . $cd["DTPOSTED"] . ENV_STR_OFX_TZ . "</DTPOSTED>";
		$ret .= "<TRNAMT>" . $cd["TRNAMT"] . "</TRNAMT>";
		$ret .= "<FITID>" . $cd["FITID"] . "</FITID>";
		$ret .= "<NAME>" . $cd["NAME"] . "</NAME>";
		$ret .= "<MEMO>" . $cd["MEMO"] . "</MEMO>";
		$ret .= "</STMTTRN>";
		$ret .= "\r\n";
	}
	
	$ret .= "</BANKTRANLIST>";
	$ret .= "\r\n";
	$ret .= "<LEDGERBAL>";
	$ret .= "<BALAMT>" . $account["balance"] . "</BALAMT>";
	$ret .= "<DTASOF>" . ENV_STR_DATE_TODAY . ENV_STR_OFX_TZ . "</DTASOF>";
	$ret .= "</LEDGERBAL>";
	$ret .= "\r\n";
	
	// �������̂��o�͂���
	if($account["acctname"] != "") {
		$ret .= "<MKTGINFO>" . mb_convert_string($account["acctname"]) . "</MKTGINFO>";
		$ret .= "\r\n";
	}
	
	return $ret;
}

?>