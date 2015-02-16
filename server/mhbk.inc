<?php
/*
mhbk.inc: �݂��ً�s��OFX���OFX�t�@�C����ϊ��E��������
Copyright (C) 2012-2015 OFFICE OUTERGUY. All rights reserved.
mailto:contact@beatrek.com
Licensed under the GNU AGPLv3.
*/

// ���O�C�������擾����
$user = $ofxforms["txbCustNo"];
$pass = $ofxforms["PASSWD_LoginPwdInput"];
$auth = urlencode(mb_convert_encoding(urldecode($ofxforms["txbTestWord"]), "Shift_JIS", "UTF-8"));
$sesscookie = $ofxforms["sesscookie"];
$accesskey = $ofxforms["accesskey"];

$resp = array();
$sid = 0;
$method = "";
$query = "";
$cookie = "";
$head = "";
$body = "";

$ofx = "";

if($sesscookie != "") list($cookie, $dummy) = explode("\t", sess_decode($sesscookie));
if($accesskey != "") {
	list($ssid, $method, $uri, $query, $pass, $dummy) = explode("\t", sess_decode($accesskey), 6);
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
	list($head, $body) = mhbk_http11($method, $uris, $query, $cookie);
	
	// ���_�C���N�g����
	$retry = 0;
	while($retry++ < ENV_NUM_HTTP11_RETRY) {
		$locations = parse_header($head, "location");
		if(count($locations) > 0) {
			$method = "GET";
			$uris = parse_uri($locations[0], $uris);
			$query = "";
			$cookie = mhbk_update_cookie($head, $cookie);
			list($head, $body) = mhbk_http11($method, $uris, $query, $cookie);
		}
	}
	
	// ���O�C����ʂ��擾����
	$as = parse_tag($body, "a");
	foreach($as as $a) if(parse_tag_search(parse_tag($a["innerHTML"], "img"), "alt", "�݂��ك_�C���N�g���O�C��") != -1) {
		$method = "GET";
		$uris = parse_uri($a["href"], $uris);
		$query = "";
		$cookie = mhbk_update_cookie($head, $cookie);
		list($head, $body) = mhbk_http11($method, $uris, $query, $cookie);
		break;
	}
	
	// ���S�ɂ����p�����������߂̂����Ӊ�ʂ��\�������ꍇ�A���߂ă��O�C����ʂ��擾����
	$as = parse_tag($body, "a");
	foreach($as as $a) if(parse_tag_search(parse_tag($a["innerHTML"], "img"), "alt", "�݂��ك_�C���N�g���O�C����ʂɐi��") != -1) {
		$method = "GET";
		$uris = parse_uri($a["href"], $uris);
		$query = "";
		$cookie = mhbk_update_cookie($head, $cookie);
		list($head, $body) = mhbk_http11($method, $uris, $query, $cookie);
		break;
	}
	
	// ���_�C���N�g����
	$metas = parse_tag($body, "meta");
	$c = parse_tag_search($metas, "http-equiv", "refresh");
	if($c != -1) {
		list($dummy, $uri) = explode("=", $metas[$c]["content"], 2);
		
		$method = "GET";
		$uris = parse_uri(trim($uri), $uris);
		$query = "";
		$cookie = mhbk_update_cookie($head, $cookie);
		list($head, $body) = mhbk_http11($method, $uris, $query, $cookie);
	}
	
	$loop = 0;
	do {
		// ���q���ܔԍ�����͂���
		$forms = parse_tag($body, "form");
		$c = parse_tag_search($forms, "name", "LOGBNK_00000B");
		if($c != -1) {
			$inputs = parse_tag($forms[$c]["innerHTML"], "input");
			$queries = array();
			foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
			// $queries["pm_fp"] = "pm_fp=";
			$queries["txbCustNo"] = "txbCustNo=" . $user;
			
			$method = $forms[$c]["method"];
			$uris = parse_uri($forms[$c]["action"], $uris);
			$query = implode("&", $queries);
			$cookie = mhbk_update_cookie($head, $cookie);
			list($head, $body) = mhbk_http11($method, $uris, $query, $cookie);
		}
		
		// ���_�C���N�g����
		$retry = 0;
		while($retry++ < ENV_NUM_HTTP11_RETRY) {
			$locations = parse_header($head, "location");
			if(count($locations) > 0) {
				$method = "GET";
				$uris = parse_uri($locations[0], $uris);
				$query = "";
				$cookie = mhbk_update_cookie($head, $cookie);
				list($head, $body) = mhbk_http11($method, $uris, $query, $cookie);
			}
		}
	} while($loop++ < ENV_NUM_HTTP11_RETRY && strpos($body, "���q���ܔԍ������") !== false);
}

