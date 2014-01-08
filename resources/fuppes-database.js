function updateStatus() {
	
	var request = new FuppesControl({
		action : 'GetDatabaseStatus',
		onComplete : function(result) {
			$$('.database-status').set('text', result.get('text'));
			
			window.setTimeout(updateStatus, 10000);
		}
	}).send();	
}

window.addEvent('domready', function() {	
	
	if (0 < $$('.database-status').length) {	
		updateStatus();
	}
		
	if(!$('dbcontrols')) {
		return;
	}	
	
	var btnUpdate = new Element('button');
	btnUpdate.set('text', 'update');
	btnUpdate.addEvent('click', function(event) {

        var set = new FuppesControl({
	        action : 'DatabaseUpdate',
	        onRequest: function() {	    
	        	btnUpdate.set('text', '...');
	        },
	        onComplete : function(result) {
	        	btnUpdate.set('text', 'update');
	        	$$('.database-status').set('text', result.get('text'));
	        },
            onError : function(error) {
            	btnUpdate.set('text', 'err');
            }
        }).send();
        return false;
	});
	btnUpdate.inject($('dbcontrols'), 'bottom');
    
    
    var btnRebuild = new Element('button');
    btnRebuild.set('text', 'rebuild');
    btnRebuild.addEvent('click', function(event) {

        var set = new FuppesControl({
	        action : 'DatabaseRebuild',
	        onRequest: function() {	    
	        	btnRebuild.set('text', '...');
	        },
	        onComplete : function(result) {
	        	btnRebuild.set('text', 'rebuild');
	        	$$('.database-status').set('text', result.get('text'));
	        },
            onError : function(error) {
            	btnRebuild.set('text', 'err');
            }
        }).send();
        return false;
	});
    btnRebuild.inject($('dbcontrols'), 'bottom');
});
