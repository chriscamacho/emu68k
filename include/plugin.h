#include <gmodule.h>
#include "raylib.h"
#include "../support/Musashi/m68k.h"

// plugin emu api - not much for now...
void setBreakPoint(unsigned int bp);
void setRunning(bool run);
void doStep();
void doSkip();
bool isRunning();

// gui access 
void setMouseOffset(float, float); // for now...


typedef unsigned char byte;

typedef void (* initialiseFunc) (void* pInst);
typedef int (* getAddressSizeFunc) ();
typedef byte (* getAddressFunc) (void* pInst, int address);
typedef void (* setAddressFunc) (void* pInst, int address, byte data);
typedef void (* clickedFunc) (void* pInst, int x, int y);
typedef void (* updateAndDrawFunc) (void* pInst);
typedef void (* setPropertyFunc) (void* inst, const char* prop, void* value);

typedef struct {
	GModule *module;
	char libName[1024];
  Texture resTx; // shared single texture resource - all plugins of same type have this same texture 
	Font font;
  
	initialiseFunc initialise;
	getAddressSizeFunc getAddressSize;
	getAddressFunc getAddress;
	setAddressFunc setAddress;
	clickedFunc clicked;
	updateAndDrawFunc draw;
	setPropertyFunc setProperty;
} pluginStruct;

typedef struct {
  pluginStruct* plug;	// type of plugin
  char name[80];
  int addressStart;   // the first address in the block of memory mapped to the plugin
	RenderTexture2D outTx; // each instance has its own render texture for output

	Vector2 pos;
	Vector2 size;

  void* data;   // custom data for the plugin

} plugInstStruct;




void integratePlugin(pluginStruct *plugin);
void namePluginInstance(plugInstStruct *plugInst, char* str);
void setPluginInstanceStartAddress(plugInstStruct *plugInst, int a);
