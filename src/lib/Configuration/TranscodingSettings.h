/*
 * This file is part of fuppes
 *
 * (c) 2014 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _CONFIGURATION_TRANSCODING_SETTINGS
#define _CONFIGURATION_TRANSCODING_SETTINGS

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string>

#include "BaseSettings.h"
#include "../Transcoding/Configuration.h"

namespace Configuration
{

	class TranscodingSettings: public BaseSettings, public Transcoding::Configuration
	{
		public:
			TranscodingSettings();

			void readConfig(ConfigFile& configFile) throw (::Configuration::ReadException);
			void setup(PathFinder& pathFinder) throw (::Configuration::SetupException);
			void writeConfig(ConfigFile& configFile) throw (::Configuration::WriteException);

			std::string tempDirOpt;
			std::string tempDirRead;
			std::string tempDirAuto;
	};

}

#endif // _CONFIGURATION_TRANSCODING_SETTINGS
