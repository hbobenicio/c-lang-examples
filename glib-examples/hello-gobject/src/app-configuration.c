#define G_LOG_DOMAIN "app-configuration"

#include "app-configuration.h"
#include <assert.h>

typedef enum {
    APP_CONFIGURATION_PROP_FILE_PATH,
    APP_CONFIGURATION_PROPS_COUNT
} AppConfigurationProps;

typedef struct {
    const char* prefix;
} AppConfigurationPrivate;

struct _AppConfiguration {
    GObjectClass parent_class;
};

G_DEFINE_FINAL_TYPE_WITH_PRIVATE(AppConfiguration, app_configuration, G_TYPE_OBJECT)

static void app_configuration_dispose(GObject* gobject);
static void app_configuration_finalize(GObject* gobject);

static void app_configuration_class_init(AppConfigurationClass* klass)
{
    g_debug("%s()", __func__);
    g_assert(G_IS_OBJECT_CLASS(klass));

    // upcast
    GObjectClass* object_class = G_OBJECT_CLASS(klass);

    // set class virtual methods
    object_class->dispose  = app_configuration_dispose;
    object_class->finalize = app_configuration_finalize;
}

static void app_configuration_init(AppConfiguration* self)
{
    g_debug("%s()", __func__);
    g_assert(APP_IS_CONFIGURATION(self));

    AppConfigurationPrivate* priv = app_configuration_get_instance_private(self);
    priv->prefix = NULL;
}

AppConfiguration* app_configuration_new(const char* prefix)
{
    g_debug("%s()", __func__);

    AppConfiguration* obj = g_object_new(APP_TYPE_CONFIGURATION, NULL);
    g_assert(obj != NULL);

    AppConfigurationPrivate* priv = app_configuration_get_instance_private(obj);
    g_assert(priv != NULL);

    priv->prefix = prefix;

    return obj;
}

static void app_configuration_dispose(GObject* gobject)
{
    g_debug("%s()", __func__);
    g_assert(APP_IS_CONFIGURATION(gobject));
}

static void app_configuration_finalize(GObject* gobject)
{
    g_debug("%s()", __func__);
    g_assert(APP_IS_CONFIGURATION(gobject));
    //TODO call super->finalize();
}

const char* app_configuration_prefix_get(AppConfiguration* self)
{
    g_debug("%s()", __func__);
    g_assert(APP_IS_CONFIGURATION(self));

    AppConfigurationPrivate* priv = app_configuration_get_instance_private(self);
    g_assert(priv != NULL);

    return priv->prefix;
}
