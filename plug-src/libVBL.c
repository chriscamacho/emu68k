
#include "plugin.h"
#include <stdlib.h>
#include <stdio.h>
#include "raylib.h"

// keeps track of the frame, if the value changes its a new frame
// allows the sim to sync up with the UI
// think of it like waiting for the vertical blank...



typedef struct {
	byte val;
	char bitNames[8][16];
} VBLvars;



G_MODULE_EXPORT void initialise(void* inst) //, Vector2 at, Font fnt) 
{
	
  plugInstStruct* pl = (plugInstStruct*)inst;
  pl->data = malloc(sizeof(VBLvars));
  VBLvars* vars = ((VBLvars*)pl->data);
  pl->size = (Vector2){64,64};
  pl->outTx = LoadRenderTexture(pl->size.x, pl->size.y);
  SetTextureFilter(pl->outTx.texture, FILTER_BILINEAR); 
  if (pl->plug->resTx.id==0) pl->plug->resTx = LoadTexture("resources/template.png");
  SetTextureFilter(pl->plug->resTx, FILTER_BILINEAR);  // Texture scale filter to use

	vars->val = 0x00;
}

G_MODULE_EXPORT void setProperty(void* inst, char* prop, void* value)
{

}

// This function can access the UI
G_MODULE_EXPORT void draw(void* inst) 
{
	// rendering takes place on plugin instances render texture.
  plugInstStruct* pl = (plugInstStruct*)inst;
  VBLvars* vars = ((VBLvars*)pl->data);
  vars->val++;
  BeginTextureMode(pl->outTx);
  ClearBackground(BLANK);
    DrawTexture(pl->plug->resTx, 0,0, WHITE);
    DrawText(FormatText("%02X",vars->val), 28, 48, 20, BLACK);
  EndTextureMode();
}


// The following functions cannot access the UI
G_MODULE_EXPORT void clicked(void* inst, int x, int y) 
{

}

// TODO put in plugInstStruct set in initialise
G_MODULE_EXPORT int getAddressSize() { return 1; }

// is is only called with a valid address for the device so
// we don't need to check it (only 1 byte)
G_MODULE_EXPORT byte getAddress(void* inst, int address) 
{
	plugInstStruct* pl = (plugInstStruct*)inst;
  VBLvars* vars = ((VBLvars*)pl->data); 
	return vars->val;
}
G_MODULE_EXPORT void setAddress(void* inst, int address, byte data) {/* ignored TODO ??? WARN?*/  }
