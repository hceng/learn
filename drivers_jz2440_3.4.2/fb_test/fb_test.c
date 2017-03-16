/*****************************************
 Copyright 2001-2003	
 Sigma Designs, Inc. All Rights Reserved
 Proprietary and Confidential
 *****************************************/
 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/time.h>
#include "fb.h"

 
#define ALLOW_OS_CODE 1
/*#include "../rua/include/rua.h"*/

#if 0
#define DEB(f) (f)
#else
#define DEB(f)
#endif

typedef unsigned char RMuint8;
typedef unsigned short RMuint16;
typedef unsigned int RMuint32;

struct fb_var_screeninfo fb_var;
struct fb_fix_screeninfo fb_fix;
char * fb_base_addr = NULL;

/* Set a pixel color
 * @param p_osd osd descriptor
 * @param x
 * @param y
 * @color 0xAARRGGBB (AA = alpha : 0 transparent)
 */
static void set_pixel(RMuint32 x, RMuint32 y, RMuint32 color)
{
	/*static RMuint32 i=0;*/
	/* TODO We assume for now we have contigus regions */
	switch (fb_var.bits_per_pixel){
		case 32:
			{
				RMuint32 *addr= (RMuint32 *)fb_base_addr+(y*fb_var.xres+x);
				*addr = color;
			}
			break;
		case 24:
			{
				/*RMuint32 *addr= (RMuint32 *)fb_base_addr+(y*fb_var.xres+x);
				*addr = (0x00FFFFFF) & color;*/
				RMuint8 *addr = (RMuint8 *)fb_base_addr+(y*fb_var.xres+x)*3;
				*addr = (RMuint8) (color & 0xFF);
				*(addr+1) = (RMuint8) (color >> 8 & 0xFF);
				*(addr+2) = (RMuint8) (color >> 16 & 0xFF);
			}
			break;
		case 16:
			{
				RMuint16 *addr = (RMuint16 *) fb_base_addr+(y*fb_var.xres+x);
				*addr = (RMuint16) color;
			}
			break;
		case 8:
			{
				RMuint8 *addr = (RMuint8 *)fb_base_addr+(y*fb_var.xres+x);
				*addr = (RMuint8) color;
			}
			break;
		case 4:
			{
				RMuint8 *addr = (RMuint8 *)fb_base_addr+(y*fb_var.xres+x) / 2;
				RMuint32 bit = (RMuint32) ((RMuint8 *)fb_base_addr+(y*fb_var.xres+x) % 2);
				RMuint8 pixel = *addr;
				pixel = pixel & ( 0x0F << bit * 4 );
				color = ( color & 0x0000000F ) << 4;
				*addr = pixel | ( (RMuint8) color >> bit * 4 );
			}
			break;
		case 2:
			{
				RMuint8 *addr = (RMuint8 *)fb_base_addr+(y*fb_var.xres+x) / 4;
				RMuint32 bit = (RMuint32) ((RMuint8 *)fb_base_addr+(y*fb_var.xres+x) % 4);
				RMuint8 pixel = *addr;
				pixel = pixel & ( 0xFF ^ ( 0x3 << bit * 2));
				color = color & 0x00000003;
				*addr = pixel | ( (RMuint8) color << bit * 2 );
			}
			break;
		case 1:
			{
				/*RMuint8 *addr = (RMuint8 *)fb_base_addr+(y*fb_var.xres+x);
				*addr = (RMuint8) color;*/
				RMuint8 *addr = (RMuint8 *)fb_base_addr+(y*fb_var.xres+x) / 8;
				RMuint32 bit = (RMuint32) ((RMuint8 *)fb_base_addr+(y*fb_var.xres+x) % 8);
				RMuint8 pixel = *addr;
				pixel = pixel & ( 0xFF ^ ( 0x1 << bit ));
				color = color & 0x00000001;
				*addr = pixel | ( (RMuint8) color << bit );
			}
			break;
		default:
			fprintf(stderr,"Unknown bpp : %d\n",fb_var.bits_per_pixel);
			break;
	}
	/*if (i<10){
		DEB(fprintf(stderr,"(%ld,%ld) [%p] <- %lX\n",x,y,addr,*addr));
		i++;
	}*/
}

static void mire()
{
	RMuint32 x,y;
	RMuint32 color;
	RMuint8 red,green,blue,alpha;

	DEB(fprintf(stderr,"begin mire\n"));
	for (y=0;y<fb_var.yres;y++)
		for (x=0;x<fb_var.xres;x++){
			color = ((x-fb_var.xres/2)*(x-fb_var.xres/2) + (y-fb_var.yres/2)*(y-fb_var.yres/2))/64;
			red   = (color/8) % 256;
			green = (color/4) % 256;
			blue  = (color/2) % 256;
			alpha = (color*2) % 256;
			/*alpha = 0xFF;*/

			color |= ((RMuint32)alpha << 24);
			color |= ((RMuint32)red   << 16);
			color |= ((RMuint32)green << 8 );
			color |= ((RMuint32)blue       );

			set_pixel(x,y,color);
		}

	DEB(fprintf(stderr,"end mire\n"));
}

