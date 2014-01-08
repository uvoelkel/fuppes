/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "Plugin.h"

#include "../Common/File.h"

Plugin::Plugin::Plugin()
{
    m_type = ptUnknown;

    m_handle = NULL;
    m_register = NULL;
    m_unregister = NULL;

    m_data = NULL;
}

Plugin::Plugin::Plugin(Plugin* plugin)
{
    m_type = plugin->m_type;
    m_name = plugin->m_name;

    m_handle = plugin->m_handle;
    m_register = plugin->m_register;
    m_unregister = plugin->m_unregister;

    m_data = NULL;
}

Plugin::Plugin::~Plugin()
{
}
/*
Plugin::Plugin::Plugin()
{
    m_handle = NULL;
    m_type = ptUnknown;
    m_data = NULL;
}

Plugin::Plugin::~Plugin()
{
    std::map<std::string, ProcedureHandle>::iterator iter = m_procedures.find("fuppes_unregister_plugin");
    if (m_procedures.end() != iter && NULL != iter->second) {
        ((unregister_t)iter->second)();
    }

    if (NULL != m_handle) {
        Plugin::closeLibrary(m_handle);
    }
}


bool Plugin::Plugin::load(LibraryHandle handle, register_t registerProc, unregister_t unregisterProc, plugin_info* info)
{
    m_handle = handle;

    m_procedures["fuppes_register_plugin"] = (ProcedureHandle)registerProc;
    m_procedures["fuppes_unregister_plugin"] = (ProcedureHandle)unregisterProc;

    m_type = info->type;
    m_name = info->name;

    return init();
}
*/

bool Plugin::Plugin::load(LibraryHandle handle, register_t registerProc, unregister_t unregisterProc, plugin_info* info)
{
    m_handle = handle;

    m_register = registerProc;
    m_unregister = unregisterProc;

    m_type = info->type;
    m_name = info->name;

    return init(m_handle);
}

void Plugin::Plugin::unload()
{
    m_unregister();
    Plugin::closeLibrary(m_handle);
}

/*
void Plugin::Plugin::init(LibraryHandle handle)
{
    std::cout << "Plugin::init" << std::endl;
}
*/

Plugin::LibraryHandle Plugin::Plugin::loadLibrary(const std::string filename) // static
{
    std::string ext = fuppes::File::extension(filename);
    if (ext.empty()) {
        return NULL;
    }

#ifdef WIN32
    if(0 != ext.compare("dll")) {
        return NULL;
    }

    return LoadLibrary(filename.c_str());
#else
    if (0 != ext.compare("so") && 0 != ext.compare("dylib")) {
        return NULL;
    }

    LibraryHandle handle = dlopen(filename.c_str(), RTLD_LAZY);
    const char* error;
    if ((error = dlerror()) != NULL) {
        //fprintf(stderr, "%s\n", error);
        return NULL;
    }
    return handle;
#endif
}

Plugin::ProcedureHandle Plugin::Plugin::getProcedure(LibraryHandle handle, const std::string procedure) // static
{
#ifdef WIN32
    return GetProcAddress(handle, procedure.c_str());
#else
    ProcedureHandle proc = dlsym(handle, procedure.c_str());
    const char* error;
    if ((error = dlerror()) != NULL) {
        //fprintf(stderr, "%s\n", error);
        return NULL;
    }
    return proc;
#endif
}

void Plugin::Plugin::closeLibrary(LibraryHandle handle) // static
{
#ifdef WIN32
    FreeLibrary(handle);
#else
    dlclose(handle);
    dlerror();
#endif
    handle = NULL;
}
