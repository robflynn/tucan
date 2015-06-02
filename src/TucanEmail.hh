#ifndef __TUCAN_EMAIL_H_
#define __TUCAN_EMAIL_H_

#include <string>

#define MAXSIZE 1024 * 32

class TucanEmail
{
	 public:
                virtual void open(string username, string password) = 0;
                virtual void close() = 0;
                virtual int getMessageCount() = 0;
                virtual int getMessageSize(int mnum) = 0;
                virtual string getMessage(int mnum) = 0;
                virtual string getMessageId(int mnum) = 0;
                virtual void deleteAllMessages() = 0;
                virtual void deleteMessage(int mnum) = 0;
};


#endif
