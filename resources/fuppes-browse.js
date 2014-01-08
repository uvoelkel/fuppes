/***************************************************************************
 *            fuppes-browse.js
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
	
    var request = new FuppesControl({
	    action : 'GetVirtualContainers',
	    onComplete : function(result) {
	    	
	    	if ('true' != result.getFirst().getProperty('enabled')) {
	    		return;
	    	}
	    	
	    	result.getFirst().getChildren().each(function(item) {
	    		if ('true' == item.getProperty('enabled')) {	    	
		    		var el = new Element('option');
		    		el.set('text', item.get('text'));
		    		$('virtual-layout').grab(el);
	    		}	    		
	    	});
	    }
    }).send();
	
});

function browseDirectChildren(objectId, startIdx, requestCnt, vfolder) 
{

  var request = new Request({
    url: '/UPnPServices/ContentDirectory/control/', 
    onSuccess: function(responseText, responseXML) {

    var table = '<table id="table-browse" width="100%">' +
      '<tr>' +
      '<th width="100">id</th>' +
      '<th width="100">class</th>' +
      '<th>title</th>' +
      '<th width="60">count</th>' +
      '</tr>';

		table += 
			'<tr>' +
         '<td id="self-id"></td>' +
	       '<td colspan="3" id="self-label">&nbsp;</td>' +
			'</tr>' +
			'<tr>' +
         '<td id="parent-id"></td>' +
	       '<td colspan="3" id="parent-browse">&nbsp;</td>' +
			'</tr>';


    var msg = '';
    msg += 'NumberReturned: ' + responseXML.documentElement.getElement('NumberReturned').get('text') + '<br />';
    msg += 'TotalMatches: ' + responseXML.documentElement.getElement('TotalMatches').get('text') + '<br />';


    var result = loadXmlResult(responseXML.documentElement.getElement('Result').get('text'));
    var items = result.documentElement.getChildren();
    items.each(function(item, index) {

      var nodes = item.getChildren();

      var upnp_class = null;
      var dc_title = null;
      var res = null;
      nodes.each(function(node, node_index) {

        if(node.get('tag') == 'upnp:class') {
          upnp_class = node.get('text');
        }
        else if(node.get('tag') == 'dc:title') {
          dc_title = node.get('text');
        }
        else if(node.get('tag') == 'res') {
          res = node.get('text');
        }

      });


      var title = null;
      var type = null;

      if(upnp_class.indexOf("object.container.") != -1) {
        title = '<a href="javascript:browseDirectChildren(\'' + item.get('id') + '\', 0, 0, \'' + vfolder + '\');">' + dc_title + '</a>';
        type = '<a href="javascript:showObjectDetails(\'' + item.get('id') + '\', \'' + vfolder + '\');">' + "container" + '</a>';
      }
      else if(upnp_class.indexOf("object.item.") != -1) {
        title = dc_title;
        type = '<a href="javascript:showObjectDetails(\'' + item.get('id') + '\', \'' + vfolder + '\');">' + "item" + '</a>';
      }

      table += '<tr>';

      table += '<td>' + item.get('id')  + '</td>';
      table += '<td>' + type  + '</td>';
      table += '<td>' + title  + '</td>';
      table += '<td>' + item.get('childCount')  + '</td>';

      table += '</tr>';

      table += '<tr>';
      table += '<td colspan="4" class="td-detail" style="display: none;" id="detail-td-' + item.get('id') + '">load details</td>';
      table += '</tr>';
    });


    table += '</table>';

    $('browse-result').innerHTML = table;

		browseMetadata(objectId, vfolder);
  }});

  request.setHeader('SOAPAction','"urn:schemas-upnp-org:service:ContentDirectory:1#Browse"');
  request.setHeader('Content-Type','text/xml; charset=utf-8');
  request.setHeader('User-Agent','fuppes webinterface');
  request.setHeader('Virtual-Layout', vfolder);

  var body = '<?xml version="1.0"?>' +
    '<s:Envelope xmlns:s="http://schemas.xmlsoap.org/soap/envelope/" s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">' +
    '<s:Body>' + 
    '<u:Browse xmlns:u="urn:schemas-upnp-org:service:ContentDirectory:1"> ' +
    '<ObjectID>' + objectId + '</ObjectID>' + 
    '<Filter>*</Filter>' + 
    '<StartingIndex>' + startIdx + '</StartingIndex>' + 
    '<RequestedCount>' + requestCnt + '</RequestedCount>' + 
    '<SortCriteria></SortCriteria>' + 
    '<BrowseFlag>BrowseDirectChildren</BrowseFlag>' + 
    '</u:Browse>' +
    '</s:Body>' +
    '</s:Envelope>';

  request.send({
    method: 'post',
    data: body
  });

}



function browseMetadata(objectId, vfolder, details) 
{
	if(details == undefined)
		details = false;

  var request = new Request({
    url: '/UPnPServices/ContentDirectory/control/', 
    onSuccess: function(responseText, responseXML) {

			var result = loadXmlResult(responseXML.documentElement.getElement('Result').get('text'));
		  var items = result.documentElement.getChildren();
		  items.each(function(item, index) {

				if(!details) {
					$('parent-id').innerHTML = item.get('parentID');
					if(item.get('parentID') != -1)
						$('parent-browse').innerHTML = '<a href="javascript:browseDirectChildren(\'' + item.get('parentID') + '\', 0, 0, \''  + vfolder + '\');">up</a>';
					else
						$('parent-browse').innerHTML = '';


					$('self-id').innerHTML = item.get('id');
					$('self-label').innerHTML = item.get('id');
				}
				else {
					setObjectDetails(item);
				}
			});

 	}});

  request.setHeader('SOAPAction','"urn:schemas-upnp-org:service:ContentDirectory:1#Browse"');
  request.setHeader('Content-Type','text/xml; charset=utf-8');
  request.setHeader('User-Agent','fuppes webinterface');
  request.setHeader('Virtual-Layout', vfolder);

  var body = '<?xml version="1.0"?>' +
    '<s:Envelope xmlns:s="http://schemas.xmlsoap.org/soap/envelope/" s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">' +
    '<s:Body>' + 
    '<u:Browse xmlns:u="urn:schemas-upnp-org:service:ContentDirectory:1"> ' +
    '<ObjectID>' + objectId + '</ObjectID>' + 
    '<Filter>*</Filter>' + 
    '<StartingIndex>0</StartingIndex>' + 
    '<RequestedCount>0</RequestedCount>' + 
    '<SortCriteria></SortCriteria>' + 
    '<BrowseFlag>BrowseMetadata</BrowseFlag>' + 
    '</u:Browse>' +
    '</s:Body>' +
    '</s:Envelope>';

  request.send({
    method: 'post',
    data: body
  });

}



function showObjectDetails(objectId, vfolder)
{
	var details = $('detail-td-' + objectId);
  if(details == undefined)
    return;

	//details.setStyle('background-color', "#00FF00");

	visible = (details.getStyle('display') != "none");
  if(visible) {
		closeObjectDetails(objectId);
    return;
  }

	browseMetadata(objectId, vfolder, true);

  details.fade('hide');
  details.setStyle('display', '');
  //details.get('tween', {property: 'opacity', duration: 'short'}).start(1);
  details.fade('in');
}

function setObjectDetails(object)
{

	var values = new Object();
	values['title'] = undefined;
	values['class'] = undefined;
	values['albumart'] = undefined;
	values['res'] = new Object();
	values['res']['url'] = undefined;

	debug = "";
  var nodes = object.getChildren();
  nodes.each(function(node, node_index) {

		if(node.get('tag') == 'dc:title') {
			values['title'] = node.get('text');
		}
		else if(node.get('tag') == 'upnp:class') {
			values['class'] = node.get('text');
		}

		else if(node.get('tag') == 'upnp:artist') {
		}
		else if(node.get('tag') == 'upnp:album') {
		}
		else if(node.get('tag') == 'upnp:genre') {
		}
		else if(node.get('tag') == 'upnp:originaltracknumber') {
		}

		else if(node.get('tag') == 'upnp:albumarturi') {
			values['albumart'] = node.get('text');
		}
		else if(node.get('tag') == 'res') {
			values['res']['url'] = node.get('text');

			values['res']['protocolInfo'] = node.get('protocolInfo');
			values['res']['duration'] = node.get('duration');
			values['res']['resolution'] = node.get('resolution');
			values['res']['bitrate'] = node.get('bitrate');
			values['res']['size'] = node.get('size');

			if(values['class'].indexOf('object.item.imageItem') == 0)
				values['albumart'] = values['res']['url'];
		}
		else {
			debug += "*" + node.get('tag') + ": " + node.get('text') + "*<br />";
		}

  });

	result = '<div class="object-details">';
	result += '<div class="object-details-close"><a href="javascript:closeObjectDetails(\'' + object.get('id') + '\');">close</a></div>';
	result += '<div>';

		result += '<div class="album-art-image">';
		if(values['albumart'] != undefined)
			result += '<img src="' + values['albumart'] + '" height="100" alt=""/>';
		else
			result += 'no image available';
		result += '</div>';

		result += '<div class="object-values">';
		result += '<table>';
			result += '<tr>';
			result += '<th colspan="4">' + values['title'] + '</th>';
			result += '</tr>';

			result += '<tr>';
			result += '<td>class</td><td colspan="3">' + values['class'] + '</td>';
			result += '</tr>';

			result += '<tr>';
			result += '<td>object id</td><td>' + object.get('id') + '</td><td>parent id</td><td>' + object.get('parentID') + '</td>';
			result += '</tr>';
			
			result += '<tr>';
			result += '<td>res</td><td colspan="3">' + 
									values['res']['url'] + 
									/*'<ul>' +
										'<li>protocolInfo: ' + values['res']['protocolInfo'] + '</li>' +
										'<li>duration: ' + values['res']['duration'] + '</li>' +
										'<li>resolution: ' + values['res']['resolution'] + '</li>' +
										'<li>bitrate: ' + values['res']['bitrate'] + '</li>' +
										'<li>size: ' + values['res']['size'] + '</li>' +
									'</ul>' +*/
								'</td>';
			result += '</tr>';

		result += '</table>';
		result += debug;
		result += '</div>';
		

    if(values['class'].indexOf('object.item.audioItem') == 0) {
      result += "<div>";
        result += "<audio controls=\"controls\" preload=\"none\">";
        result += "<source src=\"" + values['res']['url'] + "\" type=\"" + "audio/mpeg" + "\" />"; // values['res']['protocolInfo']
        // add at least ogg and mp3 sources
        result += "Your browser does not support the audio element.";
        result += "</audio>";
      result += "</div>";
    }


	result += '</div>';
	result += '</div>';

	var details = $('detail-td-' + object.get('id'));
	details.innerHTML = result;
}

