#!/usr/bin/python

import os, sys, time, rand

email = sys.stdin.read()

filename = ""

filename = "%s%d.%d" % (filename, time.time(), rand.rand())

try:
	filename = "%s.%s" % (filename, os.environ["HOSTNAME"])
except:
	pass


filename = "%s/tucan_email/inbox/%s" % (os.environ["HOME"], filename)

w = open(filename, "w")
w.write(email)
w.close()

