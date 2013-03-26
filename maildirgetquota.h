#ifndef maildirgetquota_h
#define maildirgetquota_h

/*
 ** Copyright 1998 - 1999 Double Precision, Inc.
 ** See COPYING for distribution information.
 */

#if     HAVE_CONFIG_H
#include        "config.h"
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

#ifdef  __cplusplus
extern "C" {
#endif

    static const char maildirgetquota_h_rcsid[] = "$Id: maildirgetquota.h,v 1.5 1999/12/06 13:21:05 mrsam Exp $";

#define	MDQUOTA_SIZE	'S'	/* Total size of all messages in maildir */
#define	MDQUOTA_BLOCKS	'B'	/* Total # of blocks for all messages in
				maildir -- NOT IMPLEMENTED */
#define	MDQUOTA_COUNT	'C'	/* Total number of messages in maildir */

    struct maildirquota {
        off_t nbytes; /* # of bytes, 0 - unlimited */
        int nmessages; /* # of messages, 0 - unlimited */
    };

    struct maildirsize {
        int fd; /* Opened file descriptor for the maildirsize file */
        char *maildir; /* Pathname to the maildir */
        char *maildirsizefile; /* The name of the maildirsize file */
        struct maildirquota quota; /* 1st line in maildirsize */
        struct maildirquota size; /* Actual counts 2+ line */
        int recalculation_needed; /* size is not calculated */
        struct stat statbuf; /* The stat on the maidlirsize file */
        unsigned nlines; /* # of lines in the maildirsize file */
    };

#define QUOTABUFSIZE    256

    int maildir_getquota(const char *, char [QUOTABUFSIZE]);
    int maildir_openquotafile_init(struct maildirsize *info,
        const char *maildir);
    void maildir_closequotafile(struct maildirsize *info);

#ifdef  __cplusplus
}
#endif

#endif
