
#include "plugin.h"
#include <stdlib.h>
#include <stdio.h>
#include "raylib.h"
#include "raygui.h"
#include <math.h>

/*
 *   RunStepSkip plugin
 * 
 *    provides buttons allowing the user to control the emulation
 * 
 *    
 * 
 */
  

typedef struct {
    bool running;
} rssVars;


// set up
G_MODULE_EXPORT void initialise(void* inst)
{
  plugInstStruct* pl = (plugInstStruct*)inst;
  pl->data = malloc(sizeof(rssVars));
  rssVars* vars = ((rssVars*)pl->data);
  
  pl->size = (Vector2){256,32};    // size is always the same for all instances
  pl->outTx = LoadRenderTexture(pl->size.x, pl->size.y);  // plugin should only draw on this
  
  SetTextureFilter(pl->outTx.texture, FILTER_BILINEAR); 

  // clear out / initialize anything in vars like strings etc
//  vars->running = false;
}


G_MODULE_EXPORT void setProperty(void* inst, char* prop, void* value) { }

// This function can access the UI
G_MODULE_EXPORT void draw(void* inst) 
{
	// rendering takes place on plugin instances render texture.
  plugInstStruct* pl = (plugInstStruct*)inst;
  rssVars* vars = ((rssVars*)pl->data);
  
  SetMouseOffset(-pl->pos.x, -pl->pos.y);   // TODO caller sets this ?
  
  BeginTextureMode(pl->outTx);
    ClearBackground(BLANK);
    
    // run stop button
    vars->running = isRunning(); // in case something else stops us (break plugin for example)
    bool res;
    if (vars->running) {
      res = GuiToggle((Rectangle){0,0,48,31}, "Stop", vars->running);
    } else {
      res = GuiToggle((Rectangle){0,0,48,31}, "Run", vars->running);
    }
    if (vars->running != res) {
      vars->running = res;
      setRunning(vars->running);
    }
    
    // step button
    if (GuiButton((Rectangle){52,0,48,31},"Step")) {
      doStep();
    }
    
    // skip button
    if (GuiButton((Rectangle){100,0,48,31},"Skip")) {
      doSkip();
    }
    
    
  EndTextureMode();

  SetMouseOffset(0, 0);   // TODO caller sets this ?
  
}


// The following functions cannot access the UI
G_MODULE_EXPORT void clicked(void* inst, int x, int y) { }

// TODO put in plugInstStruct set in initialise
G_MODULE_EXPORT int getAddressSize() { return 0; }  // TODO important signals not memory mapped

G_MODULE_EXPORT byte getAddress(void* inst, int address) {	return 0xff; }

G_MODULE_EXPORT void setAddress(void* inst, int address, byte data) { }
