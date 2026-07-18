#pragma once

#include <glib-object.h>

#define APP_TYPE_PERSON app_person_get_type()
G_DECLARE_FINAL_TYPE(AppPerson, app_person, APP, PERSON, GObject)

struct _AppPersonClass {
    GObjectClass parent_class;

    // Virtual/Overridable Methods
    void (*set_property)(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);
    void (*get_property)(GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
};

AppPerson* app_person_new(void);
void app_person_println(const AppPerson* self);
