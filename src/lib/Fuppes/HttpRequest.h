/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _FUPPES_HTTP_REQUEST_H
#define _FUPPES_HTTP_REQUEST_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../Http/Request.h"

#include "../DeviceIdentification/DeviceSettings.h"

namespace Fuppes
{

    class HttpRequest: public Http::Request
    {
        public:
            HttpRequest() :
                Http::Request()
            {
                m_deviceSettings = NULL;
                m_defaultDeviceSettings = NULL;
                m_vfoldersEnabled = false;
            }
            
            CDeviceSettings* getDeviceSettings() { return m_deviceSettings; }
            void setDeviceSettings(CDeviceSettings* settings) { m_deviceSettings = settings; }
            
            CDeviceSettings* getDefaultDeviceSettings() { return m_defaultDeviceSettings; }
            void setDefaultDeviceSettings(CDeviceSettings* settings) { m_defaultDeviceSettings = settings; }
            
            std::string getVfolderLayout() { return m_vfolderLayout; }
            void setVfolderLayout(const std::string layout) { m_vfolderLayout = layout; }
            bool getVfoldersEnabled() { return m_vfoldersEnabled; }
            void setVfoldersEnabled(const bool enabled) { m_vfoldersEnabled = enabled; }
            
        private:
            CDeviceSettings* 		m_deviceSettings;
            CDeviceSettings* 		m_defaultDeviceSettings;
            std::string             m_vfolderLayout;
            bool                    m_vfoldersEnabled;

    };

}

#endif // _FUPPES_HTTP_REQUEST_H
