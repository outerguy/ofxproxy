<?php
/*
jpbank.inc: �䂤�����s��HTML���OFX�t�@�C���𐶐�����
Copyright (C) 2012-2015 OFFICE OUTERGUY. All rights reserved.
mailto:contact@beatrek.com
Licensed under the GNU AGPLv3.
*/

// ���O�C�������擾����
$user = str_replace("-", "", $ofxforms["okyakusamaBangou"]);
$pass = $ofxforms["loginPassword"];
$auth = urlencode(mb_convert_encoding(urldecode($ofxforms["aikotoba"]), "Shift_JIS", "UTF-8"));
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
	list($head, $body) = jpbank_http11($method, $uris, $query, $cookie);
	
	// ���O�C����ʂ��擾����
	$as = parse_tag($body, "a");
	foreach($as as $a) if(parse_tag_search(parse_tag($a["innerHTML"], "img"), "alt", "�䂤����_�C���N�g�@���O�C��") != -1) {
		$method = "GET";
		$uris = parse_uri($a["href"], $uris);
		$query = "";
		$cookie = jpbank_update_cookie($head, $cookie);
		list($head, $body) = jpbank_http11($method, $uris, $query, $cookie);
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
			$cookie = jpbank_update_cookie($head, $cookie);
			list($head, $body) = jpbank_http11($method, $uris, $query, $cookie);
		}
	}
	
	// �����S���Ă����p���������ɂ������Ă̂����Ӊ�ʂ��\�������ꍇ�A���߂ă��O�C����ʂ��擾����
	$as = parse_tag($body, "a");
	foreach($as as $a) if(parse_tag_search(parse_tag($a["innerHTML"], "img"), "alt", "��L���m�F�̂����A���O�C����ʂɐi��") != -1 || parse_tag_search(parse_tag($a["innerHTML"], "img"), "alt", "���O�C����ʂɐi��") != -1) {
		$method = "GET";
		$uris = parse_uri($a["href"], $uris);
		$query = "";
		$cookie = jpbank_update_cookie($head, $cookie);
		list($head, $body) = jpbank_http11($method, $uris, $query, $cookie);
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
			$cookie = jpbank_update_cookie($head, $cookie);
			list($head, $body) = jpbank_http11($method, $uris, $query, $cookie);
		}
	}
	
	// ���q���ܔԍ�����͂���
	$forms = parse_tag($body, "form");
	$c = parse_tag_search($forms, "name", "submitData");
	if($c != -1) {
		$inputs = parse_tag($forms[$c]["innerHTML"], "input");
		$queries = array();
		foreach($inputs as $input) switch($input["value"]) {
		case "����":
			$dcs = jpbank_parse_dcrequest($input["onclick"]);
			// break���Ȃ�
		default:
			if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
			break;
		}
		// $queries["pm_fp"] = $pmfp;
		$forms[$c]["action"] = $dcs["path"];
		if(count($dcs["kvs"]) > 0) foreach($dcs["kvs"] as $k => $v) $queries[$k] = $v;
		$queries["okyakusamaBangou1"] = "okyakusamaBangou1=" . substr($user, 0, 4);
		$queries["okyakusamaBangou2"] = "okyakusamaBangou2=" . substr($user, 4, 4);
		$queries["okyakusamaBangou3"] = "okyakusamaBangou3=" . substr($user, 8);
		
		$method = $forms[$c]["method"];
		$uris = parse_uri($forms[$c]["action"], $uris);
		$query = implode("&", $queries);
		$cookie = jpbank_update_cookie($head, $cookie);
		list($head, $body) = jpbank_http11($method, $uris, $query, $cookie);
	}
	
	// ���_�C���N�g����
	$retry = 0;
	while($retry++ < ENV_NUM_HTTP11_RETRY) {
		$locations = parse_header($head, "location");
		if(count($locations) > 0) {
			$method = "GET";
			$uris = parse_uri($locations[0], $uris);
			$query = "";
			$cookie = jpbank_update_cookie($head, $cookie);
			list($head, $body) = jpbank_http11($method, $uris, $query, $cookie);
		}
	}
}

