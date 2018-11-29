#include <iostream>
#include "test.h"


int main()
{
    using namespace std;
    string str;
#if 0
    if ( test(str) == 0 ) {
        try_tts(str);
    }
#else
    test(str);
#endif
    return 0;
}
