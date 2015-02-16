<?php
/*
mobilesuica.inc: ���o�C��Suica��HTML���OFX�t�@�C���𐶐�����
Copyright (C) 2014-2015 OFFICE OUTERGUY. All rights reserved.
mailto:contact@beatrek.com
Licensed under the GNU AGPLv3.
*/

// ���O�C�������擾����
$user = $ofxforms["MailAddress"];
$pass = $ofxforms["Password"];

// ���s���ԁi�^�C���A�E�g�j���Đݒ肷��
@set_time_limit(ENV_NUM_TIMEOUT);

// �z�[����ʁi���O�C����ʁj���擾����
$method = "GET";
$uris = parse_uri($settings["home"]);
$query = "";
$cookie = "";
list($head, $body) = mobilesuica_http11($method, $uris, $query, $cookie);

// ���O�C������
$forms = parse_tag($body, "form");
$c = parse_tag_search($forms, "name", "form1");
if($c != -1) {
	$inputs = parse_tag($forms[$c]["innerHTML"], "input");
	$queries = array();
	foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
	$queries["MailAddress"] = "MailAddress=" . $user;
	$queries["Password"] = "Password=" . $pass;
	
	$method = $forms[$c]["method"];
	$uris = parse_uri($forms[$c]["action"], $uris);
	$query = implode("&", $queries);
	$cookie = mobilesuica_update_cookie($head, $cookie);
	list($head, $body) = mobilesuica_http11($method, $uris, $query, $cookie);
}