if(strpos($body, "�����t���͂P") !== false) {
	// �����t����͂���i�P�j
	$trs = parse_tag($body, "tr");
	$bufs = array();
	foreach($trs as $tr) {
		$ths = parse_tag($tr["innerHTML"], "th");
		$tds = parse_tag($tr["innerHTML"], "td");
		if(count($ths) == 1 && count($tds) == 1 && trim(strip_tags($ths[0]["innerHTML"])) == "����") {
			$shitsumon = trim(strip_tags($tds[0]["innerHTML"]));
			break;
		}
	}
	
	$forms = parse_tag($body, "form");
	$c = parse_tag_search($forms, "name", "submitData");
	if($c != -1) {
		$inputs = parse_tag($forms[$c]["innerHTML"], "input");
		$queries = array();
		foreach($inputs as $input) switch($input["value"]) {
		case "����":
			$dcs = jpbank_parse_dcrequest($input["onclick"]);
			// break���Ȃ�
		default:
			if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
			break;
		}
		// $queries["pm_fp"] = $pmfp;
		$forms[$c]["action"] = $dcs["path"];
		if(count($dcs["kvs"]) > 0) foreach($dcs["kvs"] as $k => $v) $queries[$k] = $v;
		$queries["aikotoba"] = "aikotoba=";
		
		$method = $forms[$c]["method"];
		$uris = parse_uri($forms[$c]["action"], $uris);
		$query = implode("&", $queries);
		$cookie = jpbank_update_cookie($head, $cookie);
	}
	
	$sid = 1;
	$sess = sess_encode(implode("\t", array((string)$sid, $method, $uris["scheme"] . "://" . $uris["host"] . $uris["path"], $query, $cookie, $pass, ENV_STR_SESSION_PADDING)));
	$head = "";
	$body = "";
} else if($sid == 1) {
	$sid = 0;
	$query = str_replace("aikotoba=", "aikotoba=" . $auth, $query);
	list($head, $body) = jpbank_http11($method, $uris, $query, $cookie);
	
	// ���_�C���N�g����
	$retry = 0;
	while($retry++ < ENV_NUM_HTTP11_RETRY) {
		$locations = parse_header($head, "location");
		if(count($locations) > 0) {
			$method = "GET";
			$uris = parse_uri($locations[0], $uris);
			$query = "";
			$cookie = jpbank_update_cookie($head, $cookie);
			list($head, $body) = jpbank_http11($method, $uris, $query, $cookie);
		}
	}
}

