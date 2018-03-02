#include <string>
#include <iostream>

namespace pbox {
    std::string get_version_string();
}

void test_get_version()
{
    using namespace std;
    cout << __func__ << "==> " << pbox::get_version_string() << endl;
}
