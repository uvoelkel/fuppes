/***************************************************************************
 *            fuppes.js
 *
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2010 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 */

window.addEvent('domready', function() {
	$('js-check').setStyle('display', 'none');
});


function loadXmlResult(result)
{
  var doc = undefined;

  // code for IE
  if (window.ActiveXObject) {
    doc = new ActiveXObject("Microsoft.XMLDOM");
    doc.async = "false";
    doc.loadXML(result);
  }
  // code for Mozilla, Firefox, Opera, etc.
  else {
    var parser = new DOMParser();
    doc = parser.parseFromString(result,"text/xml");
  }

  return doc;  
}


function deviceDetails(idx)
{
  div = $('remote-device-details-' + idx);
  if(div == null) {
    alert("no div");
    return;
  }

  visible = div.getStyle('display') == "block"; 
  if(!visible) {
    div.setStyle('display', 'block');
  }
  else {
    div.setStyle('display', 'none');
  }
  
}


function logLevel(level) {
	body = "<log-level>" + level + "</log-level>";

	var request = new FuppesControl({
		action: 'SetLogLevel',
		body: body,
		onComplete: function(result) {
		},
		onError: function(error) {
		}
	});
	request.send();
}

function logSender(sender) {
	checkbox = $('log-sender-' + sender);
	action = (checkbox.checked ? "AddLogSender" : "DelLogSender");
	body = "<log-sender>" + sender + "</log-sender>";

	var request = new FuppesControl({
		action: action,
		body: body,
		onComplete: function(result) {
		},
		onError: function(error) {
		}
	});
	request.send();
}






var UPnPRequest = new Class({

	Implements: [Options, Events],
	
	options: {
		target: undefined,
		action: undefined,

		vfolder: undefined,

		onSuccess: undefined,
  },

	initialize: function(options) {
		this.setOptions(options);
	},

	exec: function(content) {
		
		var request = new Request({
    	url: '/UPnPServices/' + this.options.target + '/control/', 
    	onSuccess: this.options.onSuccess.bind(this)
		});

		request.setHeader('SOAPAction', 
			'"urn:schemas-upnp-org:service:' + this.options.target + ':1#' + this.options.action + '"');
		request.setHeader('Content-Type', 'text/xml; charset=utf-8');
		if(this.options.vfolder)
			request.setHeader('Virtual-Layout', this.options.vfolder);

		var body = '<?xml version="1.0"?>' +
			'<s:Envelope xmlns:s="http://schemas.xmlsoap.org/soap/envelope/" s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">' +
			'<s:Body>' + 
			'<u:' + this.options.action + ' xmlns:u="urn:schemas-upnp-org:service:' + this.options.target + ':1"> ' +
				content +
			'</u:' + this.options.action + '>' +
			'</s:Body>' +
			'</s:Envelope>';

		request.send({
			method: 'post',
			data: body
		});

	},

	xmlEscape: function(xml) {
		xml = xml.replace(/</g, "&lt;");
		xml = xml.replace(/>/g, "&gt;");
		xml = xml.replace(/\"/g, "&quot;");
		return xml;
	},

});

var ContentDirectory = new Class({

	Extends: UPnPRequest,

	initialize: function(options) {
		this.parent(options);
		this.options.target = "ContentDirectory";
	},


	createObject: function(containerId, title) {

		this.options.action = "CreateObject";
		this.options.onSuccess = this.createObjectSuccess;

		var content = 
			'<ContainerID>' + containerId + '</ContainerID>' +
			'<Elements>' + 
			this.xmlEscape(
			'<DIDL-Lite xmlns="urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/" xmlns:dc="http://purl.org/dc/elements/1.1/" xmlns:upnp="urn:schemas-upnp-org:metadata-1-0/upnp/">' +
			'<container id="" parentID="' + containerId + '" restricted="false">' +
			'<dc:title>' + title + '</dc:title>' +
			'<upnp:class>object.container.storageFolder</upnp:class>' +
			'</container>' +
			'</DIDL-Lite>') +
			'</Elements>';

		this.exec(content);
	},

	createObjectSuccess: function(responseText, responseXML) {
		var objectId = responseXML.documentElement.getElement('ObjectID').get('text');
		alert('CreateObjectSuccess ' + objectId);
	},


	createReference: function(containerId, objectId) {
		this.options.action = "CreateReference";
		this.options.onSuccess = this.createReferenceSuccess;

		var content = 
			'<ContainerID>' + containerId + '</ContainerID>' +
			'<ObjectID>' + objectId + '</ObjectID>';

		this.exec(content);
	},

	createReferenceSuccess: function(responseText, responseXML) {
		var newId = responseXML.documentElement.getElement('NewID').get('text');
		alert('createReferenceSuccess ' + newId);
	},

});


