// readtim.c - load and parse .tim file
// by Jum Hig 99
// Last updated: 11/6/1999
//

// NB: use extern TEXMAP to be able to write to TEXMAP bitmap in E3D.

// Functions:
//	LoadTim(char *filename)

/* 	Error codes (return values):
			0.	successful (OK)
			1. unrecognised extension
			2. invalid ID (<> 10h)
			3. end of file before end marker
			4. out of bounds error
			5. file open failure
            6. Clut mode unrecognised
*/

#include <stdio.h>
#include <string.h>
#include "allegro.h"
#include "e3d.h"

#define SCENEBUFSIZE	800
#define FALSE			0
#define TRUE			1
#define FGET_OCTET      uc4 = fgetc(pftim); uc3 = fgetc(pftim); uc2 = fgetc(pftim); uc1 = fgetc(pftim)
#define FPUT_OCTET      fputc(uc1, pftim); fputc(uc2, pftim); fputc(uc3, pftim); fputc(uc4, pftim)
#define OCTET2INT		uc4 + uc3 * 256 + uc2 * 65536
#define OCTET2BIGEND	bigend[0] = uc4; bigend[1] = uc3; bigend[2] = uc2; bigend[3] = uc1


RGB_Entry clut[256];
BITMAP *buffer;
int LoadTIM(char *filename);

int main(void) {
	int i;

	allegro_init();
	install_keyboard();
	set_color_depth(16);
	set_gfx_mode(GFX_AUTODETECT, 800, 600, 0, 0);

	// create render buffer
	buffer = create_bitmap(640, 480);

	fprintf(stderr, "No worries mate!\n");
    fprintf(stderr, "LoadTIM returned: %d\n", LoadTIM("font.tim"));


    while(!readkey()) ;

	exit(0);
}

