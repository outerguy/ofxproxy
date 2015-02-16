<?php
/*
OFXProxy
common.inc: �萔�E���ʊ֐����`����
Copyright (C) 2008-2015 OFFICE OUTERGUY. All rights reserved.
mailto:contact@beatrek.com
Dual-licensed under the GNU AGPLv3 and Beatrek Origin License.

class ofxDOM
Copyright (C) 2012-2015 Hiromu Takahashi. All Rights Reserved.
mailto:hiromu2000@hotmail.com
Licensed under the GNU AGPLv3.
*/

define("ENV_PRODUCT_CODENAME", "PURSE");
define("ENV_PRODUCT_VERSION", "1.0");
define("ENV_PRODUCT_FAMILY", "OFXProxy");
define("ENV_PRODUCT_COMMENT", "+https://github.com/outerguy/ofxproxy/");
define("ENV_PRODUCT_ID", ENV_PRODUCT_CODENAME . "/" . ENV_PRODUCT_VERSION);
define("ENV_PRODUCT_UA", "Mozilla/5.0 (compatible; " . ENV_PRODUCT_ID . (defined("ENV_PRODUCT_FAMILY") == true && ENV_PRODUCT_FAMILY != ""? "; " . ENV_PRODUCT_FAMILY: "") . (defined("ENV_PRODUCT_COMMENT") == true && ENV_PRODUCT_COMMENT != ""? "; " . ENV_PRODUCT_COMMENT: "") . ")");

define("ENV_BOOL_DEBUG", false); // �J���Ҍ����i�f�o�b�O�j�@�\��L���Ƃ��邩�ۂ�
define("ENV_BOOL_PENDING_DETAIL", true); // �i�N���W�b�g�J�[�h�́j���m�薾�ׂ��o�͂��邩�ۂ�
define("ENV_BOOL_ADD_RISKBASE", true); // ���X�N�x�[�X�F�ؐ������A���Z�@�ւ�OFXProxy�̐ڑ�����o�^���邩�ۂ�

define("ENV_NUM_STATUS_SUCCESS", 200);
define("ENV_NUM_STATUS_ADDITION", 202);
define("ENV_NUM_STATUS_NONE", 204);
define("ENV_NUM_STATUS_FAILURE", 403);
define("ENV_NUM_STATUS_MAINTENANCE", 503);
define("ENV_NUM_STATUS_CAUTION", 511);

define("ENV_NUM_HTTP11_SIZE_BUF", 1024);
define("ENV_NUM_HTTP11_RETRY", 8);

define("ENV_NUM_TIMEOUT", 30);

define("ENV_FILE_DIR_CLIENT", "./client/");
define("ENV_FILE_DIR_SERVER", "./server/");
define("ENV_FILE_DIR_COMMON", "./common/");
define("ENV_FILE_DIR_LOG", "./log/");

define("ENV_FILE_INI_FI", "fi.ini");
define("ENV_FILE_TEMPLATE_HTML", "template.html");
define("ENV_FILE_TEMPLATE_OFX", "ofx.xml");

define("ENV_FILE_EXT_INC", ".inc");
define("ENV_FILE_EXT_OFX", ".ofx");
define("ENV_FILE_EXT_LOG", ".log");

define("ENV_STR_SESSION_PADDING", " ");

define("ENV_STR_ACCTTYPE_SAVINGS", "SAVINGS"); // ���ʗa������
define("ENV_STR_ACCTTYPE_CHECKING", "CHECKING"); // �����a������

define("ENV_STR_DATE_TODAY", date("Ymd")); // �{��
define("ENV_STR_DATE_PASTDAY", date("Ymd", strtotime(date("Y-m-1") . " -1 month"))); // �O��1��

define("ENV_STR_OFX_CURRENCY_JPY", "JPY"); // �ʉ�
define("ENV_STR_OFX_TZ", "000000[+9:JST]"); // �����b�E�^�C���]�[��
define("ENV_STR_OFX_NODATA", "<!-- NO DATA -->"); // ��

define("ENV_STR_OFX_STOCK", "STOCK"); // ����
define("ENV_STR_OFX_FUND", "MF"); // �����M��
define("ENV_STR_OFX_DEBT", "DEBT"); // ��
define("ENV_STR_OFX_CASH", "CASH"); // �a���
define("ENV_STR_OFX_BUY", "BUY"); // ��
define("ENV_STR_OFX_SELL", "SELL"); // ��
define("ENV_STR_OFX_REINVEST", "REINVEST"); // �ē���
define("ENV_STR_OFX_MEMO", "-"); // �����̃f�t�H���g�l

define("ENV_STR_OFX_TRNTYPE_CREDIT", "CREDIT"); // Generic credit
define("ENV_STR_OFX_TRNTYPE_DEBIT", "DEBIT"); // Generic debit
define("ENV_STR_OFX_TRNTYPE_INT", "INT"); // Interest earned or paid
define("ENV_STR_OFX_TRNTYPE_DIV", "DIV"); // Dividend
define("ENV_STR_OFX_TRNTYPE_FEE", "FEE"); // FI fee
define("ENV_STR_OFX_TRNTYPE_SRVCHG", "SRVCHG"); // Service charge
define("ENV_STR_OFX_TRNTYPE_DEP", "DEP"); // Deposit
define("ENV_STR_OFX_TRNTYPE_ATM", "ATM"); // ATM debit or credit
define("ENV_STR_OFX_TRNTYPE_POS", "POS"); // Point of sale debit or credit
define("ENV_STR_OFX_TRNTYPE_XFER", "XFER"); // Transfer
define("ENV_STR_OFX_TRNTYPE_CHECK", "CHECK"); // Check
define("ENV_STR_OFX_TRNTYPE_PAYMENT", "PAYMENT"); // Electronic payment
define("ENV_STR_OFX_TRNTYPE_CASH", "CASH"); // Cash withdrawal
define("ENV_STR_OFX_TRNTYPE_DIRECTDEP", "DIRECTDEP"); // Direct deposit
define("ENV_STR_OFX_TRNTYPE_DIRECTDEBIT", "DIRECTDEBIT"); // Merchant initiated debit
define("ENV_STR_OFX_TRNTYPE_REPEATPMT", "REPEATPMT"); // Repeating payment/standing order
define("ENV_STR_OFX_TRNTYPE_OTHER", "OTHER"); // Other

