<?php
/*
jaccscard.inc: �W���b�N�X�J�[�h��HTML���OFX�t�@�C���𐶐�����
Copyright (C) 2012-2015 Hiromu Takahashi. All Rights Reserved.
mailto:hiromu2000@hotmail.com
Licensed under the GNU AGPLv3.
*/

// ���O�C�������擾����
$user = $ofxforms["IUSERID"];
$pass = $ofxforms["IPASSWORD"];

$resp = array();
$ofx = "";

// ���s���ԁi�^�C���A�E�g�j���Đݒ肷��
@set_time_limit(ENV_NUM_TIMEOUT);

// �z�[����ʂ��擾����
$urls = parse_url($settings["home"]);
$method = "GET";
$protocol = ($urls["scheme"] != ""? $urls["scheme"]: "");
$host = ($urls["host"] != ""? $urls["host"]: "");
$page = ($urls["path"] != ""? $urls["path"]: "/");
$query = "";
$cookie = "";
list($head, $body) = jaccscard_http11($method, $protocol, $host, $page, $query, $cookie);

// ���O�C����ʂ��擾����
$page = '/icmclub/icm_login.html';
list($head, $body) = jaccscard_http11($method, $protocol, $host, $page, $query, $cookie);

$protocol = 'https';
$page = '/Service';
$query = '_TRANID=JALG00001_00M';
list($head, $body) = jaccscard_http11($method, $protocol, $host, $page, $query, $cookie);

// ���O�C������
$forms = parse_tag($body, "form");
$c = parse_tag_search($forms, "name", "MainForm");
if($c != -1) {
	$method = "POST";
	$urls = parse_url($forms[$c]['action']);
	if($urls["scheme"] != "") $protocol = $urls["scheme"];
	if($urls["host"] != "") $host = $urls["host"];
	if($urls["path"] != "") $page = $urls["path"];
	if($urls["query"] != "") $page .= "?" . $urls["query"];
	
	$queries = jaccscard_getqueries($body);
	$queries["IUSERID"] .= $user;
	$queries["IPASSWORD"] .= $pass;
	$queries["_TRANID"] .= "JALG00001_01M";
	if(isset($queries["JALG00001_01M"]) == true) unset($queries["JALG00001_01M"]);
	$query = implode("&", $queries);
	$cookie = jaccscard_updatecookie($head, $cookie);
	list($head, $body) = jaccscard_http11($method, $protocol, $host, $page, $query, $cookie);
}

