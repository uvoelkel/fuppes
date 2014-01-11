'use strict';

(function($) {

$(document).ready(function() {

	var action = new FuppesControlAction({action : 'GetConfigFileName'})
        .execute(function(result) {
			$('#cfg_file_name').html(result.ConfigFileName.value);
        });


	$('.cfg_input').each(function(index, item) {
	
		var request = new FuppesControlAction({
            action: 'Get' + $(item).attr('data-function'), 
            attr: $(item).attr('data-function')
        });
        request.execute(function(result) {

            item.set('value', result[this.options.attr].value);

            $('<button/>', {
                text: 'set',
                'data-input': $(item).attr('id'),
                click: function() {

                    var button = this;
                    var input = $('#' + $(this).attr('data-input'));
                    var fn = $(input).attr('data-function');

                    var req = new FuppesControlAction({
                        action: 'Set' + fn,
                        body: '<' + fn + '>' + $(input).val() + '</' + fn + '>'
                    });
                    req.execute(function(result) {
                        $(button).html(result.Result.Message.value);
                    });

                    return false;
                }
            }).insertAfter(item);

        });
				
	});

});

})(jQuery);