// Load in a TIM image (Playstation!)
// note: LITTLE-ENDIAN !!!! NB NB NB!!!
// note: only handles 4, 8 and 16-bit non-mixed
int LoadTIM(char *filename) {

	int end, n, i, j, k, x, y;
	unsigned char uc1, uc2, uc3, uc4;
    unsigned char bigend[4];
    unsigned int flags, clutmode, bnum;
    unsigned int clut_dx, clut_dy, clut_h, clut_w;
    unsigned int image_dx, image_dy, image_h, image_w;
    unsigned int t1, t2, r, g, b;
    short pad;
    short *p_short;

	FILE *pftim;

	fprintf(stderr, "Opening TIM file %s\n", filename);
	pftim = fopen(filename, "rb");
	// check if file open failed
	if(pftim == NULL) {
		fprintf(stderr, "TIM file open failed.\n");
        return(5);
	}


	// Parse model file
	end = FALSE;

	// check ID
    FGET_OCTET;
	if(uc4 == 0x10) fprintf(stderr, "ID match for TIM\n");
    else {
    	fprintf(stderr, "TIM ID no match! Aborting load...\n");
        return(8);
	}

	// get FLAGS and get clutmode
	FGET_OCTET;
    clutmode = FALSE;
    if(uc4 == 2) clutmode = 2;				// 16-bit
    if(uc4 == 8) clutmode = 8;				// 4-bit
    if(uc4 == 9) clutmode = 9;				// 8-bit
    if(!clutmode) {
    	fprintf(stderr, "TIM CLUT mode not supported! Aborting load...\n");
        return(6);
	}
    else {
    	fprintf(stderr, "CLUT mode = %d\n", clutmode);
	}

	// get clut if 4-bit or 8-bit
    if(clutmode > 2) {
    	// get bnum
		FGET_OCTET;
        bnum = OCTET2INT;
	    fprintf(stderr, "clut_bnum = %d\n", bnum);
    	// get DX and DY
		FGET_OCTET;
        clut_dy = uc1 * 256 + uc2;
        clut_dx = uc3 * 256 + uc4;
	    fprintf(stderr, "clut_dx = %d   clut_dy = %d\n", clut_dx, clut_dy);
    	// get W and H
		FGET_OCTET;
        clut_h = uc1 * 256 + uc2;
        clut_w = uc3 * 256 + uc4;
	    fprintf(stderr, "clut_w = %d   clut_h = %d\n", clut_w, clut_h);
        // load clut data (if 8-bit or 4-bit)
        if(clutmode > 2) {
        	j = (bnum - 12) / 4;
        	for(i=0; i<j; i++) {
            	FGET_OCTET;
                t1 = uc3 * 256 + uc4;
                t2 = uc1 * 256 + uc2;
                clut[i*2].r = t1 & 31;
                clut[i*2].g = (t1 >> 5) & 31;
                clut[i*2].b = (t1 >> 10) & 31;
                clut[i*2].alpha = t1 > 15;
                clut[i*2+1].r = t2 & 31;
                clut[i*2+1].g = (t2 >> 5) & 31;
                clut[i*2+1].b = (t2 >> 10) & 31;
                clut[i*2+1].alpha = t2 > 15;

                fprintf(stderr, "clut%d   %d %d %d %d\n", i*2,
                	clut[i*2].alpha, clut[i*2].r,
                	clut[i*2].g, clut[i*2].b);
                fprintf(stderr, "clut%d   %d %d %d %d\n", i*2+1,
                	clut[i*2+1].alpha, clut[i*2+1].r,
                	clut[i*2+1].g, clut[i*2+1].b);
			}
		}
	    // convert & display palette
    	for(i=0; i<256; i++) {
        	clut[i].r *= 8;
        	clut[i].g *= 8;
        	clut[i].b *= 8;
    		rectfill(screen, i*8, 400, i*8+7, 415, makecol(clut[i].r,
        		clut[i].g, clut[i].b));
		}

	}

	// load image data
    // get bnum
	FGET_OCTET;
    bnum = OCTET2INT;
	fprintf(stderr, "image_bnum = %d\n", bnum);
   	// get DX and DY
	FGET_OCTET;
    image_dy = uc1 * 256 + uc2;
    image_dx = uc3 * 256 + uc4;
	fprintf(stderr, "image_dx = %d   image_dy = %d\n", image_dx, image_dy);
   	// get W and H
	FGET_OCTET;
    image_h = uc1 * 256 + uc2;
    image_w = uc3 * 256 + uc4;
	fprintf(stderr, "image_w = %d   image_h = %d\n", image_w, image_h);

    // load 4-bit image data
    if(clutmode == 8) {
       	j = (bnum - 12) / 4;
        x = 0; y = 0;
       	for(i=0; i<j; i++) {
        	for(k=0; k<4; k++) {
           		uc1 = fgetc(pftim);
            	t1 = (uc1 >> 4) & 15;
            	t2 = uc1 & 15;
            	putpixel(screen, x, y, makecol(clut[t1].r, clut[t1].g, clut[t1].b));
            	putpixel(screen, x+1, y, makecol(clut[t2].r, clut[t2].g, clut[t2].b));
            	x += 2;
            	if(x == (image_w * 4)) { x = 0; y++; }
			}
		}
    }
    // load 8-bit image data
    if(clutmode == 9) {
       	j = (bnum - 12) / 4;
        x = 0; y = 0;
       	for(i=0; i<j; i++) {
        	for(k=0; k<4; k++) {
           		uc1 = fgetc(pftim);
            	t1 = uc1;
            	putpixel(screen, x, y, makecol(clut[t1].r, clut[t1].g, clut[t1].b));
            	x++;
            	if(x == (image_w * 2)) { x = 0; y++; }
			}
		}
    }
	// load 16-bit image data
    if(clutmode == 2) {
    	j = (bnum - 12) / 4;
        x = 0; y = 0;
        for(i=0; i<j; i++) {
            FGET_OCTET;
            t1 = uc3 * 256 + uc4;
            t2 = uc1 * 256 + uc2;
            r = t1 & 31;
            g = (t1 >> 5) & 31;
            b = (t1 >> 10) & 31;
            putpixel(screen, x, y, makecol(r*8, g*8, b*8));
            r = t2 & 31;
            g = (t2 >> 5) & 31;
            b = (t2 >> 10) & 31;
            putpixel(screen, x+1, y, makecol(r*8, g*8, b*8));
            x += 2;
            if(x == image_w) { x=0; y++; }
		}
	}
	return(0);		// success!
}


