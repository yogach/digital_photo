#include <stdio.h>
#include <config.h>
#include <encoding_manager.h>
#include <fonts_manager.h>
#include <disp_manager.h>
#include <input_manager.h>
#include <pic_manager.h>
#include <page_manager.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>




int main (int argc,char * *argv)
{


	DebugInit ();
	InitDebugChanel ();


	DisplayInit ();
	SelectAndInitDefaultDispDev("fb");
	AllocVideoMem(5);

    InputInit();
	AllInputDevicesInit();

    PicFileParserInit();
    ShowPicFileParser();


	PagesInit();
	Page("main")->Run();



	return 0;
}



