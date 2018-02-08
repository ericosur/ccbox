#ifndef __FUNC__H__
#define __FUNC__H__

#define WAIT_MSECOND    1000
#define WAIT_REPEAT     60
#define HASH_REPEAT     3000
#define UPPER_LIMIT     100000000
#define HASH_SIZE       20
#define WAIT_FOR_SECOND 5
#define WATCH_DIR       "src/ccbox/boost/thread/"
#define SETTING_FILE    "setting.json"

void wait_thread(int repeat_times);
int waitfile_thread(int max_timeout);

#endif  // __FUNC__H__
