#include "m68k.h"
#include "m68kcpu.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <glib.h>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "raylib.h"

#include "plugin.h"

#define screenWidth 1280
#define screenHeight 720

/*
 TODO still very work in progress !
 some code needs seperating out into different units...
 UI going into plugins...
 */


static void cpu_instr_callback(unsigned int pc);
static void pc_callback(unsigned int new_pc);

#define RAMSIZE 1024*1024

uint8_t mem[RAMSIZE];
uint32_t cycles = 0;
uint32_t breakp = 0; 


unsigned int g_int_controller_pending = 0;      /* list of pending interrupts */
unsigned int g_int_controller_highest_int = 0;  /* Highest pending interrupt */
char statstr[81] = {0};
char readstr[12][81] = {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}};
char *readptr[12] = { readstr[0], readstr[1], readstr[2], readstr[3], readstr[4],
                      readstr[5], readstr[6], readstr[7], readstr[8], readstr[9],
                      readstr[10], readstr[11],};
Font font; // UI font global so xml parser can see it... (TODO do something clever with user data pointer?)

// TODO single variable with enums
bool emuStep = false; // single step flag;
bool emuRun = false; // run as fast as possible
bool emuSkip = false; // skips 1 instruction (steps over without executing)

// TODO free on exit
GList *plugins;
GList* pluglookup;


/* Called when the CPU acknowledges an interrupt */
int cpu_irq_ack(int level)
{
/*
  switch(level)
  {
    case IRQ_NMI_DEVICE:
      return nmi_device_ack();
    case IRQ_INPUT_DEVICE:
      return input_device_ack();
    case IRQ_OUTPUT_DEVICE:
      return output_device_ack();
  }
    */
    printf("TODO interrupt level %i no ack\n",level);
  return M68K_INT_ACK_SPURIOUS;
}

void int_controller_set(unsigned int value)
{
  unsigned int old_pending = g_int_controller_pending;

  g_int_controller_pending |= (1<<value);

  if(old_pending != g_int_controller_pending && value > g_int_controller_highest_int)
  {
    g_int_controller_highest_int = value;
    m68k_set_irq(g_int_controller_highest_int);
  }
}

void int_controller_clear(unsigned int value)
{
  g_int_controller_pending &= ~(1<<value);

  for(g_int_controller_highest_int = 7;g_int_controller_highest_int > 0;g_int_controller_highest_int--)
    if(g_int_controller_pending & (1<<g_int_controller_highest_int))
      break;

  m68k_set_irq(g_int_controller_highest_int);
}

// makes an ascii string of hex values from emu memory....
void make_hex(char* buff, unsigned int pc, unsigned int length) {
  char* ptr = buff;

  for(;length>0;length -= 2)
  {
    if (pc>RAMSIZE) {sprintf(ptr, "XX"); } else { sprintf(ptr, "%02X", mem[pc]<<8); }
    if (pc+1>RAMSIZE) {sprintf(ptr+2, "XX"); } else { sprintf(ptr+2, "%02X", mem[pc+1]); }
    pc += 2;
    ptr += 4;
    if(length > 2)
      *ptr++ = ' ';
  }
}

bool isRunning() {
  return emuRun;
}

void setRunning(bool run) {
  if (run) {
    emuRun  = true;
    emuStep = false;
    emuSkip = false;    
  } else {
    emuRun  = false;
    emuStep = false;
    emuSkip = false;    
  }
}

void doStep() {
  emuRun  = false;
  emuStep = true;
  emuSkip = false;      
}

void doSkip() {
  emuRun  = false;
  emuStep = false;
  emuSkip = true;    
}

    
void setBreakPoint(unsigned int bp) {
  breakp = bp;
}


// TODO get the xml loader into its own unit.
pluginStruct* plug;
plugInstStruct* plugin;

