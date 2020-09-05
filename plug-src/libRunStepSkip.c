
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
    char label[33];
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
  
  //dont need
  //if (pl->plug->resTx.id==0) pl->plug->resTx = LoadTexture("resources/template.png");  // single resource
 
  SetTextureFilter(pl->outTx.texture, FILTER_BILINEAR); 
  //SetTextureFilter(pl->plug->resTx, FILTER_BILINEAR);  // Texture scale filter to use

  // clear out / initialize anything in vars like strings etc
  vars->running = false;
  sprintf(pl->name,"Cpu Control");

}


G_MODULE_EXPORT void setProperty(void* inst, char* prop, void* value)
{
  plugInstStruct* pl = (plugInstStruct*)inst;
  rssVars* vars = ((rssVars*)pl->data);

  // TODO  -  store any labels etc in var 
  // (input can only really be char* as that what the xml loader sends)
  (void)pl;
  (void)vars; // keep compiler happy about unused variables
}

// This function can access the UI
G_MODULE_EXPORT void draw(void* inst) 
{
	// rendering takes place on plugin instances render texture.
  plugInstStruct* pl = (plugInstStruct*)inst;
  rssVars* vars = ((rssVars*)pl->data);
  
  setMouseOffset(pl->pos.x, pl->pos.y);   // TODO caller sets this ?

  
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
      vars->running = ! vars->running;
      setRunning(vars->running);
    }
    
    // step button
    if (GuiButton((Rectangle){52,0,48,31},"Step")) {
      doStep();
    }
    
    // skip button
    if (GuiButton((Rectangle){100,0,48,31},"Skip")) {
      doStep();
    }
    
    
  EndTextureMode();
  setMouseOffset(0, 0); // caller set ?

}


// The following functions cannot access the UI
G_MODULE_EXPORT void clicked(void* inst, int x, int y) 
{
  //plugInstStruct* pl = (plugInstStruct*)inst;
  //rssVars* vars = ((rssVars*)pl->data);

}

// TODO put in plugInstStruct set in initialise
G_MODULE_EXPORT int getAddressSize() { return 0; }  // important signals gui plugin

G_MODULE_EXPORT byte getAddress(void* inst, int address) 
{
//	plugInstStruct* pl = (plugInstStruct*)inst;
//  rssVars* vars = ((rssVars*)pl->data); 
	return 0xff;
}

G_MODULE_EXPORT void setAddress(void* inst, int address, byte data) 
{
//	plugInstStruct* pl = (plugInstStruct*)inst;
//  rssVars* vars = ((rssVars*)pl->data); 
}