if(strpos($body, "�����t�m�F�P") !== false) {
	// �����t����͂���i�P�j
	$spans = parse_tag($body, "span");
	$c = parse_tag_search($spans, "id", "txtQuery");
	if($c != -1) {
		$shitsumon = trim(strip_tags($spans[$c]["innerHTML"]));
	}
	
	$forms = parse_tag($body, "form");
	$c = parse_tag_search($forms, "name", "LOGWRD_00100B");
	if($c != -1) {
		$inputs = parse_tag($forms[$c]["innerHTML"], "input");
		$queries = array();
		foreach($inputs as $input) {
			if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
			if($input["value"] == "����" && preg_match("/[^\']*\'\/([^\']*)\'[^\']*/", $input["onclick"], $matches) > 0) {
				$forms[$c]["action"] = $matches[1] . ".do";
			}
		}
		$queries["txbTestWord"] = "txbTestWord=";
		
		$method = $forms[$c]["method"];
		$uris = parse_uri($forms[$c]["action"], $uris);
		$query = implode("&", $queries);
		$cookie = mhbk_update_cookie($head, $cookie);
	}
	
	// �Z�b�V������ޔ�����
	$sid = 1;
	$sess = sess_encode(implode("\t", array((string)$sid, $method, $uris["scheme"] . "://" . $uris["host"] . $uris["path"], $query, $cookie, $pass, ENV_STR_SESSION_PADDING)));
	$head = "";
	$body = "";
} else if($sid == 1) {
	// �Z�b�V�����𕜌�����
	$sid = 0;
	$uris = parse_uri($uri);
	$query = str_replace("txbTestWord=", "txbTestWord=" . $auth, $query);
	list($head, $body) = mhbk_http11($method, $uris, $query, $cookie);
	
	// ���_�C���N�g����
	$retry = 0;
	while($retry++ < ENV_NUM_HTTP11_RETRY) {
		$locations = parse_header($head, "location");
		if(count($locations) > 0) {
			$method = "GET";
			$uris = parse_uri($locations[0], $uris);
			$query = "";
			$cookie = mhbk_update_cookie($head, $cookie);
			list($head, $body) = mhbk_http11($method, $uris, $query, $cookie);
		}
	}
}

if(strpos($body, "�����t�m�F�Q") !== false) {
	// �����t����͂���i�Q�j
	$spans = parse_tag($body, "span");
	$c = parse_tag_search($spans, "id", "txtQuery");
	if($c != -1) {
		$shitsumon = trim(strip_tags($spans[$c]["innerHTML"]));
	}
	
	$forms = parse_tag($body, "form");
	$c = parse_tag_search($forms, "name", "LOGWRD_00100B");
	if($c != -1) {
		$inputs = parse_tag($forms[$c]["innerHTML"], "input");
		$queries = array();
		foreach($inputs as $input) {
			if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
			if($input["value"] == "����" && preg_match("/[^\']*\'\/([^\']*)\'[^\']*/", $input["onclick"], $matches) > 0) {
				$forms[$c]["action"] = $matches[1] . ".do";
			}
		}
		$queries["txbTestWord"] = "txbTestWord=";
		if(isset($queries["chkConfItemChk"]) == true) {
			if(ENV_BOOL_ADD_RISKBASE == true) {
				// ���񃍃O�C�����ɍ����t����͂��Ȃ�
				$queries["chkConfItemChk"] = "chkConfItemChk=";
			} else {
				// ���񃍃O�C�����ɍ����t����͂���
				$queries["chkConfItemChk"] = "chkConfItemChk=on";
			}
		}
		
		$method = $forms[$c]["method"];
		$uris = parse_uri($forms[$c]["action"], $uris);
		$query = implode("&", $queries);
		$cookie = mhbk_update_cookie($head, $cookie);
	}
	
	// �Z�b�V������ޔ�����
	$sid = 2;
	$sess = sess_encode(implode("\t", array((string)$sid, $method, $uris["scheme"] . "://" . $uris["host"] . $uris["path"], $query, $cookie, $pass, ENV_STR_SESSION_PADDING)));
	$head = "";
	$body = "";
} else if($sid == 2) {
	// �Z�b�V�����𕜌�����
	$sid = 0;
	$uris = parse_uri($uri);
	$query = str_replace("txbTestWord=", "txbTestWord=" . $auth, $query);
	list($head, $body) = mhbk_http11($method, $uris, $query, $cookie);
	
	// ���_�C���N�g����
	$retry = 0;
	while($retry++ < ENV_NUM_HTTP11_RETRY) {
		$locations = parse_header($head, "location");
		if(count($locations) > 0) {
			$method = "GET";
			$uris = parse_uri($locations[0], $uris);
			$query = "";
			$cookie = mhbk_update_cookie($head, $cookie);
			list($head, $body) = mhbk_http11($method, $uris, $query, $cookie);
		}
	}
}

