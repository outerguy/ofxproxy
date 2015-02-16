<?php
/*
chocom.inc: ����R��e�}�l�[��HTML���OFX�t�@�C���𐶐�����
Copyright (C) 2012-2015 OFFICE OUTERGUY. All rights reserved.
mailto:contact@beatrek.com
Licensed under the GNU AGPLv3.
*/

// ���O�C�������擾����
$user = $ofxforms["MAILADDR"];
$tel = str_replace("-", "", $ofxforms["TELEPHONE"]);
$pass = $ofxforms["PASSWORD"];

$resp = array();
$ofx = "";

// ���s���ԁi�^�C���A�E�g�j���Đݒ肷��
@set_time_limit(ENV_NUM_TIMEOUT);

// �z�[����ʂ��擾����
$method = "GET";
$uris = parse_uri($settings["home"]);
$query = "";
$cookie = "";
list($head, $body) = chocom_http11($method, $uris, $query, $cookie);

// ���_�C���N�g����
$retry = 0;
while($retry++ < ENV_NUM_HTTP11_RETRY) {
	$locations = parse_header($head, "location");
	if(count($locations) > 0) {
		$method = "GET";
		$uris = parse_uri($locations[0], $uris);
		$query = "";
		$cookie = chocom_update_cookie($head, $cookie);
		list($head, $body) = chocom_http11($method, $uris, $query, $cookie);
	}
}

// ���O�C����ʂ��擾����
$as = parse_tag($body, "a");
foreach($as as $a) if(parse_tag_search(parse_tag($a["innerHTML"], "img"), "alt", "������O�C��") != -1) {
	$method = "GET";
	$uris = parse_uri($a["href"], $uris);
	$query = "";
	$cookie = chocom_update_cookie($head, $cookie);
	list($head, $body) = chocom_http11($method, $uris, $query, $cookie);
	break;
}

// ���_�C���N�g����
$scripts = parse_tag($body, "script");
foreach($scripts as $script) if(preg_match("/location\.replace\(\'(.*?)\'/i", $script["innerHTML"], $matches) > 0) {
	$method = "GET";
	$uris = parse_uri($matches[1], $uris);
	$query = "";
	$cookie = chocom_update_cookie($head, $cookie);
	list($head, $body) = chocom_http11($method, $uris, $query, $cookie);
	break;
}

// ���O�C������
$forms = parse_tag($body, "form");
$c = parse_tag_search($forms, "name", "form1");
if($c != -1) {
	$inputs = parse_tag($forms[$c]["innerHTML"], "input");
	$queries = array();
	foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
	$queries["MAILADDR"] = "MAILADDR=" . $user;
	$queries["TELEPHONE"] = "TELEPHONE=" . $tel;
	$queries["PASSWORD"] = "PASSWORD=" . $pass;
	
	$method = $forms[$c]["method"];
	$uris = parse_uri($forms[$c]["action"], $uris);
	$query = implode("&", $queries);
	$cookie = chocom_update_cookie($head, $cookie);
	list($head, $body) = chocom_http11($method, $uris, $query, $cookie);
}

