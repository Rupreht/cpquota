/*
 * File:   main.c
 * Author: rupreht
 *
 * Created on 12 Март 2010 г., 7:23

 cpquota: request=smtpd_access_policy
 cpquota: protocol_state=END-OF-MESSAGE
 cpquota: protocol_name=ESMTP
 cpquota: client_address=94.75.199.171
 cpquota: client_name=vps1707.2x4.ru
 cpquota: reverse_client_name=vps1707.2x4.ru
 cpquota: helo_name=vps1707.2x4.ru
 cpquota: sender=info@pickupforum.ru
 cpquota: recipient=duba7@olympus.ru
 cpquota: recipient_count=1
 cpquota: queue_id=5DD0571C2AE5
 cpquota: instance=12ef4.4cf612f2.58198.0
 cpquota: size=6906
 cpquota: etrn_domain=
 cpquota: stress=
 cpquota: sasl_method=
 cpquota: sasl_username=
 cpquota: sasl_sender=
 cpquota:

 */

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <courierauth.h>
#include "maildirgetquota.h"
#include <stdio.h>      /* printf, fgets */
#include <stdlib.h>     /* atol */
#include <string.h>

#include <syslog.h>
#include <stdarg.h>

/*
 * 
 */

int echo = 0;
int debug = 0;
char size[BUFSIZ];
char recipient[BUFSIZ];
char line[BUFSIZ];
char queue_id[BUFSIZ];
char client_address[BUFSIZ];
char sender[BUFSIZ];

long int getsize() {
    return atol(size);
}

static void smtpd_access_policy(char *message) {
    printf("action=%s\n\n", message);
    fflush (stdout);
    if (debug) {
        char mess[1024];
        sprintf(mess, "queue_id=%sclient_address=%ssender=%srecipient=%ssize=%saction=\"%s\"", &queue_id, &client_address, &sender, &recipient, &size, message);
        syslog(LOG_DEBUG, mess);
    }
    echo = 1;
}

static int callback(struct authinfo *a, void *dummy) {
    struct maildirsize info;
    char mess[256];
    if (maildir_openquotafile_init(&info, a->maildir) == 0) {
        if (atol(a->quota) - info.size.nbytes - getsize() > 0) {
            smtpd_access_policy("dunno");
        }
        else {
            if (getsize() != 0) {
                sprintf(mess, "defer_if_permit Sorry, the user <%s> over quota, please try again later. Size messege: %d bytes. Free: %d bytes.",
                        a->address, getsize(), atol(a->quota) - info.size.nbytes);
            } else {
                sprintf(mess, "defer_if_permit Sorry, the user <%s> over quota, please try again later.",
                        a->address);
            }
            smtpd_access_policy(mess);
        }
        maildir_closequotafile(&info);
    }
    return (0);
}

int main(int argc, char** argv) {
    char *token, *last;
    char sep[2];
    sep[1] = '=';
    sep[2] = '\0';

    if (argc > 1) {
        if (strstr(argv[1], "-d")) debug = 1;
        if (strstr(argv[1], "-X")) debug = 2;
        }

    clearerr(stdin);
    while (fgets(line, sizeof (line), stdin) != NULL) {

    if (debug>1) {
        syslog(LOG_DEBUG, line);
    }

        token = strtok_r(line, sep, &last);
        if (!strcmp(token, "recipient"))
            strcpy(recipient, strtok_r(NULL, "=", &last));
        if (!strcmp(token, "size"))
            strcpy(size, strtok_r(NULL, "=", &last));
        if (!strcmp(token, "queue_id"))
            strcpy(queue_id, strtok_r(NULL, "=", &last));
        if (!strcmp(token, "client_address"))
            strcpy(client_address, strtok_r(NULL, "=", &last));
        if (!strcmp(token, "sender"))
            strcpy(sender, strtok_r(NULL, "=", &last));

        if (!strcmp(token, "request")) {
            echo = 0;
            strcpy(recipient, "");
            strcpy(size, "");
            continue;
        }

        if (!strcmp(line, "\n") && strcmp(recipient, "")
                && strcmp(size, "")) {

            if (auth_getuserinfo("login", recipient, &callback, NULL))
                smtpd_access_policy("dunno");

            if (!echo) smtpd_access_policy("dunno");
            echo = 0;
            strcpy(recipient, "");
            strcpy(size, "");
        }

    }
    return (0);
}