define("ENV_STR_OFX_BROKER_BANK", "BANK"); // ��s
define("ENV_STR_OFX_BROKER_CREDITCARD", "CREDITCARD"); // �N���W�b�g�J�[�h
define("ENV_STR_OFX_BROKER_INVESTMENT", "INVSTMT"); // �،�
define("ENV_STR_OFX_BROKER_PREPAID", "PREPAID"); // �O�������[

define("ENV_STR_OFX_SEVERITY_INFO", "INFO"); // Informational only
define("ENV_STR_OFX_SEVERITY_WARN", "WARN"); // Some problem with the request occurred but a valid response still present
define("ENV_STR_OFX_SEVERITY_ERROR", "ERROR"); // A problem severe enough that response could not be made

define("ENV_STR_OFX_CODE_STOCK", "JP:SIC"); // �،����Z���^�[
define("ENV_STR_OFX_CODE_FUND", "JP:ITAJ"); // The Investment Trusts Association, Japan
define("ENV_STR_OFX_CODE_HC", "JP:HC"); // PURSE Hashed Code

define("ENV_STR_OFX_CODE_SUCCESS", "0"); // ����
define("ENV_STR_OFX_CODE_ADDITION", "3000"); // �ǉ��F��
define("ENV_STR_OFX_CODE_FAILURE", "15500"); // �F�؃G���[
define("ENV_STR_OFX_CODE_MAINTENANCE", "16503"); // �����e�i���X�G���[
define("ENV_STR_OFX_CODE_CAUTION", "15511"); // ���擾�G���[

define("ENV_CHR_SEPARATOR", "\t");
define("ENV_CHR_CONCATENATOR", "�@");

function env_dlog($str) {
	if(ENV_BOOL_DEBUG == true) {
		list($usec, $sec) = explode(" ", (string)microtime());
		@file_put_contents(ENV_FILE_DIR_LOG . date("YmdHis", (integer)$sec) . $usec . ENV_FILE_EXT_LOG, $str);
	}
	return ENV_BOOL_DEBUG;
}

function get_fi_settings($fiid = "") {
	$ret = array();
	$ini = parse_ini_file(ENV_FILE_DIR_COMMON . ENV_FILE_INI_FI, true);
	$ks = array_keys($ini);
	$tag = "";
	foreach($ks as $k) {
		$ini[$k]["fiid"] = $k;
		switch($ini[$k]["type"]) {
		case ENV_STR_OFX_BROKER_BANK:
		case ENV_STR_OFX_BROKER_PREPAID:
			break;
		case ENV_STR_OFX_BROKER_CREDITCARD:
			$tag = "CC";
			break;
		case ENV_STR_OFX_BROKER_INVESTMENT:
			$tag = "INV";
			break;
		default:
			$ini[$k]["type"] = ENV_STR_OFX_BROKER_BANK;
			break;
		}
		$ini[$k]["tag"] = $tag;
	}
	if($fiid != "") {
		if($ini[$fiid]) $ret = $ini[$fiid];
	} else {
		$ret = $ini;
	}
	return $ret;
}

function sess_encode($str) {
	return urlencode(base64_encode(gzdeflate($str, 9)));
}

function sess_decode($str) {
	return gzinflate(base64_decode(urldecode($str)));
}

function mb_convert_uniqueid($str) {
	return strtoupper(md5($str));
}

function mb_convert_uniquename($str) {
	return strtoupper(mb_convert_kana(str_replace(array("�@", "�|", "�E"), array("", "", ""), $str), "ask"));
}

function mb_convert_string($str) {
	return htmlspecialchars(mb_convert_encoding($str, "UTF-8", "Shift_JIS"));
}

function mb_htmlspecialchars($str) {
	return htmlspecialchars($str, ENT_COMPAT | ENT_XHTML, "Shift_JIS");
}

class ofxDOM {
	private $dom;
	private $banktranlist;
	private $stmtrs;
	
	// XML DOM�c���[���쐬����
	// $ofxdom = new ofxDOM();
	// �A�J�E���g�h�c���w�肷��ꍇ
	// $ofxdom = new ofxDOM($acctid);
	public function __construct() {
		$this->dom = new DOMDocument('1.0', 'UTF-8');
		$dom = $this->dom;
		$msg = $dom->appendChild($dom->createElement('CREDITCARDMSGSRSV1'));
		$stmttrnrs = $msg->appendChild($dom->createElement('CCSTMTTRNRS'));
		$node = $stmttrnrs->appendChild($dom->createElement('TRNUID'));
		$node->appendChild($dom->createTextNode('0'));
		$status = $stmttrnrs->appendChild($dom->createElement('STATUS'));
		$node = $status->appendChild($dom->createElement('CODE'));
		$node->appendChild($dom->createTextNode('0'));
		$node = $status->appendChild($dom->createElement('SEVERITY'));
		$node->appendChild($dom->createTextNode('INFO'));
		$stmtrs = $stmttrnrs->appendChild($dom->createElement('CCSTMTRS'));
		$this->stmtrs = $stmtrs;
		$node = $stmtrs->appendChild($dom->createElement('CURDEF'));
		$node->appendChild($dom->createTextNode(ENV_STR_OFX_CURRENCY_JPY));
		$acctfrom = $stmtrs->appendChild($dom->createElement('CCACCTFROM'));
		// ACCTID���������� 
		$node = $acctfrom->appendChild($dom->createElement('ACCTID'));
		$arg = func_get_args();
		if(func_num_args() == 1) {
			$node->appendChild($dom->createTextNode($arg[0])); 
		} else {
			$node->appendChild($dom->createTextNode("0000"));
		}
		
		$this->banktranlist = $stmtrs->appendChild($dom->createElement('BANKTRANLIST'));
	}
	
