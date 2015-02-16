<?php
/*
saisoncard.inc: �Z�]���J�[�h��CSV���OFX�t�@�C���𐶐�����
Copyright (C) 2012-2015 OFFICE OUTERGUY. All rights reserved.
mailto:contact@beatrek.com
Licensed under the GNU AGPLv3.
*/

// ���O�C�������擾����
$user = $ofxforms["inputId"];
$pass = $ofxforms["inputPassword"];
$auth = urlencode(mb_convert_encoding(urldecode($ofxforms["secretAnswer"]), "Shift_JIS", "UTF-8"));
$sesscookie = $ofxforms["sesscookie"];
$accesskey = $ofxforms["accesskey"];

$resp = array();
$sid = 0;
$method = "";
$query = "";
$cookie = "";
$head = "";
$body = "";

if($sesscookie != "") list($cookie, $dummy) = explode("\t", sess_decode($sesscookie));
if($accesskey != "") {
	list($ssid, $method, $uri, $query, $pass, $dummy) = explode("\t", sess_decode($accesskey), 6);
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
	list($head, $body) = saisoncard_http11($method, $uris, $query, $cookie);
	
	// ���O�C����ʂ��擾����
	$as = parse_tag($body, "a");
	foreach($as as $a) if(parse_tag_search(parse_tag($a["innerHTML"], "img"), "alt", "���O�C���͂����炩��") != -1) {
		$method = "GET";
		$uris = parse_uri($a["href"], $uris);
		$query = "";
		$cookie = saisoncard_update_cookie($head, $cookie);
		list($head, $body) = saisoncard_http11($method, $uris, $query, $cookie);
		break;
	}
	
	// ���O�C������
	$forms = parse_tag($body, "form");
	$c = parse_tag_search($forms, "name", "_USA01Form");
	if($c != -1) {
		$inputs = parse_tag($forms[$c]["innerHTML"], "input");
		$queries = array();
		foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
		$queries["inputId"] = "inputId=" . $user;
		$queries["inputPassword"] = "inputPassword=" . $pass;
		if(isset($queries["inputIdCheckBox"]) == true) unset($queries["inputIdCheckBox"]);
		
		$method = $forms[$c]["method"];
		$uris = parse_uri($forms[$c]["action"], $uris);
		$query = implode("&", $queries);
		$cookie = saisoncard_update_cookie($head, $cookie);
		list($head, $body) = saisoncard_http11($method, $uris, $query, $cookie);
	}
}

if(strpos($body, "�閧�̎���ɂ�邲�{�l�l�m�F") !== false) {
	// �閧�̓�������͂���
	$tds = parse_tag($body, "td");
	$shitsumon = "";
	foreach($tds as $td) if($td["class"] == "td-01") {
		$shitsumon = trim(strip_tags($td["innerHTML"]));
		break;
	}
	
	$forms = parse_tag($body, "form");
	$c = parse_tag_search($forms, "name", "_UST00Form");
	if($c != -1) {
		$inputs = parse_tag($forms[$c]["innerHTML"], "input");
		$queries = array();
		foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
		$queries["secretAnswer"] = "secretAnswer=";
		
		$method = $forms[$c]["method"];
		$uris = parse_uri($forms[$c]["action"], $uris);
		$query = implode("&", $queries);
		$cookie = saisoncard_update_cookie($head, $cookie);
	}
	
	// �Z�b�V������ޔ�����
	$sid = 1;
	$sess = sess_encode(implode("\t", array((string)$sid, $method, $uris["scheme"] . "://" . $uris["host"] . $uris["path"], $query, $cookie, $pass, ENV_STR_SESSION_PADDING)));
	$head = "";
	$body = "";
} else if($sid == 1) {
	// �Z�b�V�����𕜌�����
	$sid = 0;
	$uris = parse_uri($uri);
	$query = str_replace("secretAnswer=", "secretAnswer=" . $auth, $query);
	list($head, $body) = saisoncard_http11($method, $uris, $query, $cookie);
}

