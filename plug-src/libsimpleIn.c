
#include "plugin.h"
#include <stdlib.h>
#include <stdio.h>
#include "raylib.h"



typedef struct {
	byte val;
	char bitNames[8][16];
} simpleInVars;



G_MODULE_EXPORT void initialise(void* inst) //, Vector2 at, Font fnt) 
{
	
  plugInstStruct* pl = (plugInstStruct*)inst;
  pl->data = malloc(sizeof(simpleInVars));
  simpleInVars* vars = ((simpleInVars*)pl->data);
  pl->size = (Vector2){256,64};
  pl->outTx = LoadRenderTexture(pl->size.x, pl->size.y);
  SetTextureFilter(pl->outTx.texture, FILTER_BILINEAR); 
  if (pl->plug->resTx.id==0) pl->plug->resTx = LoadTexture("resources/switch.png");
  SetTextureFilter(pl->plug->resTx, FILTER_BILINEAR);  // Texture scale filter to use

	vars->val = 0x00;
  for(int i=0; i<8; i++) {
    vars->bitNames[i][0]=0;
  }
}

G_MODULE_EXPORT void setProperty(void* inst, char* prop, void* value)
{
  plugInstStruct* pl = (plugInstStruct*)inst;
  simpleInVars* vars = ((simpleInVars*)pl->data);
  if (prop[0]=='d' || prop[0]=='D') {
    int i = (prop[1] - '0') & 7;
    char* v = (char*)value;
    snprintf(&vars->bitNames[i][0], 15, "%s", v);
    return;
  }
}

// This function can access the UI
G_MODULE_EXPORT void draw(void* inst) 
{
	// rendering takes place on plugin instances render texture.
  plugInstStruct* pl = (plugInstStruct*)inst;
  simpleInVars* vars = ((simpleInVars*)pl->data);
  BeginTextureMode(pl->outTx);
  ClearBackground(BLANK);
  for (int i=0; i<8; i++) {
    int b = 1 << (7-i);
    if (b & vars->val) {
      DrawTextureRec(pl->plug->resTx, (Rectangle){32, 0, 32, 64 },(Vector2){i*32, 0}, WHITE);
    } else {
      DrawTextureRec(pl->plug->resTx, (Rectangle){0, 0, 32, 64 },(Vector2){i*32, 0}, WHITE);
    }
  }
  for (int i=0; i<8; i++) {
    DrawTextEx(pl->plug->font, FormatText("%s",vars->bitNames[7-i]), (Vector2){ i*32+2, 28 }, 22, 2, WHITE);
  }
  DrawTextEx(pl->plug->font, FormatText("%02X ",vars->val), (Vector2){ 120, 12 }, 20, 2, WHITE);
  EndTextureMode();
}


// The following functions cannot access the UI
G_MODULE_EXPORT void clicked(void* inst, int x, int y) 
{
  plugInstStruct* pl = (plugInstStruct*)inst;
  simpleInVars* vars = ((simpleInVars*)pl->data);
	// check if a button area is hit & update val
	Vector2 pos = pl->pos;
	Vector2 sz = pl->size;
	if (x > pos.x && x < pos.x + sz.x && y > pos.y && y < pos.y + sz.y)
	{
		int p = x- pos.x;
		p = 7-(p >> 5);
		vars->val = vars->val ^ (1 << p);  // xor to toggle
	}	
}

// TODO put in plugInstStruct set in initialise
G_MODULE_EXPORT int getAddressSize() { return 1; }

// is is only called with a valid address for the device so
// we don't need to check it (only 1 byte)
G_MODULE_EXPORT byte getAddress(void* inst, int address) 
{
	plugInstStruct* pl = (plugInstStruct*)inst;
    simpleInVars* vars = ((simpleInVars*)pl->data); 
	return vars->val;
}

G_MODULE_EXPORT void setAddress(void* inst, int address, byte data) {  }