if(strpos($body, "�����e�i���X���s���Ă���܂�") !== false) {
	// �V�X�e�������e�i���X��ʂ̏ꍇ
	$resp["status"] = ENV_NUM_STATUS_MAINTENANCE;
	$resp["method"] = $method;
	$resp["uri"] = $uris["scheme"] . "://" . $uris["host"] . $uris["path"];
	$resp["query"] = $query;
	$resp["cookie"] = $cookie;
	$resp["head"] = $head;
	$resp["body"] = $body;
	$resp["ofx"] = generate_ofx($resp["status"]);
} else if(strpos($body, "JALG00012_00M") === false) {
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
	// ���j���[����u�����p������ׁv��I��
	$queries = jaccscard_getqueries($body);
	$queries['_TRANID'] .= 'JAMY00001_02M';
	$query = implode("&", $queries);
	$method = "POST";
	$cookie = jaccscard_updatecookie($head, $cookie);
	list($head, $body) = jaccscard_http11($method, $protocol, $host, $page, $query, $cookie);

	// �J�[�h���́E�J�[�h�ԍ����擾����
	preg_match_all("/<dd>(.*?)<\/dd>/s", $body, $matches);
	$account['acctname'] = $matches[1][0];
	preg_match("/([0-9]{4})/", $matches[1][1], $matches);
	$account['acctid'] = $matches[1];

	// �������擾
	$bodies[] = $body;
	preg_match_all("/'JAST00016_07M', '(20[0-9]{4})'/", $body, $months, PREG_SET_ORDER);
	while(preg_match("/(JAST[0-9]{5}_[0-9]{2}[A-Z]).*?����/", $body, $matches)) {
		$queries = jaccscard_getqueries($body);
		$queries['_TRANID'] .= $matches[1];
		$query = implode("&", $queries);
		$cookie = jaccscard_updatecookie($head, $cookie);
		list($head, $body) = jaccscard_http11($method, $protocol, $host, $page, $query, $cookie);
		$bodies[] = $body;
	}
	
	// ���̊m�茎���擾
	foreach($months as $month) {
		$queries = jaccscard_getqueries($body);
		$queries['_TRANID'] .= 'JAST00016_07M';
		$queries['_SUBINDEX'] .= $month[1];
		$query = implode("&", $queries);
		$cookie = jaccscard_updatecookie($head, $cookie);
		list($head, $body) = jaccscard_http11($method, $protocol, $host, $page, $query, $cookie);
		$bodies[] = $body;
		
		while(preg_match("/(JAST[0-9]{5}_[0-9]{2}[A-Z]).*?����/", $body, $matches)) {
			$queries = jaccscard_getqueries($body);
			$queries['_TRANID'] .= $matches[1];
			$query = implode("&", $queries);
			$cookie = jaccscard_updatecookie($head, $cookie);
			list($head, $body) = jaccscard_http11($method, $protocol, $host, $page, $query, $cookie);
			$bodies[] = $body;
		}
	}
	
	// ���m�茎���擾
	if(ENV_BOOL_PENDING_DETAIL) {
		$queries = jaccscard_getqueries($body);
		$queries['_TRANID'] .= 'JAST00016_19M';
		$query = implode("&", $queries);
		$cookie = jaccscard_updatecookie($head, $cookie);
		list($head, $body) = jaccscard_http11($method, $protocol, $host, $page, $query, $cookie);
		$bodies[] = $body;

		while(preg_match("/(JAST[0-9]{5}_[0-9]{2}[A-Z]).*?����/", $body, $matches)) {
			$queries = jaccscard_getqueries($body);
			$queries['_TRANID'] .= $matches[1];
			$query = implode("&", $queries);
			$cookie = jaccscard_updatecookie($head, $cookie);
			list($head, $body) = jaccscard_http11($method, $protocol, $host, $page, $query, $cookie);
			$bodies[] = $body;
		}
	}
	
	// ���O�A�E�g
	$queries = jaccscard_getqueries($body);
	$queries['_TRANID'] .= 'JALG00012_00M';
	$query = implode("&", $queries);
	$cookie = jaccscard_updatecookie($head, $cookie);
	list($head, $body) = jaccscard_http11($method, $protocol, $host, $page, $query, $cookie);
	
	// DOM�c���[�𐶐�
	$ofxdom = new ofxDOM($account['acctid']);
	foreach($bodies as $body) {
		// ���ׂ��p�[�X
		$cds = jaccscard_parsedom($body);
		// ���ׂ��Z�b�g
		$ofxdom->setTrans($cds);
	}
	$cds_balamt = 0;
	$cd_num = 0;
	$cds_s = "";
	$cds_e = "";
	$items = $ofxdom->getTrans();
	foreach($items as $item) {
		$dtposted = $item->getElementsByTagName('DTPOSTED')->item(0)->nodeValue;
		// �g�����U�N�V�����ԍ��i�㔼�j�𐶐�����
		if($dtposted == $cds_e) $cd_num++; else $cd_num = 0;
		$fitid = $item->getElementsByTagName('FITID')->item(0);
		$fitid->nodeValue .= sprintf("%05d", $cd_num);
		// DTSTART, DTEND���擾
		if($cds_s == "") $cds_s = $dtposted;
		$cds_e = $dtposted;
		// �c�����v�Z
		$cds_balamt += (double)$item->getElementsByTagName('TRNAMT')->item(0)->nodeValue;
	}
	
	// BANKTRANLIST
	$ofxdom->setBankTranList($cds_s . ENV_STR_OFX_TZ, $cds_e . ENV_STR_OFX_TZ);
	
	// �c��������
	$cds_balamt = (-1)*$cds_balamt;
	$ofxdom->setLedgerBalance($cds_balamt, ENV_STR_DATE_TODAY . ENV_STR_OFX_TZ);
	
	//MKGTINFO��ݒ�
	$ofxdom->setMktginfo(mb_convert_string(implode(ENV_CHR_CONCATENATOR, array($settings["name"], $account["acctname"]))));
	
	// XML DOM�c���[�𕶎���ɕϊ�
	$xml = $ofxdom->getXML();
	
	$resp["status"] = ENV_NUM_STATUS_SUCCESS;
	$resp["ofx"] = generate_ofx($resp["status"], $xml);
	//}
	
	// ���s���ԁi�^�C���A�E�g�j���Đݒ肷��
	@set_time_limit(ENV_NUM_TIMEOUT);
	
}
return $resp;

// HTTP/1.1
function jaccscard_http11($method, $protocol, $host, $page = "/", $query = "", $cookie = "") {
	$ret = "INVALID HOST";
	if(preg_match("/\.jaccs\.co\.jp$/", $host) > 0) $ret = http11($method, $protocol, $host, 0, $page, $query, "", $cookie);
	if(preg_match("/[Ll][Oo][Cc][Aa][Tt][Ii][Oo][Nn][\s\t]*:[\s\t]*([^\r\n]+)/", $ret, $matches) > 0) {
		$urls = parse_url($matches[1]);    
		if($urls["scheme"] != "") $protocol = $urls["scheme"];
		if($urls["host"] != "") $host = $urls["host"];
		if($urls["path"] != "") $page = $urls["path"];
		$query = ($urls["query"] != ""? $urls["query"]: "");
		return jaccscard_http11($method, $protocol, $host, $page, $query, $cookie);
	}
	return explode("\r\n\r\n", $ret, 2);
}

