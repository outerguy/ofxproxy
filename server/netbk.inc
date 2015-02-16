<?php
/*
netbk.inc: �Z�MSBI�l�b�g��s��CSV���OFX�t�@�C���𐶐�����
Copyright (C) 2012-2015 OFFICE OUTERGUY. All rights reserved.
mailto:contact@beatrek.com
Licensed under the GNU AGPLv3.
*/

// ���O�C�������擾����
$user = $ofxforms["userName"];
$pass = $ofxforms["loginPwdSet"];

$resp = array();

// ���s���ԁi�^�C���A�E�g�j���Đݒ肷��
@set_time_limit(ENV_NUM_TIMEOUT);

// �z�[����ʂ��擾����
$method = "GET";
$uris = parse_uri($settings["home"]);
$query = "";
$cookie = "";
list($head, $body) = netbk_http11($method, $uris, $query, $cookie);

// ���_�C���N�g����
$retry = 0;
while($retry++ < ENV_NUM_HTTP11_RETRY) {
	$locations = parse_header($head, "location");
	if(count($locations) > 0) {
		$method = "GET";
		$uris = parse_uri($locations[0], $uris);
		$query = "";
		$cookie = netbk_update_cookie($head, $cookie);
		list($head, $body) = netbk_http11($method, $uris, $query, $cookie);
	}
}

// ���O�C������
$forms = parse_tag($body, "form");
$c = parse_tag_search($forms, "name", "LoginForm");
if($c != -1) {
	$inputs = parse_tag($forms[$c]["innerHTML"], "input");
	$queries = array();
	foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
	$queries["userName"] = "userName=" . $user;
	$queries["loginPwdSet"] = "loginPwdSet=" . $pass;
	
	$method = $forms[$c]["method"];
	$uris = parse_uri($forms[$c]["action"], $uris);
	$query = implode("&", $queries);
	$cookie = netbk_update_cookie($head, $cookie);
	list($head, $body) = netbk_http11($method, $uris, $query, $cookie);
}

if(strpos($body, "��s����̏d�v�Ȃ��m�点") !== false) {
	// �d�v�Ȃ��m�点��ʂ̏ꍇ�A�X�L�b�v����
	$forms = parse_tag($body, "form");
	$c = parse_tag_search($forms, "name", "form0103_01_100");
	if($c != -1) {
		$inputs = parse_tag($forms[$c]["innerHTML"], "input");
		$queries = array();
		foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
		if(isset($queries["imptNtcCheck"]) == true) unset($queries["imptNtcCheck"]); // �u�m�F���܂����v�`�F�b�N�{�b�N�X���I�t�ɂ���
		
		$method = $forms[$c]["method"];
		$uris = parse_uri($forms[$c]["action"], $uris);
		$query = implode("&", $queries);
		$cookie = netbk_update_cookie($head, $cookie);
		list($head, $body) = netbk_http11($method, $uris, $query, $cookie);
	}
	
	// ���_�C���N�g����
	$forms = parse_tag($body, "form");
	$c = parse_tag_search($forms, "name", "form0103_01_105");
	if($c != -1) {
		$inputs = parse_tag($forms[$c]["innerHTML"], "input");
		$queries = array();
		foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
		
		$method = $forms[$c]["method"];
		$uris = parse_uri($forms[$c]["action"], $uris);
		$query = implode("&", $queries);
		$cookie = netbk_update_cookie($head, $cookie);
		list($head, $body) = netbk_http11($method, $uris, $query, $cookie);
	}
}

