
#include "plugin.h"
#include <stdlib.h>
#include <stdio.h>
#include "raylib.h"
#include "raygui.h"
#include <math.h>

/*
 *   regView plugin
 * 
 *    shows D0-D7 A0-A7 PC SR
 * 
 *    also allows editing, TODO PC/SR
 * 
 */

typedef struct {

  //char label[33];
  int dummy;
  char regStr[18][9];
  unsigned int regs[18];
  unsigned int last[18];
  bool editing[18];
  
} regViewVars;


// set up
G_MODULE_EXPORT void initialise(void* inst)
{
  plugInstStruct* pl = (plugInstStruct*)inst;
  pl->data = malloc(sizeof(regViewVars));
  regViewVars* vars = ((regViewVars*)pl->data);
  
  // does np2 really matter this day and age?
  pl->size = (Vector2){256,220};    // size is always the same for all instances
  pl->outTx = LoadRenderTexture(pl->size.x, pl->size.y);  // plugin should only draw on this
  
  SetTextureFilter(pl->outTx.texture, FILTER_BILINEAR); 

  // clear out / initialize anything in vars like strings etc

  for (int i=0; i<18; i++) {
    vars->regStr[i][0] = 0;
    vars->regs[i] = 0;
    vars->last[i] = -1;
    vars->editing[i] = false;
  }
}


G_MODULE_EXPORT void setProperty(void* inst, char* prop, void* value) { }

// This function can access the UI
G_MODULE_EXPORT void draw(void* inst) 
{
	// rendering takes place on plugin instances render texture.
  plugInstStruct* pl = (plugInstStruct*)inst;
  regViewVars* vars = ((regViewVars*)pl->data);
  
  setMouseOffset(pl->pos.x, pl->pos.y);   // TODO caller sets this ?
  //unsigned int pc = m68k_get_reg(NULL, M68K_REG_PC);

  
  for (int i=0; i<18; i++) {
    vars->regs[i] = m68k_get_reg(NULL, M68K_REG_D0 + i);
    if (vars->regs[i]!=vars->last[i]) {
      sprintf(vars->regStr[i],"%08X", vars->regs[i]);
      vars->last[i] = vars->regs[i];
    }
  }
  
  char srb[]="cvznxCVZNX";
  char srs[]="     -----";
  byte bit=1;
  
  for (int i=0;i<5;i++) {
    if(bit & vars->regs[M68K_REG_SR]) {
      srs[4-i+5] = srb[i+5];
    } else {
      srs[4-i+5] = srb[i];
    }
    bit = bit << 1;
  }

  BeginTextureMode(pl->outTx);
    ClearBackground(BLANK);
    for(int i=0; i<8; i++) {
      if (GuiTextBox((Rectangle){ 32, 4+i*20, 90, 22}, vars->regStr[i], 9, vars->editing[i])) {
        vars->editing[i]=!vars->editing[i];
        if (!vars->editing[i]) {
          unsigned int v = strtoul(vars->regStr[i], NULL, 16);
          if (v!=vars->last[i]) sprintf(vars->regStr[i],"%08X",v);
          m68k_set_reg(M68K_REG_D0 + i, v);
          vars->last[i]=v;
          printf("v=%08x\n",v);
        }
      }
      if (GuiTextBox((Rectangle){ 161, 4+i*20, 90, 22}, vars->regStr[i+8], 9, vars->editing[i+8])) {
        vars->editing[i+8]=!vars->editing[i+8];
        if (!vars->editing[i+8]) {
          unsigned int v = strtoul(vars->regStr[i], NULL, 16);
          if (v!=vars->last[i+8]) sprintf(vars->regStr[i+8],"%08X",v);
          m68k_set_reg(M68K_REG_D0 + i+8, v);
          vars->last[i+8]=v;
          printf("v=%08x\n",v);
        }
      }
      GuiLabel((Rectangle){ 2, 4+i*20, 127, 22}, FormatText("D%i",i));
      GuiLabel((Rectangle){ 129, 4+i*20, 64, 22}, FormatText("A%i",i,vars->regs[i+8]));
    }
    GuiLabel((Rectangle){ 2, 4+8.25*20, 127, 22}, FormatText("PC %08X",vars->regs[M68K_REG_PC]));
    GuiLabel((Rectangle){ 129, 4+8.25*20, 64, 22}, FormatText("SR %02X",vars->regs[M68K_REG_SR]));
    GuiLabel((Rectangle){ 129, 4+9.25*20, 64, 22}, srs);
    
  EndTextureMode();

  
  setMouseOffset(0, 0); // caller set ?
  //GuiFade(1);  // caller set ?
}


// The following functions cannot access the UI
G_MODULE_EXPORT void clicked(void* inst, int x, int y) { }

// TODO put in plugInstStruct set in initialise
G_MODULE_EXPORT int getAddressSize() { return 0; }  // important signals gui plugin

G_MODULE_EXPORT byte getAddress(void* inst, int address) { return 0xff; }

G_MODULE_EXPORT void setAddress(void* inst, int address, byte data) { }
