/***************************************************************************
 *            fuppes-playlist.js
 *
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2011 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 */


window.addEvent('domready', function() {

	var view = new BrowseColumnView({element: $('playlist-browse')});

	var browse = new FuppesBrowse({
		browse: 'DirectChildren',
		onSuccess: function(numberReturned, totalMatches, items) {
			
			//alert('success: ' + numberReturned + ' ' + totalMatches + ' ' + items);

			var col = view.addColumn();
			col.setItems(items);

		}
		
	});
	browse.exec();



	var pl = new FuppesPlaylist();

	$('playlist').setStyle('border', '1px solid red');
/*	$('playlist').addEvent('click', function() {


		pl.create();

	});
*/
});


var Column = new Class({
	
	Implements: [Options, Events],

	div: undefined,

	/*options: {
		//id: undefined
	// ,		onClick: undefined
  },*/

	initialize: function(options) {
		this.setOptions(options);


	//	onSuccess

//		this.options.id = id;

		this.div = new Element('div');
		this.div.set('id', 'col' + this.options.id);

		this.div.setStyle('border', '1px solid black');
		this.div.setStyle('height', '298px');
		this.div.setStyle('width', '198px');
		this.div.setStyle('float', 'left');
		this.div.setStyle('overflow-x', 'hidden');
		this.div.setStyle('overflow-y', 'scroll');
	},

	setItems: function(items) {

		var first = this.div.getFirst();
		if(first)
			first.destroy();

		var div = new Element('div');
		//table.set('width', '100%');

		var tmp = undefined;

		items.each(function(item, index) {

//			tr = new Element('div')

			tmp = new Element('div');
			tmp.set('text', item.title);
			tmp.set('id', item.objectId);
			tmp.addEvent('click', this.itemClick.bind(this));
			tmp.addClass('entry');
			tmp.addClass(item.type);

			tmp.addClass((index % 2) == 0 ? 'even' : 'odd');


			//tr.grab(tmp);

			div.grab(tmp);

		}.bind(this));

		this.div.grab(div);
	},


	itemClick: function(event) {
		//alert('onItemClick ' + event + this.id + this.get('class'));
		//alert(this + " " + event.target.id);

		this.fireEvent('onClick', [event, this]);
		//this.fireEvent('onClick', [numberReturned, totalMatches, items]);
		//this.options.onClick(event);

	},

});

