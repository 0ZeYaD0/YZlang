#include "core/defines.h"
#include "YLogger/logger.h"

i32 main() {

    if(YZDEBUG)
    LLOG(BLUE_TEXT("YZLang "), "v", 
        VERSION_MAJOR, ".", VERSION_MINOR, ".", VERSION_PATCH, "\n"
    );

    return 0;
}