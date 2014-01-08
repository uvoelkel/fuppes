/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "Item.h"

#include <stdio.h>

#include "ItemDetails.h"

using namespace Database;

Item::Item()
{
    details = new ItemDetails();
    clear();
}

Item::~Item()
{
    if (NULL != details) {
        delete details;
    }
}

Item& Item::operator=(Item& item)
{

    //printf("ITEM::==");

    clear();
    detailId = item.detailId;
    type = item.type;
    source = item.source;
    path = item.path;
    filename = item.filename;
    extension = item.extension;
    title = item.title;

    visible = item.visible;
    size = item.size;

    return *this;
}

void Item::clear()
{
    id = 0;
    objectId = 0;
    parentId = 0;
    detailId = 0;
    type = OBJECT_TYPE_UNKNOWN;
    source = "";
    path = "";
    filename = "";
    extension = "";
    title = "";
    refId = 0;
    device = "";
    vcontainerType = 0;
    vcontainerPath = "";
    vrefId = 0;
    visible = true;
    size = 0;
    modified = 0;
    updated = 0;

    if (NULL != details) {
        details->clear();
    }
}

std::string Item::getObjectIdAsHex() const
{
    char hex[20];
    sprintf(hex, "%"PRObjectId, objectId);
    return hex;
}

std::string Item::getParentIdAsHex() const
{
    char hex[20];
    sprintf(hex, "%"PRObjectId, parentId);
    return hex;
}

std::string Item::getRefIdAsHex() const
{
    char hex[20];
    sprintf(hex, "%"PRObjectId, refId);
    return hex;
}

bool Item::isAudio() const
{
    return (type >= ITEM_AUDIO_ITEM && type < ITEM_AUDIO_ITEM_MAX);
}

bool Item::isVideo() const
{
    return (type >= ITEM_VIDEO_ITEM && type < ITEM_VIDEO_ITEM_MAX);
}

bool Item::isImage() const
{
    return (type >= ITEM_IMAGE_ITEM && type < ITEM_IMAGE_ITEM_MAX);
}

/*
 int Item::getChildCount(std::string objectTypes) const
 {
 #warning todo
 return 0;
 }
 */
