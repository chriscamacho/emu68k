
#include "plugin.h"
#include <stdlib.h>
#include <stdio.h>
#include "raylib.h"
#include "raygui.h"
#include <math.h>

/*
 *   activityView plugin
 * 
 *    shows the emulation log
 * 
 */

typedef struct {

  char** log;
  
} activityViewVars;


// set up
G_MODULE_EXPORT void initialise(void* inst)
{
  plugInstStruct* pl = (plugInstStruct*)inst;
  pl->data = malloc(sizeof(activityViewVars));

  pl->size = (Vector2){512,360};    // size is always the same for all instances
  pl->outTx = LoadRenderTexture(pl->size.x, pl->size.y);  // plugin should only draw on this
  
  SetTextureFilter(pl->outTx.texture, TEXTURE_FILTER_BILINEAR); 

  // clear out / initialize anything in vars like strings etc

}


G_MODULE_EXPORT void setProperty(void* inst, char* prop, void* value) 
{
  plugInstStruct* pl = (plugInstStruct*)inst;
  activityViewVars* vars = ((activityViewVars*)pl->data);

  if (g_ascii_strcasecmp(prop,"log")==0) vars->log = value;

}

// This function can access the UI
G_MODULE_EXPORT void draw(void* inst) 
{
    // rendering takes place on plugin instances render texture.
  plugInstStruct* pl = (plugInstStruct*)inst;
  activityViewVars* vars = ((activityViewVars*)pl->data);

  BeginTextureMode(pl->outTx);
    ClearBackground((Color){48,96,48,128});
    for(int i=0; i<16; i++) {
      GuiLabel((Rectangle){ 2, 4+i*20, 512, 22}, vars->log[i]);
    }
  EndTextureMode();
}


// The following functions cannot access the UI
G_MODULE_EXPORT void clicked(void* inst, int x, int y) { }

// TODO put in plugInstStruct set in initialise
G_MODULE_EXPORT int getAddressSize() { return 0; }  // signals not memory mapped

G_MODULE_EXPORT byte getAddress(void* inst, int address) { return 0xff; }

G_MODULE_EXPORT void setAddress(void* inst, int address, byte data) { }
