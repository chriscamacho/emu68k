
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

  bool          ddDdone;    // D reg dropdown
  int           ddDindex;
   
  bool          ddAdone;    // A reg dropdown
  int           ddAindex;
  
  unsigned int  offsetN;    // numerical offset
  bool          offsetNedit;
  char          offsetNstr[10];
  
  
} memViewVars;


// set up
G_MODULE_EXPORT void initialise(void* inst)
{
  plugInstStruct* pl = (plugInstStruct*)inst;
  pl->data = malloc(sizeof(memViewVars));
  memViewVars* vars = ((memViewVars*)pl->data);
  
  // does np2 really matter this day and age?
  pl->size = (Vector2){496,166};    // size is always the same for all instances
  pl->outTx = LoadRenderTexture(pl->size.x, pl->size.y);  // plugin should only draw on this
  
  SetTextureFilter(pl->outTx.texture, TEXTURE_FILTER_BILINEAR); 

  // clear out / initialize anything in vars like strings etc
  vars->ddDdone = false;
  vars->ddDindex = 0;
  vars->ddAdone = false;
  vars->ddAindex = 0;
  sprintf(vars->offsetNstr,"%08X",0);
  vars->offsetN = 0;
  vars->offsetNedit = false;

}


G_MODULE_EXPORT void setProperty(void* inst, char* prop, void* value) 
{

}


    
     
// This function can access the UI
G_MODULE_EXPORT void draw(void* inst) 
{
    // rendering takes place on plugin instances render texture.
  plugInstStruct* pl = (plugInstStruct*)inst;
  memViewVars* vars = ((memViewVars*)pl->data);
  SetMouseOffset(-pl->pos.x, -pl->pos.y);

  BeginTextureMode(pl->outTx);
    ClearBackground((Color){0,0,0,16});
    
      unsigned int tos=0;
      tos = vars->offsetN;
      if (vars->ddDindex!=0) tos+=m68k_get_reg(NULL, M68K_REG_D0 + vars->ddDindex-1);
      if (vars->ddAindex!=0) tos+=m68k_get_reg(NULL, M68K_REG_A0 + vars->ddAindex-1);

      GuiLabel((Rectangle){ 98, 2, 254, 16}, "+");
      GuiLabel((Rectangle){ 176, 2, 254, 16}, "+");

      GuiLabel((Rectangle){ 258, 2, 254, 16}, TextFormat("= %08X",tos));


      
      char buff[120];

      for (int i=0; i<7; i++) {
        if (i==3) {
          GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, 0xffff00ff);
        } else {
          GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, 0xffffffff);
        }
        GuiLabel((Rectangle){ 2, 24+i*20, 253, 22}, TextFormat("%08X:", tos+i*16-48));
        make_hex(buff, pl->memPtr, tos+i*16-48, pl->RamSize, 16);
        GuiLabel((Rectangle){ 102, 24+i*20, 133, 22}, TextFormat("%s", buff));
      }
     

      #define DregStr "-- ;D0 ;D1 ;D2 ;D3 ;D4 ;D5 ;D6 ;D7 "
      #define AregStr "-- ;A0 ;A1 ;A2 ;A3 ;A4 ;A5 ;A6 ;A7 "
      
      if ( GuiTextBox((Rectangle){2,2,127-32,16}, vars->offsetNstr, 9, vars->offsetNedit) ) {
        if (vars->offsetNedit) {
          vars->offsetN = strtoul(vars->offsetNstr, NULL, 16);
        }
        vars->offsetNedit=!vars->offsetNedit;
      }
      
      int fs = GuiGetStyle(DEFAULT, TEXT_SIZE);
      GuiSetStyle(DEFAULT, TEXT_SIZE, 16);

      if (GuiDropdownBox((Rectangle){ 110, 2, 64, 16 }, DregStr, &vars->ddDindex, vars->ddDdone)) 
      {
            vars->ddDdone =! vars->ddDdone;
      }
      
 
      if (GuiDropdownBox((Rectangle){ 190, 2, 64, 16 }, AregStr, &vars->ddAindex, vars->ddAdone)) 
      {
            vars->ddAdone =! vars->ddAdone;
      }
                                    
      GuiSetStyle(DEFAULT, TEXT_SIZE, fs);


  EndTextureMode();
}


// The following functions cannot access the UI
G_MODULE_EXPORT void clicked(void* inst, int x, int y) { }

// TODO put in plugInstStruct set in initialise
G_MODULE_EXPORT int getAddressSize() { return 0; }  // signals not memory mapped

G_MODULE_EXPORT byte getAddress(void* inst, int address) { return 0xff; }

G_MODULE_EXPORT void setAddress(void* inst, int address, byte data) { }
