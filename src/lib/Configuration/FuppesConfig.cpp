/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "FuppesConfig.h"

#include "../Common/Directory.h"

using namespace Fuppes;

FuppesConfig::FuppesConfig()
{

}

void FuppesConfig::load(const std::string appDir) throw (Configuration::ReadException)
{
	globalSettings.applicationDir = fuppes::Directory::appendTrailingSlash(appDir);

	pathFinder.init(globalSettings.applicationDir);

	std::string filename = configFilename;
	if (filename.empty()) {
		filename = pathFinder.findInConfigPath("fuppes.cfg");
	}
	if (configFile.load(filename)) {
		configFilename = filename;
	}
	else {
		std::stringstream msg;
		msg << "could not find a readable config file.";
		#ifndef WIN32
		msg << std::endl <<
		        "make sure to either rename the file \"fuppes.cfg.default\" in \"" << FUPPES_LOCALSTATEDIR <<
		        "\" to \"fuppes.cfg\" or copy it to \"" << PathFinder::getXdgConfigHome() << "fuppes.cfg\"";
		#endif
		throw Configuration::ReadException(msg.str());
	}

	sharedObjects.readConfig(configFile);

	networkSettings.readConfig(configFile);

	databaseSettings.readConfig(configFile);

	globalSettings.readConfig(configFile);

	transcodingSettings.readConfig(configFile);

	virtualContainerSettings.readConfig(configFile);

	deviceIdentificationSettings.readConfig(configFile);

	contentDatabaseSettings.readConfig(configFile);
}

void FuppesConfig::setup() throw (Configuration::SetupException)
{
	sharedObjects.setup(pathFinder);

	networkSettings.setup(pathFinder);

	databaseSettings.setup(pathFinder);

	globalSettings.setup(pathFinder);

	transcodingSettings.setup(pathFinder);

	virtualContainerSettings.setup(pathFinder);

	deviceIdentificationSettings.setup(pathFinder, &virtualContainerSettings);

	contentDatabaseSettings.setup(pathFinder, databaseSettings, globalSettings, sharedObjects);
}

void FuppesConfig::save() throw (Configuration::WriteException)
{
	sharedObjects.writeConfig(configFile);

	networkSettings.writeConfig(configFile);

	databaseSettings.writeConfig(configFile);

	globalSettings.writeConfig(configFile);

	transcodingSettings.writeConfig(configFile);

	virtualContainerSettings.writeConfig(configFile);

	deviceIdentificationSettings.writeConfig(configFile);

	configFile.save();
}
