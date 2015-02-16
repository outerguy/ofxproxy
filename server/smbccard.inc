<?php
/*
smbccard.inc: �O��Z�F�J�[�h��CSV���OFX�t�@�C���𐶐�����
Copyright (C) 2012-2015 OFFICE OUTERGUY. All rights reserved.
mailto:contact@beatrek.com
Licensed under the GNU AGPLv3.
*/

// ���O�C�������擾����
$user = $ofxforms["userid"];
$pass = $ofxforms["password"];

$resp = array();

// ���s���ԁi�^�C���A�E�g�j���Đݒ肷��
@set_time_limit(ENV_NUM_TIMEOUT);

// �z�[����ʂ��擾����
$method = "GET";
$uris = parse_uri($settings["home"]);
$query = "";
$cookie = "";
list($head, $body) = smbccard_http11($method, $uris, $query, $cookie);

// ���_�C���N�g����
$retry = 0;
while($retry++ < ENV_NUM_HTTP11_RETRY) {
	$locations = parse_header($head, "location");
	if(count($locations) > 0) {
		$method = "GET";
		$uris = parse_uri($locations[0], $uris);
		$query = "";
		$cookie = smbccard_update_cookie($head, $cookie);
		list($head, $body) = smbccard_http11($method, $uris, $query, $cookie);
	}
}

// ���O�C����ʂ��擾����
$as = parse_tag($body, "a");

foreach($as as $a) if(strip_tags($a["innerHTML"]) == "�J�[�h����̕� Vpass���O�C��") {
	$method = "GET";
	$uris = parse_uri($a["href"], $uris);
	$query = "";
	$cookie = smbccard_update_cookie($head, $cookie);
	list($head, $body) = smbccard_http11($method, $uris, $query, $cookie);
	break;
}

// ���O�C������
$forms = parse_tag($body, "form");
$c = parse_tag_search($forms, "name", "InForm");
if($c != -1) {
	$inputs = parse_tag($forms[$c]["innerHTML"], "input");
	$queries = array();
	foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
	$queries["userid"] = "userid=" . $user;
	$queries["password"] = "password=" . $pass;
	
	$method = $forms[$c]["method"];
	$uris = parse_uri($forms[$c]["action"], $uris);
	$query = implode("&", $queries);
	$cookie = smbccard_update_cookie($head, $cookie);
	list($head, $body) = smbccard_http11($method, $uris, $query, $cookie);
}

// ���_�C���N�g����
$retry = 0;
while($retry++ < ENV_NUM_HTTP11_RETRY) {
	$locations = parse_header($head, "location");
	if(count($locations) > 0) {
		$method = "GET";
		$uris = parse_uri($locations[0], $uris);
		$query = "";
		$cookie = smbccard_update_cookie($head, $cookie);
		list($head, $body) = smbccard_http11($method, $uris, $query, $cookie);
	}
}

