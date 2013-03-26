# Quota software for Postfix (Policy Service)

## Depend:
	courier-authlib
	courier-authlib-ldap
	courier-imap

## Link:
/courier-authlib/libcourierauth.so

use gmake (for BSD)

## Config Postfix:

/etc/postfix/master.cf

	...
	cpquota     unix        -       n       n       -       0       spawn
	    user=vmail:courier argv=/usr/local/sbin/cpquota -d
	...

Group ":courier" look authdaemond/socket

/etc/postfix/main.cf

	...
	smtpd_end_of_data_restrictions =
	   check_policy_service unix:private/cpquota

	cpquota_time_limit = 3600
	...