function jaccscard_updatecookie($head, $cookie) {
	$ret = "";
	$cookies = array();
	
	$ckvs = explode(";", $cookie);
	foreach($ckvs as $ckv) {
		list($ck, $cv) = explode("=", $ckv, 2);
		$ck = trim($ck);
		$cv = trim($cv);
		if($ck != "" && $cv != "") $cookies[$ck] = $ck . "=" . $cv;
	}
	
	$cks = array("JSESSIONID", "[0-9]{17}");
	foreach($cks as $ck) if(preg_match_all("/[Ss][Ee][Tt]-[Cc][Oo][Oo][Kk][Ii][Ee][\s\t]*:[\s\t]*(" . $ck . ")=([^;\r\n]*)/", $head, $matches) > 0) $cookies[$matches[1][count($matches[1]) - 1]] = $matches[1][count($matches[1]) - 1] . "=" . $matches[2][count($matches[2]) - 1];
	$ret = implode("; ", $cookies);
	return $ret;
}

function jaccscard_getqueries($body) {
	$forms = parse_tag($body, "form");
	$c = parse_tag_search($forms, "name", "MainForm");
	$inputs = parse_tag($forms[$c]["innerHTML"], "input");
	$queries = array();
	foreach($inputs as $input) {
		if($input["name"] != "") {
			$queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
		}
	}
	return $queries;
}

function jaccscard_parsedom($str) {
	// ���ו\�ǂݍ��ݗpDOM�c���[�쐬
	$doc = new DOMDocument();
	// <meta �c charset=Shift_JIS">�̑O��<title>���C���������̌����ƂȂ邽�߁C�폜
	preg_match("/(<body.*<\/body>)/s", $str, $matches);
	$str = $matches[1];
	// �w�b�_�t��
	$str = "<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\"><\/head>" . $str . "<\/html>";
	// �����G���R�[�h�ϊ�
	$str = mb_convert_encoding($str, 'UTF-8', 'sjis-win');
	// &nbsp;
	$str = str_replace("&nbsp;", "", $str);
	// HTML����DOM�c���[�쐬
	@$doc->loadHTML($str);
	$xpath = new DOMXPath($doc);
	// ���ו\�̃e�[�u�����w��
	$rows = $xpath->query("//table[@id='olcreco_target']/tr[@data-pay-id]"); 
	// �m�茎���ۂ� flag=false: �m�茎, flag=true: ���m�茎
	$flag = preg_match('/ggtd web_meisai/', $str);
	
	$ret = "";
	$cds = array();
	$nrow = $rows->length;
	
	for($i=0; $i<$nrow; $i++) {
		$cd = array();
		$row = $rows->item($i);
		$cols = $row->getElementsByTagName('td');
		// ���p���ׂłȂ��s�̓X�L�b�v
		if(empty($cols->item(0)->nodeValue)) continue;
		
		$cd["TRNTYPE"] = ENV_STR_OFX_TRNTYPE_CREDIT;
		
		// ���t���擾����
		preg_match("/([0-9]{2})\/([0-9]{1,2})\/([0-9]{1,2})/", trim($cols->item(1)->nodeValue), $matches);
		$cd["DTPOSTED"] = sprintf("20%d%02d%02d", $matches[1], $matches[2], $matches[3]);
		
		// �������Ƃ������擾
        if($flag) {
            $month = substr($cols->item(5)->nodeValue,5);
        } else {
            $month = substr($cols->item(5)->nodeValue,3);
        }
		// �g�����U�N�V�����ԍ��i�O���j�𐶐�����
		$cd["FITID"] = sprintf("%02d0", $month);// . "0";
		// �E�v���擾����
		$cd["NAME"] = $cols->item(2)->nodeValue;
		$cd["NAME"] = mb_convert_kana($cd["NAME"], "KV", "UTF-8");
		$cd["NAME"] = preg_replace('/-/', mb_convert_encoding('�[', 'UTF-8', 'sjis-win'), $cd["NAME"]);
		// ���z���擾����
		if($flag) {
			$cd["TRNAMT"] = (-1)*(double)parse_amount(trim($cols->item(7)->nodeValue));
		} else {
			$cd["TRNAMT"] = (-1)*(double)parse_amount(trim($cols->item(8)->nodeValue));
		}
		array_push($cds, $cd);
	}
	return $cds;
	
}
?>