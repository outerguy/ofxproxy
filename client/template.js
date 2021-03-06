/*
template.js: 画面・機能を制御する
Copyright (C) 2012-2015 OFFICE OUTERGUY. All rights reserved.
mailto:contact@beatrek.com
Dual-licensed under the GNU AGPLv3 and Beatrek Origin License.
*/

// グローバル変数・定数を定義する
var debug = "<!--[debug]-->";
var ver = "<!--[client]-->.<!--[server]-->";
var family = "<!--[family]-->";
var flag_ofxform = false;

(function() {
	// 起動時にロード機能を呼び出す
	with(self.window) if(addEventListener) {
		addEventListener("load", fnc_load, true);
	} else if(attachEvent) {
		attachEvent("onload", fnc_load);
	} else {
		onload = fnc_load;
	}
	
	// Cookieを読み込む
	load_cookie_ofxform();
	
	// Cookieを書き込む
	save_cookie_ofxform();
	
})();


// =========================================================================
// 機能
// =========================================================================

// ロード機能
function fnc_load() {
	// バージョン情報を表示する
	var cdf = document.createDocumentFragment();
	var tag_body = dom_get_tag("body")[0];
	var title = dom_get_tag("title")[0].firstChild.nodeValue;
	var tag_div = dom_get_id("ver");
	var tag_p, tag_a;
	
	tag_p = dom_create_tag("p", { "class": "product" });
	tag_a = dom_create_tag("a", { "href": "https://github.com/outerguy/ofxproxy", "target": "_blank" });
	tag_a.appendChild(dom_create_text(title));
	tag_p.appendChild(tag_a);
	tag_p.appendChild(dom_create_text("Version " + ver));
	cdf.appendChild(tag_p);
	
	tag_p = dom_create_tag("p");
	tag_p.appendChild(dom_create_text("Copyright (C) 2008-2015 OFFICE OUTERGUY. All rights reserved."));
	cdf.appendChild(tag_p);
	
	tag_p = dom_create_tag("p");
	tag_p.appendChild(dom_create_text("Portions Copyright (C) 2012-2015 Hiromu2000. All rights reserved."));
	cdf.appendChild(tag_p);
	
	if(tag_div != null) with(tag_div) {
		while(hasChildNodes() == true) removeChild(lastChild);
		appendChild(cdf);
	}
	
	return;
}

function exec_ofxform(obj) {
	var ret = false;
	var fnc = function() {
		flag_ofxform = false;
		
		return;
	};
	with(obj) {
		if(flag_ofxform == true) {
			alert("明細のダウンロードを開始するまで、しばらくお待ちください。");
		} else {
			self.window.setTimeout(fnc, 2000);
			flag_ofxform = true;
			ret = true;
		}
	}
	
	return ret;
}

function link_ofxform(obj) {
	with(obj) target = parentNode.id;
	
	return true;
}

function help_ofxform(obj) {
	obj.target = "help_ofxform";
	
	return true;
}

function load_cookie_ofxform() {
	var cookies = self.document.cookie.split("; ");
	var i;
	for(i = 0; i < cookies.length; i++) {
		kvs = cookies[i].split("=", 2);
		if(dom_get_id(kvs[0])) {
			dom_get_id(kvs[0]).style.display = kvs[1];
			dom_get_id(kvs[0] + "_hide").checked = (kvs[1] == "none"? true: false);
		}
	}
	
	return;
}

function save_cookie_ofxform() {
	var tag_inputs = dom_get_tag("input");
	var expire = new Date();
	var i;
	
	expire.setTime(expire.getTime() + 86400000 * 60);
	
	for(i in tag_inputs) with(tag_inputs[i]) if(typeof type != "undefined" && type == "checkbox" && id != "all_hide") {
		self.document.cookie = id.replace("_hide", "") + "=" + (checked == true? "none": "block") + "; expires=" + expire.toUTCString();
	}
	
	return;
}


// =========================================================================
// 関数
// =========================================================================

// DOMよりタグに合致するエレメントを取得する
function dom_get_tag(name) {
	var obj = null;
	
	with(self.document) if(typeof getElementsByTagName != "undefined") try {
		if(typeof name == "string") obj = getElementsByTagName(name);
	} catch(e) {
		void(e);
	}
	
	return obj;
}

// DOMよりIDに合致するエレメントを取得する
function dom_get_id(name) {
	var obj = null;
	
	with(self.document) if(typeof getElementById != "undefined") try {
		if(typeof name == "string") obj = getElementById(name);
	} catch(e) {
		void(e);
	}
	
	return obj;
}

// DOMのテキストを生成する
function dom_create_text(text) {
	var obj = null;
	
	with(self.document) if(typeof createTextNode != "undefined") try {
		if(typeof text == "string") obj = createTextNode(dom_convert_escape(text));
	} catch(e) {
		void(e);
	}
	
	return obj;
}

// DOMのタグを生成する
function dom_create_tag(name, attrs) {
	var obj = null;
	var i;
	
	with(self.document) if(typeof createElement != "undefined") try {
		if(typeof name == "string") obj = createElement(name);
	} catch(e) {
		void(e);
	}
	
	if(typeof attrs != "array") for(i in attrs) try {
		obj.setAttribute(i, dom_convert_escape(attrs[i]));
	} catch(e) {
		void(e);
	}
	
	return obj;
}

// DOMの特定文字をエスケープする
function dom_convert_escape(str) {
	var ret = "";
	var buf = "";
	var hcs = { "amp": "&", "quot": "\"", "lt": "<", "gt": ">" };
	var fnc;
	var i;
	
	if(str.indexOf(hcs["amp"]) == -1) {
		ret = str;
	} else {
		fnc = function() {
			return hcs[arguments[1]];
		};
		
		for(i in hcs) buf += "|" + i;
		ret = str.replace(new RegExp(hcs["amp"] + "(" + buf.substring(1) + ");", "g"), fnc);
	}
	
	return ret;
}
