#!/usr/bin/python

#
# This is a sample python client that will connect to TUCAN and transfer all 
# pop messages on the specified pop3 account to a local TUCAN mail account
#
# Please note, you will need to have omniORBpy downloaded and installed.  You
# can get omniORB from our official RPM location at:
# 
# http://www.tgflinux.com/omni/
#
# After installing, please compile the IDL files in python format with the following
# command:
#
# omniidl -bpython -C/directory/to/install/to /path/to/Tucan.idl
#
# Please make sure that this client is in the same directory as the compiled IDL files
# are in.
#
# Rob Flynn <rob@tgflinux.com>
#

from omniORB import CORBA
import sys, os, time
import _GlobalIDL

# Connect to our orb
orb = CORBA.ORB_init(sys.argv, CORBA.ORB_ID)
ior = open('/tmp/tucan_session.ior').readline()
fact = orb.string_to_object(ior)

# Please change these :)
USERNAME = "username"
PASSWORD = "password"

# Create a factory
session = fact.create()

# Authorize ourselves
auth = session.getAuth()
auth.setUserName( USERNAME )
auth.setPassword( PASSWORD )
session.authorize(auth)

# Get an e-mail fetcher object
ior = open('/tmp/tucan_emailfetcher.ior').readline()
fact = orb.string_to_object(ior)

# Create the fetcher and sign on
fetch = fact.create()
fetch.setAuth(auth)
fetch.setName("blah")
fetch.setPassword("blah")
fetch.setServer("blah")
fetch.setPort(110)

# Note, if your account is an IMAP account, please umcomment the following line
# fetch.setProtocol("IMAP")

fetch.open()

# Print how many messages we're downloading
print "Total Messages: ", fetch.getMessageCount()

# Transfer all of them
fetch.transferAll()

# Sign off
fetch.close()

# And free up that memory.  Always free your memory :)
fetch.destroy()
auth.destroy()
session.destroy()