	// ���ׂ���������
	public function setTrans($cds) {
		$acctid = $this->dom->getElementsByTagName('ACCTID')->item(0)->nodeValue;
		$dom = $this->dom;
		$banktranlist = $this->banktranlist;
		foreach($cds as $cd) {
			$stmttrn = $banktranlist->appendChild($dom->createElement('STMTTRN'));
			
			$node = $stmttrn->appendChild($dom->createElement('TRNTYPE'));
			$node->appendChild($dom->createTextNode($cd["TRNTYPE"]));
			
			$node = $stmttrn->appendChild($dom->createElement('DTPOSTED'));
			$node->appendChild($dom->createTextNode($cd["DTPOSTED"]));
			
			$node = $stmttrn->appendChild($dom->createElement('TRNAMT'));
			$node->appendChild($dom->createTextNode($cd["TRNAMT"]));
			
			$node = $stmttrn->appendChild($dom->createElement('FITID'));
			$node->appendChild($dom->createTextNode($cd["DTPOSTED"] . $acctid . $cd["FITID"]));
			
			$node = $stmttrn->appendChild($dom->createElement('NAME'));
			$node->appendChild($dom->createTextNode($cd["NAME"]));
		}
	}
	public function getTrans() {
		return $this->dom->getElementsByTagName('STMTTRN');
	}

	public function setBankTranList($cds_s, $cds_e) {
		$dom = $this->dom;
		$banktranlist = $this->banktranlist;
		$node = $banktranlist->insertBefore($dom->createElement('DTEND'), $banktranlist->firstChild);
		$node->appendChild($dom->createTextNode($cds_e));
		
		$node = $banktranlist->insertBefore($dom->createElement('DTSTART'), $banktranlist->firstChild);
		$node->appendChild($dom->createTextNode($cds_s));
	}
	public function setLedgerBalance($ledge_balamt, $dtasof) {
		$dom = $this->dom;
		$stmtrs = $this->stmtrs;
		$ledgerbal = $stmtrs->appendChild($dom->createElement('LEDGERBAL'));
		$node = $ledgerbal->appendChild($dom->createElement('BALAMT'));
		$node->appendChild($dom->createTextNode($ledge_balamt));
		$node = $ledgerbal->appendChild($dom->createElement('DTASOF'));
		$node->appendChild($dom->createTextNode($dtasof));
	}
	public function setMktginfo($mktginfo) {
		$dom = $this->dom;
		$stmtrs = $this->stmtrs;
		$node = $stmtrs->appendChild($dom->createElement('MKTGINFO'));
		$node->appendChild($dom->createTextNode($mktginfo));
	}
	public function getXML() {
		$this->dom->formatOutput = true;
		$xml = $this->dom->saveXML();
		// XML�̃w�b�_�͍폜
		return substr($xml, strpos($xml, '?>') + 2);
	}
}

// ���z���p�[�X����
function parse_amount($str) {
	$buf = str_replace(array(",", "��", "��", "��", "��"), array("", "-", "-", "000", "0000"), trim($str));
	$ret = preg_replace("/[^0-9\-\.]/", "", $buf);
	if($ret == "") $ret = "0";
	return $ret;
}

// ���t���p�[�X����
function parse_date($str) {
	list($dy, $dm, $dd, $dummy) = explode("/", str_replace(array("�N", "��", "��", "-"), array("/", "/", "", "/"), trim($str)), 4);
	$dy = (integer)$dy;
	if($dy < 1988) $dy += 1988;
	$dm = (integer)$dm;
	$dd = (integer)$dd;
	return sprintf("%04d%02d%02d", $dy, $dm, $dd);
}

// �t�H�[���o�R�ő���M�����p�����[�^���p�[�X����
function parse_param($str) {
	return str_replace(array("%2A", "%40"), array("*", "@"), urlencode(urldecode(str_replace("+", "%2B", $str))));
}

// �w�肳�ꂽ�J�n�^�O����I���^�O�܂ł�HTML�A����у^�O�̑������擾����
function parse_tag($html, $tag, $recursive = false) {
	$ret = array();
	$buf = $html;
	
	// HTML�ȊO�̃^�O���̗v�f����菜��
	$strips = array("script", "style");
	foreach($strips as $strip) if($strip != strtolower($tag)) $buf = preg_replace("/<" . preg_quote($strip) . "(?!\w)[^>]*>.*?<\/" . preg_quote($strip) . ">/ims", "", $buf);
	
	$c = preg_match_all("/<(\/?)" . preg_quote($tag) . "((?!\w)[^>]*)>/ims", $buf, $matches);
	if(array_search("/", $matches[1]) !== false) {
		// �I���^�O�����݂���ꍇ
		$bufs = array(); // �^�O�̃l�X�g�\����ێ�����z��
		$nest = 0; // �^�O�̃l�X�g��
		$cnt = 0; // �J�n�^�O�ƏI���^�O�̃Z�b�g��
		$pos = 0; // �����J�n�ʒu�i������̐擪����̃o�C�g���j
		// �u<!-- tag></tag -->�v�̂悤�ȏI���^�O�i�R�����g�j������
		for($i = 0; $i < $c; $i++) if($matches[2][$i] != "--") {
			if($matches[1][$i] == "") {
				// �J�n�^�O�̏ꍇ�A�J�n�^�O���擾����
				$bufs[$nest]["tag"] = $matches[0][$i];
				// �J�n�^�O�̊J�n�ʒu���擾����
				$bufs[$nest]["start"] = strpos($buf, $bufs[$nest]["tag"], $pos);
				// �����J�n�ʒu���J�n�^�O�̏I���ʒu�ɍX�V����
				$pos = $bufs[$nest]["start"] + strlen($bufs[$nest]["tag"]);
				$bufs[$nest]["pos"] = $pos;
				// �l�X�g���𑝂₷
				$nest++;
			} else {
				// �I���^�O�̏ꍇ�A�l�X�g�������炷
				$nest--;
				// �����J�n�ʒu���X�V����
				$pos = strpos($buf, $matches[0][$i], $pos);
				if(($recursive == false && $nest == 0) || $recursive == true) {
					// $recursive��false���l�X�g����0�A�܂���$recursive��true�̏ꍇ
					// �J�n�^�O�̑������擾����
					$ret[$cnt] = parse_tag_attributes($bufs[$nest]["tag"], $tag);
					// �J�n�^�O����I���^�O�܂ł̒��g��؂�o��
					$ret[$cnt]["innerHTML"] = substr($buf, $bufs[$nest]["pos"], ($pos - $bufs[$nest]["pos"]));
					// �����J�n�ʒu���I���^�O�̏I���ʒu�ɍX�V����
					$pos += strlen($matches[0][$i]);
					// �J�n�^�O�ƏI���^�O���܂߂Đ؂�o��
					$ret[$cnt]["match"] = substr($buf, $bufs[$nest]["start"], ($pos - $bufs[$nest]["start"]));
					// �J�n�^�O�ƏI���^�O�̃Z�b�g���𑝂₷
					$cnt++;
				}
			}
		}
	} else {
		// �I���^�O�����݂��Ȃ��ꍇ�A�J�n�^�O�̑������擾����
		for($i = 0; $i < $c; $i++) {
			$ret[$i] = parse_tag_attributes($matches[0][$i], $tag);
			$ret[$i]["innerHTML"] = "";
			$ret[$i]["match"] = $matches[0][$i];
		}
	}
	return $ret;
}

