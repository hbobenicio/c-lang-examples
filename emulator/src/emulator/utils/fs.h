#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <emulator/commons/buffer.h>

struct buffer file_read_contents(const char* file_path);
