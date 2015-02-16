<?php
/*
mufgcard.inc: MUFG�J�[�h��HTML���OFX�t�@�C���𐶐�����
Copyright (C) 2012-2015 Hiromu Takahashi. All Rights Reserved.
mailto:hiromu2000@hotmail.com
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
	$cookie = mufgcard_updatecookie($head, $cookie);
	list($head, $body) = mufgcard_http11($method, $uris["scheme"], $uris["host"], $uris["path"], $query, $cookie);
}

// ���_�C���N�g����
$retry = 0;
while($retry++ < ENV_NUM_HTTP11_RETRY) {
	$locations = parse_header($head, "location");
	if(count($locations) > 0) {
		$method = "GET";
		$uris = parse_uri($locations[0], $uris);
		$query = "";
		$cookie = mufgcard_updatecookie($head, $cookie);
		list($head, $body) = mufgcard_http11($method, $uris["scheme"], $uris["host"], $uris["path"], $query, $cookie);
	}
}

if(strpos($body, "���݃T�[�r�X��~��") !== false || strpos($body, "�V�X�e�������e�i���X�̂���") !== false) {
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
	// ���O�A�E�g��ʂ�ޔ�����
	$uris_old = parse_uri("/inet/dy/logout.html", $uris);
		
	// �����z�E���p���׏Ɖ�	
	$method = "GET";
	$uris = parse_uri("/inet/dy/meisaisyokai/index.html", $uris);
	$query = "";
	$cookie = mufgcard_updatecookie($head, $cookie);
	list($head, $body) = mufgcard_http11($method, $uris["scheme"], $uris["host"], $uris["path"], $query, $cookie);
	
	$tables = parse_tag($body, "table");
	$tds = parse_tag($tables[0]["innerHTML"], "td");
	// �J�[�h���̂��擾����
	$account["acctname"] = implode(ENV_CHR_CONCATENATOR, array($settings["name"], $tds[1]["innerHTML"]));
	
	// �J�[�h�ԍ����擾����
	if(preg_match("/XXXX-([0-9X]{4})/", $tds[2]["innerHTML"], $matches) > 0) {
		$account["acctid"] = $matches[1];
	}
	
	// ���x�������擾����
	$tds = parse_tag($tables[1]["innerHTML"], "td");
	$account["paydate"] = parse_date($tds[1]["innerHTML"]);
	
	// ���񂲐������v�z���擾����
	$account["ledge_balamt"] = 0;
	for( $i = 0 ; $i < 3 ; $i++ ) {
		$tmp = parse_amount($tds[2+$i*4]["innerHTML"]);
		$account["ledge_balamt"] += (-1)*$tmp;
	}
	
	// ���s���ԁi�^�C���A�E�g�j���Đݒ肷��
	@set_time_limit(ENV_NUM_TIMEOUT);
	
	$bodies = array();
	// �J�[�h�E�Ɖ��I������
	$as = parse_tag($body, "a");
	foreach($as as $a){
		if(preg_match("/.*detail.*/", $a["href"])) {
			$method = "GET";
			$uris = parse_uri($a["href"], $uris);
			$cookie = mufgcard_updatecookie($head, $cookie);
			list($head, $body) = mufgcard_http11($method, $uris["scheme"], $uris["host"], $uris["path"], $uris["query"], $cookie);
			array_push($bodies, $body);
		}
	}
	
	// DOM�c���[�𐶐�
	$ofxdom = new ofxDOM($account['acctid']);
	
	// ���ׂ��p�[�X
	$cds_s = "";
	$cds_e = "";
	foreach($bodies as $body){
		list($cds, $tmp_s, $tmp_e) = mufgcard_parsedom($body);
		if($cds_s == "") $cds_s = $tmp_s;
		if($tmp_e !== "") $cds_e = $tmp_e;
		$ofxdom->setTrans($cds);
	}
	
	// BANKTRANLIST
	$ofxdom->setBankTranList($cds_s . ENV_STR_OFX_TZ, $cds_e . ENV_STR_OFX_TZ);
	
	$cds_balamt = (-1)*$cds_balamt;
	
	// �c��������
	$account["ledge_balamt"] = parse_amount($account["ledge_balamt"]);
	$ofxdom->setLedgerBalance($account["ledge_balamt"], ENV_STR_DATE_TODAY . ENV_STR_OFX_TZ);
	
	//MKGTINFO��ݒ�
	$ofxdom->setMktginfo(mb_convert_string($account["acctname"]));
	
	// XML DOM�c���[�𕶎���ɕϊ�
	$xml = $ofxdom->getXML();
	
	$resp["status"] = ENV_NUM_STATUS_SUCCESS;
	$resp["ofx"] = generate_ofx($resp["status"], $xml);
	
	// ���s���ԁi�^�C���A�E�g�j���Đݒ肷��
	@set_time_limit(ENV_NUM_TIMEOUT);
	
	// ���O�A�E�g����
	$method = "GET";
	$uris = $uris_old;
	$query = "";
	$cookie = mufgcard_updatecookie($head, $cookie);
	list($head, $body) = mufgcard_http11($method, $uris["scheme"], $uris["host"], $uris["path"], $query, $cookie);
	
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
function mufgcard_http11($method, $protocol, $host, $page = "/", $query = "", $cookie = "") {
	$ret = "INVALID HOST";
	if(preg_match("/\.mufgcard\.com$/", $host) > 0) $ret = http11(strtoupper($method), $protocol, $host, 0, $page, $query, "", $cookie);
	return explode("\r\n\r\n", $ret, 2);
}

