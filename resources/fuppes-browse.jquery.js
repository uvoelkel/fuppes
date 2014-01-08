'use strict';

$(document).ready(function() {

	var action = new FuppesControlAction({action : 'GetVirtualContainers'})
        .execute(function(result) {

        	if ('true' != result.VirtualContainers.attr.enabled) {
        		return;
        	}

            $.each(result.VirtualContainers.vcontainer, function(index, item) {
                if ('true' == item.attr.enabled) {
                    $('#virtual-layout').append('<option>' + item.value + '</option>');
                }
            });
        });
});








function browseDirectChildren(objectId, startIndex, requestCount, vfolder) 
{
    var body =           
        '<ObjectID>' + objectId + '</ObjectID>' + 
        '<Filter>*</Filter>' + 
        '<StartingIndex>' + startIndex + '</StartingIndex>' + 
        '<RequestedCount>' + requestCount + '</RequestedCount>' + 
        '<SortCriteria></SortCriteria>' + 
        '<BrowseFlag>BrowseDirectChildren</BrowseFlag>';         

    var action = new FuppesContentDirectoryAction({action: 'Browse', headers: {'Virtual-Layout': vfolder}, body: body})
        .execute(function(result) {

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
            msg += 'NumberReturned: ' + result.NumberReturned.value + '<br />';
            msg += 'TotalMatches: ' + result.TotalMatches.value + '<br />';

            $.each(result.Result.objects, function(index, item) {
  
                var title = null;
                var type = null;

                if(item.upnp_class.value.indexOf("object.container.") != -1) {
                    title = '<a href="javascript:browseDirectChildren(\'' + item.attr.id + '\', 0, 0, \'' + vfolder + '\');">' + item.dc_title.value + '</a>';
                    type = '<a href="javascript:showObjectDetails(\'' + item.attr.id + '\', \'' + vfolder + '\');">' + "container" + '</a>';
                }
                else if(item.upnp_class.value.indexOf("object.item.") != -1) {
                    title = item.dc_title.value;
                    type = '<a href="javascript:showObjectDetails(\'' + item.attr.id + '\', \'' + vfolder + '\');">' + "item" + '</a>';
                }

                table += '<tr>';
                table += '<td>' + item.attr.id  + '</td>';
                table += '<td>' + type  + '</td>';
                table += '<td>' + title  + '</td>';
                table += '<td>' + item.attr.childCount  + '</td>';
                table += '</tr>';

                table += '<tr>';
                table += '<td colspan="4" class="td-detail" style="display: none;" id="detail-td-' + item.attr.id + '">load details</td>';
                table += '</tr>';

            });


            table += '</table>';
            $('#browse-result').html(table);

    		browseMetadata(objectId, vfolder);
        });
}


function browseMetadata(objectId, vfolder) 
{
    var body =           
        '<ObjectID>' + objectId + '</ObjectID>' + 
        '<Filter>*</Filter>' + 
        '<StartingIndex>0</StartingIndex>' + 
        '<RequestedCount>0</RequestedCount>' + 
        '<SortCriteria></SortCriteria>' + 
        '<BrowseFlag>BrowseMetadata</BrowseFlag>';         

    var action = new FuppesContentDirectoryAction({action: 'Browse', headers: {'Virtual-Layout': vfolder}, body: body})
        .execute(function(result) {
            
            var item = result.Result.objects[0];

            $('#self-id').innerHTML = item.attr.id;
            $('#self-label').innerHTML = item.attr.id;

            $('#parent-id').html(item.attr.parentID);
            $('#parent-browse').html('');
            if ('-1' != item.attr.parentID) {
                $('#parent-browse').html('<a href="javascript:browseDirectChildren(\'' + item.attr.parentID + '\', 0, 0, \''  + vfolder + '\');">up</a>');
            }
        });
}

