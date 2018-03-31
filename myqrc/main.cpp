#include <iostream>
#include "test.h"


int main()
{
    using namespace std;
    string str;
    if ( test(str) == 0 ) {
        try_tts(str);
    }
    return 0;
}
