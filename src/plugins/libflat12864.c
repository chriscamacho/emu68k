
#include "plugin.h"
#include <stdlib.h>
#include <stdio.h>
#include "raylib.h"

/*
 *   a flat memory mapped 128x64x1 display
 * 
 *   in reality devices like this are likely to be laid out
 *   in pages of columns, equally this plugin has no timing
 *   constraints
 * 
 *   once plugins can access a bit more about the system
 *   like emulation step count etc then I plan on adding
 *   a more realistic st7920 (think RAMPS LCD)
 */


typedef struct {
	Image screenImage;
	Texture screenTexture;
} flat12864Vars;


// set up
G_MODULE_EXPORT void initialise(void* inst)
{
  plugInstStruct* pl = (plugInstStruct*)inst;
  pl->data = malloc(sizeof(flat12864Vars));
  flat12864Vars* vars = ((flat12864Vars*)pl->data);
  
  pl->size = (Vector2){512,256};    // size is always the same for all instances
  pl->outTx = LoadRenderTexture(pl->size.x, pl->size.y);  // plugin should only draw on this
  if (pl->plug->resTx.id==0) pl->plug->resTx = LoadTexture("resources/lcd-grid.png");  // single resource
 
  SetTextureFilter(pl->outTx.texture, FILTER_BILINEAR); 
  SetTextureFilter(pl->plug->resTx, FILTER_BILINEAR);  // Texture scale filter to use

  // clear out / initialize anything in vars like strings etc
  vars->screenImage = GenImageColor(128, 64, WHITE);
  ImageFormat(&vars->screenImage, UNCOMPRESSED_GRAYSCALE);
  
  vars->screenTexture = LoadTextureFromImage(vars->screenImage);
  
  // raw data is at vars->screenImage->data
}


G_MODULE_EXPORT void setProperty(void* inst, char* prop, void* value) { }

// This function can access the UI
G_MODULE_EXPORT void draw(void* inst) 
{
	// rendering takes place on plugin instances render texture.
  plugInstStruct* pl = (plugInstStruct*)inst;
  flat12864Vars* vars = ((flat12864Vars*)pl->data);

  UpdateTexture(vars->screenTexture, vars->screenImage.data);

  BeginTextureMode(pl->outTx);
    ClearBackground(BLANK);
    DrawTextureEx(vars->screenTexture, (Vector2){0}, 0, 4, GREEN);
    DrawTexture(pl->plug->resTx, 0,0, WHITE); // draw the grid
  EndTextureMode();
}


// The following functions cannot access the UI
G_MODULE_EXPORT void clicked(void* inst, int x, int y) { }

// TODO put in plugInstStruct set in initialise
G_MODULE_EXPORT int getAddressSize() { return 1024; }

// is is only called with a valid address for the device so
// we don't need to check it (a massive 1 K byte)
G_MODULE_EXPORT byte getAddress(void* inst, int address) 
{
  // pack 8 pixels back into a byte
  
	plugInstStruct* pl = (plugInstStruct*)inst;
/*  flat12864Vars* vars = ((flat12864Vars*)pl->data);
  unsigned short a = (address-pl->addressStart) >> 3;
  byte* p = (byte*)vars->screenImage.data;
  byte t = 128;
  byte b = 0;
  
  for(int i=0;i<8;i++) {
    if (p[a++]!=0) {
      b = b + t;
      printf("1");
    } else {
      printf("0");
    }
    t=t >> 1;
  }
printf("\n");
*/
	return pl->memPtr[address];
}

G_MODULE_EXPORT void setAddress(void* inst, int address, byte data) 
{
  // each bit effects a different pixel
	plugInstStruct* pl = (plugInstStruct*)inst;
  flat12864Vars* vars = ((flat12864Vars*)pl->data); 
  byte* p = (byte*)vars->screenImage.data;
  unsigned int a = address - pl->addressStart;
	a = a << 3; // 8 px a byte

  for(int i=0; i<8; i++) {
    if (data & (1 << (7-i)) ) {
      p[a++] = 0x00;
    } else {
      p[a++] = 0xff;
    }
  }
  pl->memPtr[address] = data;
}
