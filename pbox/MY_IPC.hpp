#ifndef VoiceText_IPCH
#define VoiceText_IPCH

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "UEFIType.h"
//---------------------------------------------------------------------------

class MY_IPC
{
private:
	int MYIPC_fd;
	int error_code;
	char *error_str;
	char error_str_buf[100];

	MY_IPC();
	INT32 PRE_IPC_Init(unsigned int IPCSize);
	INT32 IPC_Init();
public:
	static MY_IPC * MY_IPC_GetInstance();
	static MY_IPC * MY_IPC_GetInstance_PREInit();
	static MY_IPC * MY_IPC_GetInstance_PREInit(unsigned int IPCSize);

	INT32 IPC_Create_TAG_String(const char *TagName,const char *DefaultValue,UINT32 DefaultSize=0);
	INT32 IPC_Get_TAG_String(const char *TagName, char *Value,UINT32 ValueSize);
	INT32 IPC_Get_TAG_String(const char *TagName, char *Value,UINT32 ValueSize,UINT32 TimeOut);
	INT32 IPC_Put_TAG_String(const char *TagName, const char *Value);
	INT32 IPC_Get_TAG_INT32(const char *TagName);
	INT32 IPC_Get_TAG_INT32(const char *TagName,UINT32 TimeOut);
	INT32 IPC_Put_TAG_INT32(const char *TagName, INT32 Value);

	INT32 IPC_Stall(UINT32 ms);

	int ErrorCode();
	const char *GetError();
};

#endif

