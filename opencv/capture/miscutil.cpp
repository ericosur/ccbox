#include "miscutil.h"
#include "readsetting.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

namespace miscutil {


void print_help()
{
    printf("help!\n"
        "-i input image\n"
        "-h help message\n"
        );
}

#ifdef USE_REALSENSE
bool handleOpt(int argc, char** argv)
{
    bool configured = false;
    do {
        if (argc == 1) {
            print_help();
            break;
        }

        while(1) {
            int cmd_opt = getopt(argc, argv, "hi:");
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
                cout << "input image:" << optarg << endl;
                ReadSetting::getInstance()->input_image = optarg;
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
