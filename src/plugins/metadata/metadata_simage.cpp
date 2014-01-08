/*
 * This file is part of fuppes
 *
 * (c) 2008 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include <fuppes_plugin.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
#define SIMAGE_DLL
#endif
#include <simage.h>
	
void register_fuppes_plugin(plugin_info* info)
{
	strcpy(info->plugin_name, "simage");
	strcpy(info->plugin_author, "Ulrich Voelkel");
	info->plugin_type = PT_METADATA;
}

int fuppes_metadata_file_open(plugin_info* plugin, const char* fileName)
{
	
	if(simage_check_supported(fileName) == 1) {	
		plugin->user_data = malloc((strlen(fileName)+1) * sizeof(char));
		strcpy((char*)plugin->user_data, fileName);
		return 0;
	}
	else {
		return -1;
	}
}

int fuppes_metadata_read(plugin_info* plugin, struct metadata_t* metadata)
{
	metadata->type = MD_IMAGE;
	
	unsigned char* img;
	int width;
	int height;
	int numComponents;
		
	img = simage_read_image((const char*)plugin->user_data, &width, &height, &numComponents);
	if(img == NULL) {
		return -1;
	}
	simage_free_image(img);
	
	metadata->width  = width;
	metadata->height = height;
	
	return 0;
}

void fuppes_metadata_file_close(plugin_info* plugin)
{
	free(plugin->user_data);
	plugin->user_data = NULL;
}

void unregister_fuppes_plugin(plugin_info* plugin __attribute__((unused)))
{
}

#ifdef __cplusplus
}
#endif
