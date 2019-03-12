#include "curlutil.h"
#include "qweather.h"
#include <iostream>

#define SHOWHEADER() \
    std::cout << __func__ << " =====>>\n";

using namespace std;

void demo1()
{
    SHOWHEADER();
    const string city = "taipei";
    //cout << "default location: " << city << endl;
    query_weather(city);
}

void demo2()
{
    SHOWHEADER();
    const string qstr = "https://ifconfig.me/all.json";
    query_via_curl(qstr, "all.json");
}

int main(int argc, char* argv[])
{
    // web site for demo1 does not exist any more
    // demo1();

    demo2();

    return 0;
}
