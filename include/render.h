#ifndef __RENDER__H__
#define __RENDER__H__
#include <page_manager.h>
#include <display_manager.h>

void ReleaseButton ( PT_Layout ptLayout );
void PressButton ( PT_Layout ptLayout );
int MergerStringToCenterOfRectangleInVideoMem(int iTopLeftX, int iTopLeftY, int iBotRightX, int iBotRightY, unsigned char *pucTextString, PT_VideoMem ptVideoMem);


#endif