// �w�肳�ꂽ�^�O�̑������擾����
function parse_tag_attributes($html, $tag) {
	$ret = array();
	
	// �^�O�̑�����؂�o��
	$s = strlen($tag) + 1;
	$e = strlen($html) - $s - 1;
	$buf = substr($html, $s, $e);
	
	// �^�u�E���s�R�[�h���X�y�[�X�ɒu������
	$buf = str_replace(array("\t", "\r", "\n"), array(" ", " ", " "), $buf);
	
	// =�̑O��̃X�y�[�X����菜��
	$buf = preg_replace("/\s*=\s*/", "=", $buf);
	
	// �E�ӂ�""��''�Ŋ����Ă��Ȃ��ꍇ�A�����ɉ��s�R�[�h��ǉ�����
	$buf = preg_replace("/(=)([^\'\"])([^\\1\s]++)/", "\\1\\2\\3\n", $buf);
	
	// �E�ӂ�""��''�Ŋ����Ă���ꍇ�A��菜���A�����ɉ��s�R�[�h��ǉ�����
	$buf = preg_replace("/(=)([\'\"])([^\\2]*?)\\2/", "\\1\\3\n", $buf);
	
	// ���s�R�[�h�ŕ�������
	$kvs = explode("\n", $buf);
	foreach($kvs as $kv) {
		$kv = ltrim($kv);
		if($kv != "") {
			// key=value�𒊏o����ikey�͉p�������ɕϊ�����j
			$p = strpos($kv, "=");
			if($p !== false) {
				// key=value
				$k = strtolower(substr($kv, 0, $p));
				$v = substr($kv, $p + 1);
				if($v === false) $v = "";
			} else {
				// value�����݂��Ȃ��ꍇ�Akey=key�Ƃ���
				$k = strtolower($kv);
				$v = $k;
			}
			if($k != "/") $ret[$k] = $v;
		}
	}
	return $ret;
}

// �w�肳�ꂽ�^�O�z���葮������v�i$key=$value�j����^�O�̔z��ԍ����擾����
function parse_tag_search($tags, $key, $value) {
	$ret = -1;
	$c = count($tags);
	for($i = 0; $i < $c; $i++) if($tags[$i][$key] == $value) {
		$ret = $i;
		break;
	}
	return $ret;
}

// $head�Ɉ�v����HTTP�w�b�_�[�̔z����擾����
function parse_header($http, $head) {
	$ret = array();
	$c = preg_match_all("/" . preg_quote($head) . "[\s\t]*:[\s\t]*([^\r\n]*)/i", $http, $matches);
	for($i = 0; $i < $c; $i++) array_push($ret, $matches[1][$i]);
	return $ret;
}

// $uri_next��$uris����̐�΁E���΃p�XURI�Ƃ��ăp�[�X����
function parse_uri($uri_next, $uris = array()) {
	$ret = $uris;
	if(isset($ret["path"]) == false) {
		$ret["path"] = "/";
	} else {
		$c = strpos($ret["path"], "?");
		if($c !== false) $ret["path"] = substr($ret["path"], 0, $c);
		$c = strrpos($ret["path"], "/");
		if($c !== false) $ret["path"] = substr($ret["path"], 0, $c + 1);
	}
	if(isset($ret["query"]) == true) unset($ret["query"]);
	
	$uri_nexts = parse_url($uri_next);
	if(isset($uri_nexts["scheme"]) == true) $ret["scheme"] = $uri_nexts["scheme"];
	if(isset($uri_nexts["host"]) == true) $ret["host"] = $uri_nexts["host"];
	
	$prevs = explode("/", (substr($uri_nexts["path"], 0, 1) == "/"? "": $ret["path"]) . $uri_nexts["path"]);
	$nexts = array();
	foreach($prevs as $prev) if($prev == "..") array_pop($nexts); else if($prev != ".") array_push($nexts, $prev);
	$ret["path"] = implode("/", $nexts);
	
	if(isset($uri_nexts["query"]) == true && $uri_nexts["query"] != "") $ret["path"] .= "?" . html_entity_decode($uri_nexts["query"]);
	
	return $ret;
}

