#include "miscutil.h"
#include "readsetting.h"

#include <iostream>
#include <regex>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

namespace miscutil {

// input "color1544508130.png"
// return first group of number digits: "1544508130"
bool fetch_first_numbers(const std::string& s, std::string& match_str)
{
    std::regex word_regex("(\\d+)");
    auto words_begin =
        std::sregex_iterator(s.begin(), s.end(), word_regex);
    auto words_end = std::sregex_iterator();

    for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
        std::smatch match = *i;
        std::string m_str = match.str();
        if (m_str.size()) {
            match_str = m_str;
            return true;
        }
    }
    return false;
}

void print_help()
{
    printf("help!\n"
        "-i just show specified image\n"
        "-b input bag file\n"
        "-c color image\n"
        "-d depth image\n"
        "-p specify prefix, auto fetch color,depth,raw data\n"
        "-h help message\n"
        );
}

int replace_ext(const string& src, string& dst, const string& from, const string& to)
{
    string _s = src;
    int p = _s.find(from);
    if (p < 0) {
        return -1;
    }
    dst = _s.replace(p, _s.length()-to.length()+1, to);
    return 0;
}

#ifdef USE_REALSENSE
bool handleOpt(int argc, char** argv)
{
    bool configured = false;
    ReadSetting* sett = ReadSetting::getInstance();
    bool debug = sett->debug;
    do {
        if (argc == 1) {
            print_help();
            break;
        }

        while(1) {
            int cmd_opt = getopt(argc, argv, "hi:b:c:d:p:");
            if (cmd_opt == -1) {
                //qDebug() << "cmd_opt == -1";
                break;
            }
            switch (cmd_opt) {
            case 'h':   // help
                print_help();
                exit(2);
                break;
            case 'i':   // input image
                if (debug) {
                    cout << "input image:" << optarg << endl;
                }
                sett->input_image = optarg;
                configured = true;
                break;
            case 'b': // input bag file
                if (debug) {
                    cout << "input bag file:" << optarg << endl;
                }
                sett->input_bag = optarg;
                configured = true;
                break;
            case 'p':   // prefix
            {
                if (debug) {
                    cout << "prefix: " << optarg << endl;
                }
                string s = optarg;
                string n;
                if (fetch_first_numbers(s, n)) {
                    sett->color_image = string("color") + n + string(".png");
                    sett->depth_image = string("depth") + n + string(".png");
                    sett->depth_data = string("depth") + n + string(".bin");
                }
                configured = true;
            }
                break;
            case 'c':
                if (debug) {
                    cout << "color image:" << optarg << endl;
                }
                sett->color_image = optarg;
                configured = true;
                break;
            case 'd':
                if (debug) {
                    cout << "depth image:" << optarg << endl;
                }
                sett->depth_image = optarg;
                replace_ext(sett->depth_image, sett->depth_data, "png", "bin");
                if (debug) {
                    cout << "depth raw data:" << sett->depth_data << endl;
                }
                configured = true;
                break;
            default:
                exit(0);
                break;
            }
        }
    } while (false);

    return configured;
}
#endif

}   // namespace miscutil
