#include <TucanBbsFolder_i.hh>
#include <TucanConfig.hh>
#include <TucanDb.hh>

TucanBbsFolder_i::TucanBbsFolder_i(CORBA::ORB_var orb)
{
	myOrb = orb;
	CORBA::Object_var obj = myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);

	myName = "";
	myFullName = "";
	myParent = NULL;
	myPid = 0;
	myBid = 0;
}

CORBA::Short TucanBbsFolder_i::getBid()
{
	return myBid;
}

void TucanBbsFolder_i::setBid(CORBA::Short bid)
{
	myBid = bid;
}

CORBA::Long TucanBbsFolder_i::getPid()
{
	return myPid;
}

void TucanBbsFolder_i::setPid(CORBA::Long pid)
{
	myPid = pid;
}


CORBA::Boolean TucanBbsFolder_i::exists()
{
	TucanConfig* config = new TucanConfig();
	string dbname = config->get("dbname", "");
	string bbstable = config->get("bbsdbtable", "");
	delete config;
	int i = 0;

	connection* db = new connection(dbname);
	cursor* c = db->getCursor();

	c->execute("SELECT oid, * FROM " + bbstable + " WHERE oid=" + myName + " ORDER BY pdate, ptime");

	i = c->rowcount();

	db->close();
	c->close();

	delete db;
	delete c;

	return i;
}

CORBA::Boolean TucanBbsFolder_i::create()
{
	throw TucanException("Error: Method create() has not been implemented.");
}

CORBA::Boolean TucanBbsFolder_i::deleteFolder()
{
	TucanFolder_var folder;
	long i;

	if(!exists())
	{
		string exec_string = "Error: Folder " + myName + " does not exist.";
		throw TucanException(exec_string.c_str());
	}

	TucanStringList* msgList = listFolders();
	long count = getMessageCount();
	CORBA::Short boardId = getBid();

	for (i = 0; i < count; i++)
	{
		folder = getFolder((*msgList)[i]);
		folder->deleteFolder();
	}

	TucanConfig* config = new TucanConfig();
	string dbname = config->get("dbname", "");
	string tablename = config->get("bbsdbtable", "");
	delete config;

	connection* db = new connection(dbname);
	cursor* c = db->getCursor();

	string exec = "DELETE FROM " + tablename +
		" WHERE oid=" + myName;
	c->execute(exec);

	char temp[32];
	sprintf(temp, "%d", (boardId));
	string myBoardId(temp);

	if (getPid() == 0)
	{
		string update = "UPDATE " + tablename +
			" SET bid = bid - 1 WHERE bid > " +
			myBoardId;
		c->execute(update);
	}

	c->close();
	db->close();
	delete db;
	delete c;

	return 1;
}

TucanMessageSeq* TucanBbsFolder_i::expunge()
{
	throw TucanException("Error: Method expunge() has not been implemented.");
}

TucanFolder_ptr TucanBbsFolder_i::getFolder(const char* name)
{
	TucanConfig* config = new TucanConfig();
	string dbname = config->get("dbname", "");
	string bbstable = config->get("bbsdbtable", "");
	delete config;

	TucanFolder_var folder;
	connection* db = new connection(dbname);
	cursor* c = db->getCursor();
	string query;
	vector<string> res;

	query = "SELECT oid, subject FROM " + bbstable + " WHERE pid=" + myName + " and oid=" + name + " ORDER BY pdate, ptime";
	c->execute(query);

	if (!c->rowcount())
	{
		string exec;

		exec = "Error: Folder '";
		exec += name;
		exec += "' does not exist.";

		throw TucanException(exec.c_str());
	}

	res = c->fetchone();

	folder = getNewFolderObject();
	TucanBbsFolder_ptr the_folder = TucanBbsFolder::_narrow(folder);
	the_folder->setName(res[0].c_str());
	the_folder->setFullName(res[1].c_str());
	the_folder->setPid(atol(myName.c_str()));
	the_folder->setBid(myBid);

	db->close();
	c->close();
	res.clear();

	delete db;
	delete c;

	return the_folder;
}

void TucanBbsFolder_i::setParent(TucanFolder_ptr folder)
{
	myParent = TucanFolder::_narrow(folder);
	
}

TucanFolder_ptr TucanBbsFolder_i::getParent()
{
	return TucanFolder::_duplicate(myParent);

}

