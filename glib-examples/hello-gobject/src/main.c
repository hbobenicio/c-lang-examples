/**
 * A simple gobject demo.
 * 
 * You can set `G_MESSAGES_DEBUG=all` or maybe `G_MESSAGES_DEBUG=main,app-configuration,...` to see debug logs.
 */
#include <stdio.h>

#define G_LOG_DOMAIN "main"

#include <glib-object.h>

#include "app-configuration.h"

int main()
{
    g_debug("main called");

    g_autoptr(AppConfiguration) config = app_configuration_new("APP_");
    
    printf("It Works! prefix=\"%s\"\n", app_configuration_prefix_get(config));
}