static void 
xml_start (GMarkupParseContext *context,
           const gchar *element_name,
           const gchar **attribute_names,
           const gchar **attribute_values,
           gpointer data,
           GError **error)
{


  const gchar *element;
  element = g_markup_parse_context_get_element (context);

  int i=0;
  while(attribute_names[i]!=0) {
    if (g_ascii_strcasecmp (element, "plugin") == 0) {
      
      // libName attribute
      if (g_ascii_strcasecmp (attribute_names[i], "libName") == 0) {
        plug = NULL;
        for (GList* l = pluglookup; l != NULL; l = l->next) {
          pluginStruct* p = (pluginStruct*)l->data;
          if (g_ascii_strcasecmp (p->libName, attribute_values[i]) == 0)
                 plug = p;
        }
        
        if (plug==NULL) {
            plug = malloc(sizeof(pluginStruct));
            plug->resTx.id = 0;
            sprintf(plug->libName,(const char*)attribute_values[i]);
            plug->font = font;
            integratePlugin(plug);
            TraceLog(LOG_INFO, "EMU68K:     loaded : %s",attribute_values[i]); 
            pluglookup = g_list_append(pluglookup, plug);
        } else {
            TraceLog(LOG_INFO, "EMU68K:     already loaded plugin: %s - creating instance",attribute_values[i]); 
        }  
        
        plugin = malloc(sizeof(plugInstStruct));
        plugin->plug = plug;
        plugin->plug->initialise(plugin);
        plugins = g_list_append (plugins, plugin); 
      }
    

      if (g_ascii_strcasecmp (attribute_names[i], "label") == 0) {
        namePluginInstance(plugin,(char*)attribute_values[i]);
      }
      
      if (g_ascii_strcasecmp (attribute_names[i], "x") == 0) {
        plugin->pos.x = atoi(attribute_values[i]);
      }
      
      if (g_ascii_strcasecmp (attribute_names[i], "y") == 0) {
        plugin->pos.y = atoi(attribute_values[i]);
      }
      
      if (g_ascii_strcasecmp (attribute_names[i], "addressStart") == 0) {
        char* ptr;
        int v = strtol(attribute_values[i], &ptr, 0);
        setPluginInstanceStartAddress(plugin, v);
      }
    }
    
    
    i++;
  }
  

  
  if (g_ascii_strcasecmp(element, "property") == 0) {
    const char* name;
    const char* value;
    int i=0;
    while(attribute_names[i]!=0) {
      if (g_ascii_strcasecmp (attribute_names[i], "name") == 0) name = attribute_values[i];
      if (g_ascii_strcasecmp (attribute_names[i], "value") == 0) value = attribute_values[i];
      i++;
    }
    plug->setProperty(plugin, name, (void*)value); 
  }

}

static void
xml_err (GMarkupParseContext *context,
         GError *error,
         gpointer data)
{
  g_critical ("%s", error->message);
}

