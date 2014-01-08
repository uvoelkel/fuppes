/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _PLUGIN_PLUGIN_H
#define _PLUGIN_PLUGIN_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#ifndef WIN32
#include <dlfcn.h>
#endif

#include <fuppes_plugin.h>
#include <string>
#include <map>

namespace Plugin
{
#ifdef WIN32
    typedef HINSTANCE LibraryHandle;
    typedef FARPROC ProcedureHandle;
#else
    typedef void* LibraryHandle;
    typedef void* ProcedureHandle;
#endif

    typedef void (*register_t)(plugin_info* info);
    typedef void (*unregister_t)();

    class Plugin
    {
        public:
            Plugin(Plugin* plugin);
            virtual ~Plugin();

            static LibraryHandle loadLibrary(const std::string filename);
            static ProcedureHandle getProcedure(LibraryHandle handle, const std::string procedure);
            static void closeLibrary(LibraryHandle handle);

            bool load(LibraryHandle handle, register_t registerProc, unregister_t unregisterProc, plugin_info* info);
            void unload();

            PluginType_t getType()
            {
                return m_type;
            }
            const std::string getName()
            {
                return m_name;
            }

        protected:
            Plugin();

            virtual bool init(LibraryHandle handle) = 0;

            PluginType_t m_type;
            std::string m_name;
            void* m_data;
        private:


            LibraryHandle   m_handle;
            register_t      m_register;
            unregister_t    m_unregister;


    };
}

#endif // _PLUGIN_PLUGIN_H