// CSV�`���̃f�[�^���p�[�X����
function parse_csv($csv, $delimiter = ",", $enclosure = "\"") {
	$ret = array();
	
	$de = substr($delimiter, 0, 1);
	$en = substr($enclosure, 0, 1);
	
	$lines = array();
	switch(gettype($csv)) {
	case "array":
		foreach($csv as $line) array_push($lines, rtrim($line, "\r\n"));
		break;
	case "string":
	default:
		$buf = preg_replace("/[\r\n]+/", "\n", rtrim($csv, "\r\n"));
		if($buf != "") $lines = explode("\n", $buf);
		break;
	}
	
	foreach($lines as $line) {
		$buf = "";
		do {
			$es = strpos($line, $en, 0);
			if($es !== false) $ee = strpos($line, $en, $es + 1);
			if($es !== false && $ee !== false && $es < $ee) {
				$buf .= substr($line, 0, $es) . str_replace($de, "\n", substr($line, $es + 1, $ee - $es - 1));
				$line = substr($line, $ee + 1);
			} else {
				$buf .= $line;
				$line = "";
			}
		} while($line != "");
		
		array_push($ret, str_replace("\n", $de, explode($de, $buf)));
	}
	return $ret;
}

// HTTP/1.1�v���g�R���ŃA�N�Z�X����
function http11($method, $protocol, $host, $port = 0, $page = "/", $query = "", $basic = "", $cookie = "", $autoconv = true, $ua = ENV_PRODUCT_UA, $fr = "", $referer = "") {
	$ret = "";
	$head = "";
	$body = "";
	
	$chunked = false;
	
	// �ڑ��������肷��
	switch($protocol) {
	case "https":
		$prefix = "ssl://";
		break;
	case "http":
	default:
		$prefix = "";
		break;
	}
	if($port == 0) $port = getservbyname($protocol, "tcp");
	$querystring = ($method == "GET" && $query != "")? "?" . $query: "";
	
	// �\�P�b�g���J��
	$fp = fsockopen($prefix . $host, $port, $errno, $errstr);
	if($fp) {
		fputs($fp, $method . " " . $page . $querystring . " HTTP/1.1\r\n");
		fputs($fp, "Host: " . $host . "\r\n");
		fputs($fp, "Date: " . str_replace("+0000", "GMT", gmdate("r")) . "\r\n");
		fputs($fp, "Accept-Language: ja\r\n");
		fputs($fp, "Cache-Control: no-cache\r\n");
		fputs($fp, "Connection: close\r\n");
		// fputs($fp, "Connection: Keep-Alive\r\n");
		if($ua != "") fputs($fp, "User-Agent: " . $ua . "\r\n");
		if($fr != "") fputs($fp, "From: " . $fr . "\r\n");
		if($referer != "") fputs($fp, "Referer: " . $referer . "\r\n");
		if($basic != "") fputs($fp, "Authorization: Basic " . base64_encode($basic) . "\r\n");
		if($cookie != "") fputs($fp, "Cookie: " . $cookie . "\r\n");
		if($method == "POST") {
			fputs($fp, "Content-Type: application/x-www-form-urlencoded\r\n");
			fputs($fp, "Content-Length: " . strlen($query) . "\r\n");
			fputs($fp, "\r\n" . $query);
		} else {
			fputs($fp, "\r\n");
		}
		// while(!feof($fp)) $ret .= fgets($fp, ENV_HTTP11_SIZE_BUF);
		
		// Chunked�w�b�_�[����������
		while(!feof($fp)) {
			$line = @fgets($fp, ENV_NUM_HTTP11_SIZE_BUF);
			if(preg_match("/^[Tt][Rr][Aa][Nn][Ss][Ff][Ee][Rr]-[Ee][Nn][Cc][Oo][Dd][Ii][Nn][Gg][\s\t]*:[\s\t]*[Cc][Hh][Uu][Nn][Kk][Ee][Dd]/", $line)) {
				$chunked = true;
				continue;
			}
			$head .= $line;
			if($line == "\r\n") break;
		}
		
		// �{�f�B�[����������
		if($chunked == true) {
			while(!feof($fp)) {
				$chunk = fgets($fp, ENV_NUM_HTTP11_SIZE_BUF);
				$size = (preg_match("/^([0-9A-Fa-f]+)/", $chunk, $matches))? hexdec($matches[1]): -1;
				if($size <= 0) break;
				$body .= http11_fread_blocking($fp, $size);
				http11_fread_blocking($fp, 2);
			}
		} else {
			while(!feof($fp)) $body .= http11_fread_blocking($fp, ENV_NUM_HTTP11_SIZE_BUF);
		}
		
		// �\�P�b�g��Ԃ���
		fclose($fp);
		
		// HTML����Shift_JIS�ȊO�̏ꍇ�͕����R�[�h��ϊ�����
		$mime = (preg_match("/[Cc][Oo][Nn][Tt][Ee][Nn][Tt]-[Tt][Yy][Pp][Ee][\s\t]*:[\s\t]*([^;\r\n]+)/", $head, $matches) > 0? strtolower($matches[1]): "");
		if($autoconv == true && $mime == "text/html") {
			$metas = parse_tag($body, "meta");
			foreach($metas as $meta) {
				if($meta["charset"] != "") {
					$enc = $meta["charset"];
					break;
				}
				if(strtolower($meta["http-equiv"]) == "content-type" && preg_match("/[Cc][Hh][Aa][Rr][Ss][Ee][Tt][\s\t]*\=[\s\t]*([0-9A-Za-z\-_]+)/", $meta["content"], $matches) > 0) {
					$enc = $matches[1];
					break;
				}
			}
			switch(strtoupper($enc)) {
			case "UTF-7":
			case "UTF-8":
			case "UTF-16":
			case "UTF-32":
			case "EUC-JP":
			case "ISO-2022-JP":
				$body = str_replace($enc, "Shift_JIS", mb_convert_encoding($body, "Shift_JIS", strtoupper($enc)));
				$head = rtrim($head) . "\r\nX-PURSE-Original-Encoding: " . $enc . "\r\n\r\n";
			case "SHIFT_JIS":
			case "SJIS":
			default:
				break;
			}
		}
		$ret = $head . $body;
	}
	env_dlog("MTHD: " . $method . "\r\nPROT: " . $protocol . "\r\nHOST: " . $host . "\r\nPORT: " . $port . "\r\nPAGE: " . $page . "\r\nQERY: " . $query . "\r\nBASC: " . $basic . "\r\nCOOK: " . $cookie . "\r\nUAGT: " . $ua . "\r\nFROM: " . $fr . "\r\nREFR: " . $referer . "\r\nMIME: " . $mime . "\r\nENCD: " . $enc . "\r\nHEAD: " . $head . "\r\n\r\n" . $body);
	return $ret;
}