int main(int argc, char* argv[])
{


  FILE* fhandle;

  if(argc != 3) {
    printf("Usage: 68kemu <machine xml file> <bin file>\n");
    exit(-1);
  }

  if((fhandle = fopen(argv[2], "rb")) == NULL) {
    printf("Unable to open %s\n", argv[2]);
    exit(-1);
  }
  
  char *ext = strrchr(argv[2], '.');
  printf("\n\n%s = %s\n\n",argv[2],ext);
  if (strcasecmp(ext,".s")==0) {
    printf("its a .s! TODO srec loader here...\n");
    exit(0);
  } else {

    if(fread(mem, 1, RAMSIZE, fhandle) <= 0) {
      printf("Error reading %s", argv[2]);
      exit(-1);
    }
    fclose(fhandle);
  }

  
  

  m68k_init();
  m68k_set_cpu_type(M68K_CPU_TYPE_68000);
  m68k_set_instr_hook_callback(cpu_instr_callback);
  m68k_set_pc_changed_callback(pc_callback);
  m68k_pulse_reset();
  
  InitWindow(screenWidth, screenHeight, "Emu68k");
  SetTargetFPS(60);

  font = LoadFontEx("resources/SpaceMono-Bold.ttf", 22, 0, 250);


  
  // XML machine loader
  
  gchar* content;
  
  if (!g_file_get_contents (argv[1], &content, NULL, NULL)) {
    printf("unable to load machine spec file\n");
    exit(-1);
  }
  
  static GMarkupParser parser = { xml_start, NULL, NULL, NULL, xml_err };
  GMarkupParseContext *context;
  context = g_markup_parse_context_new (&parser, 0, NULL, NULL);
  g_markup_parse_context_parse (context, content, -1, NULL);
  
  g_markup_parse_context_free (context);
  g_free (content);
  
  signed int memview = 0;
  
  GuiSetFont(font);
  
  GuiSetStyle(TEXTBOX, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
  GuiSetStyle(TEXTBOX, TEXT_COLOR_FOCUSED, ColorToInt(RED));
  GuiSetStyle(TEXTBOX, TEXT_COLOR_PRESSED, ColorToInt(BLACK));
  GuiSetStyle(TEXTBOX, TEXT_COLOR_PRESSED, ColorToInt(WHITE));
  GuiSetStyle(TEXTBOX, BASE_COLOR_PRESSED, ColorToInt(BLACK));
  GuiSetStyle(TEXTBOX, TEXT_INNER_PADDING, 0);
//  GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);

  while(!WindowShouldClose()) {
printf("****************************\n");

    /* TODO this needs to go in a seperate thread
     * with mutex to block emulation while UI
     * is rendered from a static emulation state */
    if (emuStep || emuRun) {
      if (emuStep) {
        if (emuSkip) {
          char buff[100];
          unsigned int pc = m68k_get_reg(NULL, M68K_REG_PC);
          int instr_size = m68k_disassemble(buff, pc, M68K_CPU_TYPE_68000);
          pc += instr_size;
          m68k_set_reg(M68K_REG_PC,pc);
          emuSkip = false;
        } else {
          // 1 cycle tips it over to execute the next instruction...?
          m68k_execute(1);
        }
        emuStep = false;
      } else {
        
        //7.09002mhz = 118167.0 cycles per 60th/second 
        m68k_execute(118167);
        /*
        m68k_execute(118159);
        m68k_execute(1);  
        m68k_execute(1);  
        m68k_execute(1);  
        m68k_execute(1);  
        m68k_execute(1);  
        m68k_execute(1);  
        m68k_execute(1);  
        m68k_execute(1);  */
        
      }
    }


    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

    ClearBackground(DARKGREEN);

    DrawFPS(0, 0);

    char buff[100];
    char buff2[100];
    unsigned int pc;
    unsigned int instr_size;

    for (int i=0; i<8; i++) {
      unsigned int reg = m68k_get_reg(NULL, M68K_REG_D0+i);
      DrawTextEx(font, FormatText("D%i %08X ",i,reg), (Vector2){ 20, 20+i*20 }, font.baseSize, 0, WHITE);
      reg = m68k_get_reg(NULL, M68K_REG_A0+i);
      DrawTextEx(font, FormatText("A%i %08X ",i,reg), (Vector2){ 200, 20+i*20 }, font.baseSize, 0, WHITE);

    }
    
    pc = m68k_get_reg(NULL, M68K_REG_PPC);
    
    if (pc > RAMSIZE) {
      // assuming all plugins are inside ram range for now...
      m68k_pulse_bus_error();
      DrawTextEx(font, FormatText("PC>%08X: ", pc), (Vector2){ 360, 40 }, font.baseSize, 0, RED);
    } else {
      instr_size = m68k_disassemble(buff, pc, M68K_CPU_TYPE_68000);
      make_hex(buff2, pc, instr_size);
      DrawTextEx(font, FormatText("PC>%08X: %-16s: %s", pc, buff2, buff), (Vector2){ 360, 40 }, font.baseSize, 0, WHITE);

      for (int i=0;i<5;i++) {
        pc += instr_size;
        instr_size = m68k_disassemble(buff, pc, M68K_CPU_TYPE_68000);
        make_hex(buff2, pc, instr_size);
        DrawTextEx(font, FormatText("   %08X: %-16s: %s", pc, buff2, buff), (Vector2){ 360, 60+i*20 }, font.baseSize, 0, WHITE);
      }
      DrawTextEx(font, "- Recent memory activity -", (Vector2){ 120, 200 }, font.baseSize, 0, WHITE);
      DrawTextEx(font, FormatText(">%s", statstr), (Vector2){ 20, 220 }, font.baseSize, 0, WHITE);
      for (int i=0;i<12;i++) {
        DrawTextEx(font, FormatText("%s", readptr[i]), (Vector2){ 20, 260+i*20 }, font.baseSize, 0, WHITE);
      }
      
      // this should be a setting on or off and offset
      memview = m68k_get_reg(NULL, M68K_REG_A7)-48;
      
      
      for (int i=0; i<7; i++) {
        DrawTextEx(font, FormatText("%08X :", memview+i*16), (Vector2){ 600, 200+i*20 }, font.baseSize, 0, WHITE);
        make_hex(buff2, memview+i*16, 16);
        Color c = WHITE;
        if (i==3) c = YELLOW;
        DrawTextEx(font, FormatText("%s", buff2), (Vector2){ 720, 200+i*20 }, font.baseSize, 0, c);
      }

      
      for (GList* l = plugins; l != NULL; l = l->next) {
        plugInstStruct* p = (plugInstStruct*)l->data;
        pluginStruct* pl = p->plug;
        
        //update
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) pl->clicked(p, GetMouseX(), GetMouseY());
      
        // then draw...
        pl->draw(p); // draw to the render texture
        DrawTextureRec(p->outTx.texture,(Rectangle){0, 0, p->size.x, -p->size.y },(Vector2){p->pos.x, p->pos.y}, WHITE);
        if (p->addressStart==0){
          DrawTextEx(font, FormatText("%s", p->name), (Vector2){ p->pos.x+16, p->pos.y-font.baseSize }, font.baseSize, 0, WHITE);
        } else {
          DrawTextEx(font, FormatText("%s", p->name), (Vector2){ p->pos.x+2+p->size.x, p->pos.y-4 }, font.baseSize, 0, WHITE);
          DrawTextEx(font, FormatText("%X", p->addressStart), (Vector2){ p->pos.x+2+p->size.x, p->pos.y+14 }, font.baseSize, 0, WHITE);
        }
      }
    }
 
    EndDrawing();
    //----------------------------------------------------------------------------------
  }

  CloseWindow();

  return 0;
}


