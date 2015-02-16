<?php
/*
smbc.inc: �O��Z�F��s��OFX���OFX�t�@�C����ϊ��E��������
Copyright (C) 2012-2015 OFFICE OUTERGUY. All rights reserved.
mailto:contact@beatrek.com
Licensed under the GNU AGPLv3.
*/

// ���O�C�������擾����
$user = str_replace("-", "", $ofxforms["USRID"]);
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
list($head, $body) = smbc_http11($method, $uris, $query, $cookie);

// ���O�C����ʂ��擾����
$as = parse_tag($body, "a");
$c = parse_tag_search($as, "innerHTML", "SMBC�_�C���N�g ���O�C��");
if($c != -1) {
	$method = "GET";
	$uris = parse_uri($as[$c]["href"], $uris);
	$query = "";
	$cookie = smbc_update_cookie($head, $cookie);
	list($head, $body) = smbc_http11($method, $uris, $query, $cookie);
}

// ���O�C������
$forms = parse_tag($body, "form");
$c = parse_tag_search($forms, "name", "Login");
if($c != -1) {
	$inputs = parse_tag($forms[$c]["innerHTML"], "input");
	$queries = array();
	foreach($inputs as $input) switch($input["name"]) {
	case "":
		// �������Ȃ�
		break;
	case "domainSumitomo": // checkUserID()
		$forms[$c]["action"] = $input["value"];
		// break���Ȃ�
	default:
		$queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
		break;
	}
	$queries["USRID"] = "USRID=" . $user;
	$queries["USRID1"] = "USRID1=" . substr($user, 0, 5);
	$queries["USRID2"] = "USRID2=" . substr($user, 5);
	$queries["PASSWORD"] = "PASSWORD=" . $pass;
	$queries["LOGIN_TYPE"] = "LOGIN_TYPE=0"; // �_��Ҕԍ��Ń��O�C��
	if(isset($queries["key_Delete"]) == true) unset($queries["key_Delete"]);
	
	$method = $forms[$c]["method"];
	$uris = parse_uri($forms[$c]["action"], $uris);
	$query = implode("&", $queries);
	$cookie = smbc_update_cookie($head, $cookie);
	list($head, $body) = smbc_http11($method, $uris, $query, $cookie);
}

// ���_�C���N�g����
$retry = 0;
while($retry++ < ENV_NUM_HTTP11_RETRY) {
	$locations = parse_header($head, "location");
	if(count($locations) > 0) {
		$method = "GET";
		$uris = parse_uri($locations[0], $uris);
		$query = "";
		$cookie = smbc_update_cookie($head, $cookie);
		list($head, $body) = smbc_http11($method, $uris, $query, $cookie);
	}
}

if(strpos($body, "�O��Z�F��s����̏d�v�Ȃ��m�点") !== false) {
	// �d�v�Ȃ��m�点��ʂ̏ꍇ�A�X�L�b�v����
	$forms = parse_tag($body, "form");
	if(count($forms) == 1) {
		$inputs = parse_tag($forms[0]["innerHTML"], "input");
		$queries = array();
		foreach($inputs as $input) $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
		if(isset($queries["__btn__"]) == true) unset($queries["__btn__"]);
		if(isset($queries["STOP_INFO_FLG"]) == true) unset($queries["STOP_INFO_FLG"]);
		$queries["imgNext.y"] = "imgNext.y=" . urlencode("����");
		
		$method = $forms[0]["method"];
		$uris = parse_uri($forms[0]["action"], $uris);
		$query = implode("&", $queries);
		$cookie = smbc_update_cookie($head, $cookie);
		list($head, $body) = smbc_http11($method, $uris, $query, $cookie);
	}
	
	// ���_�C���N�g����
	$retry = 0;
	while($retry++ < ENV_NUM_HTTP11_RETRY) {
		$locations = parse_header($head, "location");
		if(count($locations) > 0) {
			$method = "GET";
			$uris = parse_uri($locations[0], $uris);
			$query = "";
			$cookie = smbc_update_cookie($head, $cookie);
			list($head, $body) = smbc_http11($method, $uris, $query, $cookie);
		}
	}
}

