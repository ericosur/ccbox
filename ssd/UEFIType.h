#ifndef _UEFI_DATA_TYPE_H
#define _UEFI_DATA_TYPE_H

typedef signed char INT8;
typedef signed short INT16;
typedef signed int INT32;
typedef signed long INT64;
typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned int UINT32;
typedef unsigned long UINT64;
typedef UINT8 BOOL;

typedef UINT64 EFI_STATUS;

#define EFI_SUCCESS 0x00000000L
#define EFI_LOAD_ERROR                 0x000000001L
#define EFI_INVALID_PARAMETER          0x000000002L
#define EFI_UNSUPPORTED                0x000000003L
#define EFI_BAD_BUFFER_SIZE            0x000000004L
#define EFI_BUFFER_TOO_SMALL           0x000000005L
#define EFI_NOT_READY                  0x000000006L
#define EFI_DEVICE_ERROR               0x000000007L
#define EFI_WRITE_PROTECTED            0x000000008L
#define EFI_OUT_OF_RESOURCES           0x000000009L
#define EFI_VOLUME_CORRUPTED           0x00000000aL
#define EFI_VOLUME_FULL                0x00000000bL
#define EFI_NO_MEDIA                   0x00000000cL
#define EFI_MEDIA_CHANGED              0x00000000dL
#define EFI_NOT_FOUND                  0x00000000eL
#define EFI_ACCESS_DENIED              0x00000000fL
#define EFI_NO_RESPONSE                0x000000010L
#define EFI_NO_MAPPING                 0x000000011L
#define EFI_TIMEOUT                    0x000000012L
#define EFI_NOT_STARTED                0x000000013L
#define EFI_ALREADY_STARTED            0x000000014L
#define EFI_ABORTED                    0x000000015L
#define EFI_ICMP_ERROR                 0x000000016L
#define EFI_TFTP_ERROR                 0x000000017L
#define EFI_PROTOCOL_ERROR             0x000000018L
#define EFI_INCOMPATIBLE_VERSION       0x000000019L
#define EFI_SECURITY_VIOLATION         0x00000001aL
#define EFI_CRC_ERROR                  0x00000001bL
#define EFI_END_OF_MEDIA	       0x00000001cL
#define EFI_END_OF_FILE	               0x00000001fL
#define EFI_INVALID_LANGUAGE           0x000000020L

#define EFI_ERROR(s) (s != EFI_SUCCESS)

#define STATIC static
#define VOID void
#define IN
#define OUT
#define EFIAPI

#define TRUE 1
#define FALSE 0

#endif
