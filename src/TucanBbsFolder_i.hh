#include <Tucan.hh>
#include <iostream.h>
#include <fstream.h>
#include <string>

class TucanBbsFolder_i: public POA_TucanBbsFolder,
	public PortableServer::RefCountServantBase 
{
	public:
		TucanBbsFolder_i(CORBA::ORB_var orb);
		virtual ~TucanBbsFolder_i() {};

		CORBA::Long getPid();
		void setPid(CORBA::Long pid);
		CORBA::Short getBid();
		void setBid(CORBA::Short bid);
		CORBA::Boolean exists();
		CORBA::Boolean create();
		CORBA::Boolean deleteFolder();
		TucanMessageSeq* expunge();
		TucanFolder_ptr getFolder(const char* name);
		void setParent(TucanFolder_ptr folder);
		TucanFolder_ptr getParent();
		TucanFolder_ptr getNewFolderObject();
		TucanStringList* listFolders();
		TucanFolderSeq* list();
		CORBA::Boolean renameTo(const char* name);
		char* getName();
		void setName(const char* name);
		char* getFullName();
		void setFullName(const char* name);
		TucanMessage_ptr getMessage(CORBA::Long msgnum);
		TucanMessageSeq* getMessages();
		TucanMessageSeq* getNewMessages();
		TucanMessageSeq* getMessagesInRange(CORBA::Long start, CORBA::Long stop);
		TucanMessage_ptr getNewMessageObject();
		CORBA::Long getMessageCount();
		CORBA::Long getNewMessageCount();
		CORBA::Boolean hasNewMessages();
		void moveMessage(CORBA::Long msgnum, TucanFolder_ptr folder);
		void moveMessages(const TucanLongList& msgnums, TucanFolder_ptr folder);
		void copyMessage(CORBA::Long msgnum, TucanFolder_ptr folder);
		void copyMessages(const TucanLongList& msgnums, TucanFolder_ptr folder);
		void appendMessage(TucanMessage_ptr msg);
		void appendMessages(const TucanMessageSeq& msgs);
		void deleteMessage(CORBA::Long msgnum);
		TucanAuth_ptr getAuth();
		void setAuth(TucanAuth_ptr auth);
		CORBA::Long getMessageFlags(CORBA::Long msgnum) { return 0; };
		void setMessageFlags(CORBA::Long msgnum, CORBA::Long flags){};
		void setMessageFlag(CORBA::Long msgnum, CORBA::Long flag) {};
		void unsetMessageFlag(CORBA::Long msgnum, CORBA::Long flag) {};
		void destroy();

	private:
		TucanAuth_var myAuth;
		string myName;
		string myFullName;
		CORBA::ORB_var myOrb;
		PortableServer::POA_var myPoa;
		TucanFolder_var myParent;
		CORBA::Long myPid;
		CORBA::Short myBid;
	
};

class TucanBbsFolderFactory_i: public POA_TucanBbsFolderFactory,
	public PortableServer::RefCountServantBase 
{

	public:
		TucanBbsFolderFactory_i(CORBA::ORB_var orb);
		virtual ~TucanBbsFolderFactory_i() {};

		TucanFolder_ptr create();

	private:
		CORBA::ORB_var myOrb;
		PortableServer::POA_var myPoa;
	
};

