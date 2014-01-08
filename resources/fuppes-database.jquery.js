'use strict';

function updateStatus() {	
	var action = new FuppesControlAction({action : 'GetDatabaseStatus'})
        .execute(function(result) {
			$('.database-status').html(result.DatabaseStatus.value);			
			//window.setTimeout(updateStatus, 10000);
        });
}

$(window).load(function() {
    updateStatus();
});

