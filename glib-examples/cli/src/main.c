#include <stdlib.h>
#include <stdio.h>

//#define GETTEXT_PACKAGE ""
#include <glib.h>
//#include <glib/goption.h>
//#include <glib/gi18n.h>

typedef struct
{
    gchar *server_host;
    gint server_port;
    gchar *log_level;
} CLIContext;

static void cli_context_print(CLIContext *ctx)
{
    printf(
        "CLIContext{ server_host = %s, server_port: %d, log_level: %s }\n",
        ctx->server_host,
        ctx->server_port,
        ctx->log_level
    );
}

int main(int argc, char *argv[])
{
    GError *error = NULL;

    CLIContext cli_context = {
        .server_host = "localhost",
        .server_port = 443,
        .log_level = "info",
    };

    // long_name, short_name, flags, arg, arg_data, description, arg_description
    GOptionEntry entries[] = {
        {
            .long_name = "host",
            .short_name = 'h',
            .flags = G_OPTION_FLAG_NONE,
            .arg = G_OPTION_ARG_STRING,
            .arg_data = &cli_context.server_host,
            .description = "Server's hostname",
            .arg_description = NULL,
        },
        {
            .long_name = "port",
            .short_name = 'p',
            .flags = G_OPTION_FLAG_NONE,
            .arg = G_OPTION_ARG_INT,
            .arg_data = &cli_context.server_port,
            .description = "Server's port",
            .arg_description = NULL,
        },
        {
            .long_name = "log-level",
            .short_name = 'l',
            .flags = G_OPTION_FLAG_NONE,
            .arg = G_OPTION_ARG_STRING,
            .arg_data = &cli_context.log_level,
            .description = "Log level (one of: INFO, WARN, DEBUG, TRACE)",
            .arg_description = NULL,
        },
        {NULL},
    };

    GOptionContext *context = g_option_context_new("");
    g_option_context_add_main_entries(context, entries, "abc");
    g_option_context_add_main_entries(context, entries, "def");

    // this will take ownership of the group and thus the group should not be freed.
    //g_option_context_add_group(context, gtk_get_option_group(TRUE));

    if (!g_option_context_parse(context, &argc, &argv, &error))
    {
        g_print("option parsing failed: %s\n", error->message);

        g_option_context_free(context);
        return EXIT_FAILURE;
    }

    g_option_context_free(context);

    cli_context_print(&cli_context);

    return EXIT_SUCCESS;
}