// rotates the pointers so as to give a scrolling list...
char* nextlog()
{
  char* tmp = readptr[11];
  readptr[11] = readptr[10];
  readptr[10] = readptr[9];
  readptr[9] = readptr[8];
  readptr[8] = readptr[7];
  readptr[7] = readptr[6];
  readptr[6] = readptr[5];
  readptr[5] = readptr[4];
  readptr[4] = readptr[3];
  readptr[3] = readptr[2];
  readptr[2] = readptr[1];
  readptr[1] = readptr[0];
  readptr[0] = tmp;

  return readptr[0]; // just so it can be a printf param...
}

void dobreak(unsigned int pc) {
      emuStep = false;
      emuRun = false;
      emuSkip = false;
      m68k_end_timeslice();  
      snprintf(nextlog(),80,"Cycl %08X :  break at %08X", cycles, pc);
}

static void pc_callback(unsigned int new_pc) {
    if (new_pc == breakp) {
      dobreak(new_pc);
    }
}

void cpu_instr_callback(unsigned int pc) {
  char scratch[100];
  unsigned int i=m68k_disassemble(scratch, pc, M68K_CPU_TYPE_68000); // break just before!
  if (pc+i==breakp) {
    dobreak(breakp);
  }
  cycles += CYC_INSTRUCTION[REG_IR];

  
}

