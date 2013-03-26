/*
 ** Copyright 1998 - 2004 Double Precision, Inc.  See COPYING for
 ** distribution information.
 */

#ifndef authstaticlist_h
#define authstaticlist_h

#if     HAVE_CONFIG_H
#include        "courier_auth_config.h"
#endif
#include        <sys/types.h>

#ifdef  __cplusplus
extern "C" {
#endif

    static const char authstaticlist_h_rcsid[] = "$Id: authstaticlist.h,v 1.13 2004/11/14 02:58:16 mrsam Exp $";

    struct authinfo;

    struct authstaticinfo {
        const char *auth_name;
        int (*auth_func)(const char *, const char *, char *,
                int (*)(struct authinfo *, void *),
                void *);
        int (*auth_prefunc)(const char *, const char *,
                int (*)(struct authinfo *, void *),
                void *);
        void (*auth_cleanupfunc)();
        int (*auth_changepwd)(const char *, /* service */
                const char *, /* userid */
                const char *, /* oldpassword */
                const char *); /* new password */

        void (*auth_idle)();
        /* Not null - gets called every 5 mins when we're idle */

        void (*auth_enumerate)(void(*cb_func)(const char *name,
                uid_t uid,
                gid_t gid,
                const char *homedir,
                const char *maildir,
                const char *options,
                void *void_arg),
                void *void_arg);
    };

    extern int auth_syspasswd(const char *,
            const char *,
            const char *,
            const char *);

#ifdef  __cplusplus
}
#endif

#endif
