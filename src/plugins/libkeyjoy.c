
#include "plugin.h"
#include <stdlib.h>
#include <stdio.h>
#include "raylib.h"

// TODO allow changing of keycodes via setProperty

typedef struct {
    byte val;
} keyJoyVars;



G_MODULE_EXPORT void initialise(void* inst)
{
    
  plugInstStruct* pl = (plugInstStruct*)inst;
  pl->data = malloc(sizeof(keyJoyVars));
  keyJoyVars* vars = ((keyJoyVars*)pl->data);
  pl->size = (Vector2){64,64};
  pl->outTx = LoadRenderTexture(pl->size.x, pl->size.y);
  SetTextureFilter(pl->outTx.texture, TEXTURE_FILTER_BILINEAR); 
  if (pl->plug->resTx.id==0) pl->plug->resTx = LoadTexture("resources/template.png");
  SetTextureFilter(pl->plug->resTx, TEXTURE_FILTER_BILINEAR);  // Texture scale filter to use

    vars->val = 0x00;

}

G_MODULE_EXPORT void setProperty(void* inst, char* prop, void* value) { }

// This function can access the UI
G_MODULE_EXPORT void draw(void* inst) 
{
    // rendering takes place on plugin instances render texture.
  plugInstStruct* pl = (plugInstStruct*)inst;
  keyJoyVars* vars = ((keyJoyVars*)pl->data);
  
  vars->val=0;
  if (IsKeyDown(KEY_UP)) vars->val = vars->val | 128;
  if (IsKeyDown(KEY_DOWN)) vars->val = vars->val | 64;
  if (IsKeyDown(KEY_LEFT)) vars->val = vars->val | 32;
  if (IsKeyDown(KEY_RIGHT)) vars->val = vars->val | 16;
  if (IsKeyDown(KEY_U)) vars->val = vars->val | 8;
  if (IsKeyDown(KEY_I)) vars->val = vars->val | 4;
  if (IsKeyDown(KEY_O)) vars->val = vars->val | 2;
  if (IsKeyDown(KEY_P)) vars->val = vars->val | 1;
  
  
  BeginTextureMode(pl->outTx);
  ClearBackground(BLANK);
  DrawTexture(pl->plug->resTx, 0,0, WHITE);

  DrawTextEx(pl->plug->font, TextFormat("%02X ",vars->val), (Vector2){ 28, 48 }, 20, 2, BLACK);
  EndTextureMode();
}


// The following functions cannot access the UI
G_MODULE_EXPORT void clicked(void* inst, int x, int y) { }

// TODO put in plugInstStruct set in initialise
G_MODULE_EXPORT int getAddressSize() { return 1; }

// is is only called with a valid address for the device so
// we don't need to check it (only 1 byte)
G_MODULE_EXPORT byte getAddress(void* inst, int address) 
{
    plugInstStruct* pl = (plugInstStruct*)inst;
  keyJoyVars* vars = ((keyJoyVars*)pl->data); 
    return vars->val;
}

G_MODULE_EXPORT void setAddress(void* inst, int address, byte data) {  }
