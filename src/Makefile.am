bin_PROGRAMS = tucan
tucan_SOURCES = TucanConfig.cc TucanDb.cc TucanAuth_i.cc TucanSession_i.cc TucanData_i.cc TucanUserPrefs_i.cc \
		TucanMain.cc TucanSK.cc TucanEmailStore_i.cc TucanEmailFolder_i.cc \
		TucanEmailMessage_i.cc TucanEmailAddress_i.cc TucanEmailTransport_i.cc \
		TucanEmailFetcher_i.cc TucanCalendarStore_i.cc TucanCalendarFolder_i.cc \
		TucanCalendarMessage_i.cc TucanCalendarTransport_i.cc \
		TucanCalendarAddress_i.cc TucanVoiceStore_i.cc TucanVoiceFolder_i.cc \
		TucanVoiceMessage_i.cc TucanVoiceAddress_i.cc TucanVoiceTransport_i.cc \
		TucanAddressBookStore_i.cc TucanAddressBookFolder_i.cc \
		TucanAddressBookMessage_i.cc TucanAddressBookAddress_i.cc\
		TucanAddressBookTransport_i.cc \
		TucanFaxStore_i.cc TucanFaxFolder_i.cc TucanFaxMessage_i.cc \
		TucanFaxAddress_i.cc TucanFaxTransport_i.cc \
		TucanBbsStore_i.cc TucanBbsFolder_i.cc TucanBbsMessage_i.cc \
		TucanBbsAddress_i.cc TucanBbsTransport_i.cc \
		TucanPop3.cc TucanIMAP.cc TucanMBOX.cc TucanUtilities.cc TucanParse.cc \
		TucanSMTPTransport_i.cc

CFLAGS += -Wall -D__OMNIORB3__ -D_REENTRANT -D__x86__ -D__linux__ -D__OSVERSION__=2 -I/usr/include/pgsql -DPREFIX_PATH=\"$(prefix)\" -g

CFLAGS += -I/usr/include/xerces
CXXFLAGS = $(CFLAGS)

EXTRA_DIST = Tucan.hh TucanAuth_i.hh TucanConfig.hh TucanDb.hh TucanSession_i.hh TucanData_i.hh \
				TucanEmailStore_i.hh TucanEmailFolder_i.hh TucanEmailMessage_i.hh \
				TucanEmailAddress_i.hh TucanEmailTransport_i.hh TucanCalendarStore_i.hh \
				TucanCalendarFolder_i.hh  TucanCalendarMessage_i.hh \
				TucanCalendarTransport_i.hh TucanCalendarAddress_i.hh\
				TucanVoiceStore_i.hh TucanVoiceFolder_i.hh TucanVoiceMessage_i.hh \
				TucanVoiceAddress_i.hh TucanVoiceTransport_i.hh \
				TucanAddressBookStore_i.hh TucanAddressBookFolder_i.hh \
				TucanAddressBookMessage_i.hh TucanAddressBookAddress_i.hh\
				TucanAddressBookTransport_i.hh \
				TucanFaxStore_i.hh TucanFaxFolder_i.hh TucanFaxMessage_i.hh \
				TucanFaxAddress_i.hh TucanFaxTransport_i.hh \
				TucanBbsStore_i.hh TucanBbsFolder_i.hh TucanBbsMessage_i.hh \
				TucanBbsAddress_i.hh TucanBbsTransport_i.hh \
				TucanUtilities.hh TucanParse.hh TucanEmailFetcher_i.hh \
				TucanPop3.hh TucanIMAP.hh TucanMBOX.hh TucanEmail.hh TucanUserPrefs_i.hh \
				TucanSMTPTransport_i.hh

install:
	install -c -d /etc/skel/tucan_email -m 770
	install -c -d /etc/skel/tucan_email/inbox -m 700
	install -c -d /etc/skel/fax -m 770
	install -c -d /etc/skel/fax/inbox -m 770
	install -c -d /etc/skel/vm -m 770
	install -c -d /etc/skel/vm/inbox -m 770
	install -c -d /etc/skel/cal -m 770
