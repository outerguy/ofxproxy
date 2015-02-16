<?php
/*
dccard.inc: DC�J�[�h��CSV���OFX�t�@�C����ϊ��E��������
Copyright (C) 2012-2015 OFFICE OUTERGUY. All rights reserved.
mailto:contact@beatrek.com
Licensed under the GNU AGPLv3.
*/

// ���O�C�������擾����
$user = $ofxforms["webId"];
$pass = $ofxforms["webPassword"];
$birth = $ofxforms["webBirthDay"];

$resp = array();
$ofx = "";

// ���s���ԁi�^�C���A�E�g�j���Đݒ肷��
@set_time_limit(ENV_NUM_TIMEOUT);

// �z�[����ʂ��擾����iNEWS+PLUS�j
$method = "GET";
$uris = parse_uri($settings["home"]);
$query = "";
$cookie = "";
list($head, $body) = mun_http11($method, $uris, $query, $cookie);

// ���O�C����ʂ��擾����iNEWS+PLUS�j
$as = parse_tag($body, "a");
foreach($as as $a) if(parse_tag_search(parse_tag($a["innerHTML"], "img"), "alt", "���O�C��") != -1) {
	$method = "GET";
	$uris = parse_uri($a["href"], $uris);
	$cookie = mun_update_cookie($head, $cookie);
	list($head, $body) = mun_http11($method, $uris, $query, $cookie);
	break;
}

// ���O�C������iNEWS+PLUS�j
$forms = parse_tag($body, "form");
$c = parse_tag_search($forms, "id", "loginFormNavi");
if($c != -1) {
	$inputs = parse_tag($forms[$c]["innerHTML"], "input");
	$queries = array();
	foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
	$queries["cardBrand"] = "cardBrand=" . $settings["brand"];
	$queries["webId"] = "webId=" . $user;
	$queries["webPassword"] = "webPassword=" . $pass;
	$queries["webAddAuthType"] = "webAddAuthType=2";
	$queries["webBirthDay"] = "webBirthDay=" . $birth;
	
	$head .= "Set-Cookie: m_cardBrand=" . $settings["brand"] . "\r\n";
	
	$method = $forms[$c]["method"];
	$uris = parse_uri($forms[$c]["action"], $uris);
	$query = implode("&", $queries);
	$cookie = mun_update_cookie($head, $cookie);
	list($head, $body) = mun_http11($method, $uris, $query, $cookie);
}

// ���_�C���N�g����
$retry = 0;
while($retry++ < ENV_NUM_HTTP11_RETRY) {
	$locations = parse_header($head, "location");
	if(count($locations) > 0) {
		$method = "GET";
		$uris = parse_uri($locations[0], $uris);
		$query = "";
		$cookie = mun_update_cookie($head, $cookie);
		list($head, $body) = mun_http11($method, $uris, $query, $cookie);
	}
}

$as = parse_tag($body, "a");
foreach($as as $a) {
	switch(trim(strip_tags($a["innerHTML"]))) {
	case "���O�A�E�g":
		// ���O�A�E�g��ʂ�ޔ�����iNEWS+PLUS�j
		$method_mun = "GET";
		$uris_mun = parse_uri($a["href"], $uris);
		$query_mun = "";
		$cookie_mun = $cookie;
		break;
	case "WEB�T�[�r�X�g�b�v":
		// WEB�T�[�r�X�g�b�v��ʂ��擾����iNEWS+PLUS�j
		$method = "GET";
		$uris = parse_uri($a["href"], $uris);
		$query = "";
		$cookie = mun_update_cookie($head, $cookie);
		list($head, $body) = mun_http11($method, $uris, $query, $cookie);
		break;
	default:
		break;
	}
}

// �ȍ~�͊e�J�[�h�u�����h�̉�ʂɑJ�ڂ���
$cookie = "";

// ���s���ԁi�^�C���A�E�g�j���Đݒ肷��
@set_time_limit(ENV_NUM_TIMEOUT);

// ���O�C������
$forms = parse_tag($body, "form");
$c = parse_tag_search($forms, "id", "seamlessForm");
if($c != -1) {
	$inputs = parse_tag($forms[$c]["innerHTML"], "input");
	$queries = array();
	foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
	
	$method = $forms[$c]["method"];
	$uris = parse_uri($forms[$c]["action"], $uris);
	$query = implode("&", $queries);
	$cookie = dccard_update_cookie($head, $cookie);
	list($head, $body) = dccard_http11($method, $uris, $query, $cookie);
}