TucanFolder_ptr TucanBbsFolder_i::getNewFolderObject()
{
	TucanConfig* config = new TucanConfig();
	string filename = config->get("bbs", "folder");
	delete config;

	string str;
	TucanFolder_ptr folder;

	ifstream infile(filename.c_str());
	getline(infile, str);
	infile.close();

	CORBA::Object_var object = myOrb->string_to_object(str.c_str());
	TucanFolderFactory_var fact = TucanFolderFactory::_narrow(object);
	folder = fact->create();

	folder->setAuth(myAuth);

	return folder;
}

TucanStringList* TucanBbsFolder_i::listFolders()
{
	TucanStringList *list = new TucanStringList;

	TucanConfig* config = new TucanConfig();
	string dbname = config->get("dbname", "");
	string bbstable = config->get("bbsdbtable", "");
	delete config;

	connection* db = new connection(dbname);
	cursor* c = db->getCursor();
	string query;
	vector< vector<string> > res;

	query = "SELECT oid FROM " + bbstable + " WHERE pid=" + myName + " ORDER BY pdate, ptime";
	c->execute(query);
	
	res = c->fetchall();

	list->length(res.size());

	for (unsigned int i = 0; i < res.size(); i++)
		(*list)[i] = CORBA::string_dup(res[i][0].c_str());

	db->close();
	c->close();

	res.clear();

	delete db;
	delete c;

	return list;

}

TucanFolderSeq* TucanBbsFolder_i::list()
{
	throw TucanException("");
}

CORBA::Boolean TucanBbsFolder_i::renameTo(const char* name)
{
	throw TucanException("");

}

char* TucanBbsFolder_i::getName()
{
	return CORBA::string_dup(myName.c_str());
}

void TucanBbsFolder_i::setName(const char* name)
{
	myName = name;
}

char* TucanBbsFolder_i::getFullName()
{
	return CORBA::string_dup(myFullName.c_str());
}

void TucanBbsFolder_i::setFullName(const char* name)
{
	myFullName = name;
}

TucanMessage_ptr TucanBbsFolder_i::getMessage(CORBA::Long msgnum)
{

	if (!exists())
	{
		// Make sure we don't barf on this
		string exec = "Error: Folder '";
		exec += myName;
		exec += "' does not exist.";
		throw TucanException(exec.c_str());
	}
	
	TucanConfig* config = new TucanConfig();
	string dbname = config->get("dbname", "");
	string bbstable = config->get("bbsdbtable", "");
	delete config;

	TucanMessage_var message;
	connection* db = new connection(dbname);
	cursor* c = db->getCursor();
	string query;
	vector<string> res;

	char buf[16];

	sprintf(buf, "%ld", msgnum);

	query = "SELECT pid,subject,body,author,email,pdate,ptime,type FROM " + bbstable + " WHERE oid=";
	query += buf;
	query += " ORDER BY pdate, ptime";
	c->execute(query);

	if (!c->rowcount())
	{
		// Make sure we close up connections and such before we bounce out.
		c->close();
		db->close();

		delete c;
		delete db;

		throw TucanException("Error: Requested message out of range.");
	}

	res = c->fetchone();

	message = getNewMessageObject();

	TucanBbsMessage_ptr the_message = TucanBbsMessage::_narrow(message);
	the_message->setMessageNumber(msgnum);
	the_message->setSubject(res[1].c_str());

	string date = res[5] + " " + res[6];

	the_message->setSentDate(date.c_str());
	the_message->setReceivedDate(date.c_str());

	TucanData_var dob = the_message->getNewDataObject();

	dob->setContentType("text/plain");
	dob->setText(res[2].c_str());
	the_message->setDataObject(dob);

	TucanAddress_var addr = the_message->getNewAddressObject();
	addr->setAddress(res[4].c_str());

	the_message->setFrom(addr);
	the_message->setAuthor(res[3].c_str());

	the_message->setPid(atol(myName.c_str()));
	the_message->setBid(myBid);
	the_message->setAccessType(atoi(res[7].c_str()));

	db->close();
	c->close();
	res.clear();

	delete db;
	delete c;

	return the_message;
}

TucanMessageSeq* TucanBbsFolder_i::getMessages()
{
	long count;
	long i;
	
	count = getMessageCount();
	TucanMessageSeq* list = new TucanMessageSeq;
	TucanMessage_var message;
	TucanStringList* msgList = listFolders();
	CORBA::Long msgNumber;
	
	list->length(count);
	for (i = 0; i < count; i++)
	{
		msgNumber = atol((*msgList)[i]);
		message = getMessage(msgNumber);
		(*list)[i] = TucanMessage::_duplicate(message);
	}

	return list;
}

