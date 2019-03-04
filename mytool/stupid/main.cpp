#include <mytool/mytool.h>

#include <stdio.h>
#include <unistd.h>


int test()
{
    const int repeat = 10;
    for (int i = 0; i < repeat; i++) {
        mytool::mylog("test", "%d\n", mytool::get_timeepoch());
        sleep(1);
    }

    return 0;
}

int main()
{
    printf("%s\n", mytool::get_version_string().c_str());
    test();
    return 0;
}

