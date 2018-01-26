#include "foo.h"
#include <iostream>
#include <vector>

#define JSONFILE "/home/rasmus/src/ccbox/usepbox/test.json"

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

int main()
{
    using namespace std;

    dump_jsonfile(JSONFILE);
#if 1
    vector<string> k;
    k.push_back("test");
    mytest(k);

    k.clear();
    k.push_back("query");
    k.push_back("result");
    k.push_back("item");
    mytest(k);
#endif
    return 0;
}
