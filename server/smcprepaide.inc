<?php
/*
smcprepaide.inc: �O��Z�FVISA�v���y�C�he��HTML���OFX�t�@�C���𐶐�����
Copyright (C) 2014-2015 OFFICE OUTERGUY. All rights reserved.
mailto:contact@beatrek.com
Licensed under the GNU AGPLv3.
*/

// ���O�C�������擾����
$user = $ofxforms["loginId"];
$pass = $ofxforms["loginPassword"];
$auth = urlencode(mb_convert_encoding(urldecode($ofxforms["authCode"]), "Shift_JIS", "UTF-8"));
$sesscookie = $ofxforms["sesscookie"];
$accesskey = $ofxforms["accesskey"];

$resp = array();
$ofx = "";

if($sesscookie != "") list($cookie, $dummy) = explode("\t", sess_decode($sesscookie));
if($accesskey != "") {
	list($ssid, $method, $uri, $query, $svid, $dummy) = explode("\t", sess_decode($accesskey), 6);
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
	list($head, $body) = smcprepaide_http11($method, $uris, $query, $cookie);
	
	// ���_�C���N�g����
	$retry = 0;
	while($retry++ < ENV_NUM_HTTP11_RETRY) {
		$locations = parse_header($head, "location");
		if(count($locations) > 0) {
			$method = "GET";
			$uris = parse_uri($locations[0], $uris);
			$query = "";
			$cookie = smcprepaide_update_cookie($head, $cookie);
			list($head, $body) = smcprepaide_http11($method, $uris, $query, $cookie);
		}
	}
	
	// ���O�C����ʂ��擾����
	$as = parse_tag($body, "a");
	foreach($as as $a) if(parse_tag_search(parse_tag($a["innerHTML"], "img"), "alt", "���O�C��") != -1) {
		$method = "GET";
		$uris = parse_uri($a["href"], $uris);
		$query = "";
		$cookie = smcprepaide_update_cookie($head, $cookie);
		list($head, $body) = smcprepaide_http11($method, $uris, $query, $cookie);
		break;
	}
	
	// ���_�C���N�g����
	$scripts = parse_tag($body, "script");
	foreach($scripts as $script) if(preg_match("/window\.location[\s\t]*=[\s\t]*\"(.*?)\";/i", $script["innerHTML"], $matches) > 0) {
		$method = "GET";
		$uris = parse_uri($matches[1], $uris);
		$query = "";
		$cookie = smcprepaide_update_cookie($head, $cookie);
		list($head, $body) = smcprepaide_http11($method, $uris, $query, $cookie);
		break;
	}
	
	// svid���擾����
	$svid = "";
	$scripts = parse_tag($body, "script");
	foreach($scripts as $script) if(preg_match("/svid[\s\t]*=[\s\t]*\"(.*?)\";/i", $script["innerHTML"], $matches) > 0) {
		$svid = $matches[1];
		break;
	}
	
	// �摜�F�؂̉摜���擾����
	$imgs = parse_tag($body, "img");
	$c = parse_tag_search($imgs, "id", "captchaImage");
	if($c != -1) {
		$imguris = parse_uri($imgs[$c]["src"], $uris);
		list($imghead, $imgbody) = smcprepaide_http11($method, $imguris, $query, $cookie, $uris["scheme"] . "://" . $uris["host"] . $uris["path"] . $query);
		$imgsrc = "data:image/jpeg;base64," . base64_encode($imgbody);
		$cookie = smcprepaide_update_cookie($imghead, $cookie);
	}
	
	// ���O�C������
	$forms = parse_tag($body, "form");
	$c = parse_tag_search($forms, "id", "WB0102SC01Form");
	if($c != -1) {
		$inputs = parse_tag($forms[$c]["innerHTML"], "input");
		$queries = array();
		foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
		$queries["loginId"] = "loginId=" . $user;
		$queries["loginPassword"] = "loginPassword=" . $pass;
		$queries["authCode"] = "authCode=";
		// if(isset($queries["forward_login"]) == true) unset($queries["forward_login"]);
		
		$method = $forms[$c]["method"];
		$uris = parse_uri($forms[$c]["action"], $uris);
		$query = implode("&", $queries);
	}
	
	// �Z�b�V������ޔ�����
	$sid = 1;
	$sess = sess_encode(implode("\t", array((string)$sid, $method, $uris["scheme"] . "://" . $uris["host"] . $uris["path"], $query, $svid, ENV_STR_SESSION_PADDING)));
	$head = "";
	$body = "";
} else if($sid == 1) {
	// �Z�b�V�����𕜌�����
	$sid = 0;
	$uris = parse_uri($uri);
	$query = str_replace("authCode=", "authCode=" . $auth, $query);
	list($head, $body) = smcprepaide_http11($method, $uris, $query, $cookie);
}