if($sid > 0) {
	// �Z�b�V�����������p��
	$resp["status"] = ENV_NUM_STATUS_ADDITION;
	$resp["aid"] = "secretAnswer";
	$resp["addition"] = $shitsumon;
	$resp["sess"] = $sess;
	$resp["sid"] = $sid;
	$resp["sesscookie"] = sess_encode(implode("\t", array($cookie, ENV_STR_SESSION_PADDING)));
	$resp["accesskey"] = sess_encode(implode("\t", array((string)$sid, $method, $uris["scheme"] . "://" . $uris["host"] . $uris["path"], $query, $pass, ENV_STR_SESSION_PADDING)));
	
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
} else if(strpos($body, "�閧�̎���ݒ�") !== false) {
	// ���O�C����̉�ʂ��ʏ�ƈقȂ�ꍇ
	$resp["status"] = ENV_NUM_STATUS_CAUTION;
	$resp["method"] = $method;
	$resp["uri"] = $uris["scheme"] . "://" . $uris["host"] . $uris["path"];
	$resp["query"] = $query;
	$resp["cookie"] = $cookie;
	$resp["head"] = $head;
	$resp["body"] = $body;
	$resp["ofx"] = generate_ofx($resp["status"]);
} else if(strpos($body, "�����e�i���X��") !== false || strpos($body, "�T�[�r�X�������p���������܂���") !== false) {
	// �V�X�e�������e�i���X��ʂ̏ꍇ
	$resp["status"] = ENV_NUM_STATUS_MAINTENANCE;
	$resp["method"] = $method;
	$resp["uri"] = $uris["scheme"] . "://" . $uris["host"] . $uris["path"];
	$resp["query"] = $query;
	$resp["cookie"] = $cookie;
	$resp["head"] = $head;
	$resp["body"] = $body;
	$resp["ofx"] = generate_ofx($resp["status"]);
} else if(strpos($body, "�O�񃍃O�C��") === false) {
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
	$account["id"] = 0;
	
	// ���s���ԁi�^�C���A�E�g�j���Đݒ肷��
	@set_time_limit(ENV_NUM_TIMEOUT);
	
	// Net�A���T�[���Ɖ��ʂ��擾����
	$as = parse_tag($body, "a");
	foreach($as as $a) if(parse_tag_search(parse_tag($a["innerHTML"], "img"), "alt", "Net�A���T�[�o�^���e�ύX Net�A���T�[ID�A�p�X���[�h�A ���[���A�h���X�Ȃ� ") != -1) {
		$method = "GET";
		$uris = parse_uri($a["href"], $uris);
		$query = "";
		$cookie = saisoncard_update_cookie($head, $cookie);
		list($head, $body) = saisoncard_http11($method, $uris, $query, $cookie);
		break;
	}
	
	$tds = parse_tag($body, "td", true); // �ċA�I�Ɏ擾����
	$bufs = array();
	foreach($tds as $td) if($td["class"] == "td-01") array_push($bufs, trim(strip_tags($td["innerHTML"])));
	if(count($bufs) >= 2) {
		// �J�[�h���̂��擾����
		$account["acctname"] = implode(ENV_CHR_CONCATENATOR, array($settings["name"], $bufs[0]));
		$account["name"] = $settings["name"];
		
		// �J�[�h�ԍ����擾����
		$account["acctid"] = $bufs[2];
	}
	
	// �����p���׉�ʂ��擾����
	$as = parse_tag($body, "a");
	$c = parse_tag_search($as, "innerHTML", "�����p���׏Ɖ�");
	if($c != -1) {
		$method = "GET";
		$uris = parse_uri($as[$c]["href"], $uris);
		$query = "";
		$cookie = saisoncard_update_cookie($head, $cookie);
		list($head, $body) = saisoncard_http11($method, $uris, $query, $cookie);
	}
	
	// �ŐV�̐��������擾����i���x������4���ł���Ɖ��肷��j
	$c = preg_match_all("/([0-9]{1,2})" . preg_quote("�����x����") . "/", $body, $matches);
	if($c > 0) {
		$matches[1][0] = sprintf("%02d", (integer)$matches[1][0]);
		$account["paydate"] = (date("m") <= $matches[1][0]? date("Y"): (string)((integer)date("Y") + 1)) . $matches[1][0] . "04";
	}
	
	$body_old = $body;
	
	// CSV�t�@�C�����_�E�����[�h����
	$as = parse_tag($body, "a");
	foreach($as as $a) if(parse_tag_search(parse_tag($a["innerHTML"], "img"), "alt", "CSV�_�E�����[�h") != -1) {
		$method = "GET";
		$uris = parse_uri($a["href"], $uris);
		$query = "";
		$cookie = saisoncard_update_cookie($head, $cookie);
		list($head, $body) = saisoncard_http11($method, $uris, $query, $cookie);
		break;
	}
	
	$creditcardmsgsrsv1 = "";
	$creditcardmsgsrsv1 .= "<CREDITCARDMSGSRSV1>";
	$creditcardmsgsrsv1 .= "\r\n";
	
	$creditcardmsgsrsv1 .= "<CCSTMTTRNRS>";
	$creditcardmsgsrsv1 .= "\r\n";
	$creditcardmsgsrsv1 .= "<TRNUID>0</TRNUID>";
	$creditcardmsgsrsv1 .= "\r\n";
	$creditcardmsgsrsv1 .= "<STATUS><CODE>0</CODE><SEVERITY>INFO</SEVERITY></STATUS>";
	$creditcardmsgsrsv1 .= "\r\n";
	$creditcardmsgsrsv1 .= "<CCSTMTRS>";
	$creditcardmsgsrsv1 .= "\r\n";
	$creditcardmsgsrsv1 .= "<CURDEF>" . ENV_STR_OFX_CURRENCY_JPY . "</CURDEF>";
	$creditcardmsgsrsv1 .= "\r\n";
	$creditcardmsgsrsv1 .= "<CCACCTFROM>";
	$creditcardmsgsrsv1 .= "<ACCTID>" . $account["acctid"] . "</ACCTID>";
	$creditcardmsgsrsv1 .= "</CCACCTFROM>";
	$creditcardmsgsrsv1 .= "\r\n";
	$creditcardmsgsrsv1 .= saisoncard_parse_csv($body, $account);
	$creditcardmsgsrsv1 .= "</CCSTMTRS>";
	$creditcardmsgsrsv1 .= "\r\n";
	$creditcardmsgsrsv1 .= "</CCSTMTTRNRS>";
	$creditcardmsgsrsv1 .= "\r\n";
	
	$creditcardmsgsrsv1 .= "</CREDITCARDMSGSRSV1>";
	$creditcardmsgsrsv1 .= "\r\n";
	
	$body = $body_old;
	
	// ���s���ԁi�^�C���A�E�g�j���Đݒ肷��
	@set_time_limit(ENV_NUM_TIMEOUT);
	
	// ���O�A�E�g����
	$as = parse_tag($body, "a");
	foreach($as as $a) if(parse_tag_search(parse_tag($a["innerHTML"], "img"), "alt", "���O�A�E�g") != -1) {
		$method = "GET";
		$uris = parse_uri($a["href"], $uris);
		$query = "";
		$cookie = saisoncard_update_cookie($head, $cookie);
		list($head, $body) = saisoncard_http11($method, $uris, $query, $cookie);
		break;
	}
	
	// OFX�t�@�C�����o�͂���
	$resp["status"] = ENV_NUM_STATUS_SUCCESS;
	if(strpos($creditcardmsgsrsv1, "<BANKTRANLIST>") === false) {
		// ���ׂ����݂��Ȃ��ꍇ
		$resp["ofx"] = generate_ofx($resp["status"]);
	} else {
		// ���ׂ����݂���ꍇ
		$resp["ofx"] = generate_ofx($resp["status"], $creditcardmsgsrsv1);
	}
}
return $resp;

