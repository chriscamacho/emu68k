
#include "plugin.h"
#include <stdlib.h>
#include <stdio.h>
#include "raylib.h"
#include "raygui.h"
#include <math.h>

/*
 *   memView plugin
 * 
 *    shows contents of memory
 * 
 *    TODO add address input and selector for Ax and Dx offset
 * 
 */

typedef struct {

  char** log;
  
} memViewVars;


// set up
G_MODULE_EXPORT void initialise(void* inst)
{
  plugInstStruct* pl = (plugInstStruct*)inst;
  pl->data = malloc(sizeof(memViewVars));
  //memViewVars* vars = ((memViewVars*)pl->data);
  
  // does np2 really matter this day and age?
  pl->size = (Vector2){512,360};    // size is always the same for all instances
  pl->outTx = LoadRenderTexture(pl->size.x, pl->size.y);  // plugin should only draw on this
  
  SetTextureFilter(pl->outTx.texture, FILTER_BILINEAR); 

  // clear out / initialize anything in vars like strings etc

}


G_MODULE_EXPORT void setProperty(void* inst, char* prop, void* value) 
{

}



static int dropDown001EditMode=0;
static int active = 0;     
     
// This function can access the UI
G_MODULE_EXPORT void draw(void* inst) 
{
	// rendering takes place on plugin instances render texture.
  plugInstStruct* pl = (plugInstStruct*)inst;
  //memViewVars* vars = ((memViewVars*)pl->data);
  SetMouseOffset(-pl->pos.x, -pl->pos.y);

  BeginTextureMode(pl->outTx);
    ClearBackground((Color){48,96,48,128});

      GuiLabel((Rectangle){ 2, 4, 254, 22}, "hello");
     

#define DregStr " -- ; D0 ; D1 ; D2 ; D3 ; D4 ; D5 ; D6 ; D7 "
      
      if (GuiDropdownBox((Rectangle){ 25, 65, 125, 30 }, DregStr,
                                  &active, dropDown001EditMode)) {
                                      dropDown001EditMode=!dropDown001EditMode;
                                  }
                                    

  EndTextureMode();
}


// The following functions cannot access the UI
G_MODULE_EXPORT void clicked(void* inst, int x, int y) { }

// TODO put in plugInstStruct set in initialise
G_MODULE_EXPORT int getAddressSize() { return 0; }  // signals not memory mapped

G_MODULE_EXPORT byte getAddress(void* inst, int address) { return 0xff; }

G_MODULE_EXPORT void setAddress(void* inst, int address, byte data) { }
