#include "plugin.h"
#include <stdlib.h>
#include <stdio.h>

#include <dlfcn.h>

void integratePlugin(pluginStruct *plugin)
{
    gchar *module_path = NULL, *curr = NULL, *plug = NULL;


    if(g_module_supported() == FALSE) {
        g_error("Modules not supported :(");
        exit(-1);
    }

    curr = g_get_current_dir();
    plug = g_strdup_printf ("%s/plugins", curr);

    module_path = g_module_build_path((const gchar *) plug , plugin->libName);

    //g_print("Module path: %s\n", module_path);

    plugin->module = g_module_open ( module_path , G_MODULE_BIND_LOCAL);
    if (!plugin->module) {
        g_print("could not load plugin %s %s\n", plugin->libName, module_path);
        exit(-1);
    }

    if (!g_module_symbol (plugin->module, "initialise", (gpointer *)&plugin->initialise)) {

        g_print("couldn't get initialise function pointer %s\n", plugin->libName);
        exit(-1);
    }

    if (!g_module_symbol (plugin->module, "getAddressSize", (gpointer *)&plugin->getAddressSize)) {
        g_print("couldn't get getAddressSize function pointer %s\n", plugin->libName);
        exit(-1);
    }

    if (!g_module_symbol (plugin->module, "getAddress", (gpointer *)&plugin->getAddress)) {
        g_print("couldn't get getAddress function pointer %s\n", plugin->libName);
        exit(-1);
    }
    if (!g_module_symbol (plugin->module, "setAddress", (gpointer *)&plugin->setAddress)) {
        g_print("couldn't get setAddress function pointer %s\n", plugin->libName);
        exit(-1);
    }
    if (!g_module_symbol (plugin->module, "clicked", (gpointer *)&plugin->clicked)) {
        g_print("couldn't get clicked function pointer %s\n", plugin->libName);
        exit(-1);
    }
    if (!g_module_symbol (plugin->module, "draw", (gpointer *)&plugin->draw)) {
        g_print("couldn't get draw function pointer %s\n", plugin->libName);
        exit(-1);
    }
    if (!g_module_symbol (plugin->module, "setProperty", (gpointer *)&plugin->setProperty)) {
        g_print("couldn't get setProperty function pointer %s\n", plugin->libName);
        exit(-1);
    }
    
    g_free(module_path);
    g_free(plug);
    g_free(curr);
}

// you should use these rather than directly modifying the struct
void namePluginInstance(plugInstStruct *plugInst, char* str) {
    snprintf(plugInst->name, 79, "%s", str);
}

void setPluginInstanceStartAddress(plugInstStruct *plugInst, int a) {
    plugInst->addressStart = a;
}


