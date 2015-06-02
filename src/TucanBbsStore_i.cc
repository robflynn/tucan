#include <TucanBbsStore_i.hh>
#include <TucanConfig.hh>
#include <TucanDb.hh>

TucanBbsStore_i::TucanBbsStore_i(CORBA::ORB_var orb)
{
	myOrb = orb;
	CORBA::Object_var obj = myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);
	
	myName = "";
}

char* TucanBbsStore_i::getName()
{
	return CORBA::string_dup(myName.c_str());
}

void TucanBbsStore_i::setName(const char* name)
{
	myName = name;
}

TucanFolder_ptr TucanBbsStore_i::getDefaultFolder()
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

	query = "SELECT oid, subject, bid FROM " + bbstable + " WHERE pid=0 ORDER BY pdate, ptime";
	c->execute(query);
	
	if (!c->rowcount())
	{
		string exec;
		exec = "Error: Default folder does not exist.'";

		throw TucanException(exec.c_str());
	}

	res = c->fetchone();

	folder = getNewFolderObject();
	TucanBbsFolder_ptr the_folder = TucanBbsFolder::_narrow(folder);

	the_folder->setName(res[0].c_str());
	the_folder->setFullName(res[1].c_str());
	the_folder->setPid(0);
	the_folder->setBid(atoi(res[2].c_str()));

	db->close();
	c->close();
	res.clear();

	delete db;
	delete c;

	return the_folder;
}

TucanFolder_ptr TucanBbsStore_i::getFolder(const char* name)
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

	query = "SELECT oid, pid, subject, bid FROM " + bbstable + " WHERE oid=" + name + " ORDER BY pdate, ptime";
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
	the_folder->setPid(atoi(res[1].c_str()));
	the_folder->setBid(atoi(res[3].c_str()));
	the_folder->setFullName(res[2].c_str());

	db->close();
	c->close();
	res.clear();

	delete db;
	delete c;

	return the_folder;
}

TucanFolder_ptr TucanBbsStore_i::getNewFolderObject()
{
	string filename;
	string str;
	TucanFolder_ptr folder;
	
	TucanConfig* config = new TucanConfig();
	filename = config->get("bbs", "folder");
	delete config;

	ifstream infile(filename.c_str());
	getline(infile, str);
	infile.close();

	CORBA::Object_var object = myOrb->string_to_object(str.c_str());
	TucanFolderFactory_var fact = TucanFolderFactory::_narrow(object);
	folder = fact->create();
	folder->setAuth(myAuth);

	return folder;
}

TucanStringList* TucanBbsStore_i::listFolders()
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

	query = "SELECT oid FROM " + bbstable + " WHERE pid=0 ORDER BY pdate, ptime";
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

TucanAuth_ptr TucanBbsStore_i::getAuth()
{
	return TucanAuth::_duplicate(myAuth);
}

void TucanBbsStore_i::setAuth(TucanAuth_ptr auth)
{
	myAuth = TucanAuth::_duplicate(auth);
}

void TucanBbsStore_i::destroy()
{
	PortableServer::ObjectId_var id = myPoa->servant_to_id(this);
	myPoa->deactivate_object(id);

}

TucanBbsStoreFactory_i::TucanBbsStoreFactory_i(CORBA::ORB_var orb)
{
	myOrb = orb;
	CORBA::Object_var obj = myOrb->resolve_initial_references("RootPOA");
	myPoa = PortableServer::POA::_narrow(obj);
}

TucanStore_ptr TucanBbsStoreFactory_i::create()
{
	TucanBbsStore_i* myBbsStore = new TucanBbsStore_i(myOrb);
	PortableServer::ObjectId_var myBbsStoreId = myPoa->activate_object(myBbsStore);
	CORBA::Object_var obj = myPoa->servant_to_reference(myBbsStore);
	myBbsStore->_remove_ref();

	return TucanBbsStore::_narrow(obj);
}
