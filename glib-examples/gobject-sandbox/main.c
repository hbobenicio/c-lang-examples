#include <stdio.h>

#include "app-person.h"

static GValue create_gvalue_static_string(const char* s) {
    GValue value = G_VALUE_INIT;
    g_value_init(&value, G_TYPE_STRING);
    g_value_set_static_string(&value, "Hugo");
    return value;
}

int main(void) {
    g_autoptr(AppPerson) person = app_person_new();

    GValue name = create_gvalue_static_string("Hugo");

    // g_object_set(person, "name", "Hugo", NULL);
    g_object_set_property(G_OBJECT(person), "name", &name);
    g_object_set(person, "age", 37, NULL);

    app_person_println(person);
}