function mufgcard_updatecookie($head, $cookie) {
	$ret = "";
	$cookies = array();
	
	$ckvs = explode(";", $cookie);
	foreach($ckvs as $ckv) {
		list($ck, $cv) = explode("=", $ckv, 2);
		$ck = trim($ck);
		$cv = trim($cv);
		if($ck != "" && $cv != "") $cookies[$ck] = $ck . "=" . $cv;
	}
	
	$cks = array("AS0[1-9]");
	foreach($cks as $ck) {
		$c = preg_match_all("/[Ss][Ee][Tt]-[Cc][Oo][Oo][Kk][Ii][Ee][\s\t]*:[\s\t]*(" . $ck . ")=([^;\r\n]*)/", $head, $matches);
		for($i = 0; $i < $c; $i++) $cookies[$matches[1][$i]] = $matches[1][$i] . "=" . $matches[2][$i];
	}
	$ret = implode("; ", $cookies);
	return $ret;
}

function mufgcard_parsedom($str) {
	// ���ו\�ǂݍ��ݗpDOM�c���[�쐬
	$doc = new DOMDocument();
	// �������Ƃ����擾
	$month = (preg_match("/" .preg_quote("���x����") . ".*?" . preg_quote("�N") . "([0-9]+)" . preg_quote("��") . "/s", $str, $matches) > 0 ? $matches[1] : "00");	
	// <meta �c charset=Shift_JIS">�̑O��<title>���C���������̌����ƂȂ邽�߁C�폜
	$str = preg_replace('/<title>.*<\/title>/', '', $str);
	// &nbsp;
	$str = str_replace("&nbsp;", "", $str);
	// �����G���R�[�h�ϊ�
	$str = mb_convert_encoding($str, 'UTF-8', 'SJIS');
	$str = str_replace('Shift_JIS','UTF-8',$str);
	// HTML����DOM�c���[�쐬
	$doc->loadHTML($str);
	$xpath = new DOMXPath($doc);
	// ���ו\�̃e�[�u�����w��
	$tables = $xpath->query("//table[@class='mod-table font-x-small sp-font-normal transform']");
	// �������Ȃ��ꍇ
	if($tables->length == 0) { 
		return array(array(), "", "");
	}
	$rows = $tables->item(0)->getElementsByTagName('tr');
	
	$ret = "";
	$cds = array();
	$cds_balamt = 0;
	$cds_s = "";
	$cds_e = "";
	$cd_date = "";
	$cd_num = 0;
	$nrow = $rows->length;
	for($i=3; $i<$nrow; $i++) {
		$cd = array();
		$row = $rows->item($i);
		$cols = $row->getElementsByTagName('dd');
		// ���p���ׂłȂ��s�̓X�L�b�v
		if(empty($cols->item(0)->nodeValue)) continue;
		
		$cd["TRNTYPE"] = ENV_STR_OFX_TRNTYPE_CREDIT;
		
		// ���t���擾����
		$cd["DTPOSTED"] = parse_date(trim($cols->item(0)->nodeValue));
		if($cds_s == "") $cds_s = $cd["DTPOSTED"];
		$cds_e = $cd["DTPOSTED"];
		// �ʔԂ𐶐�����
		if($cd["DTPOSTED"] == $cd_date) $cd_num++; else $cd_num = 0;
		// �g�����U�N�V�����ԍ��𐶐�����
		$cd["FITID"] = sprintf("%02d", $month) . "0" . sprintf("%05d", $cd_num);
		// �E�v���擾����
		$cd["NAME"] = $cols->item(1)->nodeValue;
		// ���z���擾����
		$cd["TRNAMT"] = (-1)*(double)parse_amount(trim($cols->item(4)->nodeValue));
		
		array_push($cds, $cd);
		$cd_date = $cd["DTPOSTED"];
	}
	return array($cds, $cds_s, $cds_e);
}
?>