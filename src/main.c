#include "m68k.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include "raylib.h"
#include "plugin.h"

#define screenWidth 1280
#define screenHeight 720

/*
 TODO still very work in progress !
 some code needs seperating out into different units...
 */


void cpu_instr_callback();

#define RAMSIZE 1024*1024

uint8_t mem[RAMSIZE];
int stop = 0;
static uint32_t numStep = 0;

unsigned int g_int_controller_pending = 0;      /* list of pending interrupts */
unsigned int g_int_controller_highest_int = 0;  /* Highest pending interrupt */
char statstr[81] = {0};
char readstr[6][81] = {{0},{0},{0},{0},{0},{0}};
char *readptr[6] = { readstr[0], readstr[1], readstr[2], readstr[3], readstr[4], readstr[5] };
Font font; // UI font global so xml parser can see it... (TODO do something clever with user data pointer?)

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
    sprintf(ptr, "%04X", mem[pc]<<8 | mem[pc+1]);
    pc += 2;
    ptr += 4;
    if(length > 2)
      *ptr++ = ' ';
  }
}


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

  if(fread(mem, 1, RAMSIZE, fhandle) <= 0) {
    printf("Error reading %s", argv[2]);
    exit(-1);
  }
  

  
  

  m68k_init();
  m68k_set_cpu_type(M68K_CPU_TYPE_68000);
  m68k_set_instr_hook_callback(cpu_instr_callback);
  m68k_pulse_reset();
  
  InitWindow(screenWidth, screenHeight, "Emu68k");
  SetTargetFPS(60);


  font = LoadFontEx("resources/SpaceMono-Bold.ttf", 24, 0, 250);
  
  
  
  // XML machine loader
  
  gchar* content;
  
  if (!g_file_get_contents (argv[1], &content, NULL, NULL)) {
    printf("unable to load machine spec file\n");
    exit(-1);
  }
  
  //static GMarkupParser parser = { xml_start, xml_end, xml_element, NULL, xml_err };
  static GMarkupParser parser = { xml_start, NULL, NULL, NULL, xml_err };
  GMarkupParseContext *context;
  context = g_markup_parse_context_new (&parser, 0, NULL, NULL);
  g_markup_parse_context_parse (context, content, -1, NULL);
  
  g_markup_parse_context_free (context);
  g_free (content);
  
  signed int memview = 0;

  while(!stop && !WindowShouldClose()) {

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
          m68k_execute(1);
          numStep++;
        }
        emuStep = false;
      } else {
 
        m68k_execute(99999);
        m68k_execute(1); // TODO work-a-round for pc display not updating ???
        numStep+=100000;
        
      }
    }



    // Draw
    // (this is where the emu thread should be blocked
    // seems it wouldn't save much but the UI thread will be hanging
    // around waiting for the frame to flip....)
    // NB getting good emu speed on single core, low priotity
    //----------------------------------------------------------------------------------
    BeginDrawing();

    ClearBackground(DARKGREEN);

    if (IsKeyPressed(KEY_SPACE)) {
      emuStep = true;
      emuRun = false;
      emuSkip = false;
    }
    if (IsKeyPressed(KEY_S)) {
      emuSkip = true;
      emuStep = true;
      emuRun = false;
    }
    if (IsKeyPressed(KEY_R)) {
      emuRun = true;
      emuSkip = false;
      emuStep = false;
    }
    if (IsKeyPressed(KEY_PAGE_DOWN)) {
      memview+=16*8;
      if (memview>RAMSIZE-16*8) memview=0;
    }
    if (IsKeyPressed(KEY_PAGE_UP)) {
      memview-=16*8;
      if (memview<0) memview=RAMSIZE-16*8;
    }
    

    DrawFPS(0, 0);

    static char buff[100];
    static char buff2[100];
    static unsigned int pc;
    static unsigned int instr_size;


    for (int i=0; i<8; i++) {
      unsigned int reg = m68k_get_reg(NULL, M68K_REG_D0+i);
      DrawTextEx(font, FormatText("D%i %08X ",i,reg), (Vector2){ 20, 20+i*20 }, font.baseSize, 2, WHITE);
      reg = m68k_get_reg(NULL, M68K_REG_A0+i);
      DrawTextEx(font, FormatText("A%i %08X ",i,reg), (Vector2){ 200, 20+i*20 }, font.baseSize, 2, WHITE);

    }
    
    pc = m68k_get_reg(NULL, M68K_REG_PC);
    
    if (pc > RAMSIZE) {
      // assuming all plugins are inside ram range for now...
      m68k_pulse_bus_error();
      DrawTextEx(font, FormatText("PC>%08X: ", pc), (Vector2){ 360, 40 }, font.baseSize, 2, RED);
    } else {
      instr_size = m68k_disassemble(buff, pc, M68K_CPU_TYPE_68000);
      make_hex(buff2, pc, instr_size);
      DrawTextEx(font, FormatText("PC>%08X: %-16s: %s", pc, buff2, buff), (Vector2){ 360, 40 }, font.baseSize, 2, WHITE);

      for (int i=0;i<5;i++) {
        pc += instr_size;
        instr_size = m68k_disassemble(buff, pc, M68K_CPU_TYPE_68000);
        make_hex(buff2, pc, instr_size);
        DrawTextEx(font, FormatText("   %08X: %-16s: %s", pc, buff2, buff), (Vector2){ 360, 60+i*20 }, font.baseSize, 2, WHITE);
      }
      DrawTextEx(font, "- Recent memory activity -", (Vector2){ 120, 200 }, font.baseSize, 2, WHITE);
      DrawTextEx(font, FormatText(">%s", statstr), (Vector2){ 20, 220 }, font.baseSize, 2, WHITE);
      for (int i=0;i<6;i++) {
        DrawTextEx(font, FormatText("%s", readptr[i]), (Vector2){ 20, 260+i*20 }, font.baseSize, 2, WHITE);
      }
      
      for (int i=0; i<8; i++) {
        DrawTextEx(font, FormatText("%08X :", memview+i*16), (Vector2){ 600, 200+i*20 }, font.baseSize, 2, WHITE);
        make_hex(buff2, memview+i*16, 16);
        DrawTextEx(font, FormatText("%s", buff2), (Vector2){ 720, 200+i*20 }, font.baseSize, 2, WHITE);
      }

      
      for (GList* l = plugins; l != NULL; l = l->next) {
        plugInstStruct* p = (plugInstStruct*)l->data;
        pluginStruct* pl = p->plug;
        
        //update
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) pl->clicked(p, GetMouseX(), GetMouseY());
      
        // then draw...
        pl->draw(p); // draw to the render texture
        DrawTextureRec(p->outTx.texture,(Rectangle){0, 0, p->size.x, -p->size.y },(Vector2){p->pos.x, p->pos.y}, WHITE);
        DrawTextEx(font, FormatText("%s", p->name), (Vector2){ p->pos.x+2+p->size.x, p->pos.y-4 }, font.baseSize, 2, WHITE);
        DrawTextEx(font, FormatText("%X", p->addressStart), (Vector2){ p->pos.x+2+p->size.x, p->pos.y+14 }, font.baseSize, 2, WHITE);
      }
    }
    
    EndDrawing();
    //----------------------------------------------------------------------------------
  }

  CloseWindow();

  return 0;
}