function closeObjectDetails(objectId)
{
	var details = $('detail-td-' + objectId);
  if(details == undefined)
    return;

  visible = (details.getStyle('display') != "none");
  if(!visible) {
    return;
  }
	details.setStyle('display', 'none');
  details.getElement('.object-details').destroy();
}





function UPnPItem(){

	var objectId = undefined;
	var childCount = 0;
	var type = undefined;  // item | container

	var	title = undefined;





};


var FuppesBrowse = new Class({

	Implements: [Options, Events],
	
	options: {
		browse: "DirectChildren", // DirectChildren | Metadata

		objectId: '0',
		startingIndex: 0,
		requestedCount: 0,
		
		vfolder: undefined,


		onSuccess: undefined
  },

	initialize: function(options) {
		this.setOptions(options);


	},

	exec: function() {
		if(this.options.browse == "DirectChildren")
			this.directChildren();
		else if(this.options.browse == "Metadata")
			this.metadata();
	},


	directChildren: function () {

		
		var request = new Request({
			parent: this,
    	url: '/UPnPServices/ContentDirectory/control/', 
    	onSuccess: function(responseText, responseXML) {

				var items = [];

				// parse the result
    		var numberReturned = responseXML.documentElement.getElement('NumberReturned').get('text').toInt();
    		var totalMatches = responseXML.documentElement.getElement('TotalMatches').get('text').toInt();

    		var xml = loadXmlResult(responseXML.documentElement.getElement('Result').get('text'));
		    xml.documentElement.getChildren().each(function(item, index) {

      		var nodes = item.getChildren();

		      var upnp_class = undefined;
		      var dc_title = undefined;
		      var res = undefined;
		      nodes.each(function(node, node_index) {

    		    if(node.get('tag') == 'upnp:class') {
    		      upnp_class = node.get('text');
    		    }
    		    else if(node.get('tag') == 'dc:title') {
    		      dc_title = node.get('text');
    		    }
    		    else if(node.get('tag') == 'res') {
    		      res = node.get('text');
    		    }
    			});

					var tmp = new UPnPItem();
					tmp.objectId = item.get('id');
					tmp.childCount = item.get('childCount');
					tmp.title = dc_title;

					if(upnp_class.indexOf("object.container.") != -1)
						tmp.type = "container";
	      	else if(upnp_class.indexOf("object.item.") != -1)
						tmp.type = "item";

					items.push(tmp);
				});

				

				this.options.parent.fireEvent('onSuccess', [numberReturned, totalMatches, items]);

				//alert(parent.options.onSuccess);
				//parent.options.onSuccess(
			}

		});

		request.setHeader('SOAPAction','"urn:schemas-upnp-org:service:ContentDirectory:1#Browse"');
		request.setHeader('Content-Type','text/xml; charset=utf-8');
		if(this.options.vfolder)
			request.setHeader('Virtual-Layout', this.options.vfolder);

		var body = '<?xml version="1.0"?>' +
			'<s:Envelope xmlns:s="http://schemas.xmlsoap.org/soap/envelope/" s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">' +
			'<s:Body>' + 
			'<u:Browse xmlns:u="urn:schemas-upnp-org:service:ContentDirectory:1"> ' +
			'<ObjectID>' + this.options.objectId + '</ObjectID>' + 
			'<Filter>*</Filter>' + 
			'<StartingIndex>' + this.options.startingIndex + '</StartingIndex>' + 
			'<RequestedCount>' + this.options.requestedCount + '</RequestedCount>' + 
			'<SortCriteria></SortCriteria>' + 
			'<BrowseFlag>BrowseDirectChildren</BrowseFlag>' + 
			'</u:Browse>' +
			'</s:Body>' +
			'</s:Envelope>';

		request.send({
			method: 'post',
			data: body
		});

	},
	
	metadata: function () {
		alert('metadata');
	}




});


