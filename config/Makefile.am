EXTRA_DIST = TucanConfig.xml procmail.tucan tucan_procmail_filter.py
Configdir = $(datadir)/config
Config_DATA = TucanConfig.xml

bin_SCRIPTS = tucan_procmail_filter.py

install:
	install -c -m 755 procmail.tucan /etc/skel/.procmailrc