if(strpos($body, "�����t���͂Q") !== false) {
	// �����t����͂���i�Q�j
	$trs = parse_tag($body, "tr");
	$bufs = array();
	foreach($trs as $tr) {
		$ths = parse_tag($tr["innerHTML"], "th");
		$tds = parse_tag($tr["innerHTML"], "td");
		if(count($ths) == 1 && count($tds) == 1 && trim(strip_tags($ths[0]["innerHTML"])) == "����") {
			$shitsumon = trim(strip_tags($tds[0]["innerHTML"]));
			break;
		}
	}
	
	$forms = parse_tag($body, "form");
	$c = parse_tag_search($forms, "name", "submitData");
	if($c != -1) {
		$inputs = parse_tag($forms[$c]["innerHTML"], "input");
		$queries = array();
		foreach($inputs as $input) switch($input["value"]) {
		case "����":
			$dcs = jpbank_parse_dcrequest($input["onclick"]);
			// break���Ȃ�
		default:
			if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
			break;
		}
		// $queries["pm_fp"] = $pmfp;
		$forms[$c]["action"] = $dcs["path"];
		if(count($dcs["kvs"]) > 0) foreach($dcs["kvs"] as $k => $v) $queries[$k] = $v;
		$queries["aikotoba"] = "aikotoba=";
		if(isset($queries["riyouKankyouTourokuFuyouUmu"]) == true) {
			if(ENV_BOOL_ADD_RISKBASE == true) {
				// ���񃍃O�C�����ɍ����t����͂��Ȃ�
				unset($queries["riyouKankyouTourokuFuyouUmu"]);
			} else {
				// ���񃍃O�C�����ɍ����t����͂���
				$queries["riyouKankyouTourokuFuyouUmu"] = "riyouKankyouTourokuFuyouUmu=1";
			}
		}
		
		$method = $forms[$c]["method"];
		$uris = parse_uri($forms[$c]["action"], $uris);
		$query = implode("&", $queries);
		$cookie = jpbank_update_cookie($head, $cookie);
	}
	
	$sid = 2;
	$sess = sess_encode(implode("\t", array((string)$sid, $method, $uris["scheme"] . "://" . $uris["host"] . $uris["path"], $query, $cookie, $pass, ENV_STR_SESSION_PADDING)));
	$head = "";
	$body = "";
} else if($sid == 2) {
	$sid = 0;
	$query = str_replace("aikotoba=", "aikotoba=" . $auth, $query);
	list($head, $body) = jpbank_http11($method, $uris, $query, $cookie);
	
	// ���_�C���N�g����
	$retry = 0;
	while($retry++ < ENV_NUM_HTTP11_RETRY) {
		$locations = parse_header($head, "location");
		if(count($locations) > 0) {
			$method = "GET";
			$uris = parse_uri($locations[0], $uris);
			$query = "";
			$cookie = jpbank_update_cookie($head, $cookie);
			list($head, $body) = jpbank_http11($method, $uris, $query, $cookie);
		}
	}
}

if(strpos($body, "���O�C���p�X���[�h����") !== false) {
	// ���O�C���p�X���[�h����͂���
	$forms = parse_tag($body, "form");
	$c = parse_tag_search($forms, "name", "submitData");
	if($c != -1) {
		$inputs = parse_tag($forms[$c]["innerHTML"], "input");
		$queries = array();
		foreach($inputs as $input) switch($input["value"]) {
		case "���O�C��":
			$dcs = jpbank_parse_dcrequest($input["onclick"]);
			// break���Ȃ�
		default:
			if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
			break;
		}
		// $queries["pm_fp"] = $pmfp;
		$forms[$c]["action"] = $dcs["path"];
		if(count($dcs["kvs"]) > 0) foreach($dcs["kvs"] as $k => $v) $queries[$k] = $v;
		$queries["loginPassword"] = "loginPassword=" . $pass;
		
		$method = $forms[$c]["method"];
		$uris = parse_uri($forms[$c]["action"], $uris);
		$query = implode("&", $queries);
		$cookie = jpbank_update_cookie($head, $cookie);
		list($head, $body) = jpbank_http11($method, $uris, $query, $cookie);
	}
	
	// ���_�C���N�g����
	$retry = 0;
	while($retry++ < ENV_NUM_HTTP11_RETRY) {
		$locations = parse_header($head, "location");
		if(count($locations) > 0) {
			$method = "GET";
			$uris = parse_uri($locations[0], $uris);
			$query = "";
			$cookie = jpbank_update_cookie($head, $cookie);
			list($head, $body) = jpbank_http11($method, $uris, $query, $cookie);
		}
	}
}

