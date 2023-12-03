
#include "plugin.h"
#include <stdlib.h>
#include <stdio.h>
#include "raylib.h"
#include "raygui.h"
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
  
  // does np2 really matter this day and age?
  pl->size = (Vector2){128,32};    // size is always the same for all instances
  pl->outTx = LoadRenderTexture(pl->size.x, pl->size.y);  // plugin should only draw on this
  
  SetTextureFilter(pl->outTx.texture, ICON_FILTER_BILINEAR); 

  // clear out / initialize anything in vars like strings etc
  vars->breakp = 0xffffffff;
  vars->breakedit = false;
  sprintf(vars->breakstr,"%08X",vars->breakp);
}


G_MODULE_EXPORT void setProperty(void* inst, char* prop, void* value) { }

// This function can access the UI
G_MODULE_EXPORT void draw(void* inst) 
{
    // rendering takes place on plugin instances render texture.
  plugInstStruct* pl = (plugInstStruct*)inst;
  breakVars* vars = ((breakVars*)pl->data);
  
  SetMouseOffset(-pl->pos.x, -pl->pos.y);   // TODO caller sets this ?
  if (m68k_get_reg(NULL, M68K_REG_PC) == vars->breakp) {
    vars->fade+=0.1;
    // ?? GuiFade(0.9+(sin(vars->fade)*.1));
  }
  
  BeginTextureMode(pl->outTx);
    ClearBackground(BLANK);
    if ( GuiTextBox((Rectangle){0,0,127-32,31}, vars->breakstr, 9, vars->breakedit) ) {
      vars->breakedit=!vars->breakedit;
      if (!vars->breakedit) {
        vars->breakp = strtoul(vars->breakstr, NULL, 16);
        sprintf(vars->breakstr,"%08X",vars->breakp);
        setBreakPoint(vars->breakp);
      }
    }
  EndTextureMode();
  SetMouseOffset(0, 0); // caller set ?
  // ?? GuiFade(1);  // caller set ?
}


// The following functions cannot access the UI
G_MODULE_EXPORT void clicked(void* inst, int x, int y) { }

// TODO put in plugInstStruct set in initialise
G_MODULE_EXPORT int getAddressSize() { return 0; }  // important signals gui plugin

G_MODULE_EXPORT byte getAddress(void* inst, int address) {  return 0xff; }

G_MODULE_EXPORT void setAddress(void* inst, int address, byte data) { }
