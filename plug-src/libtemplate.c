
#include "plugin.h"
#include <stdlib.h>
#include <stdio.h>
#include "raylib.h"

/*
 *   template plugin
 * 
 * this template does little else than allow a single byte to
 * be read, written and displayed
 * 
 * 
 */


typedef struct {
	byte val;				// to save the effects of writes
} templateVars;


// set up
G_MODULE_EXPORT void initialise(void* inst)
{
  plugInstStruct* pl = (plugInstStruct*)inst;
  pl->data = malloc(sizeof(templateVars));
  templateVars* vars = ((templateVars*)pl->data);
  
  pl->size = (Vector2){64,64};    // size is always the same for all instances
  pl->outTx = LoadRenderTexture(pl->size.x, pl->size.y);  // plugin should only draw on this
  if (pl->plug->resTx.id==0) pl->plug->resTx = LoadTexture("resources/template.png");  // single resource
 
  SetTextureFilter(pl->outTx.texture, FILTER_BILINEAR); 
  SetTextureFilter(pl->plug->resTx, FILTER_BILINEAR);  // Texture scale filter to use

  // clear out / initialize anything in vars like strings etc
  vars->val = 0;
}


G_MODULE_EXPORT void setProperty(void* inst, char* prop, void* value)
{
  plugInstStruct* pl = (plugInstStruct*)inst;
  templateVars* vars = ((templateVars*)pl->data);

  // TODO  -  store any labels etc in var 
  // (input can only really be char* as that what the xml loader sends)
  // see libsimpleOut.c where each data bit d0-d7 has its own label property
  (void)pl;
  (void)vars; // keep compiler happy about unused variables
}

// This function can access the UI
G_MODULE_EXPORT void draw(void* inst) 
{
	// rendering takes place on plugin instances render texture.
  plugInstStruct* pl = (plugInstStruct*)inst;
  templateVars* vars = ((templateVars*)pl->data);

  BeginTextureMode(pl->outTx);
    ClearBackground(BLANK);
    DrawTexture(pl->plug->resTx, 0,0, WHITE);
    DrawText(FormatText("%02X",vars->val), 28, 48, 20, BLACK);
  EndTextureMode();
}


// The following functions cannot access the UI
G_MODULE_EXPORT void clicked(void* inst, int x, int y) 
{
  //plugInstStruct* pl = (plugInstStruct*)inst;
  //templateVars* vars = ((templateVars*)pl->data);

}

// TODO put in plugInstStruct set in initialise
// TODO return zero if the plugin isn't memory mapped
G_MODULE_EXPORT int getAddressSize() { return 1; }

// is is only called with a valid address for the device so
// we don't need to check range check it
G_MODULE_EXPORT byte getAddress(void* inst, int address) 
{
	plugInstStruct* pl = (plugInstStruct*)inst;
  templateVars* vars = ((templateVars*)pl->data); 
	return vars->val;
}

G_MODULE_EXPORT void setAddress(void* inst, int address, byte data) 
{
	plugInstStruct* pl = (plugInstStruct*)inst;
  templateVars* vars = ((templateVars*)pl->data); 
	vars->val = data;
}
