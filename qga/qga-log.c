#include "log.c"

bool ga_logging_enabled(GAState *s)
{
    return s->logging_enabled;
}

void ga_disable_logging(GAState *s)
{
    s->logging_enabled = false;
}

void ga_enable_logging(GAState *s)
{
    s->logging_enabled = true;
}


static void ga_log(const gchar *domain, GLogLevelFlags level,
                   const gchar *msg, gpointer opaque)
{
    GAState *s = opaque;
    const char *level_str = ga_log_level_str(level);

    if (!ga_logging_enabled(s)) {
        return;
    }

    level &= G_LOG_LEVEL_MASK;
    if (g_strcmp0(domain, "syslog") == 0) {
        system_log()
    } else if (level & s->log_level) {
        file_log()
    }
}

void init_log(GAState *s,GAConfig *cofig){
    s->log_level = config->log_level;
    s->log_file = stderr;
#ifdef CONFIG_FSFREEZE
    s->fsfreeze_hook = config->fsfreeze_hook;
#endif
    s->pstate_filepath = g_strdup_printf("%s/qga.state", config->state_dir);
    s->state_filepath_isfrozen = g_strdup_printf("%s/qga.state.isfrozen",
                                                 config->state_dir);
    s->frozen = check_is_frozen(s);

    g_log_set_default_handler(ga_log, s);
    g_log_set_fatal_mask(NULL, G_LOG_LEVEL_ERROR);
    ga_enable_logging(s);

    g_debug("Guest agent version %s started", QEMU_FULL_VERSION);

#ifdef _WIN32
    s->event_log = RegisterEventSource(NULL, "qemu-ga");
    if (!s->event_log) {
        g_autofree gchar *errmsg = g_win32_error_message(GetLastError());
        g_critical("unable to register event source: %s", errmsg);
        return NULL;
    }

    /* On win32 the state directory is application specific (be it the default
     * or a user override). We got past the command line parsing; let's create
     * the directory (with any intermediate directories). If we run into an
     * error later on, we won't try to clean up the directory, it is considered
     * persistent.
     */
    if (g_mkdir_with_parents(config->state_dir, S_IRWXU) == -1) {
        g_critical("unable to create (an ancestor of) the state directory"
                   " '%s': %s", config->state_dir, strerror(errno));
        return NULL;
    }
#endif

    if (ga_is_frozen(s)) {
        if (config->daemonize) {
            /* delay opening/locking of pidfile till filesystems are unfrozen */
            s->deferred_options.pid_filepath = config->pid_filepath;
            become_daemon(NULL);
        }
        if (config->log_filepath) {
            /* delay opening the log file till filesystems are unfrozen */
            s->deferred_options.log_filepath = config->log_filepath;
        }
        ga_disable_logging(s);
        qmp_for_each_command(&ga_commands, ga_disable_not_allowed, NULL);
    } else {
        if (config->daemonize) {
            become_daemon(config->pid_filepath);
        }
        if (config->log_filepath) {
            FILE *log_file = ga_open_logfile(config->log_filepath);
            if (!log_file) {
                g_critical("unable to open specified log file: %s",
                           strerror(errno));
                return NULL;
            }
            s->log_file = log_file;
        }
    }
}