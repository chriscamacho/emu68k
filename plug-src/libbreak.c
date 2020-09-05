
#include "plugin.h"
#include <stdlib.h>
#include <stdio.h>
#include "raylib.h"
#include "raygui.h"
#include "../support/Musashi/m68k.h"
#include <math.h>

/*
 *   break plugin
 * 
 *    provides a single field where by a breakpoint can be set
 *    set to zero or some other unlikely address to "disable"
 * 
 *    
 * 
 */
/*
                              // break point gadget TODO make into a plugin
                              if ( GuiTextBox((Rectangle){70,600,92,18}, breakstr, 9, breakedit) ) {
                                breakedit=!breakedit;
                                if (!breakedit) {
                                  // TODO str2hex
                                  breakp = strtoul(breakstr, NULL, 16); //  well that was easy!
                                  sprintf(breakstr,"%08X",breakp);
                                }
                              }
//                              DrawTextEx(font, "BREAK @", (Vector2){ 2, 598 }, font.baseSize, 0, WHITE);
*/   

typedef struct {
    unsigned int breakp;
    char breakstr[12];
    bool breakedit;
    char label[33];
    float fade;
} breakVars;


// set up
G_MODULE_EXPORT void initialise(void* inst)
{
  plugInstStruct* pl = (plugInstStruct*)inst;
  pl->data = malloc(sizeof(breakVars));
  breakVars* vars = ((breakVars*)pl->data);
  
  pl->size = (Vector2){128,32};    // size is always the same for all instances
  pl->outTx = LoadRenderTexture(pl->size.x, pl->size.y);  // plugin should only draw on this
  
  //dont need
  //if (pl->plug->resTx.id==0) pl->plug->resTx = LoadTexture("resources/template.png");  // single resource
 
  SetTextureFilter(pl->outTx.texture, FILTER_BILINEAR); 
  //SetTextureFilter(pl->plug->resTx, FILTER_BILINEAR);  // Texture scale filter to use

  // clear out / initialize anything in vars like strings etc
  vars->breakp = 0;
  vars->breakedit = false;
  sprintf(pl->name,"Break @");
  sprintf(vars->breakstr,"%08X",vars->breakp);
}


G_MODULE_EXPORT void setProperty(void* inst, char* prop, void* value)
{
  plugInstStruct* pl = (plugInstStruct*)inst;
  breakVars* vars = ((breakVars*)pl->data);

  // TODO  -  store any labels etc in var 
  // (input can only really be char* as that what the xml loader sends)
  (void)pl;
  (void)vars; // keep compiler happy about unused variables
}

// TODO api-ise !
void setMouseOffset(float, float);
extern unsigned int breakp;

// This function can access the UI
G_MODULE_EXPORT void draw(void* inst) 
{
	// rendering takes place on plugin instances render texture.
  plugInstStruct* pl = (plugInstStruct*)inst;
  breakVars* vars = ((breakVars*)pl->data);
  
  setMouseOffset(pl->pos.x, pl->pos.y);
  if (m68k_get_reg(NULL, M68K_REG_PC) == vars->breakp) {
    vars->fade+=0.1;
    GuiFade(0.9+(sin(vars->fade)*.1));
  }
  
  BeginTextureMode(pl->outTx);
    ClearBackground(BLANK);
    if ( GuiTextBox((Rectangle){0,0,127-32,31}, vars->breakstr, 9, vars->breakedit) ) {
      vars->breakedit=!vars->breakedit;
      if (!vars->breakedit) {
        vars->breakp = strtoul(vars->breakstr, NULL, 16);
        sprintf(vars->breakstr,"%08X",vars->breakp);
        breakp = vars->breakp;
      }
    }
  EndTextureMode();
  setMouseOffset(0, 0);
  GuiFade(1);
}


// The following functions cannot access the UI
G_MODULE_EXPORT void clicked(void* inst, int x, int y) 
{
  //plugInstStruct* pl = (plugInstStruct*)inst;
  //breakVars* vars = ((breakVars*)pl->data);

}

// TODO put in plugInstStruct set in initialise
G_MODULE_EXPORT int getAddressSize() { return 0; }  // important signals gui plugin

G_MODULE_EXPORT byte getAddress(void* inst, int address) 
{
//	plugInstStruct* pl = (plugInstStruct*)inst;
//  breakVars* vars = ((breakVars*)pl->data); 
	return 0xff;
}

G_MODULE_EXPORT void setAddress(void* inst, int address, byte data) 
{
//	plugInstStruct* pl = (plugInstStruct*)inst;
//  breakVars* vars = ((breakVars*)pl->data); 
}