if(strpos($body, "�V�X�e�������e�i���X�̂���") !== false) {
	// �V�X�e�������e�i���X��ʂ̏ꍇ
	$resp["status"] = ENV_NUM_STATUS_MAINTENANCE;
	$resp["method"] = $method;
	$resp["uri"] = $uris["scheme"] . "://" . $uris["host"] . $uris["path"];
	$resp["query"] = $query;
	$resp["cookie"] = $cookie;
	$resp["head"] = $head;
	$resp["body"] = $body;
	$resp["ofx"] = generate_ofx($resp["status"]);
} else if(strpos($body, "�O�񃍃O�C������") === false) {
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
	// ���L�����ꗗ��ʂ��擾����
	$as = parse_tag($body, "a");
	$c = parse_tag_search($as, "innerHTML", "���L�����ꗗ");
	if($c != -1) {
		$method = "GET";
		$uris = parse_uri($as[$c]["href"], $uris);
		$query = "";
		$cookie = smbc_update_cookie($head, $cookie);
		list($head, $body) = smbc_http11($method, $uris, $query, $cookie);
	}
	
	// ���_�C���N�g����
	$retry = 0;
	while($retry++ < ENV_NUM_HTTP11_RETRY) {
		$locations = parse_header($head, "location");
		if(count($locations) > 0) {
			$method = "GET";
			$uris = parse_uri($locations[0], $uris);
			$query = "";
			$cookie = smbc_update_cookie($head, $cookie);
			list($head, $body) = smbc_http11($method, $uris, $query, $cookie);
		}
	}
	
	$accounts = array();
	
	// �����ꗗ���擾����
	$c = 0;
	$trs = parse_tag($body, "tr");
	foreach($trs as $tr) {
		$tds = parse_tag($tr["innerHTML"], "td");
		if(count($tds) == 5) {
			$inputs = parse_tag($tds[4]["innerHTML"], "input");
			foreach($inputs as $input) if(preg_match("/[^\']+?\'([0-9]+)\'.*?/", $input["onclick"], $matches) > 0) {
				$branch = str_replace("�@", "", trim(strip_tags($tds[1]["innerHTML"])));
				$accttype = trim(strip_tags($tds[2]["innerHTML"]));
				$accounts[$c]["branchid"] = smbc_get_branchid($settings, $branch);
				$accounts[$c]["acctid"] = trim(strip_tags($tds[3]["innerHTML"]));
				$accounts[$c]["id"] = $matches[1];
				$accounts[$c]["acctname"] = implode(ENV_CHR_CONCATENATOR, array($settings["name"], $branch, $accttype));
				$c++;
				break;
			}
		}
	}
	
	// ���ݎ�����萼��Ƙa��̍����擾����
	$delta = 0;
	$divs = parse_tag($body, "div", true);
	$c = parse_tag_search($divs, "class", "curTime");
	if($c != -1) {
		$geny = (preg_match("/([0-9]+)" . preg_quote("�N") . "/", $divs[$c]["innerHTML"], $matches) > 0? $matches[1]: "0");
		$delta = (integer)substr(ENV_STR_DATE_TODAY, 0, 4) - (integer)$geny;
	}
	
	// �����������[�v����
	foreach($accounts as $account) {
		// ���s���ԁi�^�C���A�E�g�j���Đݒ肷��
		@set_time_limit(ENV_NUM_TIMEOUT);
		
		// �c���E���o�����׉�ʂ��擾����
		$forms = parse_tag($body, "form");
		$c = parse_tag_search($forms, "name", "accnt_ryu_web");
		if($c == -1) $c = parse_tag_search($forms, "name", "accnt_ryu");
		if($c != -1) {
			$inputs = parse_tag($forms[$c]["innerHTML"], "input");
			$queries = array();
			foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
			$selects = parse_tag($forms[$c]["innerHTML"], "select");
			foreach($selects as $select) {
				$options = parse_tag($select["innerHTML"], "option");
				$v = "";
				switch($select["name"]) {
				case "FromYear":
					$v = (string)((integer)substr(ENV_STR_DATE_PASTDAY, 0, 4) - $delta);
					break;
				case "FromMonth":
					$v = (string)substr(ENV_STR_DATE_PASTDAY, 4, 2);
					break;
				case "FromDate":
					$v = (string)substr(ENV_STR_DATE_PASTDAY, 6, 2);
					break;
				case "ToYear":
					$v = (string)((integer)substr(ENV_STR_DATE_TODAY, 0, 4) - $delta);
					break;
				case "ToMonth":
					$v = (string)substr(ENV_STR_DATE_TODAY, 4, 2);
					break;
				case "ToDate":
					$v = (string)substr(ENV_STR_DATE_TODAY, 6, 2);
					break;
				default:
					break;
				}
				$j = ($v != ""? parse_tag_search($options, "innerHTML", $v): -1);
				if($j != -1) $queries[$select["name"]] = urlencode($select["name"]) . "=" . urlencode($options[$j]["value"]); else foreach($options as $option) if($select["name"] != "" && $option["selected"] == "selected") $queries[$select["name"]] = urlencode($select["name"]) . "=" . urlencode($option["value"]);
			}
			$queries["select_mode"] = "select_mode=1"; // 1=���Ԏw��
			$queries["RYUDO_ACCNT_IDX"] = "RYUDO_ACCNT_IDX=" . $account["id"];
			
			$method = $forms[$c]["method"];
			$uris = parse_uri($forms[$c]["action"], $uris);
			$query = implode("&", $queries);
			$cookie = smbc_update_cookie($head, $cookie);
			list($head, $body) = smbc_http11($method, $uris, $query, $cookie);
		}
		
		// ���_�C���N�g����
		$retry = 0;
		while($retry++ < ENV_NUM_HTTP11_RETRY) {
			$locations = parse_header($head, "location");
			if(count($locations) > 0) {
				$method = "GET";
				$uris = parse_uri($locations[0], $uris);
				$query = "";
				$cookie = smbc_update_cookie($head, $cookie);
				list($head, $body) = smbc_http11($method, $uris, $query, $cookie);
			}
		}
		
		if(strpos($body, "�w����Ԃ̓��o���͂������܂���") === false) {
			$body_old = $body;
			
			// OFX�t�@�C�����擾����
			$as = parse_tag($body, "a");
			$c = parse_tag_search($as, "innerHTML", "�}�C�N���\�t�g�}�l�[�Ɏ捞��");
			if($c != -1) {
				$method = "GET";
				$uris = parse_uri($as[$c]["href"], $uris);
				$query = "";
				$cookie = smbc_update_cookie($head, $cookie);
				list($head, $body) = smbc_http11($method, $uris, $query, $cookie);
			}
			
			// ���_�C���N�g����
			$retry = 0;
			while($retry++ < ENV_NUM_HTTP11_RETRY) {
				$locations = parse_header($head, "location");
				if(count($locations) > 0) {
					$method = "GET";
					$uris = parse_uri($locations[0], $uris);
					$query = "";
					$cookie = smbc_update_cookie($head, $cookie);
					list($head, $body) = smbc_http11($method, $uris, $query, $cookie);
				}
			}
			
			if(strpos($head, "Content-Type: text/html") === false) {
				$str = convert_ofx($body);
				
				// BRANCHID��␳����
				$str = str_replace("<BRANCHID>0" . $account["branchid"] . "</BRANCHID>", "<BRANCHID>" . $account["branchid"] . "</BRANCHID>", $str);
				
				// ACCTID��␳����
				$str = str_replace("<ACCTID>0" . $account["acctid"] . "</ACCTID>", "<ACCTID>" . $account["acctid"] . "</ACCTID>", $str);
				
				// �������̂��o�͂���
				if($account["acctname"] != "") $str = str_replace("</STMTRS>", "<MKTGINFO>" . mb_convert_string($account["acctname"]) . "</MKTGINFO>\r\n</STMTRS>", $str);
			}
			
			$body = $body_old;
		} else {
			// �c�����擾����
			$spans = parse_tag($body, "span", true);
			$c = parse_tag_search($spans, "class", "amount");
			if($c != -1) $account["balance"] = parse_amount($spans[$c]["innerHTML"]);
			
			// ���OFX�t�@�C���𐶐�����
			$bankmsgsrsv1 = "";
			$bankmsgsrsv1 .= "<BANKMSGSRSV1>";
			$bankmsgsrsv1 .= "\r\n";
			
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
			
			// BANKTRANLIST
			$bankmsgsrsv1 .= "<BANKTRANLIST>";
			$bankmsgsrsv1 .= "\r\n";
			$bankmsgsrsv1 .= "<DTSTART>" . ENV_STR_DATE_PASTDAY . ENV_STR_OFX_TZ . "</DTSTART>";
			$bankmsgsrsv1 .= "<DTEND>" . ENV_STR_DATE_TODAY . ENV_STR_OFX_TZ . "</DTEND>";
			$bankmsgsrsv1 .= "\r\n";
			$bankmsgsrsv1 .= "</BANKTRANLIST>";
			$bankmsgsrsv1 .= "\r\n";
			$bankmsgsrsv1 .= "<LEDGERBAL>";
			$bankmsgsrsv1 .= "<BALAMT>" . $account["balance"] . "</BALAMT>";
			$bankmsgsrsv1 .= "<DTASOF>" . ENV_STR_DATE_TODAY . ENV_STR_OFX_TZ . "</DTASOF>";
			$bankmsgsrsv1 .= "</LEDGERBAL>";
			$bankmsgsrsv1 .= "\r\n";
			
			// �������̂��o�͂���
			if($account["acctname"] != "") {
				$bankmsgsrsv1 .= "<MKTGINFO>" . mb_convert_string($account["acctname"]) . "</MKTGINFO>";
				$bankmsgsrsv1 .= "\r\n";
			}
			
			$bankmsgsrsv1 .= "</STMTRS>";
			$bankmsgsrsv1 .= "\r\n";
			$bankmsgsrsv1 .= "</STMTTRNRS>";
			$bankmsgsrsv1 .= "\r\n";
			
			$bankmsgsrsv1 .= "</BANKMSGSRSV1>";
			$bankmsgsrsv1 .= "\r\n";
			
			$resp["status"] = ENV_NUM_STATUS_SUCCESS;
			$str = generate_ofx($resp["status"], $bankmsgsrsv1);
		}
		
		// OFX�t�@�C���������ƂȂ�ꍇ�͘A������
		if($ofx != "") {
			$c = strpos($str, "<BANKMSGSRSV1>");
			if($c !== false) $str = substr($str, $c + 14);
			$c = strpos($ofx, "</BANKMSGSRSV1>");
			if($c !== false) $ofx = substr($ofx, 0, $c - 1) . $str;
		} else {
			$ofx .= $str;
		}
	}
	
	// ���s���ԁi�^�C���A�E�g�j���Đݒ肷��
	@set_time_limit(ENV_NUM_TIMEOUT);
	
	// ���O�A�E�g����
	$as = parse_tag($body, "a");
	foreach($as as $a) if(parse_tag_search(parse_tag($a["innerHTML"], "img"), "alt", "���O�A�E�g") != -1) {
		$method = "GET";
		$uris = parse_uri($a["href"], $uris);
		$query = "";
		$cookie = smbc_update_cookie($head, $cookie);
		list($head, $body) = smbc_http11($method, $uris, $query, $cookie);
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
			$cookie = smbc_update_cookie($head, $cookie);
			list($head, $body) = smbc_http11($method, $uris, $query, $cookie);
		}
	}
	
	// ���J�����b�Z�[�W��ʂ��X�L�b�v����
	$as = parse_tag($body, "a");
	$c = parse_tag_search($as, "innerHTML", "�m�F�����ɂ��̂܂܃��O�A�E�g����");
	if($c != -1) {
		$method = "GET";
		$uris = parse_uri($as[$c]["href"], $uris);
		$query = "";
		$cookie = smbc_update_cookie($head, $cookie);
		list($head, $body) = smbc_http11($method, $uris, $query, $cookie);
	}
	
	// ���_�C���N�g����
	$retry = 0;
	while($retry++ < ENV_NUM_HTTP11_RETRY) {
		$locations = parse_header($head, "location");
		if(count($locations) > 0) {
			$method = "GET";
			$uris = parse_uri($locations[0], $uris);
			$query = "";
			$cookie = smbc_update_cookie($head, $cookie);
			list($head, $body) = smbc_http11($method, $uris, $query, $cookie);
		}
	}
	
	// OFX�t�@�C�����o�͂���
	if($ofx == "") {
		// OFX�f�[�^�����݂��Ȃ��ꍇ
		$resp["status"] = ENV_NUM_STATUS_SUCCESS;
		$resp["ofx"] = generate_ofx($resp["status"]);
	} else {
		// OFX�f�[�^�����݂���ꍇ
		$resp["status"] = ENV_NUM_STATUS_SUCCESS;
		$resp["ofx"] = $ofx;
	}
}
return $resp;

