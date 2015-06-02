#!/usr/bin/python

from omniORB import CORBA
import sys, os
import _GlobalIDL

orb = CORBA.ORB_init(sys.argv, CORBA.ORB_ID)
ior = open('/tmp/tucan_session.ior').readline()
fact = orb.string_to_object(ior)

session = fact.create()
auth = session.getAuth()
auth.setUserName("username")
auth.setPassword("password")
session.authorize(auth)

store = session.getStoreByProtocol("email")
folder = store.getDefaultFolder()
transport = session.getTransportByProtocol("email")

# Get a new Message	
message = folder.getNewMessageObject()

# Get a new address
address = message.getNewAddressObject()
# Set it's data
address.setAddress("you@yourhost.com")
# And set the from object
message.setFrom(address)

# Same as above
address = message.getNewAddressObject()
address.setAddress("them@theirhost.com")
message.setRecipient("to", address)

# Set our Subject
message.setSubject("TUCAN test!")

dataobj = message.getNewDataObject()

# Set the contents of the dataobject
dataobj.setText("This is a test of the TUCAN Email system!")

# Add a new part
dataobj.addPart("text/html", [], "<b>This is a little <i>test</i></b>.  This is part of an HTML file.")

# Set the data object
message.setDataObject(dataobj)

# And send the message
transport.send(message)
transport.destroy()

message.destroy()

folder.destroy()
store.destroy()

auth.destroy()
session.destroy()

