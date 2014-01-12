/*
 * This file is part of fuppes
 *
 * (c) 2014 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "TranscodingSettings.h"

#include "../Log/Log.h"

using namespace Configuration;

TranscodingSettings::TranscodingSettings()
{
	audioTarget = Transcoding::Memory;
	imageTarget = Transcoding::Memory;
	videoTarget = Transcoding::File;
}

void TranscodingSettings::readConfig(ConfigFile& configFile) throw (::Configuration::ReadException)
{
	tempDirRead = configFile.getStringValue("/fuppes_config/cache/temp_dir/text()");

	// old temp dir
	if (tempDirRead.empty()) {
		tempDirRead = configFile.getStringValue("/fuppes_config/global_settings/temp_dir/text()");
	}
}

void TranscodingSettings::setup(PathFinder& pathFinder) throw (::Configuration::SetupException)
{
	// command line option has the highest priority
	if (!tempDirOpt.empty()) {
		tempDir = tempDirOpt;
	}

	// the value from the config file comes second
	if (tempDir.empty()) {
		tempDir = tempDirRead;
	}

	// autodetection comes last
	if (tempDir.empty()) {
		#ifdef WIN32
		tempDirAuto = getenv("TEMP") + std::string("\\fuppes\\");
		#else
		char* szTmp = getenv("TEMP");
		if (szTmp != NULL) {
			tempDirAuto = std::string(szTmp) + "/fuppes/";
		}
		else {
			tempDirAuto = "/tmp/fuppes/";
		}
		#endif

		tempDir = tempDirAuto;
	}

	tempDir = fuppes::Directory::appendTrailingSlash(tempDir);
	if (!fuppes::Directory::create(tempDir)) {
		tempDir = "";

		log("Configuration")<< Log::Log::warning << Log::Log::printf(fuppes::Directory::exists(tempDir) ? "temp dir %s not writable" : "can't create temp % dir", tempDir.c_str());

		// reset transcoding targets if temp dir could not be created or is not writable
		audioTarget = Transcoding::Memory;
		imageTarget = Transcoding::Memory;
		videoTarget = Transcoding::Invalid;
	}
}

void TranscodingSettings::writeConfig(ConfigFile& configFile) throw (::Configuration::WriteException)
{
	// temp dir set from command line
	if (!tempDirOpt.empty() && 0 == tempDirOpt.compare(tempDir)) {
		return;
	}

	// temp dir read from config file
	if (!tempDirRead.empty() && 0 == tempDirRead.compare(tempDir)) {
		return;
	}

	// temp dir autodetected
	if (!tempDirAuto.empty() && 0 == tempDirAuto.compare(tempDir)) {
		return;
	}

	xmlNodePtr node = configFile.getNode("/fuppes_config/cache/temp_dir");
	configFile.setNodeText(node, tempDir);
}