// ���_�C���N�g����
$retry = 0;
while($retry++ < ENV_NUM_HTTP11_RETRY) {
	$locations = parse_header($head, "location");
	if(count($locations) > 0) {
		$method = "GET";
		$uris = parse_uri($locations[0], $uris);
		$query = "";
		$cookie = dccard_update_cookie($head, $cookie);
		list($head, $body) = dccard_http11($method, $uris, $query, $cookie);
	}
}

// MenuFrame��ʂ��擾����
$frames = parse_tag($body, "frame");
$c = parse_tag_search($frames, "name", "MenuFrame");
if($c != -1) {
	$method = "GET";
	$uris = parse_uri($frames[$c]["src"], $uris);
	$query = "";
	$cookie = dccard_update_cookie($head, $cookie);
	list($head, $body) = dccard_http11($method, $uris, $query, $cookie);
}

if(strpos($body, "���݃T�[�r�X��~��") !== false || strpos($body, "�����e�i���X��Ƃ̂��߃T�[�r�X���ꎞ��~") !== false || strpos($body, "�V�X�e�������e�i���X�̂���") !== false) {
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
	$as = parse_tag($body, "a");
	foreach($as as $a) {
		$imgs = parse_tag($a["innerHTML"], "img");
		
		// ���O�A�E�g��ʂ�ޔ�����
		if(parse_tag_search($imgs, "alt", "���O�A�E�g") != -1) $uris_old = parse_uri($a["href"], $uris);
		
		// �����p������׏Ɖ�i�m��j��ʂ��擾����
		if(parse_tag_search($imgs, "alt", "�����p������׏Ɖ�i�m��j") != -1) {
			$method = "GET";
			$uris = parse_uri($a["href"], $uris);
			$query = "";
			$cookie = dccard_update_cookie($head, $cookie);
			list($head, $body) = dccard_http11($method, $uris, $query, $cookie);
		}
	}
	
	// MainFrame��ʂ��擾����
	$frames = parse_tag($body, "frame");
	$c = parse_tag_search($frames, "name", "MainFrame");
	if($c != -1) {
		$method = "GET";
		$uris = parse_uri($frames[$c]["src"], $uris);
		$query = "";
		$cookie = dccard_update_cookie($head, $cookie);
		list($head, $body) = dccard_http11($method, $uris, $query, $cookie);
	}
	
	// PBodyFrame��ʂ��擾����
	$frames = parse_tag($body, "frame");
	$c = parse_tag_search($frames, "name", "PBodyFrame");
	if($c != -1) {
		$method = "GET";
		$uris = parse_uri($frames[$c]["src"], $uris);
		$query = "";
		$cookie = dccard_update_cookie($head, $cookie);
		list($head, $body) = dccard_http11($method, $uris, $query, $cookie);
	}
	
	// ���_�C���N�g����
	$scripts = parse_tag($body, "script");
	foreach($scripts as $script) if(preg_match("/location\.href[\s\t]*=[\s\t]*\"(.*?)\";/i", $script["innerHTML"], $matches) > 0) {
		$method = "GET";
		$uris = parse_uri($matches[1], $uris);
		$query = "";
		$cookie = dccard_update_cookie($head, $cookie);
		list($head, $body) = dccard_http11($method, $uris, $query, $cookie);
		break;
	}
	
	// ���_�C���N�g����i�V�X�e�������e�i���X���j
	$retry = 0;
	while($retry++ < ENV_NUM_HTTP11_RETRY) {
		$locations = parse_header($head, "location");
		if(count($locations) > 0) {
			$method = "GET";
			$uris = parse_uri($locations[0], $uris);
			$query = "";
			$cookie = dccard_update_cookie($head, $cookie);
			list($head, $body) = dccard_http11($method, $uris, $query, $cookie);
		}
	}
	
	// �V�X�e�������e�i���X��ʂ̏ꍇ
	if(strpos($body, "���q�l�̂������z���m��") !== false || strpos($body, "�������܃T�[�o�[�����e�i���X��") !== false) {
		$resp["status"] = ENV_NUM_STATUS_MAINTENANCE;
		$resp["method"] = $method;
		$resp["uri"] = $uris["scheme"] . "://" . $uris["host"] . $uris["path"];
		$resp["query"] = $query;
		$resp["cookie"] = $cookie;
		$resp["head"] = $head;
		$resp["body"] = $body;
	} else {
		// �ŐV�̐��������擾����i���x������10���ł���Ɖ��肷��j
		$selects = parse_tag($body, "select");
		$c = parse_tag_search($selects, "name", "selmonth");
		$selmonth = "";
		if($c != -1) {
			$options = parse_tag($selects[$c]["innerHTML"], "option");
			if(count($options) > 0) {
				$selmonth = $options[0]["value"];
				$m = sprintf("%02d", (integer)str_replace("��", "", trim(strip_tags($options[0]["innerHTML"]))));
				$ymd = (date("m") <= $m? date("Y"): (string)((integer)date("Y") + 1)) . $m . "10";
			}
		}
		
		$accounts = array();
		
		// �J�[�h�����擾����
		$c = 0;
		$as = parse_tag($body, "a");
		$bufs = array();
		foreach($as as $a) if(parse_tag_search(parse_tag($a["innerHTML"], "img"), "alt", "�Ɖ�") != -1) array_push($bufs, $a["href"]);
		for($i = 0; $i < count($bufs); $i++) if(preg_match("/javascript:[^\']+?\'([0-9]+)\'.*?/i", $bufs[$i], $matches) > 0) {
			$accounts[$c]["id"] = (integer)$matches[1];
			$accounts[$c]["sendmonth"] = $selmonth;
			$accounts[$c]["acctid"] = "";
			$accounts[$c]["paydate"] = $ymd;
			$c++;
		}
		
		$body_old = $body;
		
		// �J�[�h�������[�v����
		foreach($accounts as $account) {
			$body = $body_old;
			
			// ���s���ԁi�^�C���A�E�g�j���Đݒ肷��
			@set_time_limit(ENV_NUM_TIMEOUT);
			
			// �J�[�h�E�Ɖ��I������
			$scripts = parse_tag($body, "script");
			foreach($scripts as $script) if(preg_match("/location\.href[\s\t]*=[\s\t]*([^;]+?);/i", $script["innerHTML"], $matches) > 0) {
				$method = "GET";
				$uris = parse_uri(str_replace(array("'", "\"", " ", "+", "num", "sendmonth"), array("", "", "", "", (string)$account["id"], $account["sendmonth"]), $matches[1]), $uris);
				$query = "";
				$cookie = dccard_update_cookie($head, $cookie);
				list($head, $body) = dccard_http11($method, $uris, $query, $cookie);
				break;
			}
			
			// RRFrame��ʂ��擾����
			$frames = parse_tag($body, "frame");
			$c = parse_tag_search($frames, "name", "RRFrame");
			if($c != -1) {
				$method = "GET";
				$uris = parse_uri($frames[$c]["src"], $uris);
				$query = "";
				$cookie = dccard_update_cookie($head, $cookie);
				list($head, $body) = dccard_http11($method, $uris, $query, $cookie);
			}
			
			// ���_�C���N�g����
			$scripts = parse_tag($body, "script");
			foreach($scripts as $script) if(preg_match("/location\.href[\s\t]*=[\s\t]*\"(.*?)\";/i", $script["innerHTML"], $matches) > 0) {
				$method = "GET";
				$uris = parse_uri($matches[1], $uris);
				$query = "";
				$cookie = dccard_update_cookie($head, $cookie);
				list($head, $body) = dccard_http11($method, $uris, $query, $cookie);
				break;
			}
			
			$trs = parse_tag($body, "tr", true); // �ċA�I�Ɏ擾����
			$bufs = array();
			foreach($trs as $tr) {
				$tds = parse_tag($tr["innerHTML"], "td");
				if(count($tds) == 2) {
					$k = trim(strip_tags($tds[0]["innerHTML"]));
					$v = trim(strip_tags($tds[1]["innerHTML"]));
					switch($k) {
					case "�J�[�h����":
						// �J�[�h���̂��擾����
						$account["acctname"] = implode(ENV_CHR_CONCATENATOR, array($settings["name"], $v));
						$account["name"] = $settings["name"];
						break;
					case "�J�[�h�ԍ�":
						// �J�[�h�ԍ����擾����
						if(preg_match("/([0-9\*\-]+)/", str_replace("�|", "-", $v), $matches) > 0) $account["acctid"] = $matches[1];
						break;
					case "���x����":
						// ���x�������擾����
						$account["paydate"] = parse_date($v);
						break;
					case "���񂲐������v�z":
						// ���񂲐������v�z���擾����
						$account["ledge_balamt"] = parse_amount($v);
						break;
					default:
						break;
					}
				}
			}
			
			$str = "";
			
			// CSV�t�@�C�����_�E�����[�h����
			$as = parse_tag($body, "a");
			$c = parse_tag_search($as, "innerHTML", "�_�E�����[�h�F�e�L�X�g�iCSV�j�`��");
			if($c != -1) {
				$method = "GET";
				$uris = parse_uri($as[$c]["href"], $uris);
				$query = "";
				$cookie = dccard_update_cookie($head, $cookie);
				list($head, $body) = dccard_http11($method, $uris, $query, $cookie);
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
			$creditcardmsgsrsv1 .= dccard_parse_csv($body, $account);
			$creditcardmsgsrsv1 .= "</CCSTMTRS>";
			$creditcardmsgsrsv1 .= "\r\n";
			$creditcardmsgsrsv1 .= "</CCSTMTTRNRS>";
			$creditcardmsgsrsv1 .= "\r\n";
			
			$creditcardmsgsrsv1 .= "</CREDITCARDMSGSRSV1>";
			$creditcardmsgsrsv1 .= "\r\n";
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
	
	// ���s���ԁi�^�C���A�E�g�j���Đݒ肷��
	@set_time_limit(ENV_NUM_TIMEOUT);
	
	// ���O�A�E�g����
	$method = "GET";
	$uris = $uris_old;
	$query = "";
	$cookie = dccard_update_cookie($head, $cookie);
	list($head, $body) = dccard_http11($method, $uris, $query, $cookie);
	
	// ���O�A�E�g����iNEWS+PLUS�j
	$method = $method_mun;
	$uris = $uris_mun;
	$query = $query_mun;
	$cookie = $cookie_mun;
	list($head, $body) = mun_http11($method, $uris, $query, $cookie);
}
return $resp;

// HTTP/1.1�iNEWS+PLUS�j
function mun_http11($method, $uris, $query = "", $cookie = "") {
	$ret = "INVALID HOST";
	if(preg_match("/\.cr\.mufg\.jp$/", $uris["host"]) > 0) $ret = http11(strtoupper($method), $uris["scheme"], $uris["host"], 0, $uris["path"], $query, "", $cookie);
	return explode("\r\n\r\n", $ret, 2);
}

function mun_update_cookie($head, $cookie) {
	return update_cookie(array("PHPSESSID", "session-srv", "m_cardBrand"), parse_header($head, "set-cookie"), $cookie);
}

// HTTP/1.1
function dccard_http11($method, $uris, $query = "", $cookie = "") {
	$ret = "INVALID HOST";
	if(preg_match("/\.dccard\.co\.jp$/", $uris["host"]) > 0) $ret = http11(strtoupper($method), $uris["scheme"], $uris["host"], 0, $uris["path"], $query, "", $cookie);
	return explode("\r\n\r\n", $ret, 2);
}

function dccard_update_cookie($head, $cookie) {
	return update_cookie(array("club", "ASPSESSIONID[A-Z]{8}"), parse_header($head, "set-cookie"), $cookie);
}

function dccard_parse_csv($str, $account) {
	$ret = "";
	$lines = parse_csv($str);
	$cds = array();
	$cds_balamt = $account["ledge_balamt"];
	$cds_paydate = $account["paydate"];
	$cds_s = "";
	$cds_e = "";
	$cd_date = "";
	$cd_num = 0;
	$ledge_balamt = $cds_balamt;
	
	foreach($lines as $line) {
		$cd = array();
		
		if(count($line) == 11 && $line[0] != "���p��") {
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
			$cd["NAME"] = mb_convert_string($line[3]);
			
			// ���z���擾����
			$cd["TRNAMT"] = (string)(-1 * (double)parse_amount($line[5]));
			$ledge_balamt += (double)$cd["TRNAMT"];
			
			// �����p�敪�A����є��l���擾����
			$cd["MEMO"] = mb_convert_string($line[2] != "" || $line[10] != ""? $line[2] . $line[10]: implode(ENV_CHR_CONCATENATOR, array($line[2], $line[10])));
			if($cd["MEMO"] == "") $cd["MEMO"] = ENV_STR_OFX_MEMO;
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