if(strpos($body, "���O�C���p�X���[�h�𔼊p�p�����œ���") !== false) {
	// ���O�C���p�X���[�h����͂���
	$forms = parse_tag($body, "form");
	$c = parse_tag_search($forms, "name", "LOGBNK_00005B");
	if($c != -1) {
		$inputs = parse_tag($forms[$c]["innerHTML"], "input");
		$queries = array();
		foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
		if(isset($queries["Cancel"]) == true) unset($queries["Cancel"]);
		$queries["PASSWD_LoginPwdInput"] = "PASSWD_LoginPwdInput=" . $pass;
		
		$method = $forms[$c]["method"];
		$uris = parse_uri($forms[$c]["action"], $uris);
		$query = implode("&", $queries);
		$cookie = mhbk_update_cookie($head, $cookie);
		list($head, $body) = mhbk_http11($method, $uris, $query, $cookie);
	}
	
	// ���_�C���N�g����
	$retry = 0;
	while($retry++ < ENV_NUM_HTTP11_RETRY) {
		$locations = parse_header($head, "location");
		if(count($locations) > 0) {
			$method = "GET";
			$uris = parse_uri($locations[0], $uris);
			$query = "";
			$cookie = mhbk_update_cookie($head, $cookie);
			list($head, $body) = mhbk_http11($method, $uris, $query, $cookie);
		}
	}
}

