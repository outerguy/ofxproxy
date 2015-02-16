<?php
/*
sbisec.inc: SBI�،���CSV���OFX�t�@�C���𐶐�����
Copyright (C) 2010-2015 OFFICE OUTERGUY. All rights reserved.
mailto:contact@beatrek.com
Licensed under the GNU AGPLv3.
*/

// ���O�C�������擾����
$user = $ofxforms["user_id"];
$pass = $ofxforms["user_password"];

$resp = array();

define("ENV_STR_NAME_NETBK", "�U�ց@�r�a�h�،�");

define("ENV_STR_ASSET_STOCK_NAME", "����");
define("ENV_STR_ASSET_STOCK_BUY", "��");
define("ENV_STR_ASSET_STOCK_SELL", "��");
define("ENV_STR_ASSET_FUND_NAME", "���M");
define("ENV_STR_ASSET_FUND_BUY", "���t");
define("ENV_STR_ASSET_FUND_SELL", "���");
define("ENV_STR_ASSET_FUND_PURCHASE", "����");
define("ENV_STR_ASSET_FUND_DIV", "���z���ē���");
define("ENV_STR_ASSET_CASH_DEPOSIT", "����");
define("ENV_STR_ASSET_CASH_PAYMENT", "�o��");

define("ENV_NUM_MAX_DETAIL", 10000);

// ���s���ԁi�^�C���A�E�g�j���Đݒ肷��
@set_time_limit(ENV_NUM_TIMEOUT);

// �z�[����ʂ��擾����
$method = "GET";
$uris = parse_uri($settings["home"]);
$query = "";
$cookie = "";
list($head, $body) = sbisec_http11($method, $uris, $query, $cookie);

// ���_�C���N�g����
$retry = 0;
while($retry++ < ENV_NUM_HTTP11_RETRY) {
	$locations = parse_header($head, "location");
	if(count($locations) > 0) {
		$method = "GET";
		$uris = parse_uri($locations[0], $uris);
		$query = "";
		$cookie = sbisec_update_cookie($head, $cookie);
		list($head, $body) = sbisec_http11($method, $uris, $query, $cookie);
	}
}

// ���O�C������
$forms = parse_tag($body, "form");
$c = parse_tag_search($forms, "name", "form_login");
if($c != -1) {
	$inputs = parse_tag($forms[$c]["innerHTML"], "input");
	$queries = array();
	foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
	$queries["JS_FLG"] = "JS_FLG=1";
	$queries["BW_FLG"] = "BW_FLG=ie%2C8";
	$queries["user_id"] = "user_id=" . $user;
	$queries["user_password"] = "user_password=" . $pass;
	
	$method = $forms[$c]["method"];
	$uris = parse_uri($forms[$c]["action"], $uris);
	$query = implode("&", $queries);
	$cookie = sbisec_update_cookie($head, $cookie);
	list($head, $body) = sbisec_http11($method, $uris, $query, $cookie);
}

// ���_�C���N�g����
$forms = parse_tag($body, "form");
$c = parse_tag_search($forms, "name", "formSwitch");
if($c != -1) {
	$inputs = parse_tag($forms[$c]["innerHTML"], "input");
	$queries = array();
	foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
	
	$method = $forms[$c]["method"];
	$uris = parse_uri($forms[$c]["action"], $uris);
	$query = implode("&", $queries);
	$cookie = sbisec_update_cookie($head, $cookie);
	list($head, $body) = sbisec_http11($method, $uris, $query, $cookie);
}

// ���_�C���N�g����i�V�X�e�������e�i���X���j
$retry = 0;
while($retry++ < ENV_NUM_HTTP11_RETRY) {
	$locations = parse_header($head, "location");
	if(count($locations) > 0) {
		$method = "GET";
		$uris = parse_uri($locations[0], $uris);
		$query = "";
		$cookie = sbisec_update_cookie($head, $cookie);
		list($head, $body) = sbisec_http11($method, $uris, $query, $cookie);
	}
}

// ���_�C���N�g����i�V�X�e�������e�i���X���j
$scripts = parse_tag($body, "script");
foreach($scripts as $script) if(preg_match("/window\.location[\s\t]*=[\s\t]*\"(.*?)\";/i", $script["innerHTML"], $matches) > 0) {
	$method = "GET";
	$uris = parse_uri($matches[1], $uris);
	$query = "";
	$cookie = sbisec_update_cookie($head, $cookie);
	list($head, $body) = sbisec_http11($method, $uris, $query, $cookie);
	break;
}

// ���_�C���N�g����i�V�X�e�������e�i���X���j
$metas = parse_tag($body, "meta");
$c = parse_tag_search($metas, "http-equiv", "refresh");
if($c != -1) {
	list($dummy, $uri) = explode("=", $metas[$c]["content"], 2);
	
	$method = "GET";
	$uris = parse_uri(trim($uri), $uris);
	$query = "";
	$cookie = sbisec_update_cookie($head, $cookie);
	list($head, $body) = sbisec_http11($method, $uris, $query, $cookie);
}