if($sid > 0) {
	// �Z�b�V�����������p��
	$resp["status"] = ENV_NUM_STATUS_ADDITION;
	$resp["aid"] = "authCode";
	$resp["addition"] = $imgsrc;
	$resp["sess"] = $sess;
	$resp["sid"] = $sid;
	$resp["sesscookie"] = sess_encode(implode("\t", array($cookie, ENV_STR_SESSION_PADDING)));
	$resp["accesskey"] = sess_encode(implode("\t", array((string)$sid, $method, $uris["scheme"] . "://" . $uris["host"] . $uris["path"], $query, $svid, ENV_STR_SESSION_PADDING)));
	
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
} else if(strpos($body, "���܂��͗Վ������e�i���X�̂���") !== false) {
	// �V�X�e�������e�i���X��ʂ̏ꍇ
	$resp["status"] = ENV_NUM_STATUS_MAINTENANCE;
	$resp["method"] = $method;
	$resp["uri"] = $uris["scheme"] . "://" . $uris["host"] . $uris["path"];
	$resp["query"] = $query;
	$resp["cookie"] = $cookie;
	$resp["head"] = $head;
	$resp["body"] = $body;
	$resp["ofx"] = generate_ofx($resp["status"]);
} else if(strpos($body, "�d�v�Ȃ��m�点������܂�") !== false) {
	// ���O�C����̉�ʂ��ʏ�ƈقȂ�ꍇ
	$resp["status"] = ENV_NUM_STATUS_CAUTION;
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
	$accounts = array();
	
	// �����������[�v����
	$forms = parse_tag($body, "form");
	for($c = 1; $c < count($forms); $c++) {
		// �o�^�J�[�h�ڍ׉�ʂ��擾����
		$inputs = parse_tag($forms[$c]["innerHTML"], "input");
		$queries = array();
		foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
		// if(isset($queries["forward_toWB3202SC01"]) == true) unset($queries["forward_toWB3202SC01"]); // �ڍ�
		if(isset($queries["forward_toWB2301SC01"]) == true) unset($queries["forward_toWB2301SC01"]); // �`���[�W
		if(isset($queries["forward_toWB2601SC01"]) == true) unset($queries["forward_toWB2601SC01"]); // ���b�N
		if(isset($queries["forward_toWB2602SC01"]) == true) unset($queries["forward_toWB2602SC01"]); // ���b�N����
		if(isset($queries["forward_toWB3301SC01"]) == true) unset($queries["forward_toWB3301SC01"]); // ���p�����Ɖ�
		if(isset($queries["forward_toWB2901SC01"]) == true) unset($queries["forward_toWB2901SC01"]); // �Ïؔԍ��ݒ�
		
		$method = $forms[$c]["method"];
		$uris = parse_uri($forms[$c]["action"], $uris);
		$query = implode("&", $queries);
		$cookie = smcprepaide_update_cookie($head, $cookie);
		list($head, $body) = smcprepaide_http11($method, $uris, $query, $cookie);
		
		// ���p�����Ɖ��ʂ��擾����
		$forms2 = parse_tag($body, "form");
		$inputs = parse_tag($forms2[0]["innerHTML"], "input");
		
		// �������i�j�b�N�l�[���j���擾����
		$d = parse_tag_search($inputs, "name", "cardNickname");
		if($d != -1) $accounts[$c - 1]["acctname"] = $settings["name"] . ENV_CHR_CONCATENATOR . $inputs[$d]["value"];
		
		// �x�X�ԍ����擾����
		$accounts[$c - 1]["branchid"] = "0";
		
		// �����ԍ����擾����
		$d = parse_tag_search($inputs, "name", "vcn");
		if($d != -1) $accounts[$c - 1]["acctid"] = $inputs[$d]["value"];
		
		// �c�����擾����
		$d = parse_tag_search($inputs, "name", "chargeBalance");
		if($d != -1) $accounts[$c - 1]["balance"] = $inputs[$d]["value"];
		
		$queries = array();
		foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
		unset($queries["forward_toWB2301SC01"]); // �`���[�W
		// if(isset($queries["forward_toWB3301SC01"]) == true) unset($queries["forward_toWB3301SC01"]); // ���p�����Ɖ�
		if(isset($queries["forward_toWB2601SC01"]) == true) unset($queries["forward_toWB2601SC01"]); // �����p���b�N�ݒ�E����
		if(isset($queries["forward_toWB2701SC01"]) == true) unset($queries["forward_toWB2701SC01"]); // �J�[�h���̂̕ύX
		if(isset($queries["forward_toWB2901SC01"]) == true) unset($queries["forward_toWB2901SC01"]); // �Ïؔԍ��ݒ�
		if(isset($queries["forward_toWB3201SC01"]) == true) unset($queries["forward_toWB3201SC01"]); // �߂�
		
		$method = $forms2[0]["method"];
		$uris = parse_uri($forms2[0]["action"], $uris);
		$query = implode("&", $queries);
		$cookie = smcprepaide_update_cookie($head, $cookie);
		list($head, $body) = smcprepaide_http11($method, $uris, $query, $cookie);
		
		$detail = "";
		
		do {
			$tbodys = parse_tag($body, "tbody");
			if(count($tbodys) > 1) $detail .= $tbodys[1]["innerHTML"];
			
			$as = parse_tag($body, "a");
			$d = parse_tag_search($as, "innerHTML", "����");
			if($d != -1) {
				$metod = "GET";
				$uris = parse_uri($as[$d]["href"], $uris);
				$query = "";
				$cookie = smcprepaide_update_cookie($head, $cookie);
				list($head, $body) = smcprepaide_http11($method, $uris, $query, $cookie);
			}
		} while($d != -1);
		
		if($detail != "") $accounts[$c - 1]["details"] = smcprepaide_get_details($detail);
		
		/*
		// �J�[�h�ꗗ��ʂ��擾����
		$as = parse_tag($body, "a");
		$c = parse_tag_search($as, "innerHTML", "�J�[�h�ꗗ");
		if($c != -1) {
			$method = "GET";
			$uris = parse_uri($as[$c]["href"] . $svid, $uris);
			$query = "";
			$cookie = smcprepaide_update_cookie($head, $cookie);
			list($head, $body) = smcprepaide_http11($method, $uris, $query, $cookie);
		}
		*/
	}
	
	// ���s���ԁi�^�C���A�E�g�j���Đݒ肷��
	@set_time_limit(ENV_NUM_TIMEOUT);
	
	// ���O�A�E�g����
	$as = parse_tag($body, "a");
	foreach($as as $a) if(parse_tag_search(parse_tag($a["innerHTML"], "img"), "alt", "���O�A�E�g") != -1) {
		$method = "GET";
		$uris = parse_uri($a["href"] . $svid, $uris);
		$query = "";
		$cookie = smcprepaide_update_cookie($head, $cookie);
		list($head, $body) = smcprepaide_http11($method, $uris, $query, $cookie);
		break;
	}
	
	$bankmsgsrsv1 = "";
	$bankmsgsrsv1 .= "<BANKMSGSRSV1>";
	$bankmsgsrsv1 .= "\r\n";
	
	foreach($accounts as $account) {
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
		$bankmsgsrsv1 .= "<ACCTTYPE>" . ENV_STR_ACCTTYPE_SAVINGS . "</ACCTTYPE>";
		$bankmsgsrsv1 .= "</BANKACCTFROM>";
		$bankmsgsrsv1 .= "\r\n";
		$bankmsgsrsv1 .= smcprepaide_parse_details($account);
		$bankmsgsrsv1 .= "</STMTRS>";
		$bankmsgsrsv1 .= "\r\n";
		$bankmsgsrsv1 .= "</STMTTRNRS>";
		$bankmsgsrsv1 .= "\r\n";
	}
	
	$bankmsgsrsv1 .= "</BANKMSGSRSV1>";
	$bankmsgsrsv1 .= "\r\n";
	
	// OFX�t�@�C�����o�͂���
	$resp["status"] = ENV_NUM_STATUS_SUCCESS;
	if(strpos($bankmsgsrsv1, "<BANKTRANLIST>") === false) {
		// ���ׂ����݂��Ȃ��ꍇ
		$resp["ofx"] = generate_ofx($resp["status"]);
	} else {
		// ���ׂ����݂���ꍇ
		$resp["ofx"] = generate_ofx($resp["status"], $bankmsgsrsv1);
	}
}
return $resp;

