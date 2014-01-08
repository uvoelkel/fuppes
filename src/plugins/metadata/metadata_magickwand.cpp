/*
 * This file is part of fuppes
 *
 * (c) 2008-2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include <fuppes_plugin.h>
#include <fuppes_database_objects.h>
#include <wand/magick-wand.h>

void magick_set_date(MagickWand* wand, Database::AbstractItem& item) //char dateMetadata[], size_t size)
{
    char* date = MagickGetImageProperty(wand, "exif:DateTimeOriginal");

    if(date == 0) {
        date = MagickGetImageProperty(wand, "exif:DateTimeDigitized");
        if(date == 0)
            return;
    }

    item.details->date = date;
    //set_value(dateMetadata, size, date);
    MagickRelinquishMemory(date);
}

#ifdef __cplusplus
extern "C"
{
#endif

    void register_fuppes_plugin(plugin_info* info)
    {
        strcpy(info->plugin_name, "magickWand");
        strcpy(info->plugin_author, "Ulrich Voelkel");
        info->plugin_type = PT_METADATA;

#ifdef WIN32
        MagickWandGenesis();
#endif
    }

    int fuppes_metadata_file_open(plugin_info* plugin, const char* fileName)
    {
        MagickBooleanType status;

        plugin->user_data = NewMagickWand();
        MagickWand* wand = (MagickWand*)plugin->user_data;
        status = MagickReadImage(wand, fileName);

        if(status == MagickFalse) {

            /*ExceptionType severity;
             char* description;
             description = MagickGetException(plugin->user_data, &severity);
             (void)fprintf(stderr,"%s %s %lu %s\n", GetMagickModule(), description);
             description = (char*)MagickRelinquishMemory(description);	*/

            wand = DestroyMagickWand(wand);
            plugin->user_data = wand;
            return -1;
        }

        return 0;
    }

    int fuppes_metadata_read2(plugin_info* plugin, Database::AbstractItem& item) //struct metadata_t* metadata)
    {

        item.details->iv_width = MagickGetImageWidth((MagickWand*)plugin->user_data);
        item.details->iv_height = MagickGetImageHeight((MagickWand*)plugin->user_data);

        magick_set_date((MagickWand*)plugin->user_data, item); //metadata->date, sizeof(metadata->date));

        /*metadata->type = MD_IMAGE;

         metadata->width  = MagickGetImageWidth(plugin->user_data);
         metadata->height = MagickGetImageHeight(plugin->user_data);

         magick_set_date(plugin->user_data, metadata->date, sizeof(metadata->date));
         */

        return 0;
    }

    void fuppes_metadata_file_close(plugin_info* plugin)
    {
        plugin->user_data = DestroyMagickWand((MagickWand*)plugin->user_data);
        plugin->user_data = NULL;
    }

    void unregister_fuppes_plugin(plugin_info* plugin __attribute__((unused)))
    {
        MagickWandTerminus();
    }

#ifdef __cplusplus
}
#endif