void cpu_instr_callback() {
  unsigned int pc = m68k_get_reg(NULL, M68K_REG_PC);
  // explicitly check for the stop instruction!
  if (mem[pc]==0x4e && mem[pc+1]==0x72) stop = 1;
}

unsigned int m68k_read_disassembler_16(unsigned int address) {
  return mem[address]<<8 | mem[address+1];
}
unsigned int m68k_read_disassembler_32(unsigned int address) {
  return mem[address]<<24 | mem[address+1]<<16 | mem[address+2]<<8 | mem[address+3];
}

char dummy[81]={0};
// rotates the pointers so as to give a scrolling list...
char* nextlog()
{
  char* tmp = readptr[5];
  readptr[5] = readptr[4];
  readptr[4] = readptr[3];
  readptr[3] = readptr[2];
  readptr[2] = readptr[1];
  readptr[1] = readptr[0];
  readptr[0] = tmp;
  
  if (emuRun) {
    return dummy;
  } else {
    return readptr[0];
  }

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
    snprintf(nextlog(),80,"Step %08X : 8 bit read at %08X, %02X", numStep, address, mem[address]);
    return mem[address];
  }
  snprintf(nextlog(),80,"TODO bus error, access not handled %08X",address);
  return 0xff;
}

unsigned int m68k_read_memory_16(unsigned int address) 
{
  if (address < RAMSIZE-1) {
    snprintf(nextlog(),80,"Step %08X : 16 bit read at %08X, %04X", numStep, address, mem[address]<<8 | mem[address+1]);
    return mem[address]<<8 | mem[address+1];
  }
  snprintf(nextlog(),80,"TODO bus error, access not handled %08X",address);
  return 0xffff;
}

unsigned int m68k_read_memory_32(unsigned int address) 
{
  if (address < RAMSIZE-3) {
    snprintf(nextlog(),80,"Step %08X : 32 bit read at %08X, %08X", numStep, address, mem[address]<<24 | mem[address+1]<<16 | mem[address+2]<<8 | mem[address+3]);
    return mem[address]<<24 | mem[address+1]<<16 | mem[address+2]<<8 | mem[address+3];
  }
  snprintf(nextlog(),80,"TODO bus error, access not handled %08X",address);
  return 0xffffffff;
}


// potentially later not in same thread as UI, so can't draw stuff.....
void m68k_write_memory_8(unsigned int address, unsigned int value) 
{
  if (address < RAMSIZE) {
    mem[address] = value & 0xff;
    
    for (GList* l = plugins; l != NULL; l = l->next) {
      plugInstStruct* p = (plugInstStruct*)l->data;
      pluginStruct* pl = p->plug;
      int a = p->addressStart;
      int s = a + pl->getAddressSize(p);
      if ( address >= a && address < s ) {
        return pl->setAddress(p, address, value);
      }
    } 
    snprintf(statstr,80,"Step %08X : 8 bit write at %08X, %02X\n", numStep, address, mem[address]);
  } else {
    snprintf(nextlog(),80,"TODO bus error, access not handled %08X",address);
  }
}

void m68k_write_memory_16(unsigned int address, unsigned int value) 
{
  if (address < RAMSIZE-1) {
    mem[address]   = (value>>8) & 0xff;
    mem[address+1] = (value)    & 0xff;
    snprintf(statstr,80,"Step %08X : 16 bit write at %08X, %04X", numStep, address, value&0xffff);
  } else {
    snprintf(nextlog(),80,"TODO bus error, access not handled %08X",address);
  }
}

void m68k_write_memory_32(unsigned int address, unsigned int value) 
{
  if (address < RAMSIZE-3) {
    mem[address]   = (value>>24) & 0xff;
    mem[address+1] = (value>>16) & 0xff;
    mem[address+2] = (value>>8)  & 0xff;
    mem[address+3] = (value)     & 0xff;
    snprintf(statstr,80,"Step %08X : 32 bit write at %08X, %08X", numStep, address, value&0xffffffff);
  } else {
    snprintf(nextlog(),80,"TODO bus error, access not handled %08X",address);
  }
}
