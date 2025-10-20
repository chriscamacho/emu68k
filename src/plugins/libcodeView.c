
#include "plugin.h"
#include <stdlib.h>
#include <stdio.h>
#include "raylib.h"
#include "raygui.h"
#include <math.h>

/*
 *   codeView plugin
 * 
 *    Show a dissassembly of the code near PC
 * 
 */

typedef struct {


  
} codeViewVars;


// set up
G_MODULE_EXPORT void initialise(void* inst)
{
  plugInstStruct* pl = (plugInstStruct*)inst;
  pl->data = malloc(sizeof(codeViewVars));
  //codeViewVars* vars = ((codeViewVars*)pl->data);
  

  pl->size = (Vector2){550,124};    // size is always the same for all instances
  pl->outTx = LoadRenderTexture(pl->size.x, pl->size.y);  // plugin should only draw on this
  
  SetTextureFilter(pl->outTx.texture, ICON_FILTER_BILINEAR); 

  // clear out / initialize anything in vars like strings etc


}


G_MODULE_EXPORT void setProperty(void* inst, char* prop, void* value) { }



// This function can access the UI
G_MODULE_EXPORT void draw(void* inst) 
{
    // rendering takes place on plugin instances render texture.
  plugInstStruct* pl = (plugInstStruct*)inst;
  //codeViewVars* vars = ((codeViewVars*)pl->data);
  
  SetMouseOffset(-pl->pos.x, -pl->pos.y);   // TODO caller sets this ?
  unsigned int pc = m68k_get_reg(NULL, M68K_REG_PC);

  char buff[100],buff2[100];

  BeginTextureMode(pl->outTx);
    ClearBackground((Color){0,0,0,16});

        
        int instr_size = m68k_disassemble(buff, pc, M68K_CPU_TYPE_68000);
        make_hex(buff2, pl->memPtr, pc, pl->RamSize, instr_size);
        GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, 0xffff00ff);
        GuiLabel((Rectangle){ 8, 4, 512, 18}, TextFormat("?%08X:%-15s:%s", pc, buff2, buff));
        GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, 0xffffffff);
        
        for (int i=0;i<5;i++) {
          pc += instr_size;
          instr_size = m68k_disassemble(buff, pc, M68K_CPU_TYPE_68000);
          make_hex(buff2, pl->memPtr, pc, pl->RamSize, instr_size);

          GuiLabel((Rectangle){ 8, 24+i*20, 512, 22}, TextFormat("!%08X:%-15s:%s", pc, buff2, buff));
            

        }


    
  EndTextureMode();

  
  SetMouseOffset(0, 0); // caller set ?
  //GuiFade(1);  // caller set ?
}


// The following functions cannot access the UI
G_MODULE_EXPORT void clicked(void* inst, int x, int y) { }

// TODO put in plugInstStruct set in initialise
G_MODULE_EXPORT int getAddressSize() { return 0; }  // important signals gui plugin

G_MODULE_EXPORT byte getAddress(void* inst, int address) { return 0xff; }

G_MODULE_EXPORT void setAddress(void* inst, int address, byte data) { }
