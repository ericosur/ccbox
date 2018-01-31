// no library need to list
// need specify boost include file path while compiling
// $ g++ -I../../../boost_1_58_0/ -o first.app first.cpp

#include <boost/lambda/lambda.hpp>
#include <iostream>
#include <iterator>
#include <algorithm>


// easy sample to use boost lambda
int main()
{
    using namespace boost::lambda;
    typedef std::istream_iterator<int> in;

    std::cout << "enter numbers and enter, use Ctrl-C to quit...\n";
    std::for_each(
        in(std::cin), in(), std::cout << (_1 * 3.0) << "\n"
    );

    return 0;
}
