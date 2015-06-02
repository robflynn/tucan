#!/usr/bin/python

from omniORB import CORBA
import sys, os, mimetools, time
import _GlobalIDL

orb = CORBA.ORB_init(sys.argv, CORBA.ORB_ID)
ior = open('/tmp/tucan_session.ior').readline()
fact = orb.string_to_object(ior)

count = 0

while count < 1:
	#Get a Session
	session = fact.create()

	#Get an empty auth object
	auth = session.getAuth()

	#Put in the proper values
	auth.setUserName("user")
	auth.setPassword("pass")

	#Authorize the session
	session.authorize(auth)
	
	#Get our store and print it
	store = session.getStoreByProtocol("addressbook")
	print store

	#Get a list of folders and print it
	folder_list = store.listFolders()
	print folder_list

	#Specify a folder to retrieve and print it
	folder = store.getFolder("work")
	print folder

	message = folder.getMessage(1)
	print message

	message.destroy()
	folder.destroy()
	store.destroy()
	auth.destroy()
	session.destroy()

	count = count + 1
