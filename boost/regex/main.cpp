#include <boost/regex.hpp>
#include <string>
#include <iostream>

/**
 * example source code from
 * https://theboostcpplibraries.com/boost.regex
 */

void test()
{
    using namespace std;
    string s = "Terry has 100 dollars, and Mary has 200 dollars";
    //boost::regex expr{"([A-Z][a-z]+)"};
    boost::regex expr{"(\\d+)"};

    boost::regex_token_iterator<string::iterator> it{s.begin(), s.end(), expr, 1};
    boost::regex_token_iterator<string::iterator> end;

    while (it != end) {
        cout << *it++ << endl;
    }
}

int main()
{
    test();
    return 0;
}
