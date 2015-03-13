// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// A generic onclick callback function.
		
function onClickHandler(info, tab) {
	console.log("info: " + JSON.stringify(info));
	console.log("tab: " + JSON.stringify(tab));
	
	var str540 = "_540.";
	var str400 = "_400.";
	var str500 = "_500.";
	var str1280 = "_1280.";
	var oldurl = info.srcUrl;
	var newurl = oldurl;
	if(newurl.indexOf(str540) > -1){	
		newurl = oldurl.replace(str540, str1280);
	}
	else if(newurl.indexOf(str400) > -1){	
		newurl = oldurl.replace(str400, str1280);
	}
	else if(newurl.indexOf(str500) > -1){	
		newurl = oldurl.replace(str500, str1280);
	}
	else{
		//return;
	}
	openNewTab(tab, newurl);
};

function openNewTab(tab, url0) {
	console.log("opennewtab");
	chrome.tabs.create({url: url0 , active: false } );
	//chrome.tabs.create({url:chrome.extension.getURL("tabs_api.html")});
	//chrome.tabs.update(tabs[0].id, newurl);	
};

function cloneTab(tab) {
	chrome.tabs.update({}, function(tab) {
      chrome.tabs.duplicate(tab.id);
  });
};

chrome.contextMenus.onClicked.addListener(onClickHandler);

// Set up context menu tree at install time.
chrome.runtime.onInstalled.addListener(function() {
	// Create one test item for each context type.
	var contexts = ["image"];
	//["page","selection","link","editable","image","video", "audio"];
		
	for (var i = 0; i < contexts.length; i++) {
	  var context = contexts[i];
	  var title = "get 1280 img";
	  var id = chrome.contextMenus.create({"title": title, "contexts":[context],
											 "id": "context" + context});
	  console.log("'" + context + "' item:" + id);
	}

	// Create a parent item and two children.
	var parent = chrome.contextMenus.create({"title": "Test parent item"});
	console.log("parent:" + parent );
});
