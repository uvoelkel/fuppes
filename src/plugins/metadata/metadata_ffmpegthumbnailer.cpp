/*
 * This file is part of fuppes
 *
 * (c) 2009-2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include <fuppes_plugin.h>
#include <libffmpegthumbnailer/videothumbnailer.h>

typedef struct {
	char* filename;
} ffmpegthumbnailer_t;

extern "C" void fuppes_register_plugin(plugin_info* plugin)
{
    plugin->type = ptMetadata;
    strcpy(plugin->name, "ffmpegthumbnailer");
}

extern "C" int fuppes_open_file(const char* filename, void** data)
{	
	*data = malloc(sizeof(ffmpegthumbnailer_t));
	((ffmpegthumbnailer_t*)*data)->filename = (char*)malloc(strlen(filename) + 1);
	strcpy(((ffmpegthumbnailer_t*)*data)->filename, filename);
		
	return 0;
}

extern "C" int fuppes_read_image(char* mimeType, unsigned char** buffer, fuppes_off_t* size, void** data)
{
	ffmpegthumbnailer_t* tn = (ffmpegthumbnailer_t*)*data;
	int width = 0;
    if(0 == width) {
        width = 160;
    }

    try {
        ffmpegthumbnailer::VideoThumbnailer videoThumbnailer;
        videoThumbnailer.setThumbnailSize(width);

        std::vector<uint8_t> image;
        videoThumbnailer.generateThumbnail(tn->filename, Jpeg, image);

        *size = image.size();
        *buffer = (unsigned char*)realloc(*buffer, *size);
        memcpy(*buffer, &image.front(), *size);

        strcpy(mimeType, "image/jpeg");
    }
    catch(...) {
        return -1;
    }

    return 0;
}

extern "C" void fuppes_close_file(void** data)
{
	free(((ffmpegthumbnailer_t*)*data)->filename);
	free(*data);
}

extern "C" void fuppes_unregister_plugin()
{
}