// Chunked�̃f�[�^��fread�œǂݍ��݂���ہA�w��T�C�Y���������擾�ł��Ȃ��ꍇ������̂����P����
function http11_fread_blocking($fp, $len) {
	$ret = "";
	$size = $len;
	while(!feof($fp) && $size > 0) {
		$buf = fread($fp, $size);
		$size -= strlen($buf);
		$ret .= $buf;
	}
	return $ret;
}

// $memories�ɑ��݂���L�[��$heads���T���A$cookie�̒l���X�V����
function update_cookie($memories, $heads, $cookie) {
	$ret = "";
	$cookies = array();
	
	$kvs = explode(";", $cookie);
	foreach($kvs as $kv) {
		list($k, $v) = explode("=", $kv, 2);
		$k = trim($k);
		$v = trim($v);
		if($k != "" && $v != "") $cookies[$k] = $k . "=" . $v;
	}
	
	foreach($memories as $memory) foreach($heads as $head) {
		$c = preg_match_all("/(" . $memory . ")=([^;\r\n]*)/", $head, $matches);
		for($i = 0; $i < $c; $i++) $cookies[$matches[1][$i]] = $matches[1][$i] . "=" . $matches[2][$i];
	}
	
	$ret = implode("; ", $cookies);
	return $ret;
}

// OFX 1.0.2����OFX 2.1.1�ւƐ��`����
function convert_ofx($ofx) {
	$ret = "";
	$head = "";
	$body = "";
	
	// �w�b�_�[�ƃ{�f�B�[�ɕ�������
	$i = strpos($ofx, "<OFX>");
	if($i !== false) $head = substr($ofx, 0, $i - 1); else $i = 0;
	$j = strpos($ofx, "</OFX>");
	if($j !== false) $body = substr($ofx, $i, $j + 6);
	
	// �w�b�_�[����͂���
	$heads = array();
	if(preg_match_all("/([A-Z]+):([^\r\n]+)/", $head, $matches) > 0) for($i = 0; $i < count($matches[0]); $i++) $heads[$matches[1][$i]] = $matches[2][$i];
	if($heads["OFXHEADER"] == "100" && $heads["VERSION"] == "102" && $heads["DATA"] == "OFXSGML") {
		// OFX 1.0.2�̏ꍇ
		if($heads["ENCODING"] != "" && $heads["ENCODING"] != "UTF-8") $body = mb_convert_encoding($body, "UTF-8", $heads["ENCODING"]);
		$ret .= preg_replace("/<OFX>[\w\W]*<\/OFX>[\r\n]+?/", "", file_get_contents(ENV_FILE_DIR_COMMON . ENV_FILE_TEMPLATE_OFX));
		
		// ���s�R�[�h�𓝈ꂷ��
		$body = str_replace(array("\r\n", "\r"), array("\n", "\n"), $body);
		
		// �s���̃^�u���폜���A�s���ɂȂ��^�O���s���Ɉړ����A��s���폜����
		$body = preg_replace(array("/\n\t+/", "/([^\n])</", "/\n{2,}/"), array("\n", "\\1\n<", "\n"), $body);
		
		$bodys = explode("\n", $body);
		$c = count($bodys);
		for($i = 0; $i < $c; $i++) {
			$bodys[$i] = trim($bodys[$i]);
			if(preg_match("/^<(\/)?([^>]+?)>(.*)$/", $bodys[$i], $matches) > 0) {
				// �^�O�̏ꍇ
				if($matches[1] == "/") {
					// �I���^�O�̏ꍇ
					if($i > 0 && strpos($bodys[$i - 1], "<" . $matches[2] . ">") === false) $ret .= $bodys[$i] . "\r\n";
				} else {
					// �J�n�^�O�̏ꍇ
					$ret .= $bodys[$i] . ($matches[3] != ""? "</" . $matches[2] . ">": "") . "\r\n";
				}
			} else {
				// �^�O�ȊO�̏ꍇ
				$ret .= $bodys[$i] . "\r\n";
			}
		}
	}
	return $ret;
}

