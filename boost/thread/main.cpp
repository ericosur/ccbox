#include <boost/thread/thread.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/chrono.hpp>
#include <boost/shared_ptr.hpp>

#include <iostream>
#include <cstdint>

#include "func.h"

#include <boost/version.hpp>
#if BOOST_VERSION < 105000
#define TIME_UTC_ TIME_UTC
#endif

struct thread_alarm
{
    thread_alarm(int secs) : m_secs(secs) { exit_code = IntPtr( new int(0) ); }
    void operator()()
    {
        boost::xtime xt;
        boost::xtime_get(&xt, boost::TIME_UTC_);
        xt.sec += m_secs;

        boost::thread::sleep(xt);

        std::cout << "alarm sounded..." << std::endl;

        *exit_code = 0xDEADBEEF;
    }

    int m_secs;

    typedef boost::shared_ptr<int> IntPtr;
    IntPtr exit_code;
};

int main(int argc, char* argv[])
{
    using namespace std;


    // int secs = WAIT_FOR_SECOND;
    // std::cout << "setting alarm for " << WAIT_FOR_SECOND << " seconds..." << std::endl;
    // thread_alarm alarm(secs);
    // boost::thread thrd(alarm);


    boost::thread waitfile(waitfile_thread, WAIT_REPEAT);
    waitfile.join();
    //std::cout << "exit code == 0x" << std::hex << *(alarm.exit_code) << std::endl;
    cout << "end of main\n";

    return 0;
}
