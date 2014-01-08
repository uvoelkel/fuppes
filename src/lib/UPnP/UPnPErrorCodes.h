
#ifndef _UPNP_UPNPERRORCODES_H
#define _UPNP_UPNPERRORCODES_H

#include <string>

typedef enum {

  upnp_200_ok,

  upnp_401_invalid_action,
  upnp_402_invalid_args,
  
  upnp_701_no_such_object,                                    // Browse()
  
  upnp_708_unsupported_or_invalid_search_criteria,            // Search()
  upnp_709_unsupported_or_invalid_sort_criteria,              // Browse()
  upnp_710_no_such_container,                                 // Search()

  upnp_712_bad_metadata,                                      // CreateObject()
  upnp_713_restricted_parent_object,                          // CreateObject()
  
  upnp_720_cannot_process_the_request                         // BS

} upnp_error_code_t;

#endif // _UPNP_UPNPERRORCODES_H