TucanMessageSeq* TucanBbsFolder_i::getNewMessages()
{
	return getMessages();
}

TucanMessageSeq* TucanBbsFolder_i::getMessagesInRange(CORBA::Long start, CORBA::Long stop)
{
	long count;
	long i;
	
	count = getMessageCount();
	TucanMessageSeq* list = new TucanMessageSeq;
	TucanMessage_var message;
	
	list->length(count);
	for (i = start; i <= stop; i++)
	{
		message = getMessage(i);
		(*list)[i] = TucanMessage::_duplicate(message);
	}

	return list;
}

TucanMessage_ptr TucanBbsFolder_i::getNewMessageObject()
{
	TucanConfig* config = new TucanConfig();
	string filename = config->get("bbs", "message");
	delete config;

	string str;
	TucanMessage_ptr message;

	ifstream infile(filename.c_str());
	getline(infile, str);
	infile.close();

	CORBA::Object_var object = myOrb->string_to_object(str.c_str());
	TucanMessageFactory_var fact = TucanMessageFactory::_narrow(object);
	message = fact->create();

	return message;
}

CORBA::Long TucanBbsFolder_i::getMessageCount()
{
	TucanConfig* config = new TucanConfig();
	string dbname = config->get("dbname", "");
	string bbstable = config->get("bbsdbtable", "");
	delete config;

	connection* db = new connection(dbname);
	cursor* c = db->getCursor();
	string query;

	query = "SELECT oid FROM " + bbstable + " WHERE pid=" + myName + " ORDER BY pdate, ptime";
	c->execute(query);

	CORBA::Long count = c->rowcount();
	
	db->close();
	c->close();

	delete db;
	delete c;

	return count;
}

CORBA::Long TucanBbsFolder_i::getNewMessageCount()
{
	return getMessageCount();
}

CORBA::Boolean TucanBbsFolder_i::hasNewMessages()
{
	if (getNewMessageCount())
		return 1;
	else
		return 0;
}

void TucanBbsFolder_i::moveMessage(CORBA::Long msgnum, TucanFolder_ptr folder)
{
	throw TucanException("");
}

void TucanBbsFolder_i::moveMessages(const TucanLongList& msgnums, TucanFolder_ptr folder)
{
	throw TucanException("");
}

void TucanBbsFolder_i::copyMessage(CORBA::Long msgnum, TucanFolder_ptr folder)
{
	throw TucanException("");
}

void TucanBbsFolder_i::copyMessages(const TucanLongList& msgnums, TucanFolder_ptr folder)
{
	throw TucanException("");
}

void TucanBbsFolder_i::appendMessage(TucanMessage_ptr msg)
{
	throw TucanException("");
}

void TucanBbsFolder_i::appendMessages(const TucanMessageSeq& msgs)
{
	throw TucanException("");
}

void TucanBbsFolder_i::deleteMessage(CORBA::Long msgnum)
{
	TucanConfig* config = new TucanConfig();
	string dbname = config->get("dbname", "");
	string tablename = config->get("bbsdbtable", "");
	delete config;

	connection* db = new connection(dbname);
	cursor* c = db->getCursor();
	char temp[32];
	sprintf(temp, "%ld", (msgnum));
	string msg_num = temp;

	string exec = "DELETE FROM " + tablename + " WHERE oid=" + temp;
	c->execute(exec);

	c->close();
	db->close();
	delete c;
	delete db;
}

TucanAuth_ptr TucanBbsFolder_i::getAuth()
{
	return TucanAuth::_duplicate(myAuth);
}

void TucanBbsFolder_i::setAuth(TucanAuth_ptr auth)
{
	myAuth = TucanAuth::_duplicate(auth);
}

void TucanBbsFolder_i::destroy()
{
	PortableServer::ObjectId_var id = myPoa->servant_to_id(this);
	myPoa->deactivate_object(id);

}

// Factory

TucanBbsFolderFactory_i::TucanBbsFolderFactory_i(CORBA::ORB_var orb)
{
	myOrb = orb;
	CORBA::Object_var obj = myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);

}

TucanFolder_ptr TucanBbsFolderFactory_i::create()
{
	TucanBbsFolder_i* myFolder = new TucanBbsFolder_i(myOrb);
	PortableServer::ObjectId_var myId = myPoa->activate_object(myFolder);
	CORBA::Object_var obj = myPoa->servant_to_reference(myFolder);
	myFolder->_remove_ref();

	return TucanBbsFolder::_narrow(obj);
}
