#include <glib.h>

struct cli_ctx {
    int argc;
    char** argv;
    char* application;
    char* instance;
    char* record_type;
    char* output;
};

/**
 * cli --application x --instance 127.0.0.1 --record-type apache   --output logs.db log-1.txt log-2.txt ...
 * cli --application x --instance 127.0.0.2 --record-type gunicorn --output logs.db log-3.txt log-4.txt ...
 */
int main(int argc, char** argv)
{
    struct cli_ctx cli_ctx = {
        .argc = argc,
        .argv = argv,
    };

    GOptionEntry gopt_entries[] = {
        {
            .long_name = "application",
            .short_name = 'a',
            .flags = G_OPTION_FLAG_NONE,
            .arg = G_OPTION_ARG_STRING,
            .arg_data = &cli_ctx.application,
            .description = "This is where application arg description goes",
            .arg_description = NULL,
        },
        {
            .long_name = "instance",
            .short_name = 'i',
            .flags = G_OPTION_FLAG_NONE,
            .arg = G_OPTION_ARG_STRING,
            .arg_data = &cli_ctx.instance,
            .description = "This is where instance arg description goes",
            .arg_description = NULL,
        },
        {
            .long_name = "record-type",
            .short_name = 't',
            .flags = G_OPTION_FLAG_NONE,
            .arg = G_OPTION_ARG_STRING,
            .arg_data = &cli_ctx.record_type,
            .description = "This is where record type arg description goes",
            .arg_description = NULL,
        },
        {
            .long_name = "output",
            .short_name = 'o',
            .flags = G_OPTION_FLAG_NONE,
            .arg = G_OPTION_ARG_FILENAME,
            .arg_data = &cli_ctx.output,
            .description = "The output file path for the Sqlite DB where data should be written",
            .arg_description = NULL,
        },
        {0},
    };

    GOptionContext* gopt_ctx = g_option_context_new("- this is where my cli description goes");

    // A convenience function which creates a main group if it doesn't exist,
    // adds the entries to it and sets the translation domain.
    g_option_context_add_main_entries(gopt_ctx, gopt_entries, NULL);

    // Necessary?
    // g_option_context_add_group (context, gtk_get_option_group (TRUE));

    // Cli Parsing
    GError* gerr = NULL;
    if (!g_option_context_parse(gopt_ctx, &argc, &argv, &gerr)) {
        g_error("bad args: parsing failed: %s\n", gerr->message);
        goto err_gopt_ctx_free;
    }
    g_info("args parsing success");

    // Cli Context Validation
    if (cli_ctx.output == NULL) {
        g_error("bad args: missing required argument 'output' (-o,--output <FILE>)");
        goto err_gopt_ctx_free;
    }
    g_info("args validation success");

    free(cli_ctx.output);
    g_option_context_free(gopt_ctx);
    return 0;

err_gopt_ctx_free:
    g_option_context_free(gopt_ctx);
    return 1;
}