// HTTP/1.1
function smbc_http11($method, $uris, $query = "", $cookie = "") {
	$ret = "INVALID HOST";
	if(preg_match("/\.smbc\.co\.jp$/", $uris["host"]) > 0) $ret = http11(strtoupper($method), $uris["scheme"], $uris["host"], 0, $uris["path"], $query, "", $cookie);
	return explode("\r\n\r\n", $ret, 2);
}

function smbc_update_cookie($head, $cookie) {
	return update_cookie(array("JSESSIONID"), parse_header($head, "set-cookie"), $cookie);
}

function emap_http11($method, $uris, $query = "", $cookie = "") {
	$ret = "INVALID HOST";
	if(preg_match("/\.e\-map\.ne\.jp$/", $uris["host"]) > 0) $ret = http11(strtoupper($method), $uris["scheme"], $uris["host"], 0, $uris["path"], $query, "", $cookie);
	return explode("\r\n\r\n", $ret, 2);
}

// �x�X�����X�Ԃ���������
function smbc_get_branchid($settings, $str) {
	$ret = mb_convert_string($str);
	
	// ������ʂ��擾����
	$method = "GET";
	$uris = parse_uri($settings["getbranchid"] . urlencode($ret));
	$query = "";
	$cookie = "";
	list($head, $body) = emap_http11($method, $uris, $query, $cookie);
	
	$body = mb_convert_encoding($body, "UTF-8", "EUC-JP");
	
	$as = parse_tag($body, "a");
	$c = parse_tag_search($as, "innerHTML", $ret);
	if($c != -1 && preg_match("/javascript:[^\']+?\'([0-9]+)\'.*?/i", stripslashes($as[$c]["href"]), $matches) > 0) $ret = substr($matches[1], strlen($matches[1]) - 3);
	
	return $ret;
}

?>