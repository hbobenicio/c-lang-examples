#include "app-person.h"

#include <stdio.h>

typedef enum {
    APP_PERSON_PROP_NAME = 1,
    APP_PERSON_PROP_AGE,
    APP_PERSON_PROPS_COUNT
} AppPersonProps;

static GParamSpec* app_person_param_specs[APP_PERSON_PROPS_COUNT] = {0};

struct _AppPerson {
    GObject parent;
    const char* name;
    int age;
};

G_DEFINE_FINAL_TYPE(AppPerson, app_person, G_TYPE_OBJECT)

static void app_person_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    g_debug("%s:%s", __FILE__, "app_person_get_property");
    const AppPerson* self = APP_PERSON(object);

    switch (property_id)
    {
        case APP_PERSON_PROP_NAME: {
            g_debug("get_name");
            g_value_set_string(value, self->name);
        } break;

        case APP_PERSON_PROP_AGE:{
            g_debug("get_age");
            g_value_set_int(value, self->age);
        } break;

        default: {
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
    }
}

static void app_person_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    g_debug("%s:%s", __FILE__, "app_person_set_property");
    AppPerson* self = APP_PERSON(object);

    switch (property_id)
    {
        case APP_PERSON_PROP_NAME: {
            g_debug("set_name");
            // self->name = g_value_get_string(value);
            g_free(self->name);  // Free the previous name if needed
            self->name = g_strdup(g_value_get_string(value));
        } break;

        case APP_PERSON_PROP_AGE: {
            g_debug("set_age");
            self->age = g_value_get_int(value);
        } break;

        default: {
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        };
    }
}

static void app_person_class_init(AppPersonClass* clazz)
{
    g_debug("%s:%s()", __FILE__, "app_person_class_init");
    GObjectClass* object_class = G_OBJECT_CLASS(clazz);
    object_class->get_property = app_person_get_property;
    object_class->set_property = app_person_set_property;

    app_person_param_specs[APP_PERSON_PROP_NAME] = g_param_spec_string("name", "Name", "The name of the person.", NULL, G_PARAM_READWRITE);
    app_person_param_specs[APP_PERSON_PROP_AGE] = g_param_spec_int("age", "Age", "The age of the person.", 0, 125, 0, G_PARAM_READWRITE);
    g_object_class_install_properties(object_class, G_N_ELEMENTS(app_person_param_specs), app_person_param_specs);
}

static void app_person_init(AppPerson* self)
{
    g_debug("%s:%s()", __FILE__, "app_person_init");
    (void) self;
}

AppPerson* app_person_new(void)
{
    g_debug("%s:%s()", __FILE__, "app_person_new");
    AppPerson* new_person = g_object_new(APP_TYPE_PERSON, NULL);
    return new_person;
}

void app_person_println(const AppPerson* self)
{
    printf("AppPerson{ .name = %s, .age = %d }\n", self->name, self->age);
}
