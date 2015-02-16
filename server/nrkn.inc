<?php
/*
nrkn.inc: ���{���R�[�h�E�L�[�s���O�E�l�b�g���[�N��HTML���OFX�t�@�C���𐶐�����
Copyright (C) 2012-2015 OFFICE OUTERGUY. All rights reserved.
mailto:contact@beatrek.com
Licensed under the GNU AGPLv3.
*/

// ���O�C�������擾����
$user = $ofxforms["userId"];
$pass = $ofxforms["password"];

$resp = array();

define("ENV_STR_ASSET_REINVESTMENT", "�ē���");
define("ENV_STR_ASSET_CONTRIBUTION", "���o");

// ���s���ԁi�^�C���A�E�g�j���Đݒ肷��
@set_time_limit(ENV_NUM_TIMEOUT);

// �z�[����ʂ��擾����
$method = "GET";
$uris = parse_uri($settings["home"]);
$query = "";
$cookie = "";
list($head, $body) = nrkn_http11($method, $uris, $query, $cookie);

// ���O�C����ʂ��擾����
$as = parse_tag($body, "a");
foreach($as as $a) if(strip_tags($a["innerHTML"]) == "���O�C��") {
	$method = "GET";
	$uris = parse_uri($a["href"], $uris);
	$query = "";
	$cookie = nrkn_update_cookie($head, $cookie);
	list($head, $body) = nrkn_http11($method, $uris, $query, $cookie);
	break;
}

// ���O�C������
$forms = parse_tag($body, "form");
if(count($forms) >= 1) {
	$inputs = parse_tag($forms[0]["innerHTML"], "input");
	$queries = array();
	foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
	$queries["userId"] = "userId=" . $user;
	$queries["password"] = "password=" . $pass;
	
	$head .= "Set-Cookie: cookiesCheck=true\r\n";
	
	$method = $forms[0]["method"];
	$uris = parse_uri($forms[0]["action"], $uris);
	$query = implode("&", $queries);
	$cookie = nrkn_update_cookie($head, $cookie);
	list($head, $body) = nrkn_http11($method, $uris, $query, $cookie);
}

// ���_�C���N�g����
$forms = parse_tag($body, "form");
if(count($forms) == 1) {
	$inputs = parse_tag($forms[0]["innerHTML"], "input");
	$queries = array();
	foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
	
	$method = $forms[0]["method"];
	$uris = parse_uri($forms[0]["action"], $uris);
	$query = implode("&", $queries);
	$cookie = nrkn_update_cookie($head, $cookie);
	list($head, $body) = nrkn_http11($method, $uris, $query, $cookie);
}

