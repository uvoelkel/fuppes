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

#include <fileref.h>
#include <tfile.h>
#include <tag.h>
#include <mpegfile.h>
#include <id3v2tag.h>
#include <attachedpictureframe.h>

#include <iostream>
using namespace std;

int taglib_open_file(const char* filename, void** data)
{
    *data = new TagLib::FileRef(filename);
    if (((TagLib::FileRef*)*data)->isNull() || (((TagLib::FileRef*)*data)->audioProperties()) == NULL) {
        delete (TagLib::FileRef*)*data;
        *data = NULL;
        return -1;
    }
    return 0;
}

void taglib_close_file(TagLib::FileRef* file)
{
    delete file;
    file = NULL;
}

void taglib_get_title(TagLib::FileRef* file, Database::AbstractItem& item) // metadata_t* audio
{
    TagLib::String sTmp = file->tag()->title();
    std::string title = sTmp.to8Bit(true);
    if (!title.empty()) {
        item.title = title;
    }
    //set_value(audio->title, sizeof(audio->title), sTmp.to8Bit(true).c_str());
}

void taglib_get_artist(TagLib::FileRef* file, Database::AbstractItem& item)
{
    TagLib::String sTmp = file->tag()->artist();
    item.details->av_artist = sTmp.to8Bit(true);
    //set_value(audio->artist, sizeof(audio->artist), sTmp.to8Bit(true).c_str());
}

void taglib_get_album(plugin_info* plugin, metadata_t* audio)
{
    TagLib::String sTmp = ((TagLib::FileRef*)plugin->user_data)->tag()->album();
    set_value(audio->album, sizeof(audio->album), sTmp.to8Bit(true).c_str());
}

void taglib_get_genre(plugin_info* plugin, metadata_t* audio)
{
    TagLib::String sTmp = ((TagLib::FileRef*)plugin->user_data)->tag()->genre();
    set_value(audio->genre, sizeof(audio->genre), sTmp.to8Bit(true).c_str());
}

void taglib_get_comment(plugin_info* plugin, metadata_t* audio)
{
    TagLib::String sTmp = ((TagLib::FileRef*)plugin->user_data)->tag()->comment();
    set_value(audio->description, sizeof(audio->description), sTmp.to8Bit(true).c_str());
}

void taglib_get_composer(plugin_info* plugin, metadata_t* audio)
{
    TagLib::MPEG::File* mpegFile = new TagLib::MPEG::File(((TagLib::FileRef*)plugin->user_data)->file()->name());
    if (mpegFile->isValid() == false || mpegFile->ID3v2Tag() == NULL) {
        delete mpegFile;
        return;
    }

    TagLib::ID3v2::Tag *tag = mpegFile->ID3v2Tag();
    const TagLib::ID3v2::FrameList frameList = tag->frameList("TCOM");
    if (frameList.isEmpty()) {
        delete mpegFile;
        return;
    }

    set_value(audio->composer, sizeof(audio->composer), frameList.front()->toString().to8Bit(true).c_str());
    delete mpegFile;
}

void taglib_get_duration(plugin_info* info, metadata_t* metadata)
{
    TagLib::String sTmp;

    long length = ((TagLib::FileRef*)info->user_data)->audioProperties()->length();
    metadata->duration_ms = length * 1000;
}

void taglib_get_channels(plugin_info* info, metadata_t* metadata)
{
    metadata->nr_audio_channels = ((TagLib::FileRef*)info->user_data)->audioProperties()->channels();
}

void taglib_get_track_no(plugin_info* info, metadata_t* metadata)
{
    metadata->track_number = ((TagLib::FileRef*)info->user_data)->tag()->track();
}

void taglib_get_year(plugin_info* info, metadata_t* metadata)
{
    metadata->year = ((TagLib::FileRef*)info->user_data)->tag()->year();
}

void taglib_get_bitrate(plugin_info* info, metadata_t* metadata)
{
    metadata->audio_bitrate = ((TagLib::FileRef*)info->user_data)->audioProperties()->bitrate();
    metadata->audio_bitrate *= 1024;
}