if($sid > 0) {
	// �Z�b�V�����������p��
	$resp["status"] = ENV_NUM_STATUS_ADDITION;
	$resp["aid"] = "txbTestWord";
	$resp["addition"] = $shitsumon;
	$resp["sess"] = $sess;
	$resp["sid"] = $sid;
	$resp["sesscookie"] = sess_encode(implode("\t", array($cookie, ENV_STR_SESSION_PADDING)));
	$resp["accesskey"] = sess_encode(implode("\t", array((string)$sid, $method, $uris["scheme"] . "://" . $uris["host"] . $uris["path"], $query, $pass, ENV_STR_SESSION_PADDING)));
	
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
} else if(strpos($body, "�T�[�r�X�͂����p���������܂���") !== false || strpos($body, "�ȉ��̎��ԑт́A�C���^�[�l�b�g�o���L���O�������p���������܂���") !== false) {
	// �V�X�e�������e�i���X��ʂ̏ꍇ
	$resp["status"] = ENV_NUM_STATUS_MAINTENANCE;
	$resp["method"] = $method;
	$resp["uri"] = $uris["scheme"] . "://" . $uris["host"] . $uris["path"];
	$resp["query"] = $query;
	$resp["cookie"] = $cookie;
	$resp["head"] = $head;
	$resp["body"] = $body;
	$resp["ofx"] = generate_ofx($resp["status"]);
} else if(strpos($body, "���C�����j���[") === false) {
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
	// ���o�����׏Ɖ��ʂ��擾����
	$forms = parse_tag($body, "form");
	$c = parse_tag_search($forms, "name", "MENTOP_02000B");
	if($c != -1) {
		$inputs = parse_tag($forms[$c]["innerHTML"], "input");
		$queries = array();
		foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
		
		$as = parse_tag($body, "a");
		foreach($as as $a) if(trim(strip_tags($a["innerHTML"])) == "���o�����׏Ɖ�" && preg_match("/[^\']*\'\/([^\']*)\'[^\']*/", $a["onclick"], $matches) > 0) {
			$forms[$c]["action"] = $matches[1] . ".do";
			break;
		}
		$method = $forms[$c]["method"];
		$uris = parse_uri($forms[$c]["action"], $uris);
		$query = implode("&", $queries);
		$cookie = mhbk_update_cookie($head, $cookie);
		list($head, $body) = mhbk_http11($method, $uris, $query, $cookie);
	}
	
	// ���_�C���N�g����
	$retry = 0;
	while($retry++ < ENV_NUM_HTTP11_RETRY) {
		$locations = parse_header($head, "location");
		if(count($locations) > 0) {
			$method = "GET";
			$uris = parse_uri($locations[0], $uris);
			$query = "";
			$cookie = mhbk_update_cookie($head, $cookie);
			list($head, $body) = mhbk_http11($method, $uris, $query, $cookie);
		}
	}
	
	$accounts = array();
	
	// ���������擾����
	$selects = parse_tag($body, "select");
	$c = parse_tag_search($selects, "name", "lstAccSel");
	if($c != -1) {
		$options = parse_tag($selects[$c]["innerHTML"], "option");
		$c = 0;
		foreach($options as $option) {
			list($branchname, $accttype, $acctid) = explode(" ", trim(strip_tags($option["innerHTML"])), 3);
			$accounts[$c]["id"] = $option["value"];
			$accounts[$c]["branchname"] = $branchname;
			$accounts[$c]["acctid"] = $acctid;
			$accounts[$c]["acctname"] = implode(ENV_CHR_CONCATENATOR, array($settings["name"], $branchname, $accttype));
			$c++;
		}
	}
	
	// MSMoney�_�E�����[�h��ʂ��擾����
	$forms = parse_tag($body, "form");
	$c = parse_tag_search($forms, "name", "ACCHST_04000B");
	if($c != -1) {
		$inputs = parse_tag($forms[$c]["innerHTML"], "input");
		$queries = array();
		foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
		
		$as = parse_tag($body, "a");
		foreach($as as $a) if(trim(strip_tags($a["innerHTML"])) == "������" && preg_match("/[^\']*\'\/([^\']*)\'[^\']*/", $a["href"], $matches) > 0) {
			$forms[$c]["action"] = $matches[1] . ".do";
			break;
		}
		$method = $forms[$c]["method"];
		$uris = parse_uri($forms[$c]["action"], $uris);
		$query = implode("&", $queries);
		$cookie = mhbk_update_cookie($head, $cookie);
		list($head, $body) = mhbk_http11($method, $uris, $query, $cookie);
	}
	
	// ���_�C���N�g����
	$retry = 0;
	while($retry++ < ENV_NUM_HTTP11_RETRY) {
		$locations = parse_header($head, "location");
		if(count($locations) > 0) {
			$method = "GET";
			$uris = parse_uri($locations[0], $uris);
			$query = "";
			$cookie = mhbk_update_cookie($head, $cookie);
			list($head, $body) = mhbk_http11($method, $uris, $query, $cookie);
		}
	}
	
	// �����������[�v����
	foreach($accounts as $account) {
		// ���s���ԁi�^�C���A�E�g�j���Đݒ肷��
		@set_time_limit(ENV_NUM_TIMEOUT);
		
		// ���׃_�E�����[�h�����E�͈͉�ʂ��擾����
		$forms = parse_tag($body, "form");
		$c = parse_tag_search($forms, "name", "ACCMNY_04610B");
		if($c != -1) {
			$queries = array();
			$inputs = parse_tag($forms[$c]["innerHTML"], "input");
			foreach($inputs as $input) {
				if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
				if($input["value"] == "����" && preg_match("/[^\']*\'\/([^\']*)\'[^\']*/", $input["onclick"], $matches) > 0) {
					$forms[$c]["action"] = $matches[1] . ".do";
				}
			}
			$selects = parse_tag($forms[$c]["innerHTML"], "select");
			foreach($selects as $select) {
				$options = parse_tag($select["innerHTML"], "option");
				if(count($options) > 0) if($select["name"] != "") $queries[$select["name"]] = urlencode($select["name"]) . "=" . urlencode($options[0]["value"]);
			}
			$queries["rdoAccSel"] = "rdoAccSel=" . $account["id"];
			
			$method = $forms[$c]["method"];
			$uris = parse_uri($forms[$c]["action"], $uris);
			$query = implode("&", $queries);
			$cookie = mhbk_update_cookie($head, $cookie);
			list($head, $body) = mhbk_http11($method, $uris, $query, $cookie);
		}
		
		// ���_�C���N�g����
		$retry = 0;
		while($retry++ < ENV_NUM_HTTP11_RETRY) {
			$locations = parse_header($head, "location");
			if(count($locations) > 0) {
				$method = "GET";
				$uris = parse_uri($locations[0], $uris);
				$query = "";
				$cookie = mhbk_update_cookie($head, $cookie);
				list($head, $body) = mhbk_http11($method, $uris, $query, $cookie);
			}
		}
		
		// ���_�C���N�g����
		$forms = parse_tag($body, "form");
		$c = parse_tag_search($forms, "name", "ACCMNY_04620B");
		if($c != -1) {
			$inputs = parse_tag($forms[$c]["innerHTML"], "input");
			$queries = array();
			foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
			
			$method = $forms[$c]["method"];
			$uris = parse_uri($forms[$c]["action"], $uris);
			$query = implode("&", $queries);
			$cookie = mhbk_update_cookie($head, $cookie);
			list($head, $body) = mhbk_http11($method, $uris, $query, $cookie);
		}
		
		// ���_�C���N�g����
		$retry = 0;
		while($retry++ < ENV_NUM_HTTP11_RETRY) {
			$locations = parse_header($head, "location");
			if(count($locations) > 0) {
				$method = "GET";
				$uris = parse_uri($locations[0], $uris);
				$query = "";
				$cookie = mhbk_update_cookie($head, $cookie);
				list($head, $body) = mhbk_http11($method, $uris, $query, $cookie);
			}
		}
		
		$body_old = $body;
		
		// OFX�t�@�C�����擾����
		$forms = parse_tag($body, "form");
		$c = parse_tag_search($forms, "name", "ACCMNY_04630B");
		if($c != -1) {
			$inputs = parse_tag($forms[$c]["innerHTML"], "input");
			$queries = array();
			foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
			
			$as = parse_tag($body, "a");
			foreach($as as $a) if(parse_tag_search(parse_tag($a["innerHTML"], "img"), "width", "135") != -1 && preg_match("/[^\']*\'\/([^\']*)\'[^\']*/", $a["href"], $matches) > 0) {
				$forms[$c]["action"] = $matches[1] . ".do";
				break;
			}
			$method = $forms[$c]["method"];
			$uris = parse_uri($forms[$c]["action"], $uris);
			$query = implode("&", $queries);
			$cookie = mhbk_update_cookie($head, $cookie);
			list($head, $body) = mhbk_http11($method, $uris, $query, $cookie);
		}
		
		// OFX�t�@�C����ϊ�����
		if(strpos($head, "Content-Type: text/html") === false) {
			$str = convert_ofx($body);
			
			// BANKTRANLIST��؂�o��
			$banktranlists = parse_tag($str, "BANKTRANLIST");
			foreach($banktranlists as $banktranlist) {
				$buf = "\r\n";
				$buf .= parse_tag($banktranlist["innerHTML"], "DTSTART")[0]["match"] . "\r\n";
				$buf .= parse_tag($banktranlist["innerHTML"], "DTEND")[0]["match"] . "\r\n";
				
				$stmttrns = parse_tag($banktranlist["innerHTML"], "STMTTRN");
				
				// STMTTRN�������ɑ}���\�[�g����
				for($i = 1; $i < count($stmttrns); $i++) {
					$temp = $stmttrns[$i];
					$p = parse_tag($temp["innerHTML"], "DTPOSTED")[0]["innerHTML"];
					for($j = $i - 1; $j >= 0; $j--) {
						if(parse_tag($stmttrns[$j]["innerHTML"], "DTPOSTED")[0]["innerHTML"] <= $p) break;
						$stmttrns[$j + 1] = $stmttrns[$j];
					}
					$stmttrns[$j + 1] = $temp;
				}
				foreach($stmttrns as $stmttrn) $buf .= $stmttrn["match"] . "\r\n";
				
				// BANKTRANLIST���\�[�g��̕�����ɒu������
				$str = str_replace($banktranlist["innerHTML"], $buf, $str);
			}
			
			// BRANCHID��␳����
			// $str = str_replace("<BRANCHID>0" . $account["branchid"] . "</BRANCHID>", "<BRANCHID>" . $account["branchid"] . "</BRANCHID>", $str);
			$str = preg_replace("/(<BRANCHID>)0([0-9]{3})(<\/BRANCHID>)/", "\\1\\2\\3", $str);
			
			// ACCTID��␳����
			$str = str_replace("<ACCTID>00" . $account["acctid"] . "</ACCTID>", "<ACCTID>" . $account["acctid"] . "</ACCTID>", $str);
			
			// MEMO��NAME�ɕ␳����
			$str = str_replace(array("<MEMO>", "</MEMO>"), array("<NAME>", "</NAME>"), $str);
			
			// TRNAMT�̐��l�̏�����␳����
			$trnamts = parse_tag($str, "TRNAMT");
			foreach($trnamts as $trnamt) $str = str_replace($trnamt["match"], "<TRNAMT>" . (string)((integer)$trnamt["innerHTML"]) . "</TRNAMT>", $str);
			
			// BALAMT�̐��l�̏�����␳����
			$balamts = parse_tag($str, "BALAMT");
			foreach($balamts as $balamt) $str = str_replace($balamt["match"], "<BALAMT>" . (string)((integer)$balamt["innerHTML"]) . "</BALAMT>", $str);
			
			// �������̂��o�͂���
			if($account["acctname"] != "") $str = str_replace("</STMTRS>", "<MKTGINFO>" . mb_convert_string($account["acctname"]) . "</MKTGINFO>\r\n</STMTRS>", $str);
			
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
		
		$body = $body_old;
		
		// �O��ʂ��擾����
		$forms = parse_tag($body, "form");
		$c = parse_tag_search($forms, "name", "ACCMNY_04630B");
		if($c != -1) {
			$queries = array();
			$inputs = parse_tag($forms[$c]["innerHTML"], "input");
			foreach($inputs as $input) {
				if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
				if($input["value"] == "�����E�͈͂��đI��" && preg_match("/[^\']*\'\/([^\']*)\'[^\']*/", $input["onclick"], $matches) > 0) {
					$forms[$c]["action"] = $matches[1] . ".do";
				}
			}
			$selects = parse_tag($forms[$c]["innerHTML"], "select");
			foreach($selects as $select) {
				$options = parse_tag($select["innerHTML"], "option");
				if(count($options) > 0) if($select["name"] != "") $queries[$select["name"]] = urlencode($select["name"]) . "=" . urlencode($options[0]["value"]);
			}
			$queries["rdoAccSel"] = "rdoAccSel=" . $account["id"];
			
			$method = $forms[$c]["method"];
			$uris = parse_uri($forms[$c]["action"], $uris);
			$query = implode("&", $queries);
			$cookie = mhbk_update_cookie($head, $cookie);
			list($head, $body) = mhbk_http11($method, $uris, $query, $cookie);
		}
		
		// ���_�C���N�g����
		$retry = 0;
		while($retry++ < ENV_NUM_HTTP11_RETRY) {
			$locations = parse_header($head, "location");
			if(count($locations) > 0) {
				$method = "GET";
				$uris = parse_uri($locations[0], $uris);
				$query = "";
				$cookie = mhbk_update_cookie($head, $cookie);
				list($head, $body) = mhbk_http11($method, $uris, $query, $cookie);
			}
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
		$cookie = mhbk_update_cookie($head, $cookie);
		list($head, $body) = mhbk_http11($method, $uris, $query, $cookie);
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
			$cookie = mhbk_update_cookie($head, $cookie);
			list($head, $body) = mhbk_http11($method, $uris, $query, $cookie);
		}
	}
	
	// OFX�t�@�C�����o�͂���
	$resp["status"] = ENV_NUM_STATUS_SUCCESS;
	if($ofx == "") {
		// OFX�f�[�^�����݂��Ȃ��ꍇ
		$resp["ofx"] = generate_ofx($resp["status"]);
	} else {
		// OFX�f�[�^�����݂���ꍇ
		$resp["ofx"] = $ofx;
	}
}
return $resp;

// HTTP/1.1
function mhbk_http11($method, $uris, $query = "", $cookie = "") {
	$ret = "INVALID HOST";
	if(preg_match("/\.mizuhobank\.co\.jp$/", $uris["host"]) > 0) $ret = http11(strtoupper($method), $uris["scheme"], $uris["host"], 0, $uris["path"], $query, "", $cookie);
	return explode("\r\n\r\n", $ret, 2);
}

function mhbk_update_cookie($head, $cookie) {
	return update_cookie(array("PMData", "BROWSER_TOKEN", "JSESSIONID", "Token", "ServerName"), parse_header($head, "set-cookie"), $cookie);
}

?>