if(strpos($body, "�V�X�e�������e�i���X��") !== false) {
	// �V�X�e�������e�i���X��ʂ̏ꍇ
	$resp["status"] = ENV_NUM_STATUS_MAINTENANCE;
	$resp["method"] = $method;
	$resp["uri"] = $uris["scheme"] . "://" . $uris["host"] . $uris["path"];
	$resp["query"] = $query;
	$resp["cookie"] = $cookie;
	$resp["head"] = $head;
	$resp["body"] = $body;
	$resp["ofx"] = generate_ofx($resp["status"]);
} else if(strpos($body, "���O�C����") === false) {
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
	$account["flag"] = false;
	$account["paydate"] = "00000000";
	$account["balamt"] = "0";
	
	// ���s���ԁi�^�C���A�E�g�j���Đݒ肷��
	@set_time_limit(ENV_NUM_TIMEOUT);
	
	if(strpos($body, "�p�X���[�h�ύX�Ɋւ��邨�肢") !== false) {
		// �p�X���[�h�ύX�Ɋւ��邨�肢��ʂ̏ꍇ�A���O�C�����@�J�[�h����̕�TOP��ʂ��擾����
		$as = parse_tag($body, "a");
		foreach($as as $a) if(parse_tag_search(parse_tag($a["innerHTML"], "img"), "alt", "���O�C�����@�J�[�h����̕�TOP") != -1) {
			$method = "GET";
			$uris = parse_uri($a["href"], $uris);
			$query = "";
			$cookie = smbccard_update_cookie($head, $cookie);
			list($head, $body) = smbccard_http11($method, $uris, $query, $cookie);
			break;
		}
	}
	
	// �J�[�h���̂��擾����
	$fonts = parse_tag($body, "font");
	$c = parse_tag_search($fonts, "color", "#006633");
	if($c != -1) {
		$account["acctname"] = implode(ENV_CHR_CONCATENATOR, array($settings["name"], trim(strip_tags($fonts[$c]["innerHTML"]))));
		$account["name"] = $settings["name"];
	}
	
	// �����p���׉�ʂ��擾����
	// �ŐV���̊m��ς݂̖��ׂ�0���̏ꍇ�A�J�[�h�ԍ����擾�ł��Ȃ����߁A�f�t�H���g�ŕ\�������i�ŐV���ȑO�Ŗ��ׂ�1���ȏ゠�錎�́j�����p���׉�ʂ��擾����
	$as = parse_tag($body, "a");
	foreach($as as $a) if(parse_tag_search(parse_tag($a["innerHTML"], "img"), "alt", "�����p���ׂ�����") != -1) {
		$method = "GET";
		$uris = parse_uri($a["href"], $uris);
		$query = "";
		$cookie = smbccard_update_cookie($head, $cookie);
		list($head, $body) = smbccard_http11($method, $uris, $query, $cookie);
		break;
	}
	
	// �J�[�h�ԍ����擾����
	$trs = parse_tag($body, "tr", true); // �ċA�I�Ɏ擾����
	foreach($trs as $tr) {
		$tds = parse_tag($tr["innerHTML"], "td");
		if(count($tds) == 2 && $tds[0]["class"] == "sdbc2") {
			$account["acctid"] = trim(strip_tags($tds[0]["innerHTML"]));
			break;
		}
	}
	if($account["acctid"] == "") $account["acctid"] = $user;
	
	// ���x�����ɂ��ĉ�ʂ��擾����
	$as = parse_tag($body, "a");
	foreach($as as $a) if(parse_tag_search(parse_tag($a["innerHTML"], "img"), "alt", "���x�����ɂ���") != -1) {
		$method = "GET";
		$uris = parse_uri($a["href"], $uris);
		$query = "";
		$cookie = smbccard_update_cookie($head, $cookie);
		list($head, $body) = smbccard_http11($method, $uris, $query, $cookie);
		break;
	}
	
	// ���x�������z�Ɖ��ʂ��擾����
	$as = parse_tag($body, "a");
	$c = parse_tag_search($as, "innerHTML", "���x�������z�Ɖ�");
	if($c != -1) {
		$method = "GET";
		$uris = parse_uri($as[$c]["href"], $uris);
		$query = "";
		$cookie = smbccard_update_cookie($head, $cookie);
		list($head, $body) = smbccard_http11($method, $uris, $query, $cookie);
	}
	
	// �i�ŐV���̊m��ς݂́j�����p���׉�ʂ��擾����
	$trs = parse_tag($body, "tr", true); // �ċA�I�Ɏ擾����
	$uri = "";
	foreach($trs as $tr) {
		$tds = parse_tag($tr["innerHTML"], "td");
		if(count($tds) == 3 && strpos($tds[0]["innerHTML"], "�m�蕪") !== false) {
			// ���x�������擾����
			if(preg_match("/([0-9]{1,2})" . preg_quote("��") . "([0-9]{1,2})" . preg_quote("�����x�����@�m�蕪") . "/", trim(strip_tags($tds[0]["innerHTML"])), $matches) > 0) {
				$matches[1] = sprintf("%02d", (integer)$matches[1]);
				$matches[2] = sprintf("%02d", (integer)$matches[2]);
				$account["paydate"] = (date("m") <= $matches[1]? date("Y"): (string)((integer)date("Y") + 1)) . $matches[1] . $matches[2];
			}
			
			// ���x�����v�z���擾����
			$account["balamt"] = parse_amount(strip_tags($tds[1]["innerHTML"]));
			
			$as = parse_tag($tds[2]["innerHTML"], "a");
			if(count($as) > 0 && parse_tag_search(parse_tag($as[0]["innerHTML"], "img"), "alt", "") != -1) $uri = $as[0]["href"];
			// break���Ȃ�
		}
	}
	if($uri != "") {
		$method = "GET";
		$uris = parse_uri($uri, $uris);
		$query = "";
		$cookie = smbccard_update_cookie($head, $cookie);
		list($head, $body) = smbccard_http11($method, $uris, $query, $cookie);
	}
	
	$body_old = $body;
	
	// CSV�t�@�C�����_�E�����[�h����
	$as = parse_tag($body, "a");
	$c = parse_tag_search($as, "innerHTML", "CSV�`���ŕۑ�");
	if($c != -1) {
		$method = "GET";
		$uris = parse_uri($as[$c]["href"], $uris);
		$query = "";
		$cookie = smbccard_update_cookie($head, $cookie);
		list($head, $body) = smbccard_http11($method, $uris, $query, $cookie);
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
	$creditcardmsgsrsv1 .= smbccard_parse_csv($body, $account); // 0�Œ�H�i�v�����j
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
		$cookie = smbccard_update_cookie($head, $cookie);
		list($head, $body) = smbccard_http11($method, $uris, $query, $cookie);
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
			$cookie = smbccard_update_cookie($head, $cookie);
			list($head, $body) = smbccard_http11($method, $uris, $query, $cookie);
		}
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
function smbccard_http11($method, $uris, $query = "", $cookie = "") {
	$ret = "INVALID HOST";
	if(preg_match("/\.smbc-card\.com$/", $uris["host"]) > 0) $ret = http11(strtoupper($method), $uris["scheme"], $uris["host"], 0, $uris["path"], $query, "", $cookie);
	return explode("\r\n\r\n", $ret, 2);
}

function smbccard_update_cookie($head, $cookie) {
	return update_cookie(array("vpasssession", "pass", "ch"), parse_header($head, "set-cookie"), $cookie);
}

function smbccard_parse_csv($str, $account) {
	$ret = "";
	$lines = array_reverse(parse_csv($str));
	$cds = array();
	$cds_balamt = "0";
	$cds_s = "";
	$cds_e = "";
	$cd_date = "";
	$cd_num = 0;
	
	// ���񂨎x�����z���擾����
	$ledge_balamt = (double)$account["balamt"];
	
	foreach($lines as $line) {
		$cd = array();
		
		if(count($line) == 7 && $line[0] != "") {
			// WEB���׏�CSV�̏ꍇ
			
			// PAYMENT�Œ�Ƃ���
			$cd["TRNTYPE"] = ENV_STR_OFX_TRNTYPE_PAYMENT;
			
			// ���t���擾����
			$cd["DTPOSTED"] = parse_date($line[0]);
			if($cds_s == "") $cds_s = $cd["DTPOSTED"];
			$cds_e = $cd["DTPOSTED"];
			
			// �ʔԂ𐶐�����
			if($cd["DTPOSTED"] == $cd_date) $cd_num++; else $cd_num = 0;
			
			// �g�����U�N�V�����ԍ��𐶐�����
			$cd["FITID"] = $cd["DTPOSTED"] . "0000" . substr($account["paydate"], 4, 2) . "0" . sprintf("%05d", $cd_num);
			
			// �E�v���擾����
			$cd["NAME"] = mb_convert_string($line[1]);
			
			// ���z���擾����
			$cd["TRNAMT"] = (string)(-1 * (double)parse_amount($line[5]));
			$ledge_balamt += (double)$cd["TRNAMT"];
			
			// �c�����擾����
			$cd["MEMO"] = ($line[7] != ""? mb_convert_string($line[7]): ENV_STR_OFX_MEMO);
			
			array_push($cds, $cd);
			$cd_date = $cd["DTPOSTED"];
		} else if(count($line) == 13) {
			// �����p���׏Ɖ�CSV�̏ꍇ
			
			// PAYMENT�Œ�Ƃ���
			$cd["TRNTYPE"] = ENV_STR_OFX_TRNTYPE_PAYMENT;
			
			// ���t���擾����
			$cd["DTPOSTED"] = parse_date($line[0]);
			if($cds_s == "") $cds_s = $cd["DTPOSTED"];
			$cds_e = $cd["DTPOSTED"];
			
			// �ʔԂ𐶐�����
			if($cd["DTPOSTED"] == $cd_date) $cd_num++; else $cd_num = 0;
			
			// �g�����U�N�V�����ԍ��𐶐�����
			$cd["FITID"] = $cd["DTPOSTED"] . "0000" . substr($account["paydate"], 4, 2) . "0" . sprintf("%05d", $cd_num);
			
			// �E�v���擾����
			$cd["NAME"] = mb_convert_string($line[1]);
			
			// ���z���擾����
			$cd["TRNAMT"] = (string)(-1 * (double)parse_amount($line[6]));
			$ledge_balamt += (double)$cd["TRNAMT"];
			
			// �c�����擾����
			$cd["MEMO"] = ($line[3] != ""? mb_convert_string($line[3]): ENV_STR_OFX_MEMO);
			
			array_push($cds, $cd);
			$cd_date = $cd["DTPOSTED"];
		}
	}
	
	if($cds_s == "") $cds_s = ENV_STR_DATE_TODAY;
	if($cds_e == "") $cds_e = ENV_STR_DATE_TODAY;
	if($cds_s > $cds_e) $cds_e = $cds_s;
	
	// �N���W�b�g�J�[�h�x�������𖾍ׂɒǉ�����
	$i = count($cds);
	$cds[$i]["DTPOSTED"] = $account["paydate"];
	$cds[$i]["NAME"] = mb_convert_string($account["name"]);
	$cds[$i]["MEMO"] = ENV_STR_OFX_MEMO;
	$cds[$i]["TRNAMT"] = $account["balamt"];
	$cds[$i]["TRNTYPE"] = ENV_STR_OFX_TRNTYPE_DIRECTDEBIT;
	$cds[$i]["FITID"] = $cds[$i]["DTPOSTED"] . "0000" . substr($account["paydate"], 4, 2) . "100000";
	
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