// �e���v���[�g���OFX 2.1.1�𐶐�����
function generate_ofx($status, $str = ENV_STR_OFX_NODATA, $cook = "", $akey = "") {
	$now = ENV_STR_DATE_TODAY . ENV_STR_OFX_TZ;
	$org = mb_htmlspecialchars(mb_convert_string(ENV_PRODUCT_ID));
	$content = trim($str);
	$code = ENV_STR_OFX_CODE_SUCCESS;
	$serverity = ENV_STR_OFX_SEVERITY_INFO;
	$message = "";
	$mfachallengetrnrs = "";
	$sesscookie = $cook;
	$accesskey = $akey;
	
	switch($status) {
	case ENV_NUM_STATUS_SUCCESS:
		// �������Ȃ�
		break;
	case ENV_NUM_STATUS_ADDITION:
		$code = ENV_STR_OFX_CODE_ADDITION;
		$severity = ENV_STR_OFX_SEVERITY_ERROR;
		$message = "ADDITION";
		$mfachallengetrnrs = $content;
		$content = "";
		break;
	case ENV_NUM_STATUS_MAINTENANCE:
		$code = ENV_STR_OFX_CODE_MAINTENANCE;
		$severity = ENV_STR_OFX_SEVERITY_ERROR;
		$message = "MAINTENANCE";
		break;
	case ENV_NUM_STATUS_CAUTION:
		$code = ENV_STR_OFX_CODE_CAUTION;
		$severity = ENV_STR_OFX_SEVERITY_ERROR;
		$message = "CAUTION";
		break;
	case ENV_NUM_STATUS_FAILURE:
	default:
		$code = ENV_STR_OFX_CODE_FAILURE;
		$severity = ENV_STR_OFX_SEVERITY_ERROR;
		$message = "FAILURE";
		break;
	}
	if($message != "") $message = "<MESSAGE>" . $message . "</MESSAGE>";
	if($sesscookie != "") $sesscookie = "<SESSCOOKIE>" . $sesscookie . "</SESSCOOKIE>";
	if($accesskey != "") $accesskey = "<ACCESSKEY>" . $accesskey . "</ACCESSKEY>";
	
	return str_replace(array("<!--[now]-->", "<!--[org]-->", "<!--[content]-->", "<!--[code]-->", "<!--[severity]-->", "<!--[message]-->", "<!--[mfachallengetrnrs]-->", "<!--[sesscookie]-->", "<!--[accesskey]-->"), array($now, $org, $content, $code, $severity, $message, $mfachallengetrnrs, $sesscookie, $accesskey), file_get_contents(ENV_FILE_DIR_COMMON . ENV_FILE_TEMPLATE_OFX));
}

// �e���v���[�g���HTML�𐶐�����
function generate_html($settings, $resp) {
	$ret = "";
	$str = "";
	
	switch($resp["status"]) {
	case ENV_NUM_STATUS_ADDITION:
		// �ǉ��F�؉�ʂ𐶐�����
		$title = "�ǉ��F��";
		$addr = str_replace("?" . $_SERVER["QUERY_STRING"], "", $_SERVER["REQUEST_URI"]);
		$defs = parse_csv($settings["auth"]);
		list($text, $attr, $type) = $defs[0];
		$attr = strtolower($attr);
		switch($attr) {
		case "text":
		case "password":
		case "hidden":
			break;
		default:
			$attr = "text";
			break;
		}
		$str .= "<p>�{�l�m�F�̂��߁A" . mb_htmlspecialchars($settings["name"]) . "���ǉ��F�؂����߂��Ă��܂��B</p>\r\n";
		$str .= "<form method=\"post\" action=\"" . $addr . "\" enctype=\"application/x-www-form-urlencoded\" accept-charset=\"UTF-8\" class=\"ofxform\" onsubmit=\"return exec_ofxform(this);\">\r\n";
		$str .= "<dl>\r\n";
		$str .= "<dt>�ǉ��F��</dt>\r\n";
		if($type == "image") {
			$str .= "<dd><img src=\"" . $resp["addition"] . "\" alt=\"�摜\" /></dd>\r\n";
		} else {
			$str .= "<dd>" . mb_htmlspecialchars($resp["addition"]) . "</dd>\r\n";
		}
		$str .= "<dt>" . mb_htmlspecialchars($text) . "</dt>\r\n";
		$str .= "<dd><input type=\"" . $attr . "\" name=\"" . $resp["aid"] . "\" value=\"\" size=\"16\" maxlength=\"256\" class=\"ofxinput\" /><input type=\"hidden\" name=\"sesscookie\" value=\"" . $resp["sesscookie"] . "\" /><input type=\"hidden\" name=\"accesskey\" value=\"" . $resp["accesskey"] . "\" /></dd>\r\n";
		$str .= "</dl>\r\n";
		$str .= "<div class=\"ofximage\"><input type=\"hidden\" name=\"fiid\" value=\"" . $settings["fiid"] . "\" /><img src=\"" . ENV_FILE_DIR_CLIENT . "btn_1.gif\" width=\"107\" height=\"40\" alt=\"We Support OFX���S\" /><input type=\"image\" src=\"" . ENV_FILE_DIR_CLIENT . "btn_3.gif\" alt=\"���s����\" style=\"width: 77px; height: 40px;\" /><a href=\"" . $addr . "\" class=\"ofxlink\" onclick=\"self.window.history.go(-" . $resp["sid"] . "); return false;\" onkeypress=\"this.onclick();\"><img src=\"" . ENV_FILE_DIR_CLIENT . "btn_4.gif\" width=\"75\" height=\"40\" alt=\"���~����\" /></a><a href=\"" . ENV_FILE_DIR_CLIENT . "help.html#" . $settings["fiid"] . "\" class=\"ofxlink\" onclick=\"return help_ofxform(this);\" onkeypress=\"this.onclick();\"><img src=\"" . ENV_FILE_DIR_CLIENT . "btn_5.gif\" width=\"61\" height=\"40\" alt=\"�w���v\" /></a></div>\r\n";
		$str .= "</form>\r\n";
		break;
	case ENV_NUM_STATUS_FAILURE:
		// �F�؃G���[��ʂ𐶐�����
		$title = "�F�؃G���[";
		$str .= "<p>���O�C���ł��܂���ł����B</p>\r\n";
		$str .= "<p>���O�C�����̓��e���m�F������A�Ď��s���Ă��������B</p>\r\n";
		break;
	case ENV_NUM_STATUS_MAINTENANCE:
		// �����e�i���X�G���[��ʂ𐶐�����
		$title = "�����e�i���X�G���[";
		$str .= "<p>�V�X�e�������e�i���X��ʂ��\�����ꂽ���߁A�����𒆒f���܂����B</p>\r\n";
		$str .= "<p><a href=\"" . mb_htmlspecialchars($settings["home"]) . "\" class=\"ofxlink\" onclick=\"return link_ofxform(this);\" onkeypress=\"this.onclick();\">" . mb_htmlspecialchars($settings["name"]) . "</a>�̃V�X�e�������e�i���X���I��������A�Ď��s���Ă��������B</p>\r\n";
		break;
	case ENV_NUM_STATUS_CAUTION:
		// ���擾�G���[��ʂ𐶐�����
		$title = "���擾�G���[";
		$str .= "<p>�u�d�v�Ȃ��m�点�v���̉�ʂ��\�����ꂽ���߁A�����𒆒f���܂����B</p>\r\n";
		$str .= "<p><a href=\"" . mb_htmlspecialchars($settings["home"]) . "\" class=\"ofxlink\" onclick=\"return link_ofxform(this);\" onkeypress=\"this.onclick();\">" . mb_htmlspecialchars($settings["name"]) . "</a>�Ƀ��O�C�����A��ʂ��m�F������A�Ď��s���Ă��������B</p>\r\n";
		break;
	case ENV_NUM_STATUS_SUCCESS:
	case ENV_NUM_STATUS_NONE:
	default:
		// �������Ȃ�
		break;
	}
	
	if($str != "") {
		if($resp["body"] != "") {
			$str .= "<p><input type=\"button\" value=\"�߂�\" onclick=\"javascript: self.window.history.go(-1); return false;\" onkeypress=\"this.click();\" /></p>\r\n";
			$str .= "<p><input type=\"button\" value=\"�ǉ����\" onclick=\"javascript: self.document.getElementById(&quot;addmsg&quot;).style.display = &quot;block&quot;; this.style.display=&quot;none&quot;;\" onkeypress=\"this.click();\" /></p>\r\n";
			$str .= "<div id=\"addmsg\" style=\"display: none;\">\r\n";
			$str .= "<pre style=\"color: #333333; font-family: monospace; line-height: 110%;\">" . mb_htmlspecialchars($resp["body"]) . "</pre>\r\n";
			$str .= "</div>\r\n";
		}
		$title = mb_convert_encoding($title, "UTF-8", "Shift_JIS");
		$str = mb_convert_encoding($str, "UTF-8", "Shift_JIS");
		$ret = str_replace(array("<!--[title]-->", "<!--[content]-->"), array(trim($title), trim($str)), file_get_contents(ENV_FILE_DIR_CLIENT . ENV_FILE_TEMPLATE_HTML));
	}
	return $ret;
}

