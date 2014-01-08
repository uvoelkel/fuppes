'use strict';

function FuppesServiceAction() {
}

FuppesServiceAction.prototype = $.extend({}, {

    constructor: FuppesServiceAction,

    options: {
        url: '/',
        action: '',
        schema: '',
        headers: {'Content-Type': 'text/xml; charset=utf-8'},
        body: ''
    },

    execute: function(onSuccess, onError) {

		var body = '<?xml version="1.0"?>' +
            '<s:Envelope xmlns:s="http://schemas.xmlsoap.org/soap/envelope/" s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">' +
            '<s:Body>' + 
	            '<u:' + this.options.action + ' xmlns:u="' + this.options.schema + '">' +
	                this.options.body +
	            '</u:' + this.options.action + '>' +
            '</s:Body>' +
            '</s:Envelope>';

        var self = this;

        $.ajax({
            type: 'POST',
            url: this.options.url,
            headers: $.extend(this.options.headers, {'SOAPAction': '"' + this.options.schema + '#' + this.options.action + '"'}),
            data: body,
            dataType: 'xml',
            success: function(data, status, xhr) {

                var response = data.documentElement.getElementsByTagName('u:' + self.options.action + 'Response');
                if (1 != response.length) {
                    // error
                    return;
                }

                if ('function' != typeof onSuccess) {
                    return;
                }



                var parseResult = function(node, result) {         

                    for (var i = 0; i < node.childNodes.length; i++) {

                        if ('#text' == node.childNodes[i].nodeName) {
                            continue;
                        }

                        var data = {};
                        var child = node.childNodes[i];
                        var childNodeName = child.nodeName.replace(/:/, '_');

                        // create an array if a node with the current name already exists
                        if ('[object Object]' == Object.prototype.toString.call(result[childNodeName])) {
                            var tmp = result[childNodeName];
                            result[childNodeName] = new Array();
                            result[childNodeName].push(tmp);
                        }

                        // always create an array for DIDL-Lite results
                        if ('DIDL-Lite' == node.nodeName) {
                            
                            if ('[object Array]' != Object.prototype.toString.call(result[childNodeName])) {
                                result[childNodeName] = new Array();
                            }

                            if ('[object Array]' != Object.prototype.toString.call(result['objects'])) {
                                result['objects'] = new Array();
                            }
                        }

                        // parse attributes
                        if (child.hasAttributes()) {
                            data['attr'] = {};
                            for (var j = 0; j < child.attributes.length; j++) {
                                data['attr'][child.attributes[j].nodeName] = child.attributes[j].nodeValue;
                            }
                        }

                        if (1 == child.childNodes.length && '#text' == child.childNodes[0].nodeName) {

                            if ('Result' == child.nodeName) {
                                var xml = $.parseXML(child.childNodes[0].nodeValue);
                                parseResult(xml.documentElement, data);
                            }
                            else {
                                data.value = child.childNodes[0].nodeValue;
                            }
                        }

                        parseResult(child, data);

                        if ('[object Array]' == Object.prototype.toString.call(result[childNodeName])) {
                            result[childNodeName].push(data);

                            if ('DIDL-Lite' == node.nodeName) {
                                result['objects'].push(data);
                            }
                        }
                        else {
                            result[childNodeName] = data;
                        }
                    }
                };
                
                var result = {};
                parseResult(response[0], result);
                onSuccess(result);              
            }
        });

    }


});



function FuppesControlAction(options) {
    if ('object' == typeof options) {
        $.extend(true, this.options, options);
    }
    this.options.schema = 'urn:fuppes:service:SoapControl:1';
}

FuppesControlAction.prototype = $.extend({}, new FuppesServiceAction(), {
    constructor: FuppesControlAction
});


function FuppesContentDirectoryAction(options) {
    if ('object' == typeof options) {
        $.extend(true, this.options, options);
    }
    this.options.url = '/UPnPServices/ContentDirectory/control/';
    this.options.schema = 'urn:schemas-upnp-org:service:ContentDirectory:1';
}

FuppesContentDirectoryAction.prototype = $.extend({}, new FuppesServiceAction(), {
    constructor: FuppesContentDirectoryAction
});

