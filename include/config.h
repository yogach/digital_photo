
#ifndef _CONFIG_H
#define _CONFIG_H

#include <stdio.h>
#include <debug_manager.h>

#define FB_DEVICE_NAME "/dev/fb0"

#define COLOR_BACKGROUND   0xE7DBB5  /* ���Ƶ�ֽ */
#define COLOR_FOREGROUND   0x514438  /* ��ɫ���� */

//#define DBG_PRINTF(...)  
//#define DBG_PRINTF DebugPrint 

#define DBG_PRINTF(fmt,...) DebugPrint("%s[%d]:"fmt,__FILE__,__LINE__,##__VA_ARGS__)

#define ICON_PATH  "/mnt/Icon/" //ͼ�걣��Ŀ¼
#define DEFAULT_PATH "/"


#endif /* _CONFIG_H */
