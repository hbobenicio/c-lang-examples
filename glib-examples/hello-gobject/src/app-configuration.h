#pragma once

#include <glib-object.h>

G_BEGIN_DECLS

#define APP_TYPE_CONFIGURATION (app_configuration_get_type())

// G_DECLARE_FINAL_TYPE declares the following:
// - typedef struct _AppConfiguration AppConfiguration;
// - typedef struct { GObjectClass parent_class; } AppConfigurationClass;
G_DECLARE_FINAL_TYPE(AppConfiguration, app_configuration, APP, CONFIGURATION, GObject)

AppConfiguration* app_configuration_new(const char* prefix);
const char* app_configuration_prefix_get(AppConfiguration* self);

G_END_DECLS
