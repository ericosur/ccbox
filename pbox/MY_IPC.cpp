
#include "MY_IPC.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>

#define MY_IPC_ERROR_VERBOSE 0

#define MY_IPC_INIT                _IO('X',0xE1)
#define MY_IPC_TAG_CREATE          _IO('X',0xE2)
#define MY_IPC_TAG_READ            _IOR('X',0xE3,int)
#define MY_IPC_TAG_WAIT            _IOR('X',0xE4,int)
#define MY_IPC_TAG_WRITE           _IOW('X',0xE5,int)
#define MY_IPC_STALL               _IOW('X',0xE6,int)

#define MYIPC_IF_PATH "/dev/myipc_if"

union MY_IPC_IOCTL_ArgList
{
	//MY_IPC_INIT Arguments
	struct{
		unsigned int TAG_Pool_Total_Size;
	}MY_IPC_INIT_Arg;

	//MY_IPC_TAG_CREATE Arguments
	struct{
		char TAGName[32];
		unsigned int DataLen;
		char *DataInit;
	}MY_IPC_TAG_CREATE_Arg;

	//MY_IPC_TAG_READ Arguments
	struct{
		char TAGName[32];
		unsigned int DataLen;
		char *Data;
	}MY_IPC_TAG_READ_Arg;

	//MY_IPC_TAG_WAIT Arguments
	struct{
		unsigned int wait_time_ms;
		char TAGName[32];
		unsigned int DataLen;
		char *Data;
	}MY_IPC_TAG_WAIT_Arg;

	//MY_IPC_TAG_WRITE Arguments
	struct{
		char TAGName[32];
		unsigned int DataLen;
		char *Data;
	}MY_IPC_TAG_WRITE_Arg;

	//MY_IPC_STALL Arguments
	struct{
		unsigned int wait_time_ms;
	}MY_IPC_STALL_Arg;
};

static MY_IPC *MY_IPC_Instance = NULL;

MY_IPC::MY_IPC()
{
	this->MYIPC_fd = -1;
	this->error_code = 0;
	this->error_str = NULL;
}

