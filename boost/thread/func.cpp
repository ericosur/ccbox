#include <iostream>

#include <boost/thread/thread.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/timer/timer.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/uuid/sha1.hpp>
#include <boost/functional/hash.hpp>

#include "simplenotify.h"
#include "pbox/pbox.h"
#include "func.h"

void display(char* hash);
void get_sha1sum(std::string& input_str, char* ohash);

void wait_thread(int repeat_times)
{
    for (int i = 0; i < repeat_times; ++i)
    {
        boost::this_thread::sleep_for(boost::chrono::milliseconds(WAIT_MSECOND));
        std::cout << "wait_thread: " << i << '\n';
    }
}

int waitfile_thread(int max_timeout)
{
    std::string fn = pbox::get_home() + '/' + WATCH_DIR + SETTING_FILE;
    std::cout << "watch folder: " << fn << "\n";
    for (int i = 0; i < max_timeout; ++i)
    {
        std::cout << "wait_thread: " << i << '\n';
        if ( !pbox::is_file_exist(fn) ) {
            std::cout << "file not found...\n";
            boost::this_thread::sleep_for(boost::chrono::milliseconds(WAIT_MSECOND));
        } else {
            std::cout << "oh yeah!\n";
            break;
        }
    }
    std::cout << "end of waitfile_thread\n";
    return 0;
}

void sha1sum_thread()
{
    std::string str = "quick smart fox jumps over the lazy dog";
    char md[HASH_SIZE];
    memset(md, 0, HASH_SIZE);

    for (int i = 0; i < HASH_REPEAT; ++i) {
        get_sha1sum(str, md);
        str = str + md;
        display(md);
        std::cout << std::dec;
        std::cout << "sha1sum_thread: " << i << ", " << "size: " << str.size() << "\n";
    }
}

int simplehash_thread()
{
    boost::hash<std::string> string_hash;

    int h = string_hash("Hash me");
    std::cout << "hash_code: " << std::hex << h << "\n";
    return h;
}

void display(char* hash)
{
    std::cout << "SHA1: " << std::hex;
    for(int i = 0; i < HASH_SIZE; ++i)
    {
        std::cout << ((hash[i] & 0x000000F0) >> 4)
                  <<  (hash[i] & 0x0000000F);
    }
    std::cout << std::endl; // Das wars
}

void get_sha1sum(std::string& input_str, char* ohash)
{
    boost::uuids::detail::sha1 s;
    char hash[HASH_SIZE];
    //std::string a = "Franz jagt im komplett verwahrlosten Taxi quer durch Bayern";
    s.process_bytes(input_str.c_str(), input_str.size());
    unsigned int digest[5];
    s.get_digest(digest);
    for (int r = 0; r < HASH_REPEAT; ++r) {
        for(int i = 0; i < 5; ++i)  {
            const char* tmp = reinterpret_cast<char*>(digest);
            hash[i*4] = tmp[i*4+3];
            hash[i*4+1] = tmp[i*4+2];
            hash[i*4+2] = tmp[i*4+1];
            hash[i*4+3] = tmp[i*4];
        }
    }

    strncpy(ohash, hash, HASH_SIZE);
}

int add_thread()
{
    boost::timer::cpu_timer timer;

    std::uint64_t total = 0;
    for (int i = 0; i < UPPER_LIMIT; ++i)
        total += i;

    std::cout << timer.format();
    std::cout << "do_add: " << total << '\n';

    return total;
}