void taglib_get_samplerate(plugin_info* info, metadata_t* metadata)
{
    metadata->audio_sample_frequency = ((TagLib::FileRef*)info->user_data)->audioProperties()->sampleRate();
}

void taglib_check_image(TagLib::FileRef* file, Database::AbstractItem& item)
{
    TagLib::MPEG::File mpegFile(file->file()->name());
    if (mpegFile.isValid() == false || mpegFile.ID3v2Tag() == NULL) {
        return;
    }

    TagLib::ID3v2::Tag *tag = mpegFile.ID3v2Tag();
    const TagLib::ID3v2::FrameList frameList = tag->frameList("APIC");
    if (frameList.isEmpty()) {
        return;
    }

    TagLib::ID3v2::AttachedPictureFrame* picFrame;
    picFrame = dynamic_cast<TagLib::ID3v2::AttachedPictureFrame*>(frameList.front());
    if (picFrame == NULL) {
        return;
    }

    //cout << "mime/type: " << picFrame->mimeType().toCString() << endl;
    item.details->albumArtMimeType = picFrame->mimeType().toCString();
    //metadata->has_image = 1;
    //set_value(metadata->image_mime_type, sizeof(metadata->image_mime_type), picFrame->mimeType().toCString());
}

int taglib_read_image(TagLib::FileRef* file, char* mimeType, unsigned char** buffer, fuppes_off_t* size)
{
    *size = 0;
    TagLib::MPEG::File* mpegFile = new TagLib::MPEG::File(file->file()->name());
    if (mpegFile->isValid() == false || mpegFile->ID3v2Tag() == NULL) {
        delete mpegFile;
        return -1;
    }

    TagLib::ID3v2::Tag *tag = mpegFile->ID3v2Tag();
    const TagLib::ID3v2::FrameList frameList = tag->frameList("APIC");
    if (frameList.isEmpty()) {
        delete mpegFile;
        return -1;
    }

    TagLib::ID3v2::AttachedPictureFrame* picFrame;
    picFrame = dynamic_cast<TagLib::ID3v2::AttachedPictureFrame*>(frameList.front());
    if (picFrame == NULL) {
        delete mpegFile;
        return -1;
    }

    strcpy(mimeType, picFrame->mimeType().toCString());

    const TagLib::ByteVector pic = picFrame->picture();
    *buffer = (unsigned char*)realloc(*buffer, pic.size());
    memcpy(*buffer, pic.data(), pic.size());
    *size = pic.size();

    delete mpegFile;
    return 0;
}


extern "C" void fuppes_register_plugin(plugin_info* plugin)
{
    plugin->type = ptMetadata;
    strcpy(plugin->name, "taglib");
}

extern "C" int fuppes_open_file(const char* filename, void** data)
{
    if (0 != taglib_open_file(filename, data)) {
        return -1;
    }

    return 0;
}

extern "C" int fuppes_read_metadata(Database::AbstractItem& item, void** data)
{
    TagLib::FileRef* file = (TagLib::FileRef*)*data;

    taglib_get_title(file, item);
    taglib_get_artist(file, item);

    /*
     taglib_get_artist(plugin, metadata);
     taglib_get_album(plugin, metadata);
     taglib_get_genre(plugin, metadata);
     taglib_get_comment(plugin, metadata);
     taglib_get_duration(plugin, metadata);
     taglib_get_channels(plugin, metadata);
     taglib_get_track_no(plugin, metadata);
     taglib_get_year(plugin, metadata);
     taglib_get_bitrate(plugin, metadata);
     taglib_get_samplerate(plugin, metadata);
     taglib_get_composer(plugin, metadata);
     // original performer // TOPE
     */
    taglib_check_image(file, item);

    //metadata->audio_bits_per_sample = 0;

    return 0;
}

extern "C" int fuppes_read_image(char* mimeType, unsigned char** buffer, fuppes_off_t* size, void** data)
{
    TagLib::FileRef* file = (TagLib::FileRef*)*data;
    return taglib_read_image(file, mimeType, buffer, size);
}

extern "C" void fuppes_close_file(void** data)
{
    TagLib::FileRef* file = (TagLib::FileRef*)*data;
    taglib_close_file(file);
}

extern "C" void fuppes_unregister_plugin()
{
}

