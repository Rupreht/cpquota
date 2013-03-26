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

## Testing:

Create file policy.txt

	request=smtpd_access_policy
	protocol_state=END-OF-MESSAGE
	protocol_name=ESMTP
	client_address=94.75.199.171
	client_name=vps1707.2x4.ru
	reverse_client_name=vps1707.2x4.ru
	helo_name=vps1707.2x4.ru
	sender=info@pickupforum.ru
	recipient=444@olympus.ru
	recipient_count=1
	queue_id=5DD0571C2AE5
	instance=12ef4.4cf612f2.58198.0
	size=2335
	etrn_domain=
	stress=
	sasl_method=
	sasl_username=
	sasl_sender=

	request=smtpd_access_policy
	protocol_state=END-OF-MESSAGE
	protocol_name=ESMTP
	client_address=94.75.199.171
	client_name=vps1707.2x4.ru
	reverse_client_name=vps1707.2x4.ru
	helo_name=vps1707.2x4.ru
	sender=info@pickupforum.ru
	recipient=444@olympus.ru
	recipient_count=1
	queue_id=5DD0571C2AE5
	instance=12ef4.4cf612f2.58198.0
	etrn_domain=
	stress=
	sasl_method=
	sasl_username=
	sasl_sender=

	etc...

And

	cat policy.txt | sudo dist/Release/GNU-Generic/cpquota -X

For debugging, see

	tail -f /var/log/debug.log