MY_IPC * MY_IPC::MY_IPC_GetInstance()
{
	if (MY_IPC_Instance != NULL) return MY_IPC_Instance;

	MY_IPC_Instance = new MY_IPC();

	if (MY_IPC_Instance != NULL && MY_IPC_Instance->IPC_Init() != 0) {
		delete MY_IPC_Instance;
		MY_IPC_Instance = NULL;
	}

	return MY_IPC_Instance;
}
MY_IPC * MY_IPC::MY_IPC_GetInstance_PREInit()
{
	if (MY_IPC_Instance != NULL){
		delete MY_IPC_Instance;
		MY_IPC_Instance = NULL;
	}

	MY_IPC_Instance = new MY_IPC();

	if (MY_IPC_Instance != NULL && MY_IPC_Instance->PRE_IPC_Init(64*1024) != 0) {
		delete MY_IPC_Instance;
		MY_IPC_Instance = NULL;
	}

	return MY_IPC_Instance;
}
MY_IPC * MY_IPC::MY_IPC_GetInstance_PREInit(unsigned int IPCSize)
{
	if (MY_IPC_Instance != NULL){
		delete MY_IPC_Instance;
		MY_IPC_Instance = NULL;
	}

	MY_IPC_Instance = new MY_IPC();

	if (MY_IPC_Instance != NULL && MY_IPC_Instance->PRE_IPC_Init(IPCSize) != 0) {
		delete MY_IPC_Instance;
		MY_IPC_Instance = NULL;
	}

	return MY_IPC_Instance;
}
INT32 MY_IPC::PRE_IPC_Init(unsigned int IPCSize)
{
	if(this->MYIPC_fd >= 0){
		close(this->MYIPC_fd);
		this->MYIPC_fd = -1;
	}

	this->MYIPC_fd = open(MYIPC_IF_PATH,O_RDWR);
	if(this->MYIPC_fd < 0){
#if MY_IPC_ERROR_VERBOSE
		fprintf(stderr,"Open file %s failly!!\n",MYIPC_IF_PATH);
#endif
		this->error_code = ENOENT; //No such file or directory
		sprintf(this->error_str_buf,"Open file %s failly!!",MYIPC_IF_PATH);
		this->error_str = this->error_str_buf;
		return -1;
	}

	union MY_IPC_IOCTL_ArgList arglist;
	arglist.MY_IPC_INIT_Arg.TAG_Pool_Total_Size = IPCSize;

	int r = ioctl(this->MYIPC_fd,MY_IPC_INIT,&arglist);
#if MY_IPC_ERROR_VERBOSE
	fprintf(stderr,"r=%d errno=%s(%d)\n",r,strerror(errno),errno);
#endif
	//don't care the error

	this->error_code = 0;
	this->error_str = NULL;
	return 0;
}
INT32 MY_IPC::IPC_Init()
{
	if(this->MYIPC_fd >= 0){
		close(this->MYIPC_fd);
		this->MYIPC_fd = -1;
	}

	this->MYIPC_fd = open(MYIPC_IF_PATH,O_RDWR);
	if(this->MYIPC_fd < 0){
#if MY_IPC_ERROR_VERBOSE
		fprintf(stderr,"Open file %s failly!!\n",MYIPC_IF_PATH);
#endif
		this->error_code = ENOENT; //No such file or directory
		sprintf(this->error_str_buf,"Open file %s failly!!",MYIPC_IF_PATH);
		this->error_str = this->error_str_buf;
		return -1;
	}

	this->error_code = 0;
	this->error_str = NULL;
	return 0;
}
INT32 MY_IPC::IPC_Create_TAG_String(const char *TagName, const char *DefaultValue,UINT32 DefaultSize)
{
	if(this->MYIPC_fd < 0){
#if MY_IPC_ERROR_VERBOSE
		fprintf(stderr,"MYIPC not initialized correctly!!\n");
#endif
		this->error_code = EBADF; //Bad file descriptor
		sprintf(this->error_str_buf,"MYIPC not initialized correctly!!");
		this->error_str = this->error_str_buf;
		return -1;
	}

	union MY_IPC_IOCTL_ArgList arglist;
	strcpy(arglist.MY_IPC_TAG_CREATE_Arg.TAGName,TagName);
	if(DefaultSize == 0)
		arglist.MY_IPC_TAG_CREATE_Arg.DataLen = strlen(DefaultValue) + 8;
	else
		arglist.MY_IPC_TAG_CREATE_Arg.DataLen = DefaultSize + 8;
	arglist.MY_IPC_TAG_CREATE_Arg.DataInit = (char *)DefaultValue;

	int r = ioctl(this->MYIPC_fd,MY_IPC_TAG_CREATE,&arglist);
#if MY_IPC_ERROR_VERBOSE
	fprintf(stderr,"r=%d errno=%s(%d)\n",r,strerror(errno),errno);
#endif
	if(r){
		this->error_code = errno;
		sprintf(this->error_str_buf,"%s(%d)",strerror(this->error_code),this->error_code);
		this->error_str = this->error_str_buf;
		return -(this->error_code);
	}

	this->error_code = 0;
	this->error_str = NULL;
	return 0;
}
INT32 MY_IPC::IPC_Get_TAG_String(const char *TagName, char *Value,UINT32 ValueSize)
{
	if(this->MYIPC_fd < 0){
#if MY_IPC_ERROR_VERBOSE
		fprintf(stderr,"MYIPC not initialized correctly!!\n");
#endif
		this->error_code = EBADF; //Bad file descriptor
		sprintf(this->error_str_buf,"MYIPC not initialized correctly!!");
		this->error_str = this->error_str_buf;
		return -1;
	}

	int r;
	union MY_IPC_IOCTL_ArgList arglist;

	//memset(Value,0,ValueSize);

	strcpy(arglist.MY_IPC_TAG_READ_Arg.TAGName,TagName);
	arglist.MY_IPC_TAG_READ_Arg.DataLen = ValueSize;
	arglist.MY_IPC_TAG_READ_Arg.Data = Value;

	r = ioctl(this->MYIPC_fd,MY_IPC_TAG_READ,&arglist);
#if MY_IPC_ERROR_VERBOSE
	fprintf(stderr,"r=%d errno=%s(%d)\n",r,strerror(errno),errno);
#endif
	if(r){
		this->error_code = errno;
		sprintf(this->error_str_buf,"%s(%d)",strerror(this->error_code),this->error_code);
		this->error_str = this->error_str_buf;
		return -(this->error_code);
	}

	this->error_code = 0;
	this->error_str = NULL;
	return 0;
}
INT32 MY_IPC::IPC_Get_TAG_String(const char *TagName, char *Value,UINT32 ValueSize,UINT32 TimeOut)
{
	if(this->MYIPC_fd < 0){
#if MY_IPC_ERROR_VERBOSE
		fprintf(stderr,"MYIPC not initialized correctly!!\n");
#endif
		this->error_code = EBADF; //Bad file descriptor
		sprintf(this->error_str_buf,"MYIPC not initialized correctly!!");
		this->error_str = this->error_str_buf;
		return -1;
	}

	int r;
	union MY_IPC_IOCTL_ArgList arglist;

	//memset(Value,0,ValueSize);

	strcpy(arglist.MY_IPC_TAG_WAIT_Arg.TAGName,TagName);
	arglist.MY_IPC_TAG_WAIT_Arg.wait_time_ms = TimeOut;
	arglist.MY_IPC_TAG_WAIT_Arg.DataLen = ValueSize;
	arglist.MY_IPC_TAG_WAIT_Arg.Data = Value;

	r = ioctl(this->MYIPC_fd,MY_IPC_TAG_WAIT,&arglist);
#if MY_IPC_ERROR_VERBOSE
	fprintf(stderr,"r=%d errno=%s(%d)\n",r,strerror(errno),errno);
#endif
	if(r){
		this->error_code = errno;
		sprintf(this->error_str_buf,"%s(%d)",strerror(this->error_code),this->error_code);
		this->error_str = this->error_str_buf;
		return -(this->error_code);
	}

	this->error_code = 0;
	this->error_str = NULL;
	return 0;
}
INT32 MY_IPC::IPC_Put_TAG_String(const char *TagName, const char *Value)
{
	if(this->MYIPC_fd < 0){
#if MY_IPC_ERROR_VERBOSE
		fprintf(stderr,"MYIPC not initialized correctly!!\n");
#endif
		this->error_code = EBADF; //Bad file descriptor
		sprintf(this->error_str_buf,"MYIPC not initialized correctly!!");
		this->error_str = this->error_str_buf;
		return -1;
	}

	union MY_IPC_IOCTL_ArgList arglist;
	strcpy(arglist.MY_IPC_TAG_WRITE_Arg.TAGName,TagName);
	arglist.MY_IPC_TAG_WRITE_Arg.DataLen = strlen(Value)+1;
	arglist.MY_IPC_TAG_WRITE_Arg.Data = (char *)Value;

	int r = ioctl(this->MYIPC_fd,MY_IPC_TAG_WRITE,&arglist);
#if MY_IPC_ERROR_VERBOSE
	fprintf(stderr,"r=%d errno=%s(%d)\n",r,strerror(errno),errno);
#endif
	if(r){
		this->error_code = errno;
		sprintf(this->error_str_buf,"%s(%d)",strerror(this->error_code),this->error_code);
		this->error_str = this->error_str_buf;
		return -(this->error_code);
	}

	this->error_code = 0;
	this->error_str = NULL;
	return 0;
}
INT32 MY_IPC::IPC_Get_TAG_INT32(const char *TagName)
{
	if(this->MYIPC_fd < 0){
#if MY_IPC_ERROR_VERBOSE
		fprintf(stderr,"MYIPC not initialized correctly!!\n");
#endif
		this->error_code = EBADF; //Bad file descriptor
		sprintf(this->error_str_buf,"MYIPC not initialized correctly!!");
		this->error_str = this->error_str_buf;
		return -1;
	}

	int r;
	char buf[128];
	union MY_IPC_IOCTL_ArgList arglist;

	//memset(buf,0,sizeof(buf));

	strcpy(arglist.MY_IPC_TAG_READ_Arg.TAGName,TagName);
	arglist.MY_IPC_TAG_READ_Arg.DataLen = sizeof(buf);
	arglist.MY_IPC_TAG_READ_Arg.Data = buf;

	r = ioctl(this->MYIPC_fd,MY_IPC_TAG_READ,&arglist);
#if MY_IPC_ERROR_VERBOSE
	fprintf(stderr,"r=%d errno=%s(%d)\n",r,strerror(errno),errno);
#endif
	if(r){
		this->error_code = errno;
		sprintf(this->error_str_buf,"%s(%d)",strerror(this->error_code),this->error_code);
		this->error_str = this->error_str_buf;
		return 0;
	}

	this->error_code = 0;
	this->error_str = NULL;
	return atoi(buf);
}
INT32 MY_IPC::IPC_Get_TAG_INT32(const char *TagName,UINT32 TimeOut)
{
	if(this->MYIPC_fd < 0){
#if MY_IPC_ERROR_VERBOSE
		fprintf(stderr,"MYIPC not initialized correctly!!\n");
#endif
		this->error_code = EBADF; //Bad file descriptor
		sprintf(this->error_str_buf,"MYIPC not initialized correctly!!");
		this->error_str = this->error_str_buf;
		return -1;
	}

	int r;
	char buf[128];
	union MY_IPC_IOCTL_ArgList arglist;

	//memset(buf,0,sizeof(buf));

	strcpy(arglist.MY_IPC_TAG_WAIT_Arg.TAGName,TagName);
	arglist.MY_IPC_TAG_WAIT_Arg.wait_time_ms = TimeOut;
	arglist.MY_IPC_TAG_WAIT_Arg.DataLen = sizeof(buf);
	arglist.MY_IPC_TAG_WAIT_Arg.Data = buf;

	r = ioctl(this->MYIPC_fd,MY_IPC_TAG_WAIT,&arglist);
#if MY_IPC_ERROR_VERBOSE
	fprintf(stderr,"r=%d errno=%s(%d)\n",r,strerror(errno),errno);
#endif
	if(r){
		this->error_code = errno;
		sprintf(this->error_str_buf,"%s(%d)",strerror(this->error_code),this->error_code);
		this->error_str = this->error_str_buf;
		return 0;
	}

	this->error_code = 0;
	this->error_str = NULL;
	return atoi(buf);
}
INT32 MY_IPC::IPC_Put_TAG_INT32(const char *TagName, INT32 Value)
{
	if(this->MYIPC_fd < 0){
#if MY_IPC_ERROR_VERBOSE
		fprintf(stderr,"MYIPC not initialized correctly!!\n");
#endif
		this->error_code = EBADF; //Bad file descriptor
		sprintf(this->error_str_buf,"MYIPC not initialized correctly!!");
		this->error_str = this->error_str_buf;
		return -1;
	}

	char buf[128];
	union MY_IPC_IOCTL_ArgList arglist;

	sprintf(buf,"%d",Value);
	strcpy(arglist.MY_IPC_TAG_WRITE_Arg.TAGName,TagName);
	arglist.MY_IPC_TAG_WRITE_Arg.DataLen = strlen(buf)+1;
	arglist.MY_IPC_TAG_WRITE_Arg.Data = (char *)buf;

	int r = ioctl(this->MYIPC_fd,MY_IPC_TAG_WRITE,&arglist);
#if MY_IPC_ERROR_VERBOSE
	fprintf(stderr,"r=%d errno=%s(%d)\n",r,strerror(errno),errno);
#endif
	if(r){
		this->error_code = errno;
		sprintf(this->error_str_buf,"%s(%d)",strerror(this->error_code),this->error_code);
		this->error_str = this->error_str_buf;
		return 0;
	}

	this->error_code = 0;
	this->error_str = NULL;
	return 0;
}
INT32 MY_IPC::IPC_Stall(UINT32 ms)
{
	if(this->MYIPC_fd < 0){
#if MY_IPC_ERROR_VERBOSE
		fprintf(stderr,"MYIPC not initialized correctly!!\n");
#endif
		this->error_code = EBADF; //Bad file descriptor
		sprintf(this->error_str_buf,"MYIPC not initialized correctly!!");
		this->error_str = this->error_str_buf;
		return -1;
	}

	union MY_IPC_IOCTL_ArgList arglist;

	arglist.MY_IPC_STALL_Arg.wait_time_ms = ms;

	int r = ioctl(this->MYIPC_fd,MY_IPC_STALL,&arglist);
#if MY_IPC_ERROR_VERBOSE
	fprintf(stderr,"r=%d errno=%s(%d)\n",r,strerror(errno),errno);
#endif
	if(r){
		this->error_code = errno;
		sprintf(this->error_str_buf,"%s(%d)",strerror(this->error_code),this->error_code);
		this->error_str = this->error_str_buf;
		return -(this->error_code);
	}

	this->error_code = 0;
	this->error_str = NULL;
	return 0;
}
int MY_IPC::ErrorCode()
{
	return this->error_code;
}
const char *MY_IPC::GetError()
{
	return this->error_str;
}

