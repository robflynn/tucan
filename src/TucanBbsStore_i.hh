#include <iostream.h>
#include <fstream.h>
#include <Tucan.hh>
#include <string>

class TucanBbsStore_i: public POA_TucanBbsStore,
	public PortableServer::RefCountServantBase 
{
public:
	TucanBbsStore_i(CORBA::ORB_var orb);
	virtual ~TucanBbsStore_i() {};

	char* getName();
	void setName(const char* name);
	TucanFolder_ptr getDefaultFolder();
	TucanFolder_ptr getFolder(const char* name);
	TucanFolder_ptr getNewFolderObject();
	TucanStringList* listFolders();
	TucanAuth_ptr getAuth();
	void setAuth(TucanAuth_ptr auth);
	void destroy();
private:
	CORBA::ORB_var myOrb;
	PortableServer::POA_var myPoa;
	TucanAuth_var myAuth;
	string myName;	
	
};

class TucanBbsStoreFactory_i: public POA_TucanBbsStoreFactory,
	public PortableServer::RefCountServantBase 
{

public:
	TucanBbsStoreFactory_i(CORBA::ORB_var orb);
	virtual ~TucanBbsStoreFactory_i() {};

	TucanStore_ptr create();
	
private:
	CORBA::ORB_var myOrb;
	PortableServer::POA_var myPoa;
};
	