unsigned int m68k_read_disassembler_16(unsigned int address) {
  return mem[address]<<8 | mem[address+1];
}
unsigned int m68k_read_disassembler_32(unsigned int address) {
  return mem[address]<<24 | mem[address+1]<<16 | mem[address+2]<<8 | mem[address+3];
}




unsigned int m68k_read_memory_8(unsigned int address) 
{
  if (address < RAMSIZE) {
    for (GList* l = plugins; l != NULL; l = l->next) {
      plugInstStruct* p = (plugInstStruct*)l->data;
      pluginStruct* pl = p->plug;
      int a = p->addressStart;
      int s = a + pl->getAddressSize(p);
      if ( address >= a && address < s ) {
        return pl->getAddress(p, address);
      }
    }    
    snprintf(nextlog(),80,"Cycl %08X : 8 bit read at %08X, %02X", cycles, address, mem[address]);
    return mem[address];
  }
  snprintf(nextlog(),80,"TODO bus error, access not handled %08X",address);
  return 0xff;
}

// TODO plugin ranges
unsigned int m68k_read_memory_16(unsigned int address) 
{
  if (address < RAMSIZE-1) {
    snprintf(nextlog(),80,"Cycl %08X : 16 bit read at %08X, %04X", cycles, address, mem[address]<<8 | mem[address+1]);
    return mem[address]<<8 | mem[address+1];
  }
  snprintf(nextlog(),80,"TODO bus error, access not handled %08X",address);
  return 0xffff;
}

// TODO plugin ranges
unsigned int m68k_read_memory_32(unsigned int address) 
{
  if (address < RAMSIZE-3) {
    snprintf(nextlog(),80,"Cycl %08X : 32 bit read at %08X, %08X", cycles, address, mem[address]<<24 | mem[address+1]<<16 | mem[address+2]<<8 | mem[address+3]);
    return mem[address]<<24 | mem[address+1]<<16 | mem[address+2]<<8 | mem[address+3];
  }
  snprintf(nextlog(),80,"TODO bus error, access not handled %08X",address);
  return 0xffffffff;
}


void m68k_write_memory_8(unsigned int address, unsigned int value) 
{
  if (address < RAMSIZE) {
    mem[address] = value & 0xff;

    snprintf(nextlog(),80,"Cycl %08X : 8 bit write at %08X, %02X\n", cycles, address, mem[address]);
    
    for (GList* l = plugins; l != NULL; l = l->next) {
      plugInstStruct* p = (plugInstStruct*)l->data;
      pluginStruct* pl = p->plug;
      int a = p->addressStart;
      int s = a + pl->getAddressSize(p);
      if ( address >= a && address < s ) {
        return pl->setAddress(p, address, value);
      }
    } 
  } else {
    snprintf(nextlog(),80,"TODO bus error, access not handled %08X",address);
  }
}

// TODO plugin ranges
void m68k_write_memory_16(unsigned int address, unsigned int value) 
{
  if (address < RAMSIZE-1) {
    mem[address]   = (value>>8) & 0xff;
    mem[address+1] = (value)    & 0xff;
    snprintf(nextlog(),80,"Cycl %08X : 16 bit write at %08X, %04X", cycles, address, value&0xffff);
  } else {
    snprintf(nextlog(),80,"TODO bus error, access not handled %08X",address);
  }
}

// TODO plugin ranges
void m68k_write_memory_32(unsigned int address, unsigned int value) 
{
  if (address < RAMSIZE-3) {
    mem[address]   = (value>>24) & 0xff;
    mem[address+1] = (value>>16) & 0xff;
    mem[address+2] = (value>>8)  & 0xff;
    mem[address+3] = (value)     & 0xff;
    snprintf(nextlog(),80,"Cycl %08X : 32 bit write at %08X, %08X", cycles, address, value&0xffffffff);
  } else {
    snprintf(nextlog(),80,"TODO bus error, access not handled %08X",address);
  }
}
