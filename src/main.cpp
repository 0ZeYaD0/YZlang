#include "core/defines.h"
#include "YLogger/logger.h"

i32 main() {

    LLOG(BLUE_TEXT("YZLang "), "v", 
        VERSION_MAJOR, ".", VERSION_MINOR, ".", VERSION_PATCH, "\n"
    );

    return 0;
}