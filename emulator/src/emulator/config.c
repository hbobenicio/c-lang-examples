#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <assert.h>
#include <string.h>

#define CONFIG_CPU_CLOCK_SPEED_HZ_ENV_VAR_KEY  "CPU_CLOCK_HZ"
#define CONFIG_CPU_CLOCK_SPEED_HZ_DEFAULT_VALUE 600

#define CONFIG_LOG_LEVEL_ENV_VAR_KEY "LOG_LEVEL"
#define CONFIG_LOG_LEVEL_DEFAULT_VALUE "info"

static uint32_t parse_string_to_u32(const char *str);
static void config_init_log_level(void);
static void config_init_cpu_clock_speed(void);

static struct config cfg = {0};

const struct config* config(void)
{
    return &cfg;
}

void config_init_from_env(void)
{
    //TODO improve error handling. don't panic, collect all invalid values and print them all
    config_init_log_level();
    config_init_cpu_clock_speed();
}

void config_init_log_level(void)
{
    const char* env_var_value = getenv(CONFIG_LOG_LEVEL_ENV_VAR_KEY);
    cfg.log_level = log_level_parse(env_var_value);
}

static void config_init_cpu_clock_speed(void)
{
    const char* cpu_clock_speed_hz_env_var_value = getenv(CONFIG_CPU_CLOCK_SPEED_HZ_ENV_VAR_KEY);
    if (cpu_clock_speed_hz_env_var_value == NULL) {
        cfg.cpu_clock_speed_hz = CONFIG_CPU_CLOCK_SPEED_HZ_DEFAULT_VALUE;
    } else {
        cfg.cpu_clock_speed_hz = parse_string_to_u32(cpu_clock_speed_hz_env_var_value);
    }
}

//TODO refactor this to utils/str.{h,c}
static uint32_t parse_string_to_u32(const char *str)
{
    char *endptr = NULL;
    errno = 0;

    unsigned long long int ull_value = strtoull(str, &endptr, 10);
    if (errno != 0) {
        fprintf(stderr, "error: config: parsing %s to uint32_t failed: [%d] %s\n", str, errno, strerror(errno));
        exit(1);
    }

    static_assert(UINT32_MAX <= ULLONG_MAX, "bad numeric limits assumptions");
    if (ull_value >= UINT32_MAX) {
        fprintf(stderr, "error: config: parsing %s to uint32_t failed: value is too high\n", str);
        exit(1);
    }

    return ull_value;
}
