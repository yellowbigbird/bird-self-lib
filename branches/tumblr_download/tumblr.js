// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// A generic onclick callback function.
function onClickHandler(info, tab) {
	
	var newurl = "http://www.baidu.com";
  console.log("item " + info.menuItemId + " was clicked");
  console.log("info: " + JSON.stringify(info));
  console.log("tab: " + JSON.stringify(tab));
  //chrome.tabs.query({'active': true}, openNewTab(chrome.tabs) );
  chrome.tabs.create(newurl, openNewTab)
};

function openNewTab(tab) {
	console.log("tab: " + JSON.stringify(tab) );
	//chrome.tabs.update(tabs[0].id, newurl);
	
};

chrome.contextMenus.onClicked.addListener(onClickHandler);

// Set up context menu tree at install time.
chrome.runtime.onInstalled.addListener(function() {
	// Create one test item for each context type.
	var contexts = ["page","selection","link","editable","image","video",
					"audio"];
	for (var i = 0; i < contexts.length; i++) {
	  var context = contexts[i];
	  var title = "download 1280 img";
	  var id = chrome.contextMenus.create({"title": title, "contexts":[context],
											 "id": "context" + context});
	  console.log("'" + context + "' item:" + id);
	}


	// Create a parent item and two children.
	var parent = chrome.contextMenus.create({"title": "Test parent item"});
	console.log("parent:" + parent );

});
