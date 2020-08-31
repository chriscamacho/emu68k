#include "../../raylib/src/raylib.h"
#include <stdio.h>

// gcc asc2char.c ../../raylib/src/libraylib.a -o asc2char  -lm -lX11 -ldl -pthread

void main() {
	SetTraceLogLevel(LOG_ERROR);
	Image img = LoadImage("ascii4x8.png");
	ImageFormat(&img, UNCOMPRESSED_GRAYSCALE);
	Image img2 = LoadImage("ascii8x8.png");
	ImageFormat(&img2, UNCOMPRESSED_GRAYSCALE);
		
	unsigned char* p;
	int a=32;
	
	printf("\n");
	printf("font4x8:\n");
	
	for (int y=0; y<6; y++) {
		for (int x=0; x<16; x++) {
			
			int xx = x * 4;
			int yy = y * 64 * 8;
			p = ((unsigned char*)img.data) + xx + yy;
			for (int i=0; i<8; i++) {
				printf("    dc.b	%%0000");
				if (p[0]==0x00) { printf("0"); } else { printf("1"); }
				if (p[1]==0x00) { printf("0"); } else { printf("1"); }
				if (p[2]==0x00) { printf("0"); } else { printf("1"); }
				if (p[3]==0x00) { printf("0"); } else { printf("1"); }
				if (i==0 && a<127) printf("    ; '%c'  %i",a,a);
				if (i==0 && a>126) printf("    ; '?'  %i",a,a);
				printf("\n");
				p=p+64;
			}
			printf("\n");
			a++;
		}
		
	}
	printf("\n");
	printf("\n");
	printf("\nfont8x8:\n");

	a = 32;
	for (int y=0; y<6; y++) {
		for (int x=0; x<16; x++) {
			
			int xx = x * 8;
			int yy = y * 128 * 8;
			p = ((unsigned char*)img2.data) + xx + yy;
			for (int i=0; i<8; i++) {
				printf("    dc.b	%%");
				if (p[0]==0x00) { printf("0"); } else { printf("1"); }
				if (p[1]==0x00) { printf("0"); } else { printf("1"); }
				if (p[2]==0x00) { printf("0"); } else { printf("1"); }
				if (p[3]==0x00) { printf("0"); } else { printf("1"); }
				if (p[4]==0x00) { printf("0"); } else { printf("1"); }
				if (p[5]==0x00) { printf("0"); } else { printf("1"); }
				if (p[6]==0x00) { printf("0"); } else { printf("1"); }
				if (p[7]==0x00) { printf("0"); } else { printf("1"); }
				if (i==0 && a<127) printf("    ; '%c'  %i",a,a);
				if (i==0 && a>126) printf("    ; '?'  %i",a,a);
				printf("\n");
				p=p+128;
			}
			printf("\n");
			a++;
		}
		
	}
	printf("\n");	
}