if(strpos($body, "���O�C�����邱�Ƃ��o���܂���ł���") !== false) {
	// �V�X�e�������e�i���X��ʂ̏ꍇ
	$resp["status"] = ENV_NUM_STATUS_MAINTENANCE;
	$resp["method"] = $method;
	$resp["uri"] = $uris["scheme"] . "://" . $uris["host"] . $uris["path"];
	$resp["query"] = $query;
	$resp["cookie"] = $cookie;
	$resp["head"] = $head;
	$resp["body"] = $body;
	$resp["ofx"] = generate_ofx($resp["status"]);
} else if(strpos($body, "�����p����") === false) {
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
	// ���O�A�E�g��ʂ�ޔ�����
	$forms = parse_tag($body, "form");
	$c = parse_tag_search($forms, "name", "clickChecker");
	if($c != -1) {
		$inputs = parse_tag($forms[$c]["innerHTML"], "input");
		$queries = array();
		foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
		$queries["clickCounter"] = "clickCounter=1";
		
		$method_old = $forms[$c]["method"];
		$uris_old = parse_uri($forms[$c]["action"], $uris);
		$query_old = implode("&", $queries);
	}
	
	$account = array();
	
	$account["acctname"] = $settings["name"];
	
	// �x�X�ԍ����擾����
	if(preg_match("/" . preg_quote("�������ԍ�") . "[\s\t]*([0-9]+)/", $body, $matches) > 0) $account["branchid"] = $matches[1];
	
	// �����ԍ����擾����
	if(preg_match("/\.Com\-ID[\s\t]*([0-9]{4}\-[0-9]{4}\-[0-9]{4}\-[0-9]{4}\-[0-9]{4}\-[0-9])/", $body, $matches) > 0) $account["acctid"] = str_replace("-", "", $matches[1]);
	
	// �c�����擾����
	if(preg_match("/" . preg_quote("�������c��") . "[\s\t]*:[\s\t]*([0-9]+)/", $body, $matches) > 0) $account["balance"] = parse_amount($matches[1]);
	
	// �����c���Ɖ��ʂ��擾����
	$as = parse_tag($body, "a");
	$c = parse_tag_search($as, "innerHTML", "�����Ɖ�");
	if($c != -1) {
		$method = "GET";
		$uris = parse_uri($as[$c]["href"], $uris);
		$query = "";
		$cookie = chocom_update_cookie($head, $cookie);
		list($head, $body) = chocom_http11($method, $uris, $query, $cookie, false); // �����R�[�h��ϊ����Ȃ�
	}
	
	// ����\����ʁi1�y�[�W�ځj���擾����
	$forms = parse_tag($body, "form");
	$c = parse_tag_search($forms, "name", "sForm");
	if($c != -1) {
		$queries = array();
		$inputs = parse_tag($forms[$c]["innerHTML"], "input");
		foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
		$selects = parse_tag($forms[$c]["innerHTML"], "select");
		foreach($selects as $select) {
			$options = parse_tag($select["innerHTML"], "option");
			foreach($options as $option) if($select["name"] != "" && $option["selected"] == "selected") $queries[$select["name"]] = urlencode($select["name"]) . "=" . urlencode($option["value"]);
		}
		$queries["yearStart"] = "yearStart=" . (string)(integer)substr(ENV_STR_DATE_PASTDAY, 0, 4);
		$queries["monthStart"] = "monthStart=" . (string)(integer)substr(ENV_STR_DATE_PASTDAY, 4, 2);
		$queries["dayStart"] = "dayStart=" . (string)(integer)substr(ENV_STR_DATE_PASTDAY, 6, 2);
		$queries["yearFinish"] = "yearFinish=" . (string)(integer)substr(ENV_STR_DATE_TODAY, 0, 4);
		$queries["monthFinish"] = "monthFinish=" . (string)(integer)substr(ENV_STR_DATE_TODAY, 4, 2);
		$queries["dayFinish"] = "dayFinish=" . (string)(integer)substr(ENV_STR_DATE_TODAY, 6, 2);
		
		$method = $forms[$c]["method"];
		$uris = parse_uri($forms[$c]["action"], $uris);
		$query = implode("&", $queries);
		$cookie = chocom_update_cookie($head, $cookie);
		list($head, $body) = chocom_http11($method, $uris, $query, $cookie, false); // �����R�[�h��ϊ����Ȃ�
	}
	
	$account["details"] = chocom_get_details($body);
	
	// ����\����ʁi2�y�[�W�ڈȍ~�j���擾����
	while(parse_tag_search(parse_tag($body, "a"), "innerHTML", "�ȑO�̗���") != -1) {
		$forms = parse_tag($body, "form");
		$c = parse_tag_search($forms, "name", "hisForm");
		if($c != -1) {
			$inputs = parse_tag($forms[$c]["innerHTML"], "input");
			$queries = array();
			foreach($inputs as $input) {
				switch($input["name"]) {
				case "":
					// �������Ȃ�
					break;
				case "E2BURL":
					$forms[$c]["action"] = $input["value"];
					// break���Ȃ�
				default:
					$queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
					break;
				}
			}
			$queries["REQ_PAGE"] = "REQ_PAGE=NEXT";
			
			$method = $forms[$c]["method"];
			$uris = parse_uri($forms[$c]["action"], $uris);
			$query = implode("&", $queries);
			$cookie = chocom_update_cookie($head, $cookie);
			list($head, $body) = chocom_http11($method, $uris, $query, $cookie, false); // �����R�[�h��ϊ����Ȃ�
			
			$account["details"] = array_merge(chocom_get_details($body), $account["details"]);
		}
	}
	
	// ���s���ԁi�^�C���A�E�g�j���Đݒ肷��
	@set_time_limit(ENV_NUM_TIMEOUT);
	
	// ���O�A�E�g����
	$method = $method_old;
	$uris = $uris_old;
	$query = $query_old;
	$cookie = chocom_update_cookie($head, $cookie);
	list($head, $body) = chocom_http11($method, $uris, $query, $cookie);
	
	// ���_�C���N�g����
	$retry = 0;
	while($retry++ < ENV_NUM_HTTP11_RETRY) {
		$locations = parse_header($head, "location");
		if(count($locations) > 0) {
			$method = "GET";
			$uris = parse_uri($locations[0], $uris);
			$query = "";
			$cookie = chocom_update_cookie($head, $cookie);
			list($head, $body) = chocom_http11($method, $uris, $query, $cookie);
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
	$bankmsgsrsv1 .= chocom_parse_details($account);
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
function chocom_http11($method, $uris, $query = "", $cookie = "", $autoconv = true) {
	$ret = "INVALID HOST";
	if(preg_match("/\.chocom\.(?:jp|net)$/", $uris["host"]) > 0) $ret = http11(strtoupper($method), $uris["scheme"], $uris["host"], 0, $uris["path"], $query, "", $cookie, $autoconv);
	return explode("\r\n\r\n", $ret, 2);
}

function chocom_update_cookie($head, $cookie) {
	return update_cookie(array("AlteonP", "JSESSIONID"), parse_header($head, "set-cookie"), $cookie);
}

function chocom_get_details($body) {
	$rets = array();
	$i = 0;
	$trs = parse_tag($body, "tr");
	foreach($trs as $tr) {
		$tds = parse_tag($tr["innerHTML"], "td");
		if(count($tds) == 6) {
			// ���p���t���擾����
			$rets[$i]["date"] = parse_date(trim(strip_tags($tds[0]["innerHTML"])));
			
			// �����p����擾����
			$name1 = str_replace("&nbsp;", "", trim(strip_tags($tds[1]["innerHTML"])));
			$name2 = str_replace("&nbsp;", "", trim(strip_tags($tds[3]["innerHTML"])));
			$rets[$i]["summary"] = ($name1 == "" || $name2 == ""? $name1 . $name2: implode(ENV_CHR_CONCATENATOR, array($name1, $name2)));
			
			// �����p���z���擾����
			$rets[$i]["amount"] = parse_amount($tds[4]["innerHTML"]);
			
			$i++;
		}
	}
	
	return $rets;
}

function chocom_parse_details($account) {
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
		$cd["MEMO"] = ENV_STR_OFX_MEMO;
		
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