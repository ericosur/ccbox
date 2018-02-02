#include "foo.h"
#include <iostream>
#include <stdio.h>
#include <vector>

#define JSONFILE "/home/rasmus/src/ccbox/usepbox/test.json"

//#define USE_DEMO1
#define USE_DEMO2

void test();

void mytest(const std::vector<std::string>& k)
{
    using namespace std;

    vector<string> strings;
    strings = get_vector_from_jsonfile(JSONFILE, k);
    cout << "test:" << endl;
#if __cplusplus >= 201103L
    for (string s : strings) {
        cout << s << endl;
    }
#else
    for (vector<string>::const_iterator i = strings.begin(); i != strings.end(); ++i) {
        cout << *i << endl;
    }
#endif
}

void print_banner(const char* msg)
{
    const int repeat = 20;
    for (int i = 0; i < repeat; ++i) {
        printf("=");
    }
    printf(" %s ", msg);
    for (int i = 0; i < repeat; ++i) {
        printf("=");
    }

    printf("\n");
}

int demo1()
{
#ifdef USE_DEMO1
    using namespace std;

    print_banner("dump_jsonfile");
    dump_jsonfile(JSONFILE);
    print_banner("get_vector_from_jsonfile");
    vector<string> k;
    // .test
    k.push_back("test");
    mytest(k);

    print_banner("get_vector_from_jsonfile");
    // .query.result.item
    k.clear();
    k.push_back("query");
    k.push_back("result");
    k.push_back("item");
    mytest(k);

    print_banner("get_double_from_jsonfile");
    // .values.pi
    k.clear();
    k.push_back("values");
    k.push_back("pi");
    printf("double from .values.pi: %lf\n",
           get_double_from_jsonfile(JSONFILE, k));
#endif
    return 0;
}

void demo2()
{
    //test();

#ifdef USE_DEMO2
    using namespace std;
    vector<string> k;

    print_banner("get_value_from_jsonfile");
    // .values.pi
    k.clear();
    k.push_back("values");
    k.push_back("pi");
    double dd;
    // if ( get_value_from_jsonfile(JSONFILE, k, dd) ) {
    //     printf("float from .values.pi: %lf\n", dd);
    // } else {
    //     printf("failed to read value\n");
    // }
    dd = get_double_from_jsonfile(JSONFILE, k);
    cout << dd << endl;

#endif  // USE_DEMO2


}

int main()
{
    demo1();
    demo2();

    return 0;
}
