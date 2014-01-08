window.addEvent('domready', function() {

	var request = new FuppesControl({
			action : 'GetConfigFileName',
			onComplete : function(result) {
				$('cfg_file_name').set('text', result.get('text'));
			}
		}).send();

	$$('.cfg_input').each(
			function(item, index) {

				var request = new FuppesControl({
					action : 'Get' + item.get('data-function'),
					onComplete : function(result) {
						item.set('value', result.get('text'));
					}
				}).send();

				var btn = new Element('button');
				btn.set('text', 'set');
				btn.addEvent('click', function(event) {

					var input = this.getPrevious();
					var funct = input.get('data-function');

					var set = new FuppesControl({
						action : 'Set' + funct,
						body : '<' + funct + '>' + input.get('value') + '</'
								+ funct + '>',
						onRequest : function() {
							btn.set('text', '...');
						},
						onComplete : function(result) {
							var code = result.getFirst().getFirst();
							var msg = code.getNext();						
							btn.set('text', msg.get('text'));
						},
						onError : function(error) {
							btn.set('text', 'err');
						}
					}).send();

					return false;
				});
				btn.inject(item, 'after');
			});

	$('selected-object-type').set('text', 'directory');

	getSharedObjects();

	var request = new FuppesControl({
		action : 'GetSharedObjectTypes',
		onComplete : function(result) {

			result.getChildren().each(function(item) {

				var el = new Element('input');
				el.set('type', 'radio');
				el.set('name', 'object-type');
				el.set('id', item.get('text'));
				el.set('value', item.get('text'));
	
				if (item.get('text') == $('selected-object-type').get(
						'text'))
					el.set('checked', 'checked');
	
				el.addEvent('click', function() {
					if ($('selected-object-type').get('text') != this
							.get('id'))
						setSelectedObjectType(this.get('id'));
				});
	
				var lb = new Element('label');
				lb.set('for', item.get('text'));
				lb.set('text', item.get('text'));
	
				$('dlg-shared-object-types').grab(el);
				$('dlg-shared-object-types').grab(lb);
			});

		}
	}).send();

});

function setSelectedObjectType(type) {

	$('selected-object-type').set('text', type);
	$('selected-object-name').set('text', '');
	$('selected-object-path').set('text', '');
	$('object-list').empty();

	if (type == "directory") {
		getDir("");
	} else {
		var el;

		el = new Element('span');
		el.set('text', 'name:');
		$('object-list').grab(el);

		el = new Element('input');
		el.set('id', 'new-object-name');
		$('object-list').grab(el);

		$('object-list').grab(new Element('br'));

		el = new Element('span');
		el.set('text', 'path:');
		$('object-list').grab(el);

		el = new Element('input');
		el.set('id', 'new-object-path');
		$('object-list').grab(el);
	}

}

function getSharedObjects() {

	var btn = new Element('button');
	btn.set('text', 'add shared object');
	btn.addEvent('click', function(event) {
		dlgAddSharedObject();
	});
	btn.inject('shared-objects-list', 'after');

	var request = new FuppesControl({
		action : 'GetSharedObjects',
		onComplete : function(result) {

			var html = '';
			html += '<div class="grid_2">type</div>';
			html += '<div class="grid_5">path</div>';
			html += '<div class="clear"></div>';

			result.getChildren().each(function(item) {

				html += '<div class="grid_2">'
						+ item.getProperty('item-type') + '</div>';
				html += '<div class="grid_5">'
						+ item.getProperty('path') + '</div>';
				html += '<div class="grid_1">'
						+ '<a href="javascript:delSharedObject(\''
						+ item.getProperty('uuid') + '\');">DEL</a>'
						+ '</div>';
				html += '<div class="clear"></div>';
			});

			$('shared-objects-list').innerHTML = html;
		}
	}).send();
}

function getDir(dir) {

	var body = "<dir show-files=\"false\">" + dir + "</dir>";

	var request = new FuppesControl({
		action : 'GetDir',
		body : body,
		onComplete : function(result) {
	
			type = undefined;
			path = undefined;
	
			var tmp = "<table width=\"100%\">";
			result.getChildren().each(function(item) {
	
				tmp += "<tr>";
	
				if (item.get('tag') == 'parent') {
					tmp += '<td colspan="2"><a href="javascript:getDir(\''
							+ item.get('text')
							+ '\');">'
							+ item.get('text')
							+ '</a></td>';
				} else {
					tmp += '<td>';
					tmp += '<a href="javascript:getDir(\''
							+ item.get('text')
							+ '\');">'
							+ item.get('name') + '</a>';
					tmp += '</td>';
					tmp += '<td>';
					tmp += '<a href="javascript:setDir(\''
							+ item.get('text')
							+ '\');">SET</a>';
					tmp += '</td>';
				}
	
				tmp += "</tr>";
	
			});
			tmp += "</table>";
	
			$('object-list').innerHTML = tmp;
		}
	}).send();
}

function setDir(dir) {
	$('selected-object-path').set('text', dir);
}

function dlgAddSharedObject() {

	var dlg = $('dlg-shared-object');
	if (dlg == undefined)
		return;

	visible = (dlg.getStyle('display') != "none");
	if (visible) {
		dlg.setStyle('display', 'none');
		return;
	}

	var width = dlg.getStyle('width').toInt(); // +
	// dlg.getStyle('padding').toInt()
	// * 2;
	var left = (window.getSize().x - width) / 2;
	dlg.setStyle('left', left);

	var height = dlg.getStyle('height').toInt(); // +
	// dlg.getStyle('padding').toInt()
	// * 2;
	var top = (window.getSize().y - height) / 2;
	dlg.setStyle('top', top);

	dlg.fade('hide');
	dlg.setStyle('display', 'block');
	dlg.fade('in');
	// dlg.get('tween', {property: 'opacity', duration: 'short'}).start(1);

	setSelectedObjectType($('selected-object-type').get('text'));
}

function submit() {

	type = $('selected-object-type').get('text');

	body = "<object type=\"" + type + "\" ";
	if (type == "directory") {
		body += "path=\"" + $('selected-object-path').get('text') + "\" ";
	} else {
		body += "path=\"" + $('new-object-path').get('value') + "\" ";
		body += "name=\"" + $('new-object-name').get('value') + "\" ";
	}
	body += "/>";

	var request = new FuppesControl({
		action : 'AddSharedObject',
		body : body,
		onComplete : function(result) {
			getSharedObjects();
			dlgAddSharedObject();
		}
	}).send();
}

function cancel() {
	dlgAddSharedObject();
}

function delSharedObject(uuid) {

	if (!confirm("delete object?"))
		return;

	body = "<object uuid=\"" + uuid + "\" />";

	var request = new FuppesControl({
		action : 'DelSharedObject',
		body : body,
		onComplete : function(result) {
			getSharedObjects();
		}
	});

	request.send();
}