var BrowseColumnView = new Class({

	Implements: [Options, Events],
	
	options: {
		element: undefined,
		columnWidth: 200,
		height: 300,
  },

	columns: [],
	currentItem: undefined,
//	column: 0,

	initialize: function(options) {
		this.setOptions(options);

		this.options.element.setStyle('border', '1px solid blue');
		this.options.element.setStyle('height', '400px');
		this.options.element.setStyle('width', this.options.height + 'px');
		this.options.element.setStyle('overflow', 'hidden');

		this.scroller = new Element('div', {id: 'scroller'});
		this.scroller.setStyle('width', this.options.columnWidth + 'px');
		this.scroller.setStyle('height', this.options.height + 'px');
		this.scroller.setStyle('border', '1px solid red');
		this.options.element.grab(this.scroller);

		this.clear = new Element('div');
		this.clear.setStyle('clear', 'both');
		this.scroller.grab(this.clear);

		var next = new Element('div', {id: 'next'});
		next.set('text', 'NEXT');
		next.addEvent('click', this.onNextClick.bind(this));
		this.options.element.getParent().grab(next); //inject(next, 'after');

		var prev = new Element('div', {id: 'prev'});
		prev.set('text', 'PREV');
		prev.addEvent('click', this.onPrevClick.bind(this));
		this.options.element.getParent().grab(prev); //inject(next, 'after');

		var current = new Element('div', {id: 'current'});
		current.set('text', '...');
		this.options.element.getParent().grab(current);
	},

	addColumn: function() {
		
		var col = new Column({
			id: this.columns.length, 
			onClick: this.onItemClick.bind(this),
		});

		this.columns.push(col);
		this.scroller.setStyle('width', (this.columns.length * this.options.columnWidth) + 'px');

		//this.clear.inject(col.div, 'before');

		col.div.inject(this.clear, 'before');

//		this.scroller.grab(col.div);


		return col;
	},

	setColumnCount: function(count) {


		if(this.columns.length == count) {
			return;
		}
		else if(this.columns.length > count) {

			for(var i = this.columns.length; i > count; i--) {
				var column = this.columns.getLast();
				column.div.destroy();
				this.columns.erase(column);
			}

		}
		else if(this.columns.length < count) {
			for(var i = this.columns.length; i < count; i++) {
				this.addColumn();
			}
		}

		this.scroller.setStyle('width', (this.columns.length * this.options.columnWidth) + 'px');

	},

	onItemClick: function(event, column) {
		//alert("onItemClick " + this + " " + event.target.id + " " + column.options.id);

		col = (column.options.id + 2);
		this.setColumnCount(col);

		var browse = new FuppesBrowse({
			browse: 'DirectChildren',
			objectId: event.target.id,
			onSuccess: this.onBrowseDirectChildrenSuccess.bind(this),
		}).exec();

		
		var items = event.target.getParent().getElements('div');
		items.each(function(item, index) {

			if(item.id == event.target.id)
				item.addClass('active');
			else
				item.removeClass('active');
		});

		this.currentItem = event.target;
		$('current').set('text', event.target.id);
	},

	onBrowseDirectChildrenSuccess: function(numberReturned, totalMatches, items) {
		//alert('browse success: ' + numberReturned + ' ' + totalMatches + ' ' + items);		
		var col = this.columns.getLast();
		col.setItems(items);
		this.onNextClick();
	},

	onNextClick: function() {
    var scrollFX = new Fx.Scroll(this.options.element, {fps: 50, duration: 250});
    scrollFX.start(this.options.element.getScroll().x + this.options.columnWidth, 0);
	},

	onPrevClick: function() {
    var scrollFX = new Fx.Scroll(this.options.element, {fps: 50, duration: 250});
    scrollFX.start(this.options.element.getScroll().x - this.options.columnWidth, 0);
	},

});



var FuppesPlaylist = new Class({

	Implements: [Options, Events],

	options: {
	},

	initialize: function(options) {
		this.setOptions(options);


		// create controls
		var ctrl = new Element('div');

		var tmp = new Element('input#create');
		tmp.set('type', 'button');
		tmp.set('value', 'create container');
		tmp.addEvent('click', this.createClicked.bind(this));

		ctrl.grab(tmp);


		$('playlist').grab(ctrl);

	},


	createClicked: function() {
		
		//alert('clicked');
		var tmp = prompt("Container name", "");
		if(null == tmp)
			return;

//		alert(tmp);

		var request = new ContentDirectory({});
		request.createObject(0, tmp);

	},


});


/*
var FuppesPlaylist = new Class({

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

		this.request.setHeader('SOAPAction','"fuppesctrl#' + this.options.action + '"');
		this.request.setHeader('Content-Type','text/xml; charset=utf-8');
		this.request.setHeader('User-Agent','fuppes webinterface');

		var body = '<?xml version="1.0"?>' +
		  '<s:Envelope xmlns:s="http://schemas.xmlsoap.org/soap/envelope/" s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">' +
		  '<s:Body>' + 
				'<c:' + this.options.action + ' xmlns:c="urn:fuppesControl">' +
				this.options.body +
				'</c:' + this.options.action + '>' +
		  '</s:Body>' +
		  '</s:Envelope>';

		this.request.send({
		  method: 'post',
		  data: body
		});

  }, // send


	onSuccess: function(responseText, responseXML) {

		result = undefined;
		error = undefined;
		action = this.options.action;

		body = responseXML.documentElement.getFirst();
		body.getChildren().each(function(item) {

			// action response
			if(item.get('tag') == ('c:' + action + 'response')) {
				result = item.getFirst();
			}
			// error
			else if(item.get('tag') == 'c:error') {
				code = item.getFirst();
				msg = code.getNext();
				error = "SOAP ERROR: " + msg.get('text') + " :: code: " + code.get('text');
			}
			// something else
			else {
				error = "unknown result";
			}

					
		});
				
		if(result != undefined)
			this.options.parent.fireEvent('onComplete', result);
		else
			this.options.parent.fireEvent('onError', error);
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
*/