if(strpos($body, "���O�C����̑S�ẴT�[�r�X���~") !== false) {
	// �V�X�e�������e�i���X��ʂ̏ꍇ
	$resp["status"] = ENV_NUM_STATUS_MAINTENANCE;
	$resp["method"] = $method;
	$resp["uri"] = $uris["scheme"] . "://" . $uris["host"] . $uris["path"];
	$resp["query"] = $query;
	$resp["cookie"] = $cookie;
	$resp["head"] = $head;
	$resp["body"] = $body;
	$resp["ofx"] = generate_ofx($resp["status"]);
} else if(strpos($body, "�ŏI���O�C��") === false) {
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
	
	// �����Ǘ���ʂ��擾����
	$as = parse_tag($body, "a");
	foreach($as as $a) if(parse_tag_search(parse_tag($a["innerHTML"], "img"), "alt", "�����Ǘ�") != -1) {
		$method = "GET";
		$uris = parse_uri($a["href"], $uris);
		$query = "";
		$cookie = sbisec_update_cookie($head, $cookie);
		list($head, $body) = sbisec_http11($method, $uris, $query, $cookie);
		break;
	}
	
	// �����ԍ����擾����
	if(preg_match("/([Z1][0-9]{2})\-(0[0-9]{6})/", $body, $matches) > 0) $acctid = $matches[1] . "-" . $matches[2];
	
	// ���������擾����
	$trs = parse_tag($body, "tr", true); // �ċA�I�Ɏ擾����
	foreach($trs as $tr) {
		$tds = parse_tag($tr["innerHTML"], "td");
		if(count($tds) == 2) {
			$k = str_replace("&nbsp;", "", trim(strip_tags($tds[0]["innerHTML"])));
			$v = str_replace("&nbsp;", "", trim(strip_tags($tds[1]["innerHTML"])));
			switch($k) {
			case "SBI�،�������":
				// SBI�،����������擾����
				$marginbalance = parse_amount($v);
				break;
			case "�����c�����i���v�j":
				// �����c�����i���v�j���擾����
				$availcash = parse_amount($v);
				break;
			default:
				break;
			}
		}
	}
	
	// �ۗL�،���ʂ��擾����
	$areas = parse_tag($body, "area");
	$c = parse_tag_search($areas, "alt", "�ۗL�،�");
	if($c != -1) {
		$method = "GET";
		$uris = parse_uri($areas[$c]["href"], $uris);
		$query = "";
		$cookie = sbisec_update_cookie($head, $cookie);
		list($head, $body) = sbisec_http11($method, $uris, $query, $cookie);
	}
	
	// �ۗL�����M���̃R�[�h���擾����
	$funds = array();
	$c = 0;
	$as = parse_tag($body, "a");
	foreach($as as $a) if(preg_match("/_ActionID=detail_tousin.*?fund_sec_code=([0-9A-Z]+)/", $a["href"], $matches) > 0) {
		$funds["code"][$c] = (strlen($matches[1]) == 9? substr($matches[1], 1): $matches[1]);
		$funds["name"][$c] = trim(strip_tags($a["innerHTML"]));
		$c++;
	}
	
	$body_old = $body;
	$securities = array();
	
	// CSV�t�@�C�����_�E�����[�h����
	$tds = parse_tag($body, "td");
	foreach($tds as $td) {
		$as = parse_tag($td["innerHTML"], "a");
		$c = parse_tag_search($as, "innerHTML", "CSV�_�E�����[�h");
		if($c != -1) {
			$method = "GET";
			$uris = parse_uri($as[$c]["href"], $uris);
			$query = "";
			$cookie = sbisec_update_cookie($head, $cookie);
			list($head, $body) = sbisec_http11($method, $uris, $query, $cookie);
			
			$securities = parse_csv($body);
			break;
		}
	}
	
	$body = $body_old;
	
	// ���������ʂ��擾����
	$as = parse_tag($body, "a");
	$c = parse_tag_search($as, "innerHTML", "�������");
	if($c != -1) {
		$method = "GET";
		$uris = parse_uri($as[$c]["href"], $uris);
		$query = "";
		$cookie = sbisec_update_cookie($head, $cookie);
		list($head, $body) = sbisec_http11($method, $uris, $query, $cookie);
	}
	
	// ���_�C���N�g����
	$forms = parse_tag($body, "form");
	$c = parse_tag_search($forms, "name", "FORM");
	if($c != -1) {
		$inputs = parse_tag($forms[$c]["innerHTML"], "input");
		$queries = array();
		foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
		$selects = parse_tag($forms[$c]["innerHTML"], "select");
		foreach($selects as $select) {
			$options = parse_tag($select["innerHTML"], "option");
			foreach($options as $option) if($select["name"] != "" && $option["selected"] == "selected") $queries[$select["name"]] = urlencode($select["name"]) . "=" . urlencode($option["value"]);
		}
		$queries["sec_kbn"] = "sec_kbn=++"; // ���i�w��: "  "=���ׂĂ̏��i
		$queries["ref_from_yyyy"] = "ref_from_yyyy=" . substr(ENV_STR_DATE_PASTDAY, 0, 4);
		$queries["ref_from_mm"] = "ref_from_mm=" . substr(ENV_STR_DATE_PASTDAY, 4, 2);
		$queries["ref_from_dd"] = "ref_from_dd=" . substr(ENV_STR_DATE_PASTDAY, 6, 2);
		$queries["ref_to_yyyy"] = "ref_to_yyyy=" . substr(ENV_STR_DATE_TODAY, 0, 4);
		$queries["ref_to_mm"] = "ref_to_mm=" . substr(ENV_STR_DATE_TODAY, 4, 2);
		$queries["ref_to_dd"] = "ref_to_dd=" . substr(ENV_STR_DATE_TODAY, 6, 2);
		$queries["max_cnt"] = "max_cnt=200";
		
		$method = $forms[$c]["method"];
		$uris = parse_uri($forms[$c]["action"], $uris);
		$query = implode("&", $queries);
		$cookie = sbisec_update_cookie($head, $cookie);
		list($head, $body) = sbisec_http11($method, $uris, $query, $cookie);
	}
	
	$body_old = $body;
	$exchanges = array();
	
	// CSV�t�@�C�����_�E�����[�h����
	$tds = parse_tag($body, "td");
	foreach($tds as $td) {
		$as = parse_tag($td["innerHTML"], "a");
		$c = parse_tag_search($as, "innerHTML", "CSV�_�E�����[�h");
		if($c != -1) {
			if(preg_match("/(number_to=)[0-9]+/", $as[$c]["href"], $matches) > 0) $as[$c]["href"] = str_replace($matches[0], $matches[1] . (string)ENV_NUM_MAX_DETAIL, $as[$c]["href"]);
			
			$method = "GET";
			$uris = parse_uri($as[$c]["href"], $uris);
			$query = "";
			$cookie = sbisec_update_cookie($head, $cookie);
			list($head, $body) = sbisec_http11($method, $uris, $query, $cookie);
			
			$exchanges = parse_csv($body);
			break;
		}
	}
	
	$body = $body_old;
	
	// ���o���E�U�։�ʂ��擾����
	$as = parse_tag($body, "a");
	foreach($as as $a) if(parse_tag_search(parse_tag($a["innerHTML"], "img"), "alt", "���o���E�U��") != -1) {
		$method = "GET";
		$uris = parse_uri($a["href"], $uris);
		$query = "";
		$cookie = sbisec_update_cookie($head, $cookie);
		list($head, $body) = sbisec_http11($method, $uris, $query, $cookie);
		break;
	}
	
	// ���_�C���N�g����
	$forms = parse_tag($body, "form");
	$c = parse_tag_search($forms, "name", "formSwitch");
	if($c != -1) {
		$inputs = parse_tag($forms[$c]["innerHTML"], "input");
		$queries = array();
		foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
		
		$method = $forms[$c]["method"];
		$uris = parse_uri($forms[$c]["action"], $uris);
		$query = implode("&", $queries);
		$cookie = sbisec_update_cookie($head, $cookie);
		list($head, $body) = sbisec_http11($method, $uris, $query, $cookie);
	}
	
	// ���o�����׉�ʂ��擾����
	$as = parse_tag($body, "a");
	$c = parse_tag_search($as, "innerHTML", "���o������");
	if($c != -1) {
		$method = "GET";
		$uris = parse_uri($as[$c]["href"], $uris);
		$query = "";
		$cookie = sbisec_update_cookie($head, $cookie);
		list($head, $body) = sbisec_http11($method, $uris, $query, $cookie);
	}
	
	// ���_�C���N�g����
	$forms = parse_tag($body, "form");
	$c = parse_tag_search($forms, "name", "formSwitch");
	if($c != -1) {
		$inputs = parse_tag($forms[$c]["innerHTML"], "input");
		$queries = array();
		foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
		
		$method = $forms[$c]["method"];
		$uris = parse_uri($forms[$c]["action"], $uris);
		$query = implode("&", $queries);
		$cookie = sbisec_update_cookie($head, $cookie);
		list($head, $body) = sbisec_http11($method, $uris, $query, $cookie);
	}
	
	// ���_�C���N�g����
	$retry = 0;
	while($retry++ < ENV_NUM_HTTP11_RETRY) {
		$locations = parse_header($head, "location");
		if(count($locations) > 0) {
			$method = "GET";
			$uris = parse_uri($locations[0], $uris);
			$query = "";
			$cookie = sbisec_update_cookie($head, $cookie);
			list($head, $body) = sbisec_http11($method, $uris, $query, $cookie);
		}
	}
	
	$body_old = $body;
	$cashs = array();
	
	// CSV�t�@�C�����_�E�����[�h����
	$forms = parse_tag($body, "form");
	$c = parse_tag_search($forms, "id", "detailInquiryForm");
	if($c != -1) {
		$forms[$c]["action"] = "/marble/deposit/detail/inquiry/detailInquiryCsvDownload.do";
		$inputs = parse_tag($forms[$c]["innerHTML"], "input");
		$queries = array();
		foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
		$selects = parse_tag($forms[$c]["innerHTML"], "select");
		foreach($selects as $select) {
			$options = parse_tag($select["innerHTML"], "option");
			foreach($options as $option) if($select["name"] != "" && $option["selected"] == "selected") $queries[$select["name"]] = urlencode($select["name"]) . "=" . urlencode($option["value"]);
		}
		$queries["in_pay_dep_kbn"] = "in_pay_dep_kbn=0";
		$queries["in_v_list_count"] = "in_v_list_count=200";
		$queries["in_kkn_sti_kbn"] = "in_kkn_sti_kbn=1";
		$queries["in_kkn_sti_from_yyyy"] = "in_kkn_sti_from_yyyy=" . substr(ENV_STR_DATE_PASTDAY, 0, 4);
		$queries["in_kkn_sti_from_mm"] = "in_kkn_sti_from_mm=" . substr(ENV_STR_DATE_PASTDAY, 4, 2);
		$queries["in_kkn_sti_from_dd"] = "in_kkn_sti_from_dd=" . substr(ENV_STR_DATE_PASTDAY, 6, 2);
		$queries["in_kkn_sti_to_yyyy"] = "in_kkn_sti_to_yyyy=" . substr(ENV_STR_DATE_TODAY, 0, 4);
		$queries["in_kkn_sti_to_mm"] = "in_kkn_sti_to_mm=" . substr(ENV_STR_DATE_TODAY, 4, 2);
		$queries["in_kkn_sti_to_dd"] = "in_kkn_sti_to_dd=" . substr(ENV_STR_DATE_TODAY, 6, 2);
		$queries["action_type"] = "action_type=1";
		$queries["dispRows"] = "dispRows=200";
		
		$method = $forms[$c]["method"];
		$uris = parse_uri($forms[$c]["action"], $uris);
		$query = implode("&", $queries);
		$cookie = sbisec_update_cookie($head, $cookie);
		list($head, $body) = sbisec_http11($method, $uris, $query, $cookie);
		
		$cashs = array_reverse(parse_csv($body));
	}
	
	$body = $body_old;
	
	// ���s���ԁi�^�C���A�E�g�j���Đݒ肷��
	@set_time_limit(ENV_NUM_TIMEOUT);
	
	// ���O�A�E�g����
	$as = parse_tag($body, "a");
	foreach($as as $a) if(parse_tag_search(parse_tag($a["innerHTML"], "img"), "alt", "���O�A�E�g") != -1) {
		$method = "GET";
		$uris = parse_uri($a["href"], $uris);
		$query = "";
		$cookie = sbisec_update_cookie($head, $cookie);
		list($head, $body) = sbisec_http11($method, $uris, $query, $cookie);
		break;
	}
	
	// �����ꗗ�𐶐�����
	$investments = array();
	$section = "";
	foreach($securities as $line) {
		if(count($line) == 8) {
			switch($line[0]) {
			case "����": // ���Z�N�V����
				$section = ENV_STR_OFX_DEBT;
				break;
			default:
				switch($section) {
				case ENV_STR_OFX_DEBT: // ��
					array_push($investments, array(ENV_STR_OFX_DEBT, md5($line[0]), mb_convert_string($line[0]), mb_convert_uniquename($line[0]), (string)((integer)$line[4] / (double)$line[5] * 100), (string)((double)$line[5]), rtrim($line[7]), (string)((integer)rtrim($line[7]) - (integer)$line[4]), $line[1], parse_date($line[2]), $line[3]));
					break;
				default:
					// ���̑��i�����ΏۊO�j
					break;
				}
				break;
			}
		} else if(count($line) == 9) {
			switch($line[0]) {
			case "�����R�[�h": // �����Z�N�V����
				$section = ENV_STR_OFX_STOCK;
				break;
			case "�t�@���h��": // �����M���Z�N�V����
				$section = ENV_STR_OFX_FUND;
				break;
			default:
				switch($section) {
				case ENV_STR_OFX_STOCK: // ����
					array_push($investments, array(ENV_STR_OFX_STOCK, $line[0], mb_convert_string($line[1]), mb_convert_uniquename($line[1]), $line[2], $line[5], $line[7], rtrim($line[8])));
					break;
				case ENV_STR_OFX_FUND: // �����M��
					$i = array_search($line[0], $funds["name"]);
					array_push($investments, array(ENV_STR_OFX_FUND, ($i !== false? $funds["code"][$i]: mb_convert_uniquename($line[0])), mb_convert_string($line[0]), mb_convert_uniquename($line[0]), $line[1], $line[4], $line[6], rtrim($line[7])));
					break;
				default:
					// ���̑��i�����ΏۊO�j
					break;
				}
				break;
			}
		} else {
			// ���̑��i�����ΏۊO�j
		}
	}
	
	$invposlist = "";
	$seclist = "";
	foreach($investments as $investment) {
		// INVPOSLIST
		$cl = array();
		$cl["CATEGORY"] = $investment[0];
		$cl["UNIQUEID"] = $investment[1];
		$cl["SECNAME"] = $investment[2];
		$cl["HELDINACCT"] = ENV_STR_OFX_CASH;
		$cl["UNITS"] = (double)$investment[4];
		$cl["POSTYPE"] = "LONG";
		$cl["MKTVAL"] = (double)$investment[6];
		$cl["PL"] = (string)((double)$investment[7]);
		$cl["DTPRICEASOF"] = ENV_STR_DATE_TODAY;
		
		switch($cl["CATEGORY"]) {
		case ENV_STR_OFX_STOCK:
			$cl["UNIQUEIDTYPE"] = ENV_STR_OFX_CODE_STOCK;
			$cl["UNITPRICE"] = (double)$investment[5];
			break;
		case ENV_STR_OFX_FUND:
			$cl["UNIQUEIDTYPE"] = ENV_STR_OFX_CODE_FUND;
			$cl["UNITPRICE"] = round((double)$investment[5] / round((double)$investment[4] * (double)$investment[5] / (double)$investment[6], 0), 4);
			break;
		default:
		case ENV_STR_OFX_DEBT:
			$cl["UNIQUEIDTYPE"] = ENV_STR_OFX_CODE_HC;
			$cl["UNITPRICE"] = $investment[5];
			break;
		default:
			$cl["CATEGORY"] = "";
			break;
		}
		
		if($cl["CATEGORY"] != "") {
			// INVPOSLIST
			$invposlist .= "<POS" . $cl["CATEGORY"] . "><INVPOS>";
			$invposlist .= "<SECID><UNIQUEID>" . $cl["UNIQUEID"] . "</UNIQUEID><UNIQUEIDTYPE>" . $cl["UNIQUEIDTYPE"] . "</UNIQUEIDTYPE></SECID>";
			$invposlist .= "<HELDINACCT>" . $cl["HELDINACCT"] . "</HELDINACCT>";
			$invposlist .= "<POSTYPE>" . $cl["POSTYPE"] . "</POSTYPE>";
			$invposlist .= "<UNITS>" . $cl["UNITS"] . "</UNITS>";
			$invposlist .= "<UNITPRICE>" . $cl["UNITPRICE"] . "</UNITPRICE>";
			$invposlist .= "<MKTVAL>" . $cl["MKTVAL"] . "</MKTVAL>";
			$invposlist .= "<DTPRICEASOF>" . $cl["DTPRICEASOF"] . ENV_STR_OFX_TZ . "</DTPRICEASOF>";
			$invposlist .= "<MEMO>" . $cl["PL"] . "</MEMO>";
			$invposlist .= "</INVPOS></POS" . $cl["CATEGORY"] . ">";
			$invposlist .= "\r\n";
			
			// SECLIST
			$seclist .= "<" . $cl["CATEGORY"] . "INFO>";
			$seclist .= "<SECINFO>";
			$seclist .= "<SECID><UNIQUEID>" . $cl["UNIQUEID"] . "</UNIQUEID><UNIQUEIDTYPE>" . $cl["UNIQUEIDTYPE"] . "</UNIQUEIDTYPE></SECID>";
			$seclist .= "<SECNAME>" . $cl["SECNAME"] . "</SECNAME>";
			$seclist .= "</SECINFO>";
			if($cl["CATEGORY"] == ENV_STR_OFX_DEBT) {
				$seclist .= "<PARVALUE>" . $cl["UNITPRICE"] . "</PARVALUE>";
				$seclist .= "<DEBTTYPE>" . (parse_amount($investment[8]) > 0? "COUPON": "ZERO") . "</DEBTTYPE>";
				$seclist .= "<COUPONRT>" . $investment[8] . "</COUPONRT>";
				$seclist .= "<DTMAT>" . $investment[9] . "</DTMAT>";
			}
			$seclist .= "</" . $cl["CATEGORY"] . "INFO>";
			$seclist .= "\r\n";
		}
	}
	
	// �����ꗗ�𐶐�����
	$cts_s = ENV_STR_DATE_TODAY;
	$cts_e = ENV_STR_DATE_TODAY;
	$cts = array();
	$ct_date = "";
	$ct_num = 0;
	foreach($exchanges as $line) {
		if($line[0] == "���ׂĂ̏��i") {
			// ��藚���̓��t�i�J�n�E�I���j���擾����
			$cts_s = parse_date($line[1]);
			$cts_e = parse_date($line[2]);
		}
		
		if(count($line) == 14 && $line[0] != "����") {
			// ��ʁA�����A�������擾����
			if(substr($line[4], 0, strlen(ENV_STR_ASSET_STOCK_NAME)) == ENV_STR_ASSET_STOCK_NAME) { // ����
				$ct_category = ENV_STR_OFX_STOCK;
				$ct_uniqueid = mb_convert_string($line[2]);
				$ct_secname = "";
				$ct_uniqueidtype = ENV_STR_OFX_CODE_STOCK;
				
				// �����ꗗ��茟������
				foreach($investments as $investment) {
					if($investment[0] == ENV_STR_OFX_STOCK && $investment[1] == $ct_uniqueid) {
						// ������������ꍇ
						$ct_secname = mb_convert_string($investment[2]);
						break;
					}
				}
				
				if($ct_secname == "") {
					// ���������Ȃ��ꍇ
					$ct_secname = mb_convert_string($line[1]);
					
					if(strpos($seclist, $ct_uniqueid) === false) {
						// SECLIST
						$seclist .= "<!-- SOLD -->";
						$seclist .= "<" . $ct_category . "INFO><SECINFO>";
						$seclist .= "<SECID><UNIQUEID>" . $ct_uniqueid . "</UNIQUEID><UNIQUEIDTYPE>" . $ct_uniqueidtype . "</UNIQUEIDTYPE></SECID>";
						$seclist .= "<SECNAME>" . $ct_secname . "</SECNAME>";
						$seclist .= "</SECINFO></" . $ct_category . "INFO>";
						$seclist .= "\r\n";
					}
				}
				
				// �P�����擾����
				$ct_unitprice = (double)$line[9];
				
				// �萔�����擾����
				$ct_fees = (integer)$line[10];
				
				// �ł��擾����
				$ct_taxes = (integer)$line[11];
				switch(substr($line[4], 8, 2)) {
				case ENV_STR_ASSET_STOCK_SELL:
					$ct_buytype = ENV_STR_OFX_SELL;
					$ct_units = -1;
					$ct_total = 1;
					break;
				case ENV_STR_ASSET_STOCK_BUY:
				default:
					$ct_buytype = ENV_STR_OFX_BUY;
					$ct_units = 1;
					$ct_total = -1;
					break;
				}
				
				// ���ʂ��擾����
				$ct_units *= (integer)$line[8];
				
				// ���z���擾����
				$ct_total *= (integer)$line[13];
			} else if(substr($line[4], 0, strlen(ENV_STR_ASSET_FUND_NAME)) == ENV_STR_ASSET_FUND_NAME || substr($line[4], 0, strlen(ENV_STR_ASSET_FUND_DIV)) == ENV_STR_ASSET_FUND_DIV) { // ���M�A���z���ē���
				$ct_category = ENV_STR_OFX_FUND;
				
				// �����ꗗ��薼�O���擾����
				$ct_uniqueid = mb_convert_uniquename($line[1]);
				$ct_uniqueidtype = ENV_STR_OFX_CODE_HC;
				$ct_secname = "";
				
				// �����ꗗ��茟������
				foreach($investments as $investment) {
					if($investment[0] == ENV_STR_OFX_FUND && strpos($investment[3], $ct_uniqueid) !== false) {
						// ������������ꍇ
						$ct_uniqueid = $investment[1];
						$ct_uniqueidtype = ENV_STR_OFX_CODE_FUND;
						$ct_secname = $investment[2];
						break;
					}
				}
				
				if($ct_secname == "") {
					// ���������Ȃ��ꍇ
					$ct_uniqueid = mb_convert_uniqueid(mb_convert_uniquename($line[1]));
					$ct_secname = mb_convert_string($line[1]);
					
					if(strpos($seclist, $ct_uniqueid) === false) {
						// SECLIST
						$seclist .= "<!-- SOLD -->";
						$seclist .= "<" . $ct_category . "INFO><SECINFO>";
						$seclist .= "<SECID><UNIQUEID>" . $ct_uniqueid . "</UNIQUEID><UNIQUEIDTYPE>" . $ct_uniqueidtype . "</UNIQUEIDTYPE></SECID>";
						$seclist .= "<SECNAME>" . $ct_secname . "</SECNAME>";
						$seclist .= "</SECINFO></" . $ct_category . "INFO>";
						$seclist .= "\r\n";
					}
				}
				
				// �P�����擾����
				$ct_unitprice = round((integer)$line[9] / 10000, 4);
				
				// �萔�����擾����
				$ct_commission = round((integer)$line[8] * $ct_unitprice - (integer)$line[13], 4);
				
				// �ł��擾����
				$ct_taxes = "";
				switch(substr($line[4], 8, 4)) {
				case ENV_STR_ASSET_FUND_SELL:
				case ENV_STR_ASSET_FUND_PURCHASE:
					$ct_buytype = ENV_STR_OFX_SELL;
					$ct_units = -1;
					$ct_total = 1;
					break;
				case ENV_STR_ASSET_FUND_BUY:
				default:
					$ct_buytype = (substr($line[4], 0, strlen(ENV_STR_ASSET_FUND_DIV)) == ENV_STR_ASSET_FUND_DIV? ENV_STR_OFX_REINVEST: ENV_STR_OFX_BUY);
					$ct_units = 1;
					$ct_total = -1;
					break;
				}
				
				// ���ʂ��擾����
				$ct_units *= (integer)$line[8];
				
				// ���z���擾����
				// �}�C�i�X�l�̎萔���͋��e����Ȃ����߁A���z�ɏ�悹���Ē�������
				$ct_total *= ((integer)$line[13] + $ct_commission);
			} else {
				$ct_category = "";
				
				// ���ʂ��擾����
				$ct_units = (integer)$line[8];
				
				// ���z���擾����
				$ct_total = (integer)$line[13];
			}
			
			// �������擾����
			$ct_dttrade = parse_date($line[0]);
			
			// �ʔԂ𐶐�����
			if($ct_dttrade == $ct_date) $ct_num++; else $ct_num = 0;
			
			// �g�����U�N�V�����ԍ��𐶐�����
			$ct_fitid = $ct_dttrade . "0000010" . sprintf("%05d", $ct_num);
			
			$ct = array();
			$ct["CATEGORY"] = $ct_category;
			$ct["FITID"] = $ct_fitid;
			$ct["DTTRADE"] = $ct_dttrade;
			$ct["UNIQUEID"] = $ct_uniqueid;
			$ct["SECNAME"] = $ct_secname;
			$ct["UNIQUEIDTYPE"] = $ct_uniqueidtype;
			$ct["UNITS"] = (string)$ct_units;
			$ct["UNITPRICE"] = (string)$ct_unitprice;
			// OFX�d�l��ACOMMISSION�AFEES�ATAXES�̓}�C�i�X�l�����e���Ȃ�
			// $ct["COMMISSION"] = (string)abs($ct_commission);
			$ct["FEES"] = (string)abs($ct_fees);
			$ct["TAXES"] = (string)abs($ct_taxes);
			$ct["TOTAL"] = (string)$ct_total;
			$ct["SUBACCTSEC"] = ENV_STR_OFX_CASH;
			$ct["SUBACCTFUND"] = ENV_STR_OFX_CASH;
			$ct["BUYTYPE"] = $ct_buytype;
			array_push($cts, $ct);
			
			$ct_date = $ct_dttrade;
		}
	}
	
	// INVTRANLIST
	$invtranlist = "";
	$invtranlist .= "<DTSTART>" . $cts_s . ENV_STR_OFX_TZ . "</DTSTART>";
	$invtranlist .= "<DTEND>" . $cts_e . ENV_STR_OFX_TZ . "</DTEND>";
	$invtranlist .= "\r\n";
	foreach($cts as $ct) if($ct["BUYTYPE"] == ENV_STR_OFX_REINVEST) {
		// �ē����̏ꍇ�A�ē����̖��ׂ𐶐�����
		$invtranlist .= "<" . $ct["BUYTYPE"] . ">";
		$invtranlist .= "<INVTRAN><FITID>" . $ct["FITID"] . "</FITID><DTTRADE>" . $ct["DTTRADE"] . ENV_STR_OFX_TZ . "</DTTRADE></INVTRAN>";
		$invtranlist .= "<SECID><UNIQUEID>" . $ct["UNIQUEID"] . "</UNIQUEID><UNIQUEIDTYPE>" . $ct["UNIQUEIDTYPE"] . "</UNIQUEIDTYPE></SECID>";
		$invtranlist .= "<INCOMETYPE>" . ENV_STR_OFX_TRNTYPE_DIV . "</INCOMETYPE>";
		$invtranlist .= "<TOTAL>" . $ct["TOTAL"] . "</TOTAL>";
		$invtranlist .= "<SUBACCTSEC>" . $ct["SUBACCTSEC"] . "</SUBACCTSEC>";
		$invtranlist .= "<UNITS>" . $ct["UNITS"] . "</UNITS>";
		$invtranlist .= "<UNITPRICE>" . $ct["UNITPRICE"] . "</UNITPRICE>";
		// �}�C�i�X�l�̎萔���͋��e����Ȃ����߁A���z�ɏ�悹���Ē�������
		// $invtranlist .= "<COMMISSION>" . $ct["COMMISSION"] . "</COMMISSION>";
		$invtranlist .= "</" . $ct["BUYTYPE"] . ">";
		$invtranlist .= "\r\n";
	} else {
		// ���̑��̏ꍇ�A�����̖��ׂ𐶐�����
		$invtranlist .= "<" . $ct["BUYTYPE"] . $ct["CATEGORY"] . ">";
		$invtranlist .= "<INV" . $ct["BUYTYPE"] . ">";
		$invtranlist .= "<INVTRAN><FITID>" . $ct["FITID"] . "</FITID><DTTRADE>" . $ct["DTTRADE"] . ENV_STR_OFX_TZ . "</DTTRADE></INVTRAN>";
		$invtranlist .= "<SECID><UNIQUEID>" . $ct["UNIQUEID"] . "</UNIQUEID><UNIQUEIDTYPE>" . $ct["UNIQUEIDTYPE"] . "</UNIQUEIDTYPE></SECID>";
		$invtranlist .= "<UNITS>" . $ct["UNITS"] . "</UNITS>";
		$invtranlist .= "<UNITPRICE>" . $ct["UNITPRICE"] . "</UNITPRICE>";
		
		switch($ct["CATEGORY"]) {
		case ENV_STR_OFX_STOCK:
			$invtranlist .= "<TAXES>" . $ct["TAXES"] . "</TAXES>";
			$invtranlist .= "<FEES>" . $ct["FEES"] . "</FEES>";
			break;
		case ENV_STR_OFX_FUND:
			// �}�C�i�X�l�̎萔���͋��e����Ȃ����߁A���z�ɏ�悹���Ē�������
			// $invtranlist .= "<COMMISSION>" . $ct["COMMISSION"] . "</COMMISSION>";
			break;
		case ENV_STR_OFX_CASH:
		default:
			break;
		}
		
		$invtranlist .= "<TOTAL>" . $ct["TOTAL"] . "</TOTAL>";
		$invtranlist .= "<SUBACCTSEC>" . $ct["SUBACCTSEC"] . "</SUBACCTSEC>";
		$invtranlist .= "<SUBACCTFUND>" . $ct["SUBACCTFUND"] . "</SUBACCTFUND>";
		$invtranlist .= "</INV" . $ct["BUYTYPE"] . ">";
		$invtranlist .= "<" . $ct["BUYTYPE"] . "TYPE>" . $ct["BUYTYPE"] . "</" . $ct["BUYTYPE"] . "TYPE>";
		$invtranlist .= "</" . $ct["BUYTYPE"] . $ct["CATEGORY"] . ">";
		$invtranlist .= "\r\n";
	}
	
	// ���o���ꗗ�𐶐�����
	$cds_s = ENV_STR_DATE_TODAY;
	$cds_e = ENV_STR_DATE_TODAY;
	$cd_date = "";
	$cd_num = 0;
	foreach($cashs as $line) {
		$cd = array();
		
		if($line[0] == "�S��") {
			// ���o�����ׂ̓��t�i�J�n�E�I���j���擾����
			$cds_s = parse_date($line[2]);
			$cds_e = parse_date($line[3]);
		}
		
		if(count($line) == 7 && $line[0] != "���o����") {
			list($cd_name, $cd_memo) = explode(" ", $line[2], 2);
			$cd["NAME"] = $cd_name;
			$cd["MEMO"] = ($cd_memo != ""? $cd_memo: ENV_STR_OFX_MEMO);
			
			switch($cd["NAME"]) {
			case "�U������":
			case "��s�U��":
				$cd["TRNTYPE"] = ENV_STR_OFX_TRNTYPE_DIRECTDEP;
				break;
			case "���n�v��":
			case "���n�v�Ō��򒥎���":
			case "���n�v�Ŋҕt��":
			case "�z�������Ŋҕt��":
				$cd["TRNTYPE"] = ENV_STR_OFX_TRNTYPE_DEP;
				break;
			case "�敨�E�I�v�V�����؋����U��":
			case "�ǉ��ۏ؋��������U�֓���":
				$cd["TRNTYPE"] = ENV_STR_OFX_TRNTYPE_XFER;
				break;
			case "SBI�n�C�u���b�h�a����莩���U�֓���":
			case "SBI�n�C�u���b�h�a���֎����U�֏o��":
			case "��p��s������莩���U�֓���":
			case "��p��s�����֎����U�֏o��":
				$cd["TRNTYPE"] = ENV_STR_OFX_TRNTYPE_DEBIT;
				$cd["MEMO"] = $cd["NAME"];
				$cd["NAME"] = ENV_STR_NAME_NETBK;
				break;
			case "�M�p�z����":
			case "�O�ؔz����":
			case "�����z����":
				$cd["TRNTYPE"] = ENV_STR_OFX_TRNTYPE_DIV;
				break;
			case "�������v��":
			case "MRF�ē���":
			case "���ҋ�":
			case "�݊�����":
			case "�݊��z���������z":
				$cd["TRNTYPE"] = ENV_STR_OFX_TRNTYPE_INT;
				break;
			case "ATM":
				$cd["TRNTYPE"] = ENV_STR_OFX_TRNTYPE_ATM;
				break;
			case "���̑�":
			default:
				$cd["TRNTYPE"] = ENV_STR_OFX_TRNTYPE_OTHER;
				break;
			}
			
			// �������擾����
			$cd_dtposted = parse_date($line[0]);
			
			// �ʔԂ𐶐�����
			if($cd_dtposted == $cd_date) $cd_num++; else $cd_num = 0;
			
			// �g�����U�N�V�����ԍ��𐶐�����
			$cd["FITID"] = $cd_dtposted . "0000000" . sprintf("%05d", $cd_num);
			$cd["DTPOSTED"] = $cd_dtposted;
			
			switch($line[1]) {
			case ENV_STR_ASSET_CASH_DEPOSIT:
				$cd["TRNAMT"] = $line[4];
				break;
			case ENV_STR_ASSET_CASH_PAYMENT:
				$cd["TRNAMT"] = "-" . $line[3];
				break;
			default:
				break;
			}
			
			$cd["SUBACCTFUND"] = ENV_STR_OFX_CASH;
			
			// INVTRANLIST
			$invtranlist .= "<INVBANKTRAN>";
			$invtranlist .= "<STMTTRN>";
			$invtranlist .= "<TRNTYPE>" . $cd["TRNTYPE"] . "</TRNTYPE>";
			$invtranlist .= "<DTPOSTED>" . $cd["DTPOSTED"] . ENV_STR_OFX_TZ . "</DTPOSTED>";
			$invtranlist .= "<TRNAMT>" . $cd["TRNAMT"] . "</TRNAMT>";
			$invtranlist .= "<FITID>" . $cd["FITID"] . "</FITID>";
			$invtranlist .= "<NAME>" . mb_convert_string($cd["NAME"]) . "</NAME>";
			$invtranlist .= "<MEMO>" . mb_convert_string($cd["MEMO"]) . "</MEMO>";
			$invtranlist .= "</STMTTRN>";
			$invtranlist .= "<SUBACCTFUND>" . $cd["SUBACCTFUND"] . "</SUBACCTFUND>";
			$invtranlist .= "</INVBANKTRAN>";
			$invtranlist .= "\r\n";
			
			$cd_date = $cd_dtposted;
		}
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
	$invstmtmsgsrsv1 .= "<AVAILCASH>" . $availcash . "</AVAILCASH>";
	$invstmtmsgsrsv1 .= "<MARGINBALANCE>" . $marginbalance . "</MARGINBALANCE>";
	$invstmtmsgsrsv1 .= "<SHORTBALANCE>0</SHORTBALANCE>";
	$invstmtmsgsrsv1 .= "</INVBAL>";
	$invstmtmsgsrsv1 .= "\r\n";
	$invstmtmsgsrsv1 .= "<MKTGINFO>" . mb_convert_string($settings["name"]) . "</MKTGINFO>";
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

function sbisec_http11($method, $uris, $query = "", $cookie = "") {
	$ret = "INVALID HOST";
	if(preg_match("/\.sbisec\.co\.jp$/", $uris["host"]) > 0) $ret = http11(strtoupper($method), $uris["scheme"], $uris["host"], 0, $uris["path"], $query, "", $cookie);
	return explode("\r\n\r\n", $ret, 2);
}

function sbisec_update_cookie($head, $cookie) {
	return update_cookie(array("trading_site", "JSESSIONID", "line1", "line2", "masso_sid", "marble_sid", "AWSELB"), parse_header($head, "set-cookie"), $cookie);
}

?>