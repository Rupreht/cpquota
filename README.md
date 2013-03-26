<<<<<<< HEAD
# Quota software for Postfix (Policy Service)

## Depend:
	courier-authlib
	courier-authlib-ldap
	courier-imap

## Link:
/courier-authlib/libcourierauth.so

## Config Postfix:

/etc/postfix/master.cf

	...
	cpquota     unix        -       n       n       -       0       spawn
	    user=vmail:courier argv=/usr/local/sbin/cpquota -d
	...

Group ":courier" luck authdaemond/socket

/etc/postfix/main.cf

	...
	smtpd_end_of_data_restrictions =
	   check_policy_service unix:private/cpquota

	cpquota_time_limit = 3600
	...
=======
# Quota software for Postfix (Policy Service)

## Depend:
	courier-authlib
	courier-authlib-ldap
	courier-imap

## Link:
/courier-authlib/libcourierauth.so

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
>>>>>>> 7daaa80a8c6230a7505a3004de47472bfa775b39
