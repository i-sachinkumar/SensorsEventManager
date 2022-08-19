#ifndef BAD_THREAD_H
#define BAD_THREAD_H

#include<exception>
#include<string>

using namespace std;

/**
 * This exception is to be thrown when the user is trying to interact with the ScopedThread
 * when it is no longer joinable.
 */
class bad_thread: public exception{
private:
	string message;

public:
	bad_thread():message{""}{}
	bad_thread(const string& str) {
		message = str;
	}
	bad_thread(const char* str){
		message = str;
	}

	virtual ~bad_thread(){}

	virtual const char* what() const throw(){
		return "Thread/ThreadPool is no longer joinable";
	}

	string getMessage() const throw(){
		return message;
	}

};

#endif
