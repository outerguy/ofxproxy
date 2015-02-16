<?php
/*
btmu.inc: �O�H����UFJ��s��CSV���OFX�t�@�C���𐶐�����
Copyright (C) 2012-2015 OFFICE OUTERGUY. All rights reserved.
mailto:contact@beatrek.com
Licensed under the GNU AGPLv3.
*/

// ���O�C�������擾����
$user = str_replace("-", "", $ofxforms["KEIYAKU_NO"]);
$pass = $ofxforms["PASSWORD"];
$auth = urlencode(mb_convert_encoding(urldecode($ofxforms["PASSWORD2"]), "Shift_JIS", "UTF-8"));
$sesscookie = $ofxforms["sesscookie"];
$accesskey = $ofxforms["accesskey"];

$resp = array();
$sid = 0;
$method = "";
$query = "";
$cookie = "";
$head = "";
$body = "";

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
	list($head, $body) = btmu_http11($method, $uris, $query, $cookie);
	
	// ���O�C����ʂ��擾����
	$as = parse_tag($body, "a");
	foreach($as as $a) if(parse_tag_search(parse_tag($a["innerHTML"], "img"), "alt", "���O�C��") != -1 && preg_match("/javascript:[^\']+?\'([^\']+)\'.*?/i", $a["href"], $matches) > 0) {
		$method = "GET";
		$uris = parse_uri($matches[1], $uris);
		$query = "";
		$cookie = btmu_update_cookie($head, $cookie);
		list($head, $body) = btmu_http11($method, $uris, $query, $cookie);
		break;
	}
	
	// ���O�C������
	$forms = parse_tag($body, "form");
	$c = parse_tag_search($forms, "name", "MainForm");
	if($c != -1) {
		$inputs = parse_tag($forms[$c]["innerHTML"], "input");
		$queries = array();
		foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
		$queries["_TRANID"] = "_TRANID=AA011_001"; // gotoPageFromAA011()
		$queries["KEIYAKU_NO"] = "KEIYAKU_NO=" . $user;
		$queries["PASSWORD"] = "PASSWORD=" . $pass;
		
		$method = $forms[$c]["method"];
		$uris = parse_uri($forms[$c]["action"], $uris);
		$query = implode("&", $queries);
		$cookie = btmu_update_cookie($head, $cookie);
		list($head, $body) = btmu_http11($method, $uris, $query, $cookie);
	}
	
	// ���_�C���N�g����
	$retry = 0;
	while($retry++ < ENV_NUM_HTTP11_RETRY) {
		$locations = parse_header($head, "location");
		if(count($locations) > 0) {
			$method = "GET";
			$uris = parse_uri($locations[0], $uris);
			$query = "";
			$cookie = btmu_update_cookie($head, $cookie);
			list($head, $body) = btmu_http11($method, $uris, $query, $cookie);
		}
	}
}