if(strpos($body, "���݂��̎���͂��戵���ł��܂���") !== false) {
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
	// �c���Ɖ�i�����ʁj��ʂ��擾����
	$as = parse_tag($body, "a");
	foreach($as as $a) if(parse_tag_search(parse_tag($a["innerHTML"], "img"), "alt", "�������") != -1) {
		$method = "GET";
		$uris = parse_uri($a["href"], $uris);
		$query = "";
		$cookie = netbk_update_cookie($head, $cookie);
		list($head, $body) = netbk_http11($method, $uris, $query, $cookie);
		break;
	}
	
	// ���������擾����
	if(preg_match("/([0-9]{3})\-([0-9]{7})/", $body, $matches) > 0) {
		$branchid = $matches[1];
		$acctid = $matches[2];
	}
	
	$accounts = array();
	
	// ���������擾����
	$trs = parse_tag($body, "tr", true); // �ċA�I�Ɏ擾����
	foreach($trs as $tr) {
		$tds = parse_tag($tr["innerHTML"], "td");
		if(count($tds) == 5) {
			$as = parse_tag($tds[4]["innerHTML"], "a");
			foreach($as as $a) if(trim(strip_tags($a["innerHTML"])) == "����" && preg_match("/.*?\/([0-9]{2})\/(01|21)\/001\/01/", $a["href"], $matches) > 0) {
				$accounts[$c]["acctname"] = str_replace(array("����", "&nbsp;-&nbsp;"), array("", ENV_CHR_CONCATENATOR), trim(strip_tags($tds[0]["innerHTML"])));
				$accounts[$c]["id"] = ($matches[2] == "01"? $matches[1]: "00"); // SBI�n�C�u���b�h�a����00�Ƃ݂Ȃ�
				$accounts[$c]["acctid"] = $matches[1]; // 01=��\���� 02-99=�ړI�ʌ���
				$accounts[$c]["accttype"] = $matches[2]; // 01=�~���� 21=SBI�n�C�u���b�h
				$accounts[$c]["page"] = $a["href"];
				$c++;
			}
		}
	}
	
	// ���q���܏��Ɖ�E�ύX��ʂ��擾����
	$as = parse_tag($body, "a");
	$c = parse_tag_search($as, "innerHTML", "���q���܏��Ɖ�E�ύX");
	if($c != -1) {
		$method = "GET";
		$uris = parse_uri($as[$c]["href"], $uris);
		$query = "";
		$cookie = netbk_update_cookie($head, $cookie);
		list($head, $body) = netbk_http11($method, $uris, $query, $cookie);
	}
	
	// �x�X�����擾����
	$otables = parse_tag($body, "table");
	foreach($otables as $otable) {
		$tables = parse_tag($otable["innerHTML"], "table");
		foreach($tables as $table) {
			$c = 0;
			$trs = parse_tag($table["innerHTML"], "tr");
			foreach($trs as $tr) {
				$divs = parse_tag($tr["innerHTML"], "div");
				if(count($divs) == 2 && trim(strip_tags($divs[0]["innerHTML"])) == "�����ԍ�") {
					$bufs = explode(" ", preg_replace("/\s{2,}/", " ", str_replace("&nbsp;", "", trim(strip_tags($divs[1]["innerHTML"])))));
					for($i = 0; $i < count($accounts); $i++) $accounts[$i]["acctname"] = implode(ENV_CHR_CONCATENATOR, array($settings["name"], $bufs[1], $accounts[$i]["acctname"]));
				}
			}
		}
	}
	
	$bankmsgsrsv1 = "";
	$bankmsgsrsv1 .= "<BANKMSGSRSV1>";
	$bankmsgsrsv1 .= "\r\n";
	
	// �����������[�v����
	foreach($accounts as $account) {
		// ���s���ԁi�^�C���A�E�g�j���Đݒ肷��
		@set_time_limit(ENV_NUM_TIMEOUT);
		
		// ��������ʂ��擾����
		$method = "GET";
		$uris = parse_uri($account["page"], $uris);
		$query = "";
		$cookie = netbk_update_cookie($head, $cookie);
		list($head, $body) = netbk_http11($method, $uris, $query, $cookie);
		
		// ���o�����׉�ʂ��擾����
		$forms = parse_tag($body, "form");
		$c = parse_tag_search($forms, "name", "form0202_01_100");
		if($c != -1) {
			$inputs = parse_tag($forms[$c]["innerHTML"], "input");
			$queries = array();
			foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
			$selects = parse_tag($forms[$c]["innerHTML"], "select");
			foreach($selects as $select) {
				$options = parse_tag($select["innerHTML"], "option");
				foreach($options as $option) if($select["name"] != "" && $option["selected"] == "selected") $queries[$select["name"]] = urlencode($select["name"]) . "=" . urlencode($option["value"]);
			}
			$queries["termLink"] = "termLink=05"; // ���Ԏw��
			$queries["term"] = "term=05"; // ���Ԏw��
			$queries["acctBusPdCodeInput"] = "acctBusPdCodeInput=" . $account["acctid"] . $account["accttype"] . "001";
			$queries["dsplyTrmSpcfdYearFrom"] = "dsplyTrmSpcfdYearFrom=" . substr(ENV_STR_DATE_PASTDAY, 0, 4);
			$queries["dsplyTrmSpcfdMonthFrom"] = "dsplyTrmSpcfdMonthFrom=" . substr(ENV_STR_DATE_PASTDAY, 4, 2);
			$queries["dsplyTrmSpcfdDayFrom"] = "dsplyTrmSpcfdDayFrom=" . substr(ENV_STR_DATE_PASTDAY, 6, 2);
			$queries["dsplyTrmSpcfdYearTo"] = "dsplyTrmSpcfdYearTo=" . substr(ENV_STR_DATE_TODAY, 0, 4);
			$queries["dsplyTrmSpcfdMonthTo"] = "dsplyTrmSpcfdMonthTo=" . substr(ENV_STR_DATE_TODAY, 4, 2);
			$queries["dsplyTrmSpcfdDayTo"] = "dsplyTrmSpcfdDayTo=" . substr(ENV_STR_DATE_TODAY, 6, 2);
			
			$method = $forms[$c]["method"];
			$uris = parse_uri($forms[$c]["action"], $uris);
			$query = implode("&", $queries);
			$cookie = netbk_update_cookie($head, $cookie);
			list($head, $body) = netbk_http11($method, $uris, $query, $cookie);
		}
		
		$body_old = $body;
		
		// CSV�t�@�C�����_�E�����[�h����
		$forms = parse_tag($body, "form");
		$c = parse_tag_search($forms, "name", "form0202_01_100");
		if($c != -1) {
			$inputs = parse_tag($forms[$c]["innerHTML"], "input");
			$queries = array();
			foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
			$selects = parse_tag($forms[$c]["innerHTML"], "select");
			foreach($selects as $select) {
				$options = parse_tag($select["innerHTML"], "option");
				foreach($options as $option) if($select["name"] != "" && $option["selected"] == "selected") $queries[$select["name"]] = urlencode($select["name"]) . "=" . urlencode($option["value"]);
			}
			$queries["termLink"] = "termLink=05"; // ���Ԏw��
			$queries["term"] = "term=05"; // ���Ԏw��
			$queries["acctBusPdCodeInput"] = "acctBusPdCodeInput=" . $account["acctid"] . $account["accttype"] . "001";
			$queries["dsplyTrmSpcfdYearFrom"] = "dsplyTrmSpcfdYearFrom=" . substr(ENV_STR_DATE_PASTDAY, 0, 4);
			$queries["dsplyTrmSpcfdMonthFrom"] = "dsplyTrmSpcfdMonthFrom=" . substr(ENV_STR_DATE_PASTDAY, 4, 2);
			$queries["dsplyTrmSpcfdDayFrom"] = "dsplyTrmSpcfdDayFrom=" . substr(ENV_STR_DATE_PASTDAY, 6, 2);
			$queries["dsplyTrmSpcfdYearTo"] = "dsplyTrmSpcfdYearTo=" . substr(ENV_STR_DATE_TODAY, 0, 4);
			$queries["dsplyTrmSpcfdMonthTo"] = "dsplyTrmSpcfdMonthTo=" . substr(ENV_STR_DATE_TODAY, 4, 2);
			$queries["dsplyTrmSpcfdDayTo"] = "dsplyTrmSpcfdDayTo=" . substr(ENV_STR_DATE_TODAY, 6, 2);
			$queries["_ActionID"] = "_ActionID=doCSVDownload"; // CSV�_�E�����[�h
			if(isset($queries["ACT_doShow"]) == true) unset($queries["ACT_doShow"]);
			
			$method = $forms[$c]["method"];
			$uris = parse_uri($forms[$c]["action"], $uris);
			$query = implode("&", $queries);
			$cookie = netbk_update_cookie($head, $cookie);
			list($head, $body) = netbk_http11($method, $uris, $query, $cookie);
			
			if(strpos($head, "Content-Type: text/html") === false) {
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
				$bankmsgsrsv1 .= "<BRANCHID>" . $branchid . "</BRANCHID>";
				$bankmsgsrsv1 .= "<ACCTID>" . $acctid . "-" . $account["id"] . "</ACCTID>";
				$bankmsgsrsv1 .= "<ACCTTYPE>" . ENV_STR_ACCTTYPE_SAVINGS . "</ACCTTYPE>";
				$bankmsgsrsv1 .= "</BANKACCTFROM>";
				$bankmsgsrsv1 .= "\r\n";
				$bankmsgsrsv1 .= netbk_parse_csv($body, $account);
				$bankmsgsrsv1 .= "</STMTRS>";
				$bankmsgsrsv1 .= "\r\n";
				$bankmsgsrsv1 .= "</STMTTRNRS>";
				$bankmsgsrsv1 .= "\r\n";
			}
		}
		
		$body = $body_old;
	}
	
	$bankmsgsrsv1 .= "</BANKMSGSRSV1>";
	$bankmsgsrsv1 .= "\r\n";
	
	// ���s���ԁi�^�C���A�E�g�j���Đݒ肷��
	@set_time_limit(ENV_NUM_TIMEOUT);
	
	// ���O�A�E�g����
	$as = parse_tag($body, "a");
	foreach($as as $a) if(parse_tag_search(parse_tag($a["innerHTML"], "img"), "alt", "���O�A�E�g") != -1) {
		$method = "GET";
		$uris = parse_uri($a["href"], $uris);
		$query = "";
		$cookie = netbk_update_cookie($head, $cookie);
		list($head, $body) = netbk_http11($method, $uris, $query, $cookie);
		break;
	}
	
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
function netbk_http11($method, $uris, $query = "", $cookie = "") {
	$ret = "INVALID HOST";
	if(preg_match("/\.netbk\.co\.jp$/", $uris["host"]) > 0) $ret = http11(strtoupper($method), $uris["scheme"], $uris["host"], 0, $uris["path"], $query, "", $cookie);
	return explode("\r\n\r\n", $ret, 2);
}

function netbk_update_cookie($head, $cookie) {
	return update_cookie(array("JSESSIONID"), parse_header($head, "set-cookie"), $cookie);
}

function netbk_parse_csv($str, $account) {
	$ret = "";
	$lines = array_reverse(parse_csv($str));
	$cds = array();
	$cds_balamt = "0";
	$cd_date = "";
	$cd_num = 0;
	
	foreach($lines as $line) {
		$cd = array();
		if(count($line) == 6 && $line[0] != "���t") {
			list($cd_name, $cd_dummy) = explode(ENV_CHR_SEPARATOR, str_replace(array("��", "�@"), array(ENV_CHR_SEPARATOR, ENV_CHR_SEPARATOR), $line[1]), 2);
			
			switch($cd_name) {
			case "�U��":
				$cd["TRNTYPE"] = ENV_STR_OFX_TRNTYPE_DIRECTDEP;
				break;
			case "����":
			case "�n����":
				$cd["TRNTYPE"] = ENV_STR_OFX_TRNTYPE_DEP;
				break;
			case "�r�a�h�n�C�u���b�h�a��":
			case "����":
				$cd["TRNTYPE"] = ENV_STR_OFX_TRNTYPE_XFER;
				break;
			case "�U��":
				$cd["TRNTYPE"] = ENV_STR_OFX_TRNTYPE_DEBIT;
				break;
			case "����":
				$cd["TRNTYPE"] = ENV_STR_OFX_TRNTYPE_INT;
				break;
			case "�`�s�l":
				$cd["TRNTYPE"] = ENV_STR_OFX_TRNTYPE_ATM;
				break;
			default:
				$cd["TRNTYPE"] = ENV_STR_OFX_TRNTYPE_OTHER;
				break;
			}
			
			// ���t���擾����
			$cd["DTPOSTED"] = parse_date($line[0]);
			
			// �ʔԂ𐶐�����
			if($cd["DTPOSTED"] == $cd_date) $cd_num++; else $cd_num = 0;
			
			// �g�����U�N�V�����ԍ��𐶐�����
			$cd["FITID"] = $cd["DTPOSTED"] . sprintf("%04d", (integer)$account["accttype"]) . "000" . sprintf("%05d", $cd_num);
			
			// �E�v���擾����
			$cd["NAME"] = mb_convert_string($line[1]);
			
			// ���z���擾����
			$cd["TRNAMT"] = (string)((double)parse_amount($line[3]) - (double)parse_amount($line[2]));
			
			// �c�����擾����
			$cds_balamt = (string)((double)parse_amount($line[4]));
			$cd["MEMO"] = mb_convert_string($line[5]);
			
			array_push($cds, $cd);
			$cd_date = $cd["DTPOSTED"];
		}
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
	$ret .= "<BALAMT>" . $cds_balamt . "</BALAMT>";
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