void printusage(char *name)
{
		fprintf(stderr,"Usage (example): %s /dev/fb/0\n", name);
		fprintf(stderr,"                 %s /dev/fb/0 set <width> <height> <bpp>\n",name);
		fprintf(stderr,"                 %s /dev/fb/0 r8 <offset> <num>\n",name);
		fprintf(stderr,"                 %s /dev/fb/0 w8 <offset> <val> <num>\n",name);
		fprintf(stderr,"                 %s /dev/fb/0 r16 <offset> <num>\n",name);
		fprintf(stderr,"                 %s /dev/fb/0 w16 <offset> <val> <num>\n",name);
		fprintf(stderr,"                 %s /dev/fb/0 r32 <offset> <num>\n",name);
		fprintf(stderr,"                 %s /dev/fb/0 w32 <offset> <val> <num>\n",name);
}

int main(int argc, char** argv)
{
	int fd = 0;
	long int screensize = 0;

	unsigned int width = 0;
	unsigned int height = 0;
	unsigned int bpp = 0;
    unsigned int *pdwAddr = 0;
    unsigned short *pwAddr = 0;
    unsigned char *pcAddr = 0;
    unsigned int dwOffset;
    unsigned int dwVal;
    unsigned int num;
    int i;

	
	if (argc < 2) {
        printusage(argv[0]);
		return -1;
	}
 
	fd = open(argv[1],O_RDWR);
	if (fd <0){
		printf("error opening %s\n",argv[1]);
		exit(1);
	}


	/* Get fixed screen information */
	if (ioctl(fd, FBIOGET_FSCREENINFO, &fb_fix)) {
		printf("Error reading fb fixed information.\n");
		exit(1);
	}

	/* Get variable screen information 	*/
	if (ioctl(fd, FBIOGET_VSCREENINFO, &fb_var)) {
		printf("Error reading fb variable information.\n");
		exit(1);
	}

    if (!strcmp(argv[1], "set"))
    {
        width = strtoul(argv[3], 0, 0);
        height = strtoul(argv[4], 0, 0);
        bpp = strtoul(argv[5], 0, 0);
        if (!width || !height || !bpp)
        {
            printusage(argv[0]);
            return 0;
        }        
        printf("Change fb as %dx%d, %dbpp\n", width, height, bpp);

        fb_var.xres = fb_var.xres_virtual = width;
        fb_var.yres = fb_var.yres_virtual = height;
        fb_var.bits_per_pixel = bpp;
        
    	if (ioctl(fd, FBIOPUT_VSCREENINFO, &fb_var)) {
    		printf("Error Set fb variable information.\n");
    		exit(1);
    	}
        
    }

    if ((argc == 2) || (argc > 2 && !strcmp(argv[2], "set")))
    {
    	printf("fb_var_screeninfo values:\n");
        printf("    xres:           %d\n", fb_var.xres);			/* visible resolution		*/
        printf("    yres:           %d\n", fb_var.yres);
    	printf("    xres_virtual:   %d\n", fb_var.xres_virtual);	/* virtual resolution		*/
    	printf("    yres_virtual:   %d\n", fb_var.yres_virtual);
    	printf("    xoffset:        %d\n", fb_var.xoffset);			/* offset from virtual to visible */
    	printf("    yoffset:        %d\n", fb_var.yoffset);	        /* resolution			*/
    	printf("    bits_per_pixel: %d\n", fb_var.bits_per_pixel);
    	printf("    grayscale:      %d\n", fb_var.grayscale);		/* != 0 Graylevels instead of colors */

    	printf("    red.offset:     %d\n", fb_var.red.offset);		/* bitfield in fb mem if true color, */
    	printf("    red.length:     %d\n", fb_var.red.length);		/* else only length is significant */
    	printf("    red.msb_right:  %d\n", fb_var.red.msb_right);	
        
    	printf("    green.offset:   %d\n", fb_var.green.offset);		/* bitfield in fb mem if true color, */
    	printf("    green.length:   %d\n", fb_var.green.length);		/* else only length is significant */
    	printf("    green.msb_right:%d\n", fb_var.green.msb_right);	
        
    	printf("    blue.offset:    %d\n", fb_var.blue.offset);		/* bitfield in fb mem if true color, */
    	printf("    blue.length:    %d\n", fb_var.blue.length);		/* else only length is significant */
    	printf("    blue.msb_right: %d\n", fb_var.blue.msb_right);	

    	printf("    transp.offset:  %d\n", fb_var.transp.offset);		/* transparency			*/	
    	printf("    transp.length:  %d\n", fb_var.transp.length);		
    	printf("    transp.msb_right:%d\n", fb_var.transp.msb_right);	

    	printf("    nonstd:         %d\n", fb_var.nonstd);			/* != 0 Non standard pixel format */

    	printf("    activate:       %d\n", fb_var.activate);			/* see FB_ACTIVATE_*		*/

    	printf("    height:         %d\n", fb_var.height);			/* height of picture in mm    */
    	printf("    width:          %d\n", fb_var.width);			/* width of picture in mm     */

    	printf("    accel_flags:    %d\n", fb_var.accel_flags);		/* acceleration flags (hints)	*/

    	/* Timing: All values in pixclocks, except pixclock (of course) */
    	printf("    pixclock:       %d\n", fb_var.pixclock);			/* pixel clock in ps (pico seconds) */
    	printf("    left_margin:    %d\n", fb_var.left_margin);		/* time from sync to picture	*/
    	printf("    right_margin:   %d\n", fb_var.right_margin);		/* time from picture to sync	*/
    	printf("    upper_margin:   %d\n", fb_var.upper_margin);		/* time from sync to picture	*/
    	printf("    lower_margin:   %d\n", fb_var.lower_margin);
    	printf("    hsync_len:      %d\n", fb_var.hsync_len);		/* length of horizontal sync	*/
    	printf("    vsync_len:      %d\n", fb_var.vsync_len);		/* length of vertical sync	*/
    	printf("    sync:           %d\n", fb_var.sync);			/* see FB_SYNC_*		*/
    	printf("    vmode:          %d\n", fb_var.vmode);			/* see FB_VMODE_*		*/

    	printf("%dx%d, %dbpp\n", fb_var.xres, fb_var.yres, fb_var.bits_per_pixel );
    }
    
	screensize = fb_var.xres * fb_var.yres * fb_var.bits_per_pixel / 8;
	
#if (EM86XX_MODE == EM86XX_MODEID_WITHHOST)
	fb_base_addr = (char *)mmap(NULL , screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
#else
	/*uclinux doesn't support MAP_SHARED or MAP_PRIVATE with PROT_WRITE, so no mmap at all is simpler*/
	fb_base_addr = (char *)fb_fix.smem_start;
#endif
	if ((int)fb_base_addr == -1) {
		printf("error mapping fb\n");
		exit(1);
	}

    if ((argc == 2) || (argc > 2 && !strcmp(argv[2], "set")))
    {
    	/*blank	*/
    	memset(fb_base_addr,0x00,screensize);

    	usleep(50000);
    	mire();
    }
    else if ((argc >= 4) && !strcmp(argv[2], "r32"))
    {
        dwOffset = strtoul(argv[3], 0, 0);
        if (argc >= 5)
            num = strtoul(argv[4], 0, 0);
        else
            num = 1;
        pdwAddr = (unsigned int *)(fb_base_addr + dwOffset);
        for (i = 0; i < num; i++)
            printf("%05d. offset : 0x%08x, value : 0x%08x\n", pdwAddr[i]);
    }
    else if ((argc >= 5) && !strcmp(argv[2], "w32"))
    {
        dwOffset = strtoul(argv[3], 0, 0);
        dwVal = strtoul(argv[4], 0, 0);
        if (argc >= 6)
            num = strtoul(argv[5], 0, 0);
        else
            num = 1;
        pdwAddr = (unsigned int *)(fb_base_addr + dwOffset);
        for (i = 0; i < num; i++)
            pdwAddr[i] = dwVal;
    }
    else if ((argc >= 4) && !strcmp(argv[2], "r16"))
    {
        dwOffset = strtoul(argv[3], 0, 0);
        if (argc >= 5)
            num = strtoul(argv[4], 0, 0);
        else
            num = 1;
        pwAddr = (unsigned short *)(fb_base_addr + dwOffset);
        for (i = 0; i < num; i++)
            printf("%05d. offset : 0x%08x, value : 0x%08x\n", pwAddr[i]);
    }
    else if ((argc >= 5) && !strcmp(argv[2], "w16"))
    {
        dwOffset = strtoul(argv[3], 0, 0);
        dwVal = strtoul(argv[4], 0, 0);
        if (argc >= 6)
            num = strtoul(argv[5], 0, 0);
        else
            num = 1;
        pwAddr = (unsigned short *)(fb_base_addr + dwOffset);
        for (i = 0; i < num; i++)
            pwAddr[i] = dwVal;
    }
    else if ((argc >= 4) && !strcmp(argv[2], "r8"))
    {
        dwOffset = strtoul(argv[3], 0, 0);
        if (argc >= 5)
            num = strtoul(argv[4], 0, 0);
        else
            num = 1;
        pcAddr = (unsigned char *)(fb_base_addr + dwOffset);
        for (i = 0; i < num; i++)
            printf("%05d. offset : 0x%08x, value : 0x%08x\n", pcAddr[i]);
    }
    else if ((argc >= 5) && !strcmp(argv[2], "w8"))
    {
        dwOffset = strtoul(argv[3], 0, 0);
        dwVal = strtoul(argv[4], 0, 0);
        if (argc >= 6)
            num = strtoul(argv[5], 0, 0);
        else
            num = 1;
        pcAddr = (unsigned char *)(fb_base_addr + dwOffset);
        for (i = 0; i < num; i++)
            pcAddr[i] = dwVal;
    }
    
#if (EM86XX_MODE == EM86XX_MODEID_WITHHOST)
	munmap(fb_base_addr, screensize);
#endif
	close(fd);
	return 0;
}