if(strpos($body, "���i�ƈقȂ������̃A�N�Z�X") !== false) {
	// �����^�C���p�X���[�h����͂���
	$divs = parse_tag($body, "div");
	$c = parse_tag_search($divs, "class", "serviceContents");
	if($c != -1) {
		$ps = parse_tag($divs[$c]["innerHTML"], "p");
		$c = parse_tag_search($ps, "class", "section");
		if($c != -1) $shitsumon = trim(strip_tags($ps[$c]["innerHTML"]));
	}
	
	$forms = parse_tag($body, "form");
	$c = parse_tag_search($forms, "name", "MainForm");
	if($c != -1) {
		$inputs = parse_tag($forms[$c]["innerHTML"], "input");
		$queries = array();
		foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
		$queries["_TRANID"] = "_TRANID=AA012_001"; // gotoPageFromAA012()
		$queries["PASSWORD"] = "PASSWORD=";
		
		$method = $forms[$c]["method"];
		$uris = parse_uri($forms[$c]["action"], $uris);
		$query = implode("&", $queries);
		$cookie = btmu_update_cookie($head, $cookie);
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
	$query = str_replace("PASSWORD=", "PASSWORD=" . $auth, $query);
	list($head, $body) = btmu_http11($method, $uris, $query, $cookie);
	
	// ���_�C���N�g����
	$retry = 0;
	while($retry++ < ENV_NUM_HTTP11_RETRY) {
		$locations = parse_header($head, "location");
		if(count($locations) > 0) {
			$method = "GET";
			$uris = parse_uri($locations[0], $uris);
			$query = "";
			$cookie = btmu_update_cookie($head, $cookie);
			list($head, $body) = btmu_http11($method, $uris, $query, $cookie);
		}
	}
}

if($sid > 0) {
	// �Z�b�V�����������p��
	$resp["status"] = ENV_NUM_STATUS_ADDITION;
	$resp["aid"] = "PASSWORD2";
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
	
	// �����ꗗ�i�c���j���݂��ʂ��擾����
	$forms = parse_tag($body, "form");
	$c = parse_tag_search($forms, "name", "MainForm");
	if($c != -1) {
		$inputs = parse_tag($forms[$c]["innerHTML"], "input");
		$queries = array();
		foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
		$as = parse_tag($body, "a");
		foreach($as as $a) if(parse_tag_search(parse_tag($a["innerHTML"], "img"), "alt", "�����ꗗ�i�c���j���݂�") != -1 && preg_match("/javascript:[^\']+?\'([^\']+)\'.*?/i", $a["onclick"], $matches) > 0) {
			$queries["_TRANID"] = "_TRANID=" . $matches[1];
			break;
		}
		
		$method = $forms[$c]["method"];
		$uris = parse_uri($forms[$c]["action"], $uris);
		$query = implode("&", $queries);
		$cookie = btmu_update_cookie($head, $cookie);
		list($head, $body) = btmu_http11($method, $uris, $query, $cookie);
	}
	
	$accs = array();
	
	// ���ʌ����̈ꗗ���擾����
	$i = 0;
	$tables = parse_tag($body, "table");
	$c = parse_tag_search($tables, "summary", "���ʗa���ꗗ�B���̗񂩂�A�����̎�ށA�������A����/�U���E�U�ւցA�֘A�����N�B");
	if($c != -1) {
		$captions = parse_tag($tables[$c]["innerHTML"], "caption");
		if(count($captions) > 0) $accttype = trim(strip_tags(str_replace("�a���ꗗ", "", $captions[0]["innerHTML"])));
		$trs = parse_tag($tables[$c]["innerHTML"], "tr");
		foreach($trs as $tr) {
			$tds = parse_tag($tr["innerHTML"], "td");
			$d = parse_tag_search($tds, "class", "balance_info");
			if($d != -1) {
				// �c�����擾����
				$ps = parse_tag($tds[$d]["innerHTML"], "p");
				$e = parse_tag_search($ps, "class", "unit");
				if($e != -1) $accounts[$i]["balance"] = parse_amount(strip_tags($ps[$e]["innerHTML"]));
				
				// ������ڂ��擾����i�u���ʁv�Œ�j
				$accounts[$i]["accttype"] = btmu_get_accttype($accttype);
				
				// �x�X���E�����ԍ����擾����
				$spans = parse_tag($tds[$d]["innerHTML"], "span");
				$e = parse_tag_search($spans, "class", "unit");
				if($e != -1) {
					$p = strrpos($spans[$e]["innerHTML"], " ");
					if($p !== false) {
						$accounts[$i]["branchid"] = btmu_get_branchid($settings, substr($spans[$e]["innerHTML"], 0, $p));
						$accounts[$i]["acctid"] = trim(strip_tags(substr($spans[$e]["innerHTML"], $p)));
						$accounts[$i]["acctname"] = implode(ENV_CHR_CONCATENATOR, array($settings["name"], substr($spans[$e]["innerHTML"], 0, $p), $accttype));
					}
				
				}
				
				// ���הԍ����擾����
				$as = parse_tag($tr["innerHTML"], "a");
				foreach($as as $a) if(parse_tag_search(parse_tag($a["innerHTML"], "img"), "alt", "����") != -1 && preg_match("/javascript:[^\(]+?\((\d+)\).*?/i", $a["onclick"], $matches) > 0) {
					$accounts[$i]["id"] = trim($matches[1]);
					$i++;
					break;
				}
			}
		}
	}
	
	// �����������[�v����
	for($i = 0; $i < count($accounts); $i++) {
		// ���s���ԁi�^�C���A�E�g�j���Đݒ肷��
		@set_time_limit(ENV_NUM_TIMEOUT);
		
		// ���׉�ʂ��擾����
		$forms = parse_tag($body, "form");
		$c = parse_tag_search($forms, "name", "MainForm");
		if($c != -1) {
			$inputs = parse_tag($forms[$c]["innerHTML"], "input");
			$queries = array();
			foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
			$queries["SENTAKU"] = "SENTAKU=" . $accounts[$i]["id"];
			$queries["_TRANID"] = "_TRANID=CO101_003"; // ����
			$queries["_TARGET"] = "_TARGET=" . $forms[$c]["name"];
			$queries["_FRAMID"] = "_FRAMID=";
			$queries["_TARGETWINID"] = "_TARGETWINID=";
			
			$method = $forms[$c]["method"];
			$uris = parse_uri($forms[$c]["action"], $uris);
			$query = implode("&", $queries);
			$cookie = btmu_update_cookie($head, $cookie);
			list($head, $body) = btmu_http11($method, $uris, $query, $cookie);
		}
		
		if(strpos($body, "�Ɖ�\�Ȗ��ׂ͂���܂���") === false) {
			// ���ׂ��_�E�����[�h��ʂ��擾����
			$forms = parse_tag($body, "form");
			$c = parse_tag_search($forms, "name", "MainForm");
			if($c != -1) {
				$inputs = parse_tag($forms[$c]["innerHTML"], "input");
				$queries = array();
				foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
				$selects = parse_tag($forms[$c]["innerHTML"], "select");
				foreach($selects as $select) {
					$options = parse_tag($select["innerHTML"], "option");
					foreach($options as $option) if($select["name"] != "" && $option["selected"] == "selected") $queries[$select["name"]] = urlencode($select["name"]) . "=" . urlencode($option["value"]);
				}
				$queries["_TRANID"] = "_TRANID=CP105_006"; // ���ׂ��_�E�����[�h
				$queries["_TARGET"] = "_TARGET=" . $forms[$c]["name"];
				$queries["_FRAMID"] = "_FRAMID=";
				$queries["_TARGETWINID"] = "_TARGETWINID=";
				
				$method = $forms[$c]["method"];
				$uris = parse_uri($forms[$c]["action"], $uris);
				$query = implode("&", $queries);
				$cookie = btmu_update_cookie($head, $cookie);
				list($head, $body) = btmu_http11($method, $uris, $query, $cookie);
			}
			
			$body_old = $body;
			
			// �_�E�����[�h��ʂ��擾����
			$forms = parse_tag($body, "form");
			$c = parse_tag_search($forms, "name", "MainForm");
			if($c != -1) {
				$inputs = parse_tag($forms[$c]["innerHTML"], "input");
				$queries = array();
				foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
				$selects = parse_tag($forms[$c]["innerHTML"], "select");
				foreach($selects as $select) {
					$options = parse_tag($select["innerHTML"], "option");
					foreach($options as $option) if($select["name"] != "" && $option["selected"] == "selected") $queries[$select["name"]] = urlencode($select["name"]) . "=" . urlencode($option["value"]);
				}
				$queries["_TRANID"] = "_TRANID=CP107_001"; // �_�E�����[�h�iCSV�`���j
				$queries["_TARGET"] = "_TARGET=" . $forms[$c]["name"];
				$queries["_FRAMID"] = "_FRAMID=";
				$queries["_TARGETWINID"] = "_TARGETWINID=";
				
				$method = $forms[$c]["method"];
				$uris = parse_uri($forms[$c]["action"], $uris);
				$query = implode("&", $queries);
				$cookie = btmu_update_cookie($head, $cookie);
				list($head, $body) = btmu_http11($method, $uris, $query, $cookie);
			}
			
			// CSV�t�@�C�����擾����
			$forms = parse_tag($body, "form");
			$c = parse_tag_search($forms, "name", "MainForm");
			if($c != -1) {
				$inputs = parse_tag($forms[$c]["innerHTML"], "input");
				$queries = array();
				foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
				$selects = parse_tag($forms[$c]["innerHTML"], "select");
				foreach($selects as $select) {
					$options = parse_tag($select["innerHTML"], "option");
					foreach($options as $option) if($select["name"] != "" && $option["selected"] == "selected") $queries[$select["name"]] = urlencode($select["name"]) . "=" . urlencode($option["value"]);
				}
				$queries["_TRANID"] = "_TRANID=CP107_003"; // �_�E�����[�h�iCSV�`���j
				$queries["_TARGET"] = "_TARGET=" . $forms[$c]["name"];
				$queries["_FRAMID"] = "_FRAMID=";
				$queries["_TARGETWINID"] = "_TARGETWINID=";
				
				$method = $forms[$c]["method"];
				$uris = parse_uri($forms[$c]["action"], $uris);
				$query = implode("&", $queries);
				$cookie = btmu_update_cookie($head, $cookie);
				list($head, $body) = btmu_http11($method, $uris, $query, $cookie);
				if(strpos($head, "Content-Type: text/html") === false) {
					$details = array();
					$j = 0;
					$lines = parse_csv($body);
					foreach($lines as $line) {
						$c = count($line); // 6=Eco���ׂłȂ� 9=Eco����
						if(($c == 6 || $c == 9) && $line[0] != "���t") {
							$details[$j]["date"] = parse_date($line[0]);
							$details[$j]["summary"] = $line[1] . ($line[2] != ""? ENV_CHR_CONCATENATOR . $line[2]: "");
							$details[$j]["withdrawals"] = parse_amount($line[3]);
							$details[$j]["deposit"] = parse_amount($line[4]);
							$details[$j]["memo"] = ($line[6] != ""? $line[6]: ENV_STR_OFX_MEMO);
							// $details[$j]["trntype"] = ($line[8] != ""? $line[8]: "");
							$j++;
						}
					}
					$accounts[$i]["details"] = $details;
				}
			}
			
			$body = $body_old;
		}
	}
	
	// ���s���ԁi�^�C���A�E�g�j���Đݒ肷��
	@set_time_limit(ENV_NUM_TIMEOUT);
	
	// ���O�A�E�g����
	$forms = parse_tag($body, "form");
	$c = parse_tag_search($forms, "name", "MainForm");
	if($c != -1) {
		$inputs = parse_tag($forms[$c]["innerHTML"], "input");
		$queries = array();
		foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
		$queries["_TRANID"] = "_TRANID=AD001_022"; // ���O�A�E�g
		
		$method = $forms[$c]["method"];
		$uris = parse_uri($forms[$c]["action"], $uris);
		$query = implode("&", $queries);
		$cookie = btmu_update_cookie($head, $cookie);
		list($head, $body) = btmu_http11($method, $uris, $query, $cookie);
	}
	
	// ���_�C���N�g����
	$retry = 0;
	while($retry++ < ENV_NUM_HTTP11_RETRY) {
		$locations = parse_header($head, "location");
		if(count($locations) > 0) {
			$method = "GET";
			$uris = parse_uri($locations[0], $uris);
			$query = "";
			$cookie = btmu_update_cookie($head, $cookie);
			list($head, $body) = btmu_http11($method, $uris, $query, $cookie);
		}
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
		$bankmsgsrsv1 .= btmu_parse_details($account);
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
function btmu_http11($method, $uris, $query = "", $cookie = "") {
	$ret = "INVALID HOST";
	if(preg_match("/\.bk\.mufg\.jp$/", $uris["host"]) > 0) $ret = http11(strtoupper($method), $uris["scheme"], $uris["host"], 0, $uris["path"], $query, "", $cookie);
	return explode("\r\n\r\n", $ret, 2);
}

function btmu_update_cookie($head, $cookie) {
	return update_cookie(array("IW_INFO", "JSESSIONID"), parse_header($head, "set-cookie"), $cookie);
}

function mapion_http11($method, $uris, $query = "", $cookie = "") {
	$ret = "INVALID HOST";
	if(preg_match("/\.mapion\.co\.jp$/", $uris["host"]) > 0) $ret = http11(strtoupper($method), $uris["scheme"], $uris["host"], 0, $uris["path"], $query, "", $cookie);
	return explode("\r\n\r\n", $ret, 2);
}

// �x�X�����X�Ԃ���������
function btmu_get_branchid($settings, $str) {
	$ret = mb_convert_string($str);
	
	// ������ʂ��擾����
	$method = "GET";
	$uris = parse_uri($settings["getbranchid"] . urlencode($ret));
	$query = "";
	$cookie = "";
	list($head, $body) = mapion_http11($method, $uris, $query, $cookie);
	
	$ths = parse_tag($body, "th");
	foreach($ths as $th) {
		list($k, $v) = explode(":", trim(strip_tags($th["innerHTML"])));
		$k = trim($k);
		$v = trim($v);
		if($k == "�X��") {
			$ret = $v;
			break;
		}
	}
	
	return $ret;
}

// �a����ނ��ACCTTYPE�����肷��
function btmu_get_accttype($str) {
	$ret = ENV_STR_ACCTTYPE_SAVINGS;
	switch($str) {
	case "����":
	case "���":
		$ret = ENV_STR_ACCTTYPE_SAVINGS;
		break;
	default:
		$ret = ENV_STR_ACCTTYPE_CHECKING;
		break;
	}
	return $ret;
}

function btmu_parse_details($account) {
	$ret = "";
	$cds = array();
	$cd_date = "";
	$cd_num = 0;
	if(is_array($account["details"]) == true) foreach($account["details"] as $line) {
		$cd = array();
		if(mb_strpos($line["summary"], "�U��") === 0) {
			$cd["TRNTYPE"] = ENV_STR_OFX_TRNTYPE_DIRECTDEP;
		} else if(mb_strpos($line["summary"], "�U��") === 0) { // ���m�F
			$cd["TRNTYPE"] = ENV_STR_OFX_TRNTYPE_XFER;
		} else if(mb_strpos($line["summary"], "�ʒ�") === 0) { // ���m�F
			$cd["TRNTYPE"] = ENV_STR_OFX_TRNTYPE_ATM;
		} else if(mb_strpos($line["summary"], "�J�[�h") === 0) {
			$cd["TRNTYPE"] = ENV_STR_OFX_TRNTYPE_ATM;
		} else if(mb_strpos($line["summary"], "����") === 0) {
			$cd["TRNTYPE"] = ENV_STR_OFX_TRNTYPE_INT;
		} else {
			$cd["TRNTYPE"] = ENV_STR_OFX_TRNTYPE_OTHER;
		}
		
		// ���t���擾����
		$cd["DTPOSTED"] = $line["date"];
		
		// �ʔԂ𐶐�����
		if($cd["DTPOSTED"] == $cd_date) $cd_num++; else $cd_num = 0;
		
		// �g�����U�N�V�����ԍ��𐶐�����
		$cd["FITID"] = $cd["DTPOSTED"] . sprintf("%04d", (integer)$account["id"]) . "000" . sprintf("%05d", $cd_num);
		
		// �E�v���擾����
		$cd["NAME"] = mb_convert_string($line["summary"]);
		
		// ���z���擾����
		$cd["TRNAMT"] = (string)((double)parse_amount($line["deposit"]) - (double)parse_amount($line["withdrawals"]));
		$cd["MEMO"] = ENV_STR_OFX_MEMO;
		
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