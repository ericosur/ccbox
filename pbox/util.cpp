
#include "pbox.h"

#include <string>
#include <stdio.h>
#include <stdarg.h>

namespace pbox {

std::string get_version_string()
{
    return "v2018.03.06";
}

void mylog(const char* name, const char* format, ...)
{
    const int UTIL_DEFAULT_MYLOG_BUFFER_SIZE = 256;
    char buffer[UTIL_DEFAULT_MYLOG_BUFFER_SIZE];
    va_list aptr;
    int ret;

    va_start(aptr, format);
    ret = vsprintf(buffer, format, aptr);
    va_end(aptr);
    (void)ret;

    printf("[%d][%s]: %s\n", pbox::get_timeepoch(), name, buffer);

}


}   // namespace pbox
