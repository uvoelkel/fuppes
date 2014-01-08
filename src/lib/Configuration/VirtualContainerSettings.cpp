/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "VirtualContainerSettings.h"

using namespace Configuration;

VirtualContainerSettings::VirtualContainerSettings()
{
    enabled = false;
}

void VirtualContainerSettings::readConfig(ConfigFile& configFile) throw (::Configuration::ReadException)
{
    xmlNodePtr node = configFile.getNode("/fuppes_config/vfolders");

    enabled = (0 == configFile.getNodeAttribute(node, "enabled").compare("true"));

    xmlNodePtr child = xmlFirstElementChild(node);
    while (child) {
        std::string name = (char*)child->name;
        if (0 == name.compare("vfolder")) {

            VirtualContainerEntry entry;
            entry.name = configFile.getNodeAttribute(child, "name");
            entry.enabled = (0 == configFile.getNodeAttribute(child, "enabled").compare("true"));
            entry.xmlNode = child;

            virtualContainers.push_back(entry);
        }
        child = xmlNextElementSibling(child);
    }
}

void VirtualContainerSettings::setup(PathFinder& pathFinder) throw (::Configuration::SetupException)
{
    VirtualContainer::VirtualContainerIter iter;
    for (iter = virtualContainers.begin(); iter != virtualContainers.end(); iter++) {

        iter->cfgfile = pathFinder.findVFolderInPath(iter->name);
        if (iter->enabled && iter->cfgfile.empty()) {
            throw ::Configuration::SetupException("no configuration for vfolder " + iter->name + " found");
        }

    }
}

void VirtualContainerSettings::writeConfig(ConfigFile& configFile) throw (::Configuration::WriteException)
{

}
