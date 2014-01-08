/***************************************************************************
 *            fuppes-control.js
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


var FuppesControl = new Class({

	Implements: [Options, Events],
  options: {
		action: '',
		body: '',
		onComplete: undefined,
		onError: undefined
  },

	initialize: function(options) {
		this.setOptions(options);

		this.request = new Request({
			parent: this,
			action: this.options.action.toLowerCase(),
		  url: '/', 
		  onSuccess: this.onSuccess,
		});
	},

	send: function() {
		this.request.setHeader('SOAPAction','"urn:fuppes:service:SoapControl:1#' + this.options.action + '"');
		/*this.request.setHeader('Content-Type','text/xml; charset=utf-8');
		//this.request.setHeader('User-Agent','fuppes webinterface');*/

		var body = '<?xml version="1.0"?>' +
		  '<s:Envelope xmlns:s="http://schemas.xmlsoap.org/soap/envelope/" s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">' +
		  '<s:Body>' + 
				'<u:' + this.options.action + ' xmlns:u="urn:fuppes:service:SoapControl:1">' +
				this.options.body +
				'</u:' + this.options.action + '>' +
		  '</s:Body>' +
		  '</s:Envelope>';

		this.request.send({
		  method: 'post',
		  data: body
		});

  }, // send


	onSuccess: function(responseText, responseXML) {

		result = null;
		error = null;
		action = this.options.action;

		body = responseXML.documentElement.getFirst();
		body.getChildren().each(function(item) {

			// action response
			if(item.get('tag') == ('u:' + action + 'response')) {
				result = item; //.getFirst();
			}
			// error
			else if(item.get('tag') == 'c:error') {
				code = item.getFirst();
				msg = code.getNext();
                if (null != msg) {
				    error = "SOAP ERROR: " + msg.get('text') + " :: code: " + code.get('text');
                }
                else {
				    error = "SOAP ERROR: code: " + code.get('text');
                }
			}
			// something else
			else {
				error = "unknown result";
			}

					
		});
				
		if(result != null) {
			this.options.parent.fireEvent('onComplete', result);
        }
		else {
			this.options.parent.fireEvent('onError', error);
        }
	} // onSuccess

});

function fuppesCtrl(action) 
{

	var request = new FuppesControl({
		action: action,
		onComplete: function(result) {


			tmp = "";

			result.getChildren().each(function(item) {

				tmp += item.get('tag') + "<br />";
			});

			$('ctrl-result').innerHTML = "complete  " + tmp;

			//alert('complete');
		}
	});

	request.send();
}

