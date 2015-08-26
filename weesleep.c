/*
 * weesleep - Plugin to disconnect weechat when the system is being suspended.
 *
 * Copyright (c) 2015 Samuel Groß
 */

#include <stdlib.h>

#include "weechat-plugin.h"

WEECHAT_PLUGIN_NAME("weesleep");
WEECHAT_PLUGIN_DESCRIPTION("Plugin to automatically disconnect and reconnect weechat when the system is being suspended");
WEECHAT_PLUGIN_AUTHOR("Samuel Groß <mail@samuel-gross.de>");
WEECHAT_PLUGIN_VERSION("0.1");
WEECHAT_PLUGIN_LICENSE("GPL3");

#define TAG "weesleep: "

/* Messages sent by the child process. These are human-readable so the binary can be tested easily. */
#define WEESLEEP_SUSPENDING "SUSPENDING\n"
#define WEESLEEP_WAKINGUP "WAKINGUP\n"

struct t_weechat_plugin* weechat_plugin = NULL;
struct t_hook* child_process;

int my_process_cb(void* data, const char* command, int return_code, const char* out, const char* err)
{
    if (return_code == WEECHAT_HOOK_PROCESS_ERROR) {
        weechat_printf (NULL, TAG "Subprocess died or failed to launch. Make sure '%s' exists and works as intended.", command);
        return WEECHAT_RC_OK;
    }

    if (return_code >= 0) {
        weechat_printf (NULL, TAG "Subprocess exited with status %d :(", return_code);
    }

    if (out) {
        /* TODO messages might arrive in multiple chunks... */
        if (weechat_strcasecmp(out, WEESLEEP_SUSPENDING) == 0) {
            weechat_printf (NULL, TAG "Suspending...", return_code);
            weechat_command(NULL, "/disconnect -all");
        } else if (weechat_strcasecmp(out, WEESLEEP_WAKINGUP) == 0) {
            weechat_printf (NULL, TAG "Waking up...", return_code);
            weechat_command(NULL, "/reconnect -all");
        } else {
            weechat_printf (NULL, TAG "Unknown message received from subprocess: %s", out);
        }
    }

    if (err) {
        weechat_printf (NULL, TAG "Received something on stderr: %s", err);
    }

    return WEECHAT_RC_OK;
}

int weechat_plugin_init(struct t_weechat_plugin* plugin, int argc, char* argv[])
{
    weechat_plugin = plugin;

    struct t_hashtable *options = weechat_hashtable_new(8, WEECHAT_HASHTABLE_STRING, WEECHAT_HASHTABLE_STRING, NULL, NULL);

    /* Send any data immediately to the callback. */
    weechat_hashtable_set(options, "buffer_flush", "1");

    /* Fork off child process and have weechat notify us whenever it writes anything to stdout. */
    child_process = weechat_hook_process_hashtable("~/.weechat/plugins/themagic", options, 0, &my_process_cb, NULL);

    weechat_hashtable_free(options);

    return WEECHAT_RC_OK;
}

int weechat_plugin_end(struct t_weechat_plugin* plugin)
{
    /* make C compiler happy */
    (void) plugin;

    weechat_unhook(child_process);

    return WEECHAT_RC_OK;
}