// $code�ɍ��v����HTTP/1.1�X�e�[�^�X�R�[�h�̕�������擾����
function get_http_status($code) {
	$status = "Unknown Status-code";
	switch((integer)$code) {
	// 100
	case 100:
		$status = "Continue";
		break;
	case 101:
		$status = "Switching Protocols";
		break;
	case 102:
		$status = "Processing";
		break;
	// 200
	case 200:
		$status = "OK";
		break;
	case 201:
		$status = "Created";
		break;
	case 202:
		$status = "Accepted";
		break;
	case 203:
		$status = "Non-Authoritative Information";
		break;
	case 204:
		$status = "No Content";
		break;
	case 205:
		$status = "Reset Content";
		break;
	case 206:
		$status = "Partial Content";
		break;
	case 207:
		$status = "Multi-Status";
		break;
	case 208:
		$status = "Already Reported";
		break;
	case 226:
		$status = "IM Used";
		break;
	// 300
	case 300:
		$status = "Multiple Choices";
		break;
	case 301:
		$status = "Moved Parmanently";
		break;
	case 302:
		$status = "Found";
		break;
	case 303:
		$status = "See Other";
		break;
	case 304:
		$status = "Not Modified";
		break;
	case 305:
		$status = "Use Proxy";
		break;
	case 306:
		$status = "(Unused)";
		break;
	case 307:
		$status = "Temporary Redirect";
		break;
	case 308:
		$status = "Permanent Redirect";
		break;
	// 400
	case 400:
		$status = "Bad Request";
		break;
	case 401:
		$status = "Unautorized";
		break;
	case 402:
		$status = "Payment Required";
		break;
	case 403:
		$status = "Forbidden";
		break;
	case 404:
		$status = "Not Found";
		break;
	case 405:
		$status = "Method Not Allowed";
		break;
	case 406:
		$status = "Not Acceptable";
		break;
	case 407:
		$status = "Proxy Authentication Required";
		break;
	case 408:
		$status = "Request Timeout";
		break;
	case 409:
		$status = "Conflict";
		break;
	case 410:
		$status = "Gone";
		break;
	case 411:
		$status = "Length Required";
		break;
	case 412:
		$status = "Precondition Failed";
		break;
	case 413:
		$status = "Request Entity Too Large";
		break;
	case 414:
		$status = "Request-URI Too Long";
		break;
	case 415:
		$status = "Unsupported Media Type";
		break;
	case 416:
		$status = "Requested Range Not Satisfiable";
		break;
	case 417:
		$status = "Expectation Failed";
		break;
	case 422:
		$status = "Unprocessable Entity";
		break;
	case 423:
		$status = "Locked";
		break;
	case 424:
		$status = "Failed Dependency";
		break;
	case 426:
		$status = "Upgrade Required";
		break;
	case 428:
		$status = "Precondition Required";
		break;
	case 429:
		$status = "Too Many Requests";
		break;
	case 431:
		$status = "Request Header Fields Too Large";
		break;
	// 500
	case 500:
		$status = "Internal Server Error";
		break;
	case 501:
		$status = "Not Implemented";
		break;
	case 502:
		$status = "Bad Gateway";
		break;
	case 503:
		$status = "Service Unavailable";
		break;
	case 504:
		$status = "Gateway Timeout";
		break;
	case 505:
		$status = "HTTP Version Not Supported";
		break;
	case 506:
		$status = "Variant Also Negotiates";
		break;
	case 507:
		$status = "Insufficient Storage";
		break;
	case 508:
		$status = "Loop Detected";
		break;
	case 510:
		$status = "Not Extended";
		break;
	case 511:
		$status = "Network Authentication Required";
		break;
	default:
		// �������Ȃ�
		break;
	}
	return "HTTP/1.1 " . (string)$code . " " . $status;
}

?>