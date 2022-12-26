#pragma once

#include <stdbool.h>

#include "register.h"

typedef Register KeyValue;

bool keyboard_is_key_pressed(KeyValue kb_value);

/**
 * @brief Returns the key value that is pressed, if any.
 * 
 * NOTE this could be improve to return all keys that are pressed. not only the first one
 * 
 * @param out_key_value OUT NULL if no key is pressed, the key value of the key that is pressed otherwise
 * @return true if any key is pressed
 * @return false if no key is pressed
 */
bool keyboard_get_pressed_key(KeyValue* out_key_value);