if(strpos($body, "SF(�d�q�}�l�[)���p����") === false) {
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
	$account["balance"] = 0;
	
	// SF�i�d�q�}�l�[�j���p������ʂ��擾����
	$as = parse_tag($body, "a");
	foreach($as as $a) if(parse_tag_search(parse_tag($a["innerHTML"], "img"), "alt", "SF(�d�q�}�l�[)���p����") != -1) {
		$method = "POST";
		if(preg_match("/javascript:[^\']+?\'([^\']+)\'.*?/i", $a["href"], $matches) > 0) $uris = parse_uri($matches[1], $uris);
		$query = "";
		$cookie = mobilesuica_update_cookie($head, $cookie);
		list($head, $body) = mobilesuica_http11($method, $uris, $query, $cookie);
		break;
	}
	
	$account["details"] = array();
	$tables = parse_tag($body, "table", true);
	foreach($tables as $table) if($table["cellpadding"] == "8") {
		$account["details"] = mobilesuica_get_details($table["innerHTML"]);
		$detail = array_shift($account["details"]);
		$account["balance"] = $detail["amount"];
	}
	
	// SuicaID�ԍ����擾����
	$forms = parse_tag($body, "form", true);
	$c = parse_tag_search($forms, "name", "form1");
	if($c != -1) {
		$inputs = parse_tag($forms[$c]["innerHTML"], "input");
		$queries = array();
		foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
		$selects = parse_tag($forms[$c]["innerHTML"], "select");
		foreach($selects as $select) {
			$options = parse_tag($select["innerHTML"], "option");
			foreach($options as $option) if($select["name"] != "" && $option["selected"] == "selected") $queries[$select["name"]] = urlencode($select["name"]) . "=" . urlencode($option["value"]);
		}
		$queries["baseVarCopy"] = "baseVarCopy=" . substr($queries["baseVariable"], strpos($queries["baseVariable"], "=") + 1);
		if($queries["SEARCH"] != "") unset($queries["SEARCH"]);
		if($queries["RETURNMENU"] != "") unset($queries["RETURNMENU"]);
		
		$method = $forms[$c]["method"];
		$uris = parse_uri($forms[$c]["action"], $uris);
		$query = implode("&", $queries);
		$cookie = mobilesuica_update_cookie($head, $cookie);
		list($head, $body) = mobilesuica_http11($method, $uris, $query, $cookie);
		
		$contentdispositions = parse_header($head, "content-disposition");
		if(count($contentdispositions) > 0) {
			$account["acctid"] = substr($contentdispositions[0], strpos($contentdispositions[0], "=") + 1, 17);
		}
	
	}
	
	// ���s���ԁi�^�C���A�E�g�j���Đݒ肷��
	@set_time_limit(ENV_NUM_TIMEOUT);
	
	// ���O�A�E�g����
	$method = "POST";
	$uris = parse_uri("/ka/lg/LogoutComplete.aspx", $uris);
	$query = "";
	$cookie = mobilesuica_update_cookie($head, $cookie);
	list($head, $body) = mobilesuica_http11($method, $uris, $query, $cookie);
	
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
	$bankmsgsrsv1 .= mobilesuica_parse_details($account);
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
function mobilesuica_http11($method, $uris, $query = "", $cookie = "") {
	$ret = "INVALID HOST";
	if(preg_match("/\.mobilesuica\.com$/", $uris["host"]) > 0) $ret = http11(strtoupper($method), $uris["scheme"], $uris["host"], 0, $uris["path"], $query, "", $cookie, true, ENV_PRODUCT_UA . " MSIE 7.0"); // ���O�C������User-Agent���`�F�b�N�����
	return explode("\r\n\r\n", $ret, 2);
}

function mobilesuica_update_cookie($head, $cookie) {
	return update_cookie(array("ASP.NET_SessionId", "sc_auth"), parse_header($head, "set-cookie"), $cookie);
}

function mobilesuica_get_details($body) {
	$rets = array();
	$trs = parse_tag($body, "tr");
	array_shift($trs);
	$i = count($trs) - 1;
	$prev = "";
	$last = "";
	foreach($trs as $tr) {
		$tds = parse_tag($tr["innerHTML"], "td");
		if(count($tds) == 7) {
			// �������擾����
			$dt = trim(str_replace("/", "", strip_tags($tds[0]["innerHTML"])));
			if($dt > substr(ENV_STR_DATE_TODAY, 4, 4)) {
				$dt = (string)((integer)substr(ENV_STR_DATE_TODAY, 0, 4) - 1) . $dt;
			} else {
				$dt = substr(ENV_STR_DATE_TODAY, 0, 4) . $dt;
			}
			
			// �����ʁA����яꏊ���擾����
			$class1 = mobilesuica_parse_string($tds[1]["innerHTML"]);
			$place1 = mobilesuica_parse_string($tds[2]["innerHTML"]);
			$class2 = mobilesuica_parse_string($tds[3]["innerHTML"]);
			$place2 = mobilesuica_parse_string($tds[4]["innerHTML"]);
			
			$name = $class1 . $class2;
			$memo = $place1;
			if($place2 != "") $memo .= "-" . $place2;
			if($memo == "") $memo = ENV_STR_OFX_MEMO;
			
			// ���x���v�Z����
			$amount = mobilesuica_parse_string($tds[6]["innerHTML"]);
			
			// �c�����擾����
			if($last == "") $last = parse_amount(str_replace("&yen;", "", trim(strip_tags($tds[5]["innerHTML"]))));
			
			$rets[$i]["date"] = $dt;
			$rets[$i]["summary"] = $name;
			$rets[$i]["amount"] = $amount;
			$rets[$i]["memo"] = $memo;
			
			$i--;
		}
	}
	$rets[0]["amount"] = $last;
	
	// �߂�l�̔z��̐擪���c���ƂȂ�i���ׂƂ��Ă͖����j
	return array_reverse($rets);
}

function mobilesuica_parse_string($str) {
	return trim(mb_convert_kana(strip_tags($str), "sKV", "Shift_JIS"));
}

function mobilesuica_parse_details($account) {
	$ret = "";
	$cds = array();
	$cd_date = "";
	$cd_num = 0;
	$dtstart = ENV_STR_DATE_PASTDAY;
	foreach($account["details"] as $line) {
		if($dtstart > $line["date"]) $dtstart = $line["date"];
		
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
	$ret .= "<DTSTART>" . $dtstart . ENV_STR_OFX_TZ . "</DTSTART>";
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