// HTTP/1.1
function saisoncard_http11($method, $uris, $query = "", $cookie = "") {
	$ret = "INVALID HOST";
	if(preg_match("/\.saisoncard\.co\.jp$/", $uris["host"]) > 0) $ret = http11(strtoupper($method), $uris["scheme"], $uris["host"], 0, $uris["path"], $query, "", $cookie);
	return explode("\r\n\r\n", $ret, 2);
}

function saisoncard_update_cookie($head, $cookie) {
	return update_cookie(array("JSESSIONID"), parse_header($head, "set-cookie"), $cookie);
}

function saisoncard_parse_csv($str, $account) {
	$ret = "";
	$lines = parse_csv($str);
	$cds = array();
	$cds_balamt = "0";
	$cds_paydate = $account["paydate"];
	$cds_s = "";
	$cds_e = "";
	$cd_date = "";
	$cd_num = 0;
	$ledge_balamt = 0;
	
	foreach($lines as $line) {
		$cd = array();
		
		if(count($line) == 2) {
			switch($line[0]) {
			case "���x����":
				$cds_paydate = parse_date($line[1]);
				break;
			case "���񂲐����z":
				$cds_balamt = (string)(double)$line[1];
				$ledge_balamt = (double)$cds_balamt;
				break;
			case "�J�[�h����":
			default:
				break;
			}
		} else if(count($line) == 7 && $line[0] != "���p��") {
			// PAYMENT�Œ�Ƃ���
			$cd["TRNTYPE"] = ENV_STR_OFX_TRNTYPE_PAYMENT;
			
			// ���t���擾����
			$cd["DTPOSTED"] = parse_date($line[0]);
			if($cds_s == "") $cds_s = $cd["DTPOSTED"];
			$cds_e = $cd["DTPOSTED"];
			
			// �ʔԂ𐶐�����
			if($cd["DTPOSTED"] == $cd_date) $cd_num++; else $cd_num = 0;
			
			// �g�����U�N�V�����ԍ��𐶐�����
			$cd["FITID"] = $cd["DTPOSTED"] . sprintf("%04d", $account["id"]) . substr($account["paydate"], 4, 2) . "0" . sprintf("%05d", $cd_num);
			
			// �E�v���擾����
			$cd["NAME"] = mb_convert_string($line[1]);
			
			// ���z���擾����
			$cd["TRNAMT"] = (string)(-1 * (double)parse_amount($line[5]));
			$ledge_balamt += (double)$cd["TRNAMT"];
			
			// �c�����擾����
			$cd["MEMO"] = ($line[6] != ""? mb_convert_string($line[6]): ENV_STR_OFX_MEMO);
			
			array_push($cds, $cd);
			$cd_date = $cd["DTPOSTED"];
		}
	}
	
	if($cds_s == "") $cds_s = ENV_STR_DATE_TODAY;
	if($cds_e == "") $cds_e = ENV_STR_DATE_TODAY;
	if($cds_s > $cds_e) $cds_e = $cds_s;
	
	// �N���W�b�g�J�[�h�x�������𖾍ׂɒǉ�����
	$i = count($cds);
	$cds[$i]["DTPOSTED"] = $cds_paydate;
	$cds[$i]["NAME"] = mb_convert_string($account["name"]);
	$cds[$i]["MEMO"] = ENV_STR_OFX_MEMO;
	$cds[$i]["TRNAMT"] = $cds_balamt;
	$cds[$i]["TRNTYPE"] = ENV_STR_OFX_TRNTYPE_DIRECTDEBIT;
	$cds[$i]["FITID"] = $cds[$i]["DTPOSTED"] . sprintf("%04d", $account["id"]) . substr($account["paydate"], 4, 2) . "100000";
	
	// BANKTRANLIST
	$ret .= "<BANKTRANLIST>";
	$ret .= "\r\n";
	$ret .= "<DTSTART>" . $cds_s . ENV_STR_OFX_TZ . "</DTSTART>";
	$ret .= "<DTEND>" . $cds_e . ENV_STR_OFX_TZ . "</DTEND>";
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
	
	// �x����c�����o�͂���
	$ret .= "<LEDGERBAL>";
	$ret .= "<BALAMT>" . (string)$ledge_balamt . "</BALAMT>";
	$ret .= "<DTASOF>" . ENV_STR_DATE_TODAY . ENV_STR_OFX_TZ . "</DTASOF>";
	$ret .= "</LEDGERBAL>";
	$ret .= "\r\n";
	
	// �J�[�h���̂��o�͂���
	if($account["acctname"] != "") {
		$ret .= "<MKTGINFO>" . mb_convert_string($account["acctname"]) . "</MKTGINFO>";
		$ret .= "\r\n";
	}
	
	return $ret;
}

?>