#pragma once

#include <stdio.h>
#include "ansi.h"

#define LOG_TAG       ANSI_HYEL LOG_NAME ANSI_RESET ": "
#define LOG_TAG_DEBUG ANSI_HCYN "debug" ANSI_RESET ": "
#define LOG_TAG_INFO  ANSI_HGRN "info" ANSI_RESET ": "
#define LOG_TAG_WARN  ANSI_HORA "warn" ANSI_RESET ": "
#define LOG_TAG_ERROR ANSI_HRED "error" ANSI_RESET ": "

#define LOG_DEBUGF(FMT, ...)   fprintf(stderr, LOG_TAG LOG_TAG_DEBUG FMT "\n", __VA_ARGS__)
#define LOG_DEBUG(S)           fputs(LOG_TAG LOG_TAG_DEBUG S "\n", stderr)

#define LOG_INFOF(FMT, ...)   fprintf(stderr, LOG_TAG LOG_TAG_INFO FMT "\n", __VA_ARGS__)
#define LOG_INFO(S)           fputs(LOG_TAG LOG_TAG_INFO S "\n", stderr)

#define LOG_WARNF(FMT, ...)  fprintf(stderr, LOG_TAG LOG_TAG_WARN ANSI_HWHT FMT ANSI_RESET "\n", __VA_ARGS__)
#define LOG_WARN(S)          fputs(LOG_TAG LOG_TAG_WARN ANSI_HWHT S ANSI_RESET "\n", stderr)

#define LOG_ERRORF(FMT, ...)  fprintf(stderr, LOG_TAG LOG_TAG_ERROR ANSI_HWHT FMT ANSI_RESET "\n", __VA_ARGS__)
#define LOG_ERROR(S)          fputs(LOG_TAG LOG_TAG_ERROR ANSI_HWHT S ANSI_RESET "\n", stderr)