// HTTP/1.1
function smcprepaide_http11($method, $uris, $query = "", $cookie = "", $referer = "") {
	$ret = "INVALID HOST";
	if(preg_match("/\.smbc-card\.com$/", $uris["host"]) > 0 || preg_match("/\.vpass\.ne\.jp$/", $uris["host"]) > 0) $ret = http11(strtoupper($method), $uris["scheme"], $uris["host"], 0, $uris["path"], $query, "", $cookie, true, ENV_PRODUCT_UA, "", $referer);
	return explode("\r\n\r\n", $ret, 2);
}

function smcprepaide_update_cookie($head, $cookie) {
	return update_cookie(array("JSESSIONID"), parse_header($head, "set-cookie"), $cookie);
}

function smcprepaide_get_details($body) {
	$rets = array();
	$i = 0;
	$trs = array_reverse(parse_tag($body, "tr"));
	foreach($trs as $tr) {
		$tds = parse_tag($tr["innerHTML"], "td");
		if(count($tds) == 9) {
			// �����p�������擾����
			$dt = explode(" ", trim(strip_tags($tds[2]["innerHTML"])));
			
			// �����O�̖��ׂ����O����
			if(ENV_STR_DATE_PASTDAY > parse_date($dt[0])) continue;
			
			$rets[$i]["date"] = parse_date($dt[0]);
			
			// �x���敪�A�����p�X���A�������z�A����т����p���z���擾����
			$buf = trim(strip_tags($tds[3]["innerHTML"]));
			switch($buf) {
			case "�����s�ԍ�����":
			case "�ԋ�":
			case "�`���[�W": // debug
				$name = $buf;
				$memo = trim(strip_tags($tds[8]["innerHTML"]));
				$amount = parse_amount(trim(strip_tags($tds[5]["innerHTML"])));
				break;
			case "���p":
			default:
				$name = trim(strip_tags($tds[4]["innerHTML"]));
				$memo = trim(strip_tags($tds[1]["innerHTML"]));
				$memo2 = trim(strip_tags($tds[8]["innerHTML"]));
				if($name == "") $name = $buf;
				if($memo2 != "") $memo .= ENV_CHR_CONCATENATOR . $memo2;
				$amount = "-" . parse_amount(trim(strip_tags($tds[6]["innerHTML"])));
				break;
			}
			$rets[$i]["summary"] = $name;
			$rets[$i]["memo"] = ($memo != ""? $memo: ENV_STR_OFX_MEMO);
			
			// �����p���z���擾����
			$rets[$i]["amount"] = $amount;
			
			$i++;
		}
	}
	
	return $rets;
}

function smcprepaide_parse_details($account) {
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