if($sid > 0) {
	// �Z�b�V�����������p��
	$resp["status"] = ENV_NUM_STATUS_ADDITION;
	$resp["aid"] = "aikotoba";
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
} else if(strpos($body, "�����p���ԊO�̂��߂��戵���������܂���") !== false || strpos($body, "�������܃T�[�r�X���s���Ă���܂���") !== false) {
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
	$account = array();
	
	// ���s���ԁi�^�C���A�E�g�j���Đݒ肷��
	@set_time_limit(ENV_NUM_TIMEOUT);
	
	// ���������擾����
	if(preg_match("/ComInfoOutput\((.+?)\);/", $body, $matches) > 0) {
		$accs = parse_csv($matches[1]);
		$accttype = jpbank_getaccttype($accs[0][7]);
		$branchid = str_replace(array("�Z", "��", "��", "�O", "�l", "��" ,"�Z" ,"��", "��", "��", "�X"), array("0", "1", "2", "3", "4", "5", "6", "7", "8", "9", ""), $accs[0][8]);
		$acctid = $accs[0][9];
		$account["balance"] = parse_amount($accs[0][12]);
		$account["acctname"] = implode(ENV_CHR_CONCATENATOR, array($settings["name"], $accs[0][8], $accs[0][7]));
		
		// ���ݎ�����萼��Ƙa��̍����擾����
		$delta = (preg_match("/[0-9]+/", $accs[0][17], $years) > 0? (integer)substr(ENV_STR_DATE_TODAY, 0, 4) - (integer)$years[0]: 0);
	}
	
	// ���o�����׏Ɖ��ʂ��擾����
	$forms = parse_tag($body, "form");
	$c = parse_tag_search($forms, "name", "submitData");
	if($c != -1) {
		$inputs = parse_tag($forms[$c]["innerHTML"], "input");
		$c = parse_tag_search($inputs, "name", "U030110"); // ���o�����׏Ɖ�
		if($c != -1) $dcs = jpbank_parse_dcrequest($inputs[$c]["onclick"]);
	}
	$c = parse_tag_search($forms, "name", "simpleTransitionForm");
	if($c != -1) {
		$inputs = parse_tag($forms[$c]["innerHTML"], "input");
		$queries = array();
		foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
		$forms[$c]["action"] = $dcs["path"];
		if(count($dcs["kvs"]) > 0) foreach($dcs["kvs"] as $k => $v) $queries[$k] = $v;
		
		$method = $forms[$c]["method"];
		$uris = parse_uri($forms[$c]["action"], $uris);
		$query = implode("&", $queries);
		$cookie = jpbank_update_cookie($head, $cookie);
		list($head, $body) = jpbank_http11($method, $uris, $query, $cookie);
	}
	
	// ���_�C���N�g����
	$retry = 0;
	while($retry++ < ENV_NUM_HTTP11_RETRY) {
		$locations = parse_header($head, "location");
		if(count($locations) > 0) {
			$method = "GET";
			$uris = parse_uri($locations[0], $uris);
			$query = "";
			$cookie = jpbank_update_cookie($head, $cookie);
			list($head, $body) = jpbank_http11($method, $uris, $query, $cookie);
		}
	}
	
	// ���o�����׉�ʂ��擾����
	$forms = parse_tag($body, "form");
	$c = parse_tag_search($forms, "name", "submitData");
	if($c != -1) {
		$inputs = parse_tag($forms[$c]["innerHTML"], "input");
		$queries = array();
		foreach($inputs as $input) switch($input["value"]) {
		case "����":
			$dcs = jpbank_parse_dcrequest($input["onclick"]);
			// break���Ȃ�
		default:
			if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
			break;
		}
		$selects = parse_tag($forms[$c]["innerHTML"], "select");
		foreach($selects as $select) {
			$options = parse_tag($select["innerHTML"], "option");
			foreach($options as $option) if($select["name"] != "" && $option["selected"] == "selected") $queries[$select["name"]] = urlencode($select["name"]) . "=" . urlencode($option["value"]);
		}
		$forms[$c]["action"] = $dcs["path"];
		if(count($dcs["kvs"]) > 0) foreach($dcs["kvs"] as $k => $v) $queries[$k] = $v;
		$queries["shoukaiTaishouSentaku"] = "shoukaiTaishouSentaku=01"; // 01=���o������
		$queries["shoukaiHaniSentaku"] = "shoukaiHaniSentaku=00"; // 00=���t�w��
		$queries["nyuushukkinShoukaiHaniWarekiKaishiNen"] = "nyuushukkinShoukaiHaniWarekiKaishiNen=" . (string)((integer)substr(ENV_STR_DATE_PASTDAY, 0, 4) - $delta);
		$queries["nyuushukkinShoukaiHaniWarekiKaishiTsuki"] = "nyuushukkinShoukaiHaniWarekiKaishiTsuki=" . (string)substr(ENV_STR_DATE_PASTDAY, 4, 2);
		$queries["nyuushukkinShoukaiHaniWarekiKaishiHi"] = "nyuushukkinShoukaiHaniWarekiKaishiHi=" . (string)substr(ENV_STR_DATE_PASTDAY, 6, 2);
		$queries["nyuushukkinShoukaiHaniWarekiShuuryouNen"] = "nyuushukkinShoukaiHaniWarekiShuuryouNen=" . (string)((integer)substr(ENV_STR_DATE_TODAY, 0, 4) - $delta);
		$queries["nyuushukkinShoukaiHaniWarekiShuuryouTsuki"] = "nyuushukkinShoukaiHaniWarekiShuuryouTsuki=" . (string)substr(ENV_STR_DATE_TODAY, 4, 2);
		$queries["nyuushukkinShoukaiHaniWarekiShuuryouHi"] = "nyuushukkinShoukaiHaniWarekiShuuryouHi=" . (string)substr(ENV_STR_DATE_TODAY, 6, 2);
		
		$method = $forms[$c]["method"];
		$uris = parse_uri($forms[$c]["action"], $uris);
		$query = implode("&", $queries);
		$cookie = jpbank_update_cookie($head, $cookie);
		list($head, $body) = jpbank_http11($method, $uris, $query, $cookie);
	}
	
	// ���_�C���N�g����
	$retry = 0;
	while($retry++ < ENV_NUM_HTTP11_RETRY) {
		$locations = parse_header($head, "location");
		if(count($locations) > 0) {
			$method = "GET";
			$uris = parse_uri($locations[0], $uris);
			$query = "";
			$cookie = jpbank_update_cookie($head, $cookie);
			list($head, $body) = jpbank_http11($method, $uris, $query, $cookie);
		}
	}
	
	$details = array();
	$n = 0;
	
	do {
		// ���ׂ��擾����
		$trs = parse_tag($body, "tr");
		foreach($trs as $tr) {
			$tds = parse_tag($tr["innerHTML"], "td");
			if(count($tds) == 5) {
				for($i = 0; $i < count($tds); $i++) $tds[$i]["innerHTML"] = str_replace("&nbsp;", "", trim(strip_tags($tds[$i]["innerHTML"])));
				// ��旘�q�̖��ׂ𖳎�����
				if($tds[3]["innerHTML"] == "��旘�q") continue;
				$details[$n]["date"] = parse_date($tds[0]["innerHTML"]);
				$details[$n]["deposit"] = parse_amount($tds[1]["innerHTML"]);
				$details[$n]["withdrawals"] = parse_amount($tds[2]["innerHTML"]);
				$details[$n]["summary"] = ($tds[3]["innerHTML"] != ""? $tds[3]["innerHTML"]: ENV_STR_OFX_MEMO);
				$details[$n]["memo"] = ENV_STR_OFX_MEMO;
				$n++;
			}
		}
		
		$f = false;
		
		// 2�y�[�W�ڈȍ~���擾����
		$inputs = parse_tag($body, "input");
		$c = parse_tag_search($inputs, "value", "����\��");
		if($c != -1) {
			$dcs = jpbank_parse_dcrequest($inputs[$c]["onclick"]);
			$f = true;
			
			$forms = parse_tag($body, "form");
			$c = parse_tag_search($forms, "name", "pageLinksBeanPageLinksForm");
			if($c != -1) {
				$inputs = parse_tag($forms[$c]["innerHTML"], "input");
				$queries = array();
				foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
				$forms[$c]["action"] = $dcs["path"];
				if(count($dcs["kvs"]) > 0) foreach($dcs["kvs"] as $k => $v) $queries[$k] = $v;
				
				$method = $forms[$c]["method"];
				$uris = parse_uri($forms[$c]["action"], $uris);
				$query = implode("&", $queries);
				$cookie = jpbank_update_cookie($head, $cookie);
				list($head, $body) = jpbank_http11($method, $uris, $query, $cookie);
			}
			
			// ���_�C���N�g����
			$retry = 0;
			while($retry++ < ENV_NUM_HTTP11_RETRY) {
				$locations = parse_header($head, "location");
				if(count($locations) > 0) {
					$method = "GET";
					$uris = parse_uri($locations[0], $uris);
					$query = "";
					$cookie = jpbank_update_cookie($head, $cookie);
					list($head, $body) = jpbank_http11($method, $uris, $query, $cookie);
				}
			}
		}
	} while($f == true);
	
	$account["details"] = $details;
	
	// ���s���ԁi�^�C���A�E�g�j���Đݒ肷��
	@set_time_limit(ENV_NUM_TIMEOUT);
	
	// ���O�A�E�g����
	$as = parse_tag($body, "a");
	$c = parse_tag_search($as, "id", "LOGOUTBT"); // ���O�A�E�g
	if($c != -1) $dcs = jpbank_parse_dcrequest($as[$c]["onclick"]);
	$forms = parse_tag($body, "form");
	$c = parse_tag_search($forms, "name", "simpleTransitionForm");
	if($c != -1) {
		$inputs = parse_tag($forms[$c]["innerHTML"], "input");
		$queries = array();
		foreach($inputs as $input) if($input["name"] != "") $queries[$input["name"]] = urlencode($input["name"]) . "=" . urlencode($input["value"]);
		$forms[$c]["action"] = $dcs["path"];
		if(count($dcs["kvs"]) > 0) foreach($dcs["kvs"] as $k => $v) $queries[$k] = $v;
		
		$method = $forms[$c]["method"];
		$uris = parse_uri($forms[$c]["action"], $uris);
		$query = implode("&", $queries);
		$cookie = jpbank_update_cookie($head, $cookie);
		list($head, $body) = jpbank_http11($method, $uris, $query, $cookie);
	}
	
	// ���_�C���N�g����
	$retry = 0;
	while($retry++ < ENV_NUM_HTTP11_RETRY) {
		$locations = parse_header($head, "location");
		if(count($locations) > 0) {
			$method = "GET";
			$uris = parse_uri($locations[0], $uris);
			$query = "";
			$cookie = jpbank_update_cookie($head, $cookie);
			list($head, $body) = jpbank_http11($method, $uris, $query, $cookie);
		}
	}
	
	$bankmsgsrsv1 = "";
	$bankmsgsrsv1 .= "<BANKMSGSRSV1>";
	$bankmsgsrsv1 .= "\r\n";
	
	// �������𐶐�����
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
	$bankmsgsrsv1 .= "<ACCTID>" . $acctid . "</ACCTID>";
	$bankmsgsrsv1 .= "<ACCTTYPE>" . $accttype . "</ACCTTYPE>";
	$bankmsgsrsv1 .= "</BANKACCTFROM>";
	$bankmsgsrsv1 .= "\r\n";
	$bankmsgsrsv1 .= jpbank_parse_details($account);
	$bankmsgsrsv1 .= "</STMTRS>";
	$bankmsgsrsv1 .= "\r\n";
	$bankmsgsrsv1 .= "</STMTTRNRS>";
	$bankmsgsrsv1 .= "\r\n";
	
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
function jpbank_http11($method, $uris, $query = "", $cookie = "") {
	$ret = "INVALID HOST";
	if(preg_match("/\.jp\-bank\.japanpost\.jp$/", $uris["host"]) > 0) $ret = http11(strtoupper($method), $uris["scheme"], $uris["host"], 0, $uris["path"], $query, "", $cookie);
	return explode("\r\n\r\n", $ret, 2);
}

function jpbank_update_cookie($head, $cookie) {
	return update_cookie(array("BROWSER_TOKEN", "JSESSIONID", "PMData"), parse_header($head, "set-cookie"), $cookie);
}

// �a����ނ��ACCTTYPE�����肷��
function jpbank_getaccttype($str) {
	$ret = ENV_STR_ACCTTYPE_SAVINGS;
	switch($str) {
	case "����":
		$ret = ENV_STR_ACCTTYPE_CHECKING;
		break;
	case "���~":
	case "����":
	default:
		break;
	}
	return $ret;
}

function jpbank_parse_dcrequest($str) {
	$ret = array();
	if(preg_match("/dcRequest\(([^,]+),([^,]+),[^\(]+\(([^\)]+)\),[^\(]+\(([^\)]+)\),([^,]+)\);/i", $str, $matches) > 0) {
		$ret["path"] = jpbank_delete_quote($matches[2]);
		$ret["kvs"] = array();
		$ks = explode(",", $matches[3]);
		$vs = explode(",", $matches[4]);
		for($i = 0; $i < count($ks); $i++) {
			$k = jpbank_delete_quote($ks[$i]);
			$v = jpbank_delete_quote($vs[$i]);
			$ret["kvs"][$k] = $k . "=" . $v;
		}
	} else if(preg_match("/dcRequest\(([^,]+),([^,]+),([^,]+),([^,]+),([^,]+)\);/i", $str, $matches) > 0) {
		$ret["path"] = jpbank_delete_quote($matches[2]);
	}
	return $ret;
}

function jpbank_delete_quote($str) {
	return preg_replace("/^.*?([\"\'])(.*?)\\1.*?$/", "\\2", $str);
}

function jpbank_parse_details($account) {
	$ret = "";
	$cds = array();
	$cd_date = "";
	$cd_num = 0;
	foreach($account["details"] as $line) {
		$cd = array();
		if(mb_strpos($line["summary"], "�U��") === 0) {
			$cd["TRNTYPE"] = ENV_STR_OFX_TRNTYPE_DIRECTDEP;
		} else if(mb_strpos($line["summary"], "�U��") === 0 || mb_strpos($line["summary"], "����") === 0 || mb_strpos($line["summary"], "����") === 0) {
			$cd["TRNTYPE"] = ENV_STR_OFX_TRNTYPE_DIRECTDEBIT;
		} else if(mb_strpos($line["summary"], "����") === 0) {
			$cd["TRNTYPE"] = ENV_STR_OFX_TRNTYPE_XFER;
		} else if(mb_strpos($line["summary"], "����") === 0 || mb_strpos($line["summary"], "���o") === 0) {
			$cd["TRNTYPE"] = ENV_STR_OFX_TRNTYPE_CASH;
		} else if(mb_strpos($line["summary"], "���") === 0) {
			$cd["TRNTYPE"] = ENV_STR_OFX_TRNTYPE_REPEATPMT;
		} else if(mb_strpos($line["summary"], "�ʁ@��") === 0) {
			$cd["TRNTYPE"] = ENV_STR_OFX_TRNTYPE_ATM;
		} else if(mb_strpos($line["summary"], "�J�[�h") === 0) {
			$cd["TRNTYPE"] = ENV_STR_OFX_TRNTYPE_ATM;
		} else if(mb_strpos($line["summary"], "���@��") === 0) {
			$cd["TRNTYPE"] = ENV_STR_OFX_TRNTYPE_FEE;
		} else if(mb_strpos($line["summary"], "���q") === 0) {
			$cd["TRNTYPE"] = ENV_STR_OFX_TRNTYPE_INT;
		} else if(mb_strpos($line["summary"], "�ŋ�") === 0) {
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