if(strpos($body, "�V�X�e�������e�i���X��") !== false || strpos($body, "�V�X�e�������e�i���X�̂���") !== false) {
	// �V�X�e�������e�i���X��ʂ̏ꍇ
	$resp["status"] = ENV_NUM_STATUS_MAINTENANCE;
	$resp["method"] = $method;
	$resp["uri"] = $uris["scheme"] . "://" . $uris["host"] . $uris["path"];
	$resp["query"] = $query;
	$resp["cookie"] = $cookie;
	$resp["head"] = $head;
	$resp["body"] = $body;
	$resp["ofx"] = generate_ofx($resp["status"]);
} else if(strpos($body, "�O�񂲗��p�i���O�C���j����") === false) {
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
	$marginbalance = "0";
	$availcash = "0";
	
	// ���s���ԁi�^�C���A�E�g�j���Đݒ肷��
	@set_time_limit(ENV_NUM_TIMEOUT);
	
	$acctid = "";
	$acctname = "";
	
	// td�^�O���p�[�X����
	$f = false;
	$tds = parse_tag($body, "td", true); // �ċA�I�Ɏ擾����
	foreach($tds as $td) {
		if($f == true) {
			$acctid = trim(strip_tags($td["innerHTML"]));
			$f = false;
		}
		if($td["innerHTML"] == "�����Ҕԍ�") $f = true;
	}
	
	if($acctid == "") $acctid = $user;
	
	// ���Y�]���z�Ɖ��ʂ��擾����
	$forms = parse_tag($body, "form");
	foreach($forms as $form) if($form["name"] == "W37S1040_Form") {
		$inputs = parse_tag($form["innerHTML"], "input");
		$queries = array();
		foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
		
		$method = $form["method"];
		$uris = parse_uri($form["action"], $uris);
		$query = implode("&", $queries);
		$cookie = nrkn_update_cookie($head, $cookie);
		list($head, $body) = nrkn_http11($method, $uris, $query, $cookie);
		break;
	}
	
	// td�^�O���p�[�X����
	$tds = parse_tag($body, "td", true); // �ċA�I�Ɏ擾����
	$securities = array();
	$bufs = array();
	foreach($tds as $td) switch($td["class"]) {
	case "s1040-t2-o1":
	case "s1040-t2-e1":
		$bufs["SECNAME"] = mb_convert_string(trim(strip_tags($td["innerHTML"]))); // ���i��
		break;
	case "s1040-t2-o3":
	case "s1040-t2-e3":
		$bufs["UNITS"] = parse_amount($td["innerHTML"]); // ����
		break;
	case "s1040-t2-o4":
	case "s1040-t2-e4":
		$bufs["UNITPRICE"] = parse_amount($td["innerHTML"]); // ����z
		break;
	case "s1040-t2-o5":
	case "s1040-t2-e5":
		$bufs["MKTVAL"] = parse_amount($td["innerHTML"]); // �]�����z
		break;
	case "s1040-t2-o7":
	case "s1040-t2-e7":
		$bufs["MEMO"] = parse_amount($td["innerHTML"]); // ���v
		break;
	case "s1040-t2-o9":
	case "s1040-t2-e9":
		$bufs["DTPRICEASOF"] = parse_date($td["innerHTML"]); // ���
		
		$bufs["UNIQUEIDTYPE"] = $settings["code"];
		$bufs["HELDINACCT"] = ENV_STR_OFX_TRNTYPE_CASH; // ����
		$bufs["POSTYPE"] = "LONG";
		$bufs["CATEGORY"] = ENV_STR_OFX_FUND; // �����M��
		$bufs["UNITPRICE"] = (string)round((double)$bufs["UNITPRICE"] / round((double)$bufs["UNITS"] * (double)$bufs["UNITPRICE"] / (double)$bufs["MKTVAL"]), 4);
		array_push($securities, $bufs);
		$bufs = array();
		break;
	default:
		break;
	}
	
	// ��������Ɖ��ʂ��擾����
	$forms = parse_tag($body, "form");
	foreach($forms as $form) if($form["name"] == "W37S1050_Head") {
		$inputs = parse_tag($form["innerHTML"], "input");
		$queries = array();
		foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
		
		$method = $form["method"];
		$uris = parse_uri($form["action"], $uris);
		$query = implode("&", $queries);
		$cookie = nrkn_update_cookie($head, $cookie);
		list($head, $body) = nrkn_http11($method, $uris, $query, $cookie);
		break;
	}
	
	// ���_�C���N�g����
	$forms = parse_tag($body, "form");
	foreach($forms as $form) if($form["name"] == "W37S1050_Form") {
		$inputs = parse_tag($form["innerHTML"], "input");
		$queries = array();
		foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
		$selects = parse_tag($form["innerHTML"], "select");
		foreach($selects as $select) {
			$options = parse_tag($select["innerHTML"], "option");
			foreach($options as $option) if($select["name"] != "" && $option["selected"] == "selected") $queries[$select["name"]] = urlencode($select["name"]) . "=" . urlencode($option["value"]);
		}
		$queries["lst_ddiv_yyyy_refBegin"] = "lst_ddiv_yyyy_refBegin=" . substr(ENV_STR_DATE_PASTDAY, 0, 4);
		$queries["lst_ddiv_mm_refBegin"] = "lst_ddiv_mm_refBegin=" . substr(ENV_STR_DATE_PASTDAY, 4, 2);
		$queries["lst_ddiv_yyyy_refEnd"] = "lst_ddiv_yyyy_refEnd=" . substr(ENV_STR_DATE_TODAY, 0, 4);
		$queries["lst_ddiv_mm_refEnd"] = "lst_ddiv_mm_refEnd=" . substr(ENV_STR_DATE_TODAY, 4, 2);
		$queries["chk_transType"] = "chk_transType=01&chk_transType=04"; // ���o�A���v���z�E�����p��
		
		$method = $form["method"];
		$uris = parse_uri($form["action"], $uris);
		$query = implode("&", $queries);
		$cookie = nrkn_update_cookie($head, $cookie);
		list($head, $body) = nrkn_http11($method, $uris, $query, $cookie);
		break;
	}
	
	$body_old = $body;
	$banks = array();
	$exchanges = array();
	
	// ���s���ԁi�^�C���A�E�g�j���Đݒ肷��
	@set_time_limit(ENV_NUM_TIMEOUT);
	
	// ���ו\����ʂ��擾����
	$forms = parse_tag($body, "form");
	$f = false;
	foreach($forms as $form) if($form["name"] == "W37S1060_Form") {
		if($f == false) {
			$f = true; // 1�ڂ̃t�H�[���͖��ׂłȂ����ߖ�������
			continue;
		}
		
		$inputs = parse_tag($form["innerHTML"], "input");
		$queries = array();
		foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
		
		$method = $form["method"];
		$uris = parse_uri($form["action"], $uris);
		$query = implode("&", $queries);
		$cookie = nrkn_update_cookie($head, $cookie);
		list($head, $body) = nrkn_http11($method, $uris, $query, $cookie);
		
		// td�^�O���p�[�X����
		$tds = parse_tag($body, "td", true); // �ċA�I�Ɏ擾����
		$bufs = array();
		foreach($tds as $td) switch($td["class"]) {
		// ���v���z
		case "s1110-t1-d1":
			$bufs["DTPOSTED"] = parse_date($td["innerHTML"]); // ���Z���E������
			break;
		case "s1110-t1-d2":
			$bufs["NAME"] = mb_convert_string(trim(strip_tags($td["innerHTML"]))); // ����敪
			break;
		case "s1110-t2-o1":
		case "s1110-t2-e1":
			$bufs["MEMO"] = mb_convert_string(trim(strip_tags($td["innerHTML"]))); // ���i��
			break;
		case "s1110-t2-o5":
		case "s1110-t2-e5":
			$bufs["TRNAMT"] = parse_amount($td["innerHTML"]); // ���z���z�E����
			
			$bufs["TRNTYPE"] = ENV_STR_OFX_TRNTYPE_DIV; // �z����
			$bufs["SUBACCTFUND"] = ENV_STR_OFX_CASH; // ����
			array_push($banks, $bufs);
			$bufs = array();
			break;
		
		// ���v���z�i�ē����j
		case "s1110-t3-o1":
		case "s1110-t3-e1":
			$bufs["SECNAME"] = mb_convert_string(trim(strip_tags($td["innerHTML"]))); // ���i��
			break;
		case "s1110-t3-o8":
		case "s1110-t3-e8":
			$bufs["DTTRADE"] = parse_date($td["innerHTML"]); // ����
			break;
		case "s1110-t3-o3":
		case "s1110-t3-e3":
			$memo = trim(strip_tags($td["innerHTML"])); // ���l
			if($memo != "") $memo = ENV_CHR_CONCATENATOR . $memo;
			$bufs["MEMO"] = mb_convert_string(ENV_STR_ASSET_REINVESTMENT . $memo); // �ē���
			break;
		case "s1110-t3-o4":
		case "s1110-t3-e4":
			$bufs["UNITS"] = parse_amount($td["innerHTML"]); // �w������
			break;
		case "s1110-t3-o5":
		case "s1110-t3-e5":
			$bufs["UNITPRICE"] = parse_amount($td["innerHTML"]); // �w���P��
			break;
		case "s1110-t3-o6":
		case "s1110-t3-e6":
			$bufs["TOTAL"] = (string)(-1 * (double)parse_amount($td["innerHTML"])); // �w�����z
			break;
		case "s1110-t3-o7":
		case "s1110-t3-e7":
			if(trim(strip_tags($td["innerHTML"])) == "����") {
				$bufs["CATEGORY"] = ENV_STR_OFX_FUND; // ���ׂĂ𓊎��M���ƌ��Ȃ�
				$bufs["SUBACCTSEC"] = ENV_STR_OFX_CASH; // ����
				$bufs["SUBACCTSEC"] = ENV_STR_OFX_CASH; // ����
				$bufs["SUBACCTFUND"] = ENV_STR_OFX_CASH; // ����
				$bufs["BUYTYPE"] = ENV_STR_OFX_BUY; // ���t
				if((double)$bufs["UNITS"] != 0 && (double)$bufs["MKTVAL"] != 0) {
					$bufs["UNITPRICE"] = (string)round((double)$bufs["UNITPRICE"] / round((double)$bufs["UNITS"] * (double)$bufs["UNITPRICE"] / (double)$bufs["MKTVAL"]), 4);
					array_push($exchanges, $bufs);
				}
			}
			$bufs = array();
			break;
		
		// ���o�i�����j
		case "s1070-t1-d2":
			$cd_date = parse_date($td["innerHTML"]); // ���o��
			break;
		case "s1070-t1-h3":
			if($td["innerHTML"] != "���o��" && strpos($td["innerHTML"], "�{") === false) $bufs["NAME"] = mb_convert_string(str_replace(array("�i�P�j", "�i�Q�j", "�z"), array("", "", ""), trim(strip_tags($td["innerHTML"])))); // ���o����
			break;
		case "s1070-t1-d3":
			if($bufs["NAME"] != "") {
				$bufs["TRNAMT"] = parse_amount($td["innerHTML"]); // �|���z
				
				$bufs["DTPOSTED"] = $cd_date;
				$bufs["MEMO"] = mb_convert_string(ENV_STR_ASSET_CONTRIBUTION);
				$bufs["TRNTYPE"] = ENV_STR_OFX_TRNTYPE_OTHER;
				$bufs["SUBACCTFUND"] = ENV_STR_OFX_CASH; // ����
				array_push($banks, $bufs);
				$bufs = array();
			}
			break;
		
		// ���o�i�����j
		case "s1070-t2-o1":
		case "s1070-t2-e1":
			$bufs["SECNAME"] = mb_convert_string(trim(strip_tags($td["innerHTML"]))); // ���i��
			break;
		case "s1070-t2-o8":
		case "s1070-t2-e8":
			$bufs["DTTRADE"] = parse_date($td["innerHTML"]); // ����
			break;
		case "s1070-t2-o3":
		case "s1070-t2-e3":
			$memo = trim(strip_tags($td["innerHTML"])); // ���l
			if($memo != "") $memo = ENV_CHR_CONCATENATOR . $memo;
			$bufs["MEMO"] = mb_convert_string(ENV_STR_ASSET_CONTRIBUTION . $memo); // ���o
			break;
		case "s1070-t2-o4":
		case "s1070-t2-e4":
			$bufs["TOTAL"] = (string)(-1 * (double)parse_amount($td["innerHTML"])); // �w�����z
			break;
		case "s1070-t2-o5":
		case "s1070-t2-e5":
			$bufs["UNITPRICE"] = parse_amount($td["innerHTML"]); // �w���P��
			break;
		case "s1070-t2-o6":
		case "s1070-t2-e6":
			$bufs["UNITS"] = parse_amount($td["innerHTML"]); // �w������
			break;
			
		case "s1070-t2-o7":
		case "s1070-t2-e7":
			if(trim(strip_tags($td["innerHTML"])) == "����") {
				$bufs["CATEGORY"] = ENV_STR_OFX_FUND; // �����M��
				$bufs["SUBACCTSEC"] = ENV_STR_OFX_CASH; // ����
				$bufs["SUBACCTFUND"] = ENV_STR_OFX_CASH; // ����
				$bufs["UNIQUEIDTYPE"] = $settings["code"];
				$bufs["BUYTYPE"] = ENV_STR_OFX_BUY; // ���t
				$bufs["UNITPRICE"] = (string)round((double)$bufs["UNITPRICE"] / round((double)$bufs["UNITS"] * (double)$bufs["UNITPRICE"] / (double)$bufs["TOTAL"] * -1), 4);
				array_push($exchanges, $bufs);
			}
			$bufs = array();
			break;
		
		default:
			break;
		}
		
		$body = $body_old;
	}
	
	// ���s���ԁi�^�C���A�E�g�j���Đݒ肷��
	@set_time_limit(ENV_NUM_TIMEOUT);
	
	// �v�������Ɖ��ʂ��擾����
	$forms = parse_tag($body, "form");
	foreach($forms as $form) if($form["name"] == "W37S1130_Head") {
		$inputs = parse_tag($form["innerHTML"], "input");
		$queries = array();
		foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
		
		$method = $form["method"];
		$uris = parse_uri($form["action"], $uris);
		$query = implode("&", $queries);
		$cookie = nrkn_update_cookie($head, $cookie);
		list($head, $body) = nrkn_http11($method, $uris, $query, $cookie);
		break;
	}
	
	$investments = array();
	$prefix = "";
	
	// td�^�O���p�[�X����
	$tds = parse_tag($body, "td", true); // �ċA�I�Ɏ擾����
	foreach($tds as $td) switch($td["class"]) {
	case "s0000-t3-d1":
		if($acctname == "") $acctname = mb_convert_string(ENV_CHR_CONCATENATOR . trim(strip_tags($td["innerHTML"]))); // �v������
		break;
	case "s0000-t3-d2":
		if($prefix == "") $prefix = trim(strip_tags($td["innerHTML"])); // �v�����ԍ�
		break;
	case "s1130-t3-o1":
	case "s1130-t3-e1":
		$uniqueid = $prefix . trim(strip_tags($td["innerHTML"])); // ���i�R�[�h
		break;
	case "s1130-t3-o2":
	case "s1130-t3-e2":
		$secname = mb_convert_string(trim(strip_tags($td["innerHTML"]))); // ���i��
		$investments[$uniqueid] = $secname;
		break;
	
	default:
		break;
	}
	
	// ���s���ԁi�^�C���A�E�g�j���Đݒ肷��
	@set_time_limit(ENV_NUM_TIMEOUT);
	
	// ���O�A�E�g����
	$forms = parse_tag($body, "form");
	foreach($forms as $form) if($form["name"] == "W37S0020_Head") {
		$inputs = parse_tag($form["innerHTML"], "input");
		$queries = array();
		foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
		
		$method = $form["method"];
		$uris = parse_uri($form["action"], $uris);
		$query = implode("&", $queries);
		$cookie = nrkn_update_cookie($head, $cookie);
		list($head, $body) = nrkn_http11($method, $uris, $query, $cookie);
		break;
	}
	
	// INVTRANLIST�i�����j
	$invtranlist = "";
	$invtranlist .= "<DTSTART>" . ENV_STR_DATE_PASTDAY . ENV_STR_OFX_TZ . "</DTSTART>";
	$invtranlist .= "<DTEND>" . ENV_STR_DATE_TODAY . ENV_STR_OFX_TZ . "</DTEND>";
	$invtranlist .= "\r\n";
	
	$cd_date = "";
	$cd_num = 0;
	
	foreach($exchanges as $exchange) {
		$uniqueid = array_search($exchange["SECNAME"], $investments);
		if($uniqueid === false) $uniqueid = mb_convert_uniqueid($exchange["SECNAME"]);
		
		if($exchange["DTTRADE"] == $cd_date) $cd_num++; else $cd_num = 0;
		$fitid = $exchange["DTTRADE"] . "0000001" . sprintf("%05d", $cd_num);
		
		$invtranlist .= "<" . $exchange["BUYTYPE"] . $exchange["CATEGORY"] . ">";
		$invtranlist .= "<INV" . $exchange["BUYTYPE"] . ">";
		$invtranlist .= "<INVTRAN>";
		$invtranlist .= "<FITID>" . $fitid . "</FITID>";
		$invtranlist .= "<DTTRADE>" . $exchange["DTTRADE"] . ENV_STR_OFX_TZ . "</DTTRADE>";
		$invtranlist .= "</INVTRAN>";
		$invtranlist .= "<SECID><UNIQUEID>" . $uniqueid . "</UNIQUEID><UNIQUEIDTYPE>" . $exchange["UNIQUEIDTYPE"] . "</UNIQUEIDTYPE></SECID>";
		$invtranlist .= "<UNITS>" . $exchange["UNITS"] . "</UNITS>";
		$invtranlist .= "<UNITPRICE>" . $exchange["UNITPRICE"] . "</UNITPRICE>";
		$invtranlist .= "<TOTAL>" . $exchange["TOTAL"] . "</TOTAL>";
		$invtranlist .= "<SUBACCTSEC>" . $exchange["SUBACCTSEC"] . "</SUBACCTSEC>";
		$invtranlist .= "<SUBACCTFUND>" . $exchange["SUBACCTFUND"] . "</SUBACCTFUND>";
		$invtranlist .= "</INV" . $exchange["BUYTYPE"] . ">";
		$invtranlist .= "<BUYTYPE>" . $exchange["BUYTYPE"] . "</BUYTYPE>";
		$invtranlist .= "</" . $exchange["BUYTYPE"] . $exchange["CATEGORY"] . ">";
		$invtranlist .= "\r\n";
		
		$cd_date = $exchange["DTTRADE"];
	}
	
	$cd_date = "";
	$cd_num = 0;
	
	// INVTRANLIST�i�����j
	foreach($banks as $bank) {
		if($bank["DTPOSTED"] == $cd_date) $cd_num++; else $cd_num = 0;
		$fitid = $bank["DTPOSTED"] . "0000000" . sprintf("%05d", $cd_num);
		
		$invtranlist .= "<INVBANKTRAN>";
		$invtranlist .= "<STMTTRN>";
		$invtranlist .= "<TRNTYPE>" . $bank["TRNTYPE"] . "</TRNTYPE>";
		$invtranlist .= "<DTPOSTED>" . $bank["DTPOSTED"] . ENV_STR_OFX_TZ . "</DTPOSTED>";
		$invtranlist .= "<TRNAMT>" . $bank["TRNAMT"] . "</TRNAMT>";
		$invtranlist .= "<FITID>" . $fitid . "</FITID>";
		$invtranlist .= "<NAME>" . $bank["NAME"] . "</NAME>";
		$invtranlist .= "<MEMO>" . $bank["MEMO"] . "</MEMO>";
		$invtranlist .= "</STMTTRN>";
		$invtranlist .= "<SUBACCTFUND>" . $bank["SUBACCTFUND"] . "</SUBACCTFUND>";
		$invtranlist .= "</INVBANKTRAN>";
		$invtranlist .= "\r\n";
		
		$cd_date = $bank["DTPOSTED"];
	}
	
	$invposlist = "";
	$seclist = "";
	
	// INVPOSLIST
	foreach($securities as $security) {
		$uniqueid = array_search($security["SECNAME"], $investments);
		if($uniqueid === false) $uniqueid = mb_convert_uniqueid($security["SECNAME"]);
		
		$invposlist .= "<POS" . $security["CATEGORY"] . "><INVPOS>";
		$invposlist .= "<SECID><UNIQUEID>" . $uniqueid . "</UNIQUEID><UNIQUEIDTYPE>" . $security["UNIQUEIDTYPE"] . "</UNIQUEIDTYPE></SECID>";
		$invposlist .= "<HELDINACCT>" . $security["HELDINACCT"] . "</HELDINACCT>";
		$invposlist .= "<POSTYPE>" . $security["POSTYPE"] . "</POSTYPE>";
		$invposlist .= "<UNITS>" . $security["UNITS"] . "</UNITS>";
		$invposlist .= "<UNITPRICE>" . $security["UNITPRICE"] . "</UNITPRICE>";
		$invposlist .= "<MKTVAL>" . $security["MKTVAL"] . "</MKTVAL>";
		$invposlist .= "<DTPRICEASOF>" . $security["DTPRICEASOF"] . ENV_STR_OFX_TZ . "</DTPRICEASOF>";
		$invposlist .= "<MEMO>" . $security["MEMO"] . "</MEMO>";
		$invposlist .= "</INVPOS></POS" . $security["CATEGORY"] . ">";
		$invposlist .= "\r\n";
		
		// SECLIST
		$seclist .= "<" . $security["CATEGORY"] . "INFO><SECINFO>";
		$seclist .= "<SECID><UNIQUEID>" . $uniqueid . "</UNIQUEID><UNIQUEIDTYPE>" . $security["UNIQUEIDTYPE"] . "</UNIQUEIDTYPE></SECID>";
		$seclist .= "<SECNAME>" . $security["SECNAME"] . "</SECNAME>";
		$seclist .= "</SECINFO></" . $security["CATEGORY"] . "INFO>";
		$seclist .= "\r\n";
	}
	
	$invstmtmsgsrsv1 = "";
	$invstmtmsgsrsv1 .= "<INVSTMTMSGSRSV1>";
	$invstmtmsgsrsv1 .= "\r\n";
	$invstmtmsgsrsv1 .= "<INVSTMTTRNRS>";
	$invstmtmsgsrsv1 .= "\r\n";
	$invstmtmsgsrsv1 .= "<TRNUID>0</TRNUID>";
	$invstmtmsgsrsv1 .= "\r\n";
	$invstmtmsgsrsv1 .= "<STATUS><CODE>0</CODE><SEVERITY>INFO</SEVERITY></STATUS>";
	$invstmtmsgsrsv1 .= "\r\n";
	$invstmtmsgsrsv1 .= "<INVSTMTRS>";
	$invstmtmsgsrsv1 .= "\r\n";
	$invstmtmsgsrsv1 .= "<DTASOF>" . ENV_STR_DATE_TODAY . ENV_STR_OFX_TZ . "</DTASOF>";
	$invstmtmsgsrsv1 .= "\r\n";
	$invstmtmsgsrsv1 .= "<CURDEF>" . ENV_STR_OFX_CURRENCY_JPY . "</CURDEF>";
	$invstmtmsgsrsv1 .= "\r\n";
	$invstmtmsgsrsv1 .= "<INVACCTFROM>";
	$invstmtmsgsrsv1 .= "<BROKERID>" . $settings["code"] . "</BROKERID>";
	$invstmtmsgsrsv1 .= "<ACCTID>" . $acctid . "</ACCTID>";
	$invstmtmsgsrsv1 .= "</INVACCTFROM>";
	$invstmtmsgsrsv1 .= "\r\n";
	$invstmtmsgsrsv1 .= "<INVTRANLIST>";
	$invstmtmsgsrsv1 .= "\r\n";
	$invstmtmsgsrsv1 .= $invtranlist;
	$invstmtmsgsrsv1 .= "</INVTRANLIST>";
	$invstmtmsgsrsv1 .= "\r\n";
	$invstmtmsgsrsv1 .= "<INVPOSLIST>";
	$invstmtmsgsrsv1 .= "\r\n";
	$invstmtmsgsrsv1 .= $invposlist;
	$invstmtmsgsrsv1 .= "</INVPOSLIST>";
	$invstmtmsgsrsv1 .= "\r\n";
	$invstmtmsgsrsv1 .= "<INVBAL>";
	$invstmtmsgsrsv1 .= "<AVAILCASH>0</AVAILCASH>";
	$invstmtmsgsrsv1 .= "<MARGINBALANCE>0</MARGINBALANCE>";
	$invstmtmsgsrsv1 .= "<SHORTBALANCE>0</SHORTBALANCE>";
	$invstmtmsgsrsv1 .= "</INVBAL>";
	$invstmtmsgsrsv1 .= "\r\n";
	$invstmtmsgsrsv1 .= "<MKTGINFO>" . mb_convert_string($settings["name"]) . $acctname . "</MKTGINFO>";
	$invstmtmsgsrsv1 .= "\r\n";
	$invstmtmsgsrsv1 .= "</INVSTMTRS>";
	$invstmtmsgsrsv1 .= "\r\n";
	$invstmtmsgsrsv1 .= "</INVSTMTTRNRS>";
	$invstmtmsgsrsv1 .= "\r\n";
	$invstmtmsgsrsv1 .= "</INVSTMTMSGSRSV1>";
	$invstmtmsgsrsv1 .= "\r\n";
	$invstmtmsgsrsv1 .= "<SECLISTMSGSRSV1>";
	$invstmtmsgsrsv1 .= "\r\n";
	$invstmtmsgsrsv1 .= "<SECLISTTRNRS>";
	$invstmtmsgsrsv1 .= "\r\n";
	$invstmtmsgsrsv1 .= "<TRNUID>0</TRNUID>";
	$invstmtmsgsrsv1 .= "\r\n";
	$invstmtmsgsrsv1 .= "<STATUS><CODE>0</CODE><SEVERITY>INFO</SEVERITY></STATUS>";
	$invstmtmsgsrsv1 .= "\r\n";
	$invstmtmsgsrsv1 .= "</SECLISTTRNRS>";
	$invstmtmsgsrsv1 .= "\r\n";
	$invstmtmsgsrsv1 .= "<SECLIST>";
	$invstmtmsgsrsv1 .= "\r\n";
	$invstmtmsgsrsv1 .= $seclist;
	$invstmtmsgsrsv1 .= "</SECLIST>";
	$invstmtmsgsrsv1 .= "\r\n";
	$invstmtmsgsrsv1 .= "</SECLISTMSGSRSV1>";
	$invstmtmsgsrsv1 .= "\r\n";
	
	$resp["status"] = ENV_NUM_STATUS_SUCCESS;
	$resp["ofx"] = generate_ofx($resp["status"], $invstmtmsgsrsv1);
}
return $resp;

function nrkn_http11($method, $uris, $query = "", $cookie = "") {
	$ret = "INVALID HOST";
	if(preg_match("/\.nrkn\.co\.jp$/", $uris["host"]) > 0) $ret = http11(strtoupper($method), $uris["scheme"], $uris["host"], 0, $uris["path"], $query, "", $cookie, true, ENV_PRODUCT_UA . " Windows NT"); // ���O�C������User-Agent���`�F�b�N�����
	return explode("\r\n\r\n", $ret, 2);
}

function nrkn_update_cookie($head, $cookie) {
	return update_cookie(array("cookiesCheck", "JSESSIONID"), parse_header($head, "set-cookie"), $cookie);
}

?>