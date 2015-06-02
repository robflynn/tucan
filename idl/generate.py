#!/usr/bin/env python

############################################################
## generate.py  
##
## Copyright (C) 2000, 2001 Difinium
############################################################
## Filename:        generate.py 
## Description:     Puts all of the idl files into one 
## Author:          Jared Peterson <jared@difinium.com>
## Created at:      Thu Nov 30 13:39:11 PST 2000
############################################################

import os, string

def processFile(file, des):

	while(file.readline() != '\012'):
		pass
	
	leftover = file.readlines()

	for line in leftover:
		des.write(line)

out = open("Tucan.idl", "w")

x = open('TucanTypes.idl')
processFile(x, out)
x = open('TucanAddress.idl')
processFile(x, out)
x = open('TucanAuth.idl')
processFile(x, out)
x = open('TucanData.idl')
processFile(x, out)
x = open('TucanFolder.idl')
processFile(x, out)
x = open('TucanMessage.idl')
processFile(x, out)
x = open('TucanStore.idl')
processFile(x, out)
x = open('TucanTransport.idl')
processFile(x, out)
x = open('TucanSession.idl')
processFile(x, out)
x = open('TucanAddressBookAddress.idl')
processFile(x, out)
x = open('TucanAddressBookFolder.idl')
processFile(x, out)
x = open('TucanAddressBookMessage.idl')
processFile(x, out)
x = open('TucanAddressBookStore.idl')
processFile(x, out)
x = open('TucanAddressBookTransport.idl')
processFile(x, out)
x = open('TucanBbsAddress.idl')
processFile(x, out)
x = open('TucanBbsFolder.idl')
processFile(x, out)
x = open('TucanBbsMessage.idl')
processFile(x, out)
x = open('TucanBbsStore.idl')
processFile(x, out)
x = open('TucanBbsTransport.idl')
processFile(x, out)
x = open('TucanCalendarAddress.idl')
processFile(x, out)
x = open('TucanCalendarFolder.idl')
processFile(x, out)
x = open('TucanCalendarMessage.idl')
processFile(x, out)
x = open('TucanCalendarStore.idl')
processFile(x, out)
x = open('TucanCalendarTransport.idl')
processFile(x, out)
x = open('TucanDirectory.idl')
processFile(x, out)
x = open('TucanEmailAddress.idl')
processFile(x, out)
x = open('TucanEmailFolder.idl')
processFile(x, out)
x = open('TucanEmailMessage.idl')
processFile(x, out)
x = open('TucanEmailStore.idl')
processFile(x, out)
x = open('TucanEmailTransport.idl')
processFile(x, out)
x = open('TucanSMTPTransport.idl')
processFile(x, out)
x = open('TucanFaxAddress.idl')
processFile(x, out)
x = open('TucanFaxFolder.idl')
processFile(x, out)
x = open('TucanFaxMessage.idl')
processFile(x, out)
x = open('TucanFaxStore.idl')
processFile(x, out)
x = open('TucanFaxTransport.idl')
processFile(x, out)
x = open('TucanVoiceAddress.idl')
processFile(x, out)
x = open('TucanVoiceFolder.idl')
processFile(x, out)
x = open('TucanVoiceMessage.idl')
processFile(x, out)
x = open('TucanVoiceStore.idl')
processFile(x, out)
x = open('TucanVoiceTransport.idl')
processFile(x, out)
x = open('TucanEmailFetcher.idl')
processFile(x, out)
x = open('TucanUserPrefs.idl')
processFile(x, out)

out.close()

os.system("docify Tucan.idl Tucan.doc")
os.system("doc++ -j -d ../docs/html Tucan.doc")
os.system("rm Tucan.doc")
os.system("omniidl -bcxx -C ../src/ Tucan.idl")
