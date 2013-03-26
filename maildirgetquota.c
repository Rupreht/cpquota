
#include        "maildirgetquota.h"
#if     HAVE_UNISTD_H
#include        <unistd.h>
#endif
#include        <stdlib.h>
#include        <string.h>
#include        <fcntl.h>
#include        <sys/types.h>
#include        <sys/stat.h>
#include	<errno.h>

#define DIG(c) ( (c) >= '0' && (c) <= '9')

/*

** Copyright 2000-2003 Double Precision, Inc.
** See COPYING for distribution information.
    From maildrop
*/

static void parsequotastr(const char *quota, struct maildirquota *q) {
    off_t i;
    const char *quota_start = quota;

    q->nbytes = 0;
    q->nmessages = 0;

    while (quota && *quota) {
        if (!DIG(*quota)) {
            ++quota;
            continue;
        }
        i = 0;
        while (DIG(*quota))
            i = i * 10 + (*quota++ -'0');
        switch (*quota) {
            case MDQUOTA_SIZE:
                q->nbytes = i;
                break;
            case MDQUOTA_COUNT:
                q->nmessages = i;
                break;
            default:
                fprintf(stderr, "WARN: quota string '%s' not parseable\n",
                        quota_start);
                break;
        }
    }
}

static int do_maildir_openquotafile(struct maildirsize *info,
        const char *filename) {
    char buf[5120];
    char *p;
    unsigned l;
    int n;
    int first;

    if ((info->fd = maildir_safeopen(filename,
            O_RDWR | O_APPEND, 0)) < 0)
        return (0); /* No quota */

    p = buf;
    l = sizeof (buf);

    while (l) {
        n = read(info->fd, p, l);
        if (n < 0) {
            close(info->fd);
            info->fd = -1;
            return (-1);
        }
        if (n == 0) break;
        p += n;
        l -= n;
    }

    if (fstat(info->fd, &info->statbuf)) /* maildir too big */ {
        close(info->fd);
        info->fd = -1;
        return (-1);
    }

    if (l == 0) /*
			** maildirsize overflowed, still need to read its
			** quota
			*/ {
        p[-1] = 0;
        p = strchr(buf, '\n');
        if (p)
            *p = 0;
        parsequotastr(buf, &info->quota);
        info->recalculation_needed = 1;
        return (0);
    }


    info->size.nbytes = 0;
    info->size.nmessages = 0;
    info->nlines = 0;
    *p = 0;
    p = buf;
    first = 1;
    while (*p) {
        off_t n = 0;
        int c = 0;
        char *q = p;
        int neg;

        while (*p)
            if (*p++ == '\n') {
                p[-1] = 0;
                break;
            }

        if (first) {
            parsequotastr(q, &info->quota);
            first = 0;
            continue;
        }

        while (*q && isspace((int) (unsigned char) * q))
            ++q;

        neg = 0;
        if (*q == '-') {
            neg = 1;
            ++q;
        }

        if (DIG(*q)) {
            while (DIG(*q)) {
                n = n * 10 + (*q++ -'0');
            }

            if (neg)
                n = -n;

            neg = 0;
            while (*q && isspace((int) (unsigned char) * q))
                ++q;

            if (*q == '-') {
                neg = 1;
                ++q;
            }
            if (DIG(*q)) {
                while (DIG(*q)) {
                    c = c * 10 + (*q++ -'0');
                }

                if (neg)
                    c = -c;

                info->size.nbytes += n;
                info->size.nmessages += c;
            }
        }
        ++info->nlines;
    }
    if (info->size.nbytes < 0 ||
             info->size.nmessages < 0)
        info->recalculation_needed = 1;
    return (0);
}

int maildir_openquotafile_init(struct maildirsize *info,
        const char *maildir) {
    int rc;
    char *buf=(char *)malloc(strlen(maildir)+sizeof("/maildirsize"));
    memset(info, 0, sizeof (*info));
    info->fd = -1;
    if (!buf)
        return (-1);

    info->maildir=strdup(maildir);
    if (!info->maildir)
    {
        free(buf);
        return (-1);
    }

    strcat(strcpy(info->maildirsizefile = buf, maildir), "/maildirsize");
    rc = do_maildir_openquotafile(info, buf);

    if (rc == 0)
        return (0);

    free(buf);
    free(info->maildir);
    return (rc);
}


int maildir_safeopen(const char *path, int mode, int perm)
{
	struct  stat    stat1;

	return maildir_safeopen_stat(path, mode, perm, &stat1);
}

int maildir_safeopen_stat(const char *path, int mode, int perm,
			    struct stat *stat1)
{
	struct  stat    stat2;

	int     fd=open(path, mode
#ifdef  O_NONBLOCK
			| O_NONBLOCK
#else
			| O_NDELAY
#endif
			, perm);

	if (fd < 0)     return (fd);
	if (fcntl(fd, F_SETFL, (mode & O_APPEND)) || fstat(fd, stat1)
	    || lstat(path, &stat2))
	{
		close(fd);
		return (-1);
	}

	if (stat1->st_dev != stat2.st_dev || stat1->st_ino != stat2.st_ino)
	{
		close(fd);
		errno=ENOENT;
		return (-1);
	}

	return (fd);
}


void maildir_closequotafile(struct maildirsize *info) {
	if (info->maildir)
		free (info->maildir);
	info->maildir=NULL;

	if (info->maildirsizefile)
		free (info->maildirsizefile);
	info->maildirsizefile=NULL;

	if (info->fd >= 0)
		close(info->fd);
	info->fd= -1;
}
