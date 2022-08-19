//============================================================================
// Name        : scoped_thread.h
// Author      : Auro
// Version     :
// Copyright   :
// Description : Persistent Thread Library
//============================================================================

#ifndef SCOPED_THREAD_H
#define SCOPED_THREAD_H

#include<thread>
#include<shared_mutex>
#include<condition_variable>
#include<queue>
#include<functional>
#include<future>
#include"bad_thread.h"

using namespace std;

/**
 * Used to create a persistent thread that lasts until the ScopedThread variable goes out of scope
 * or join() or shutdown() is called.
 */
class ScopedThread{
private:

	thread t;

	//threadMutex is for controlling access to condition, terminate_thread
	//and tasks.
	mutex threadMutex;

	condition_variable condition;
	bool terminate_thread = false;
	queue<function<void()>> tasks;

	//isJoinable is only accessed via the caller's thread so no
	//need for its mutex.
	bool isJoinable = true;

	//To distinguish one thread from another.
	unsigned int id;

	//For counting thread ids.
	static unsigned int counter;

	/**
	 * Keeps the thread active until terminated.
	 */
	void InfiniteLoopFunction(){
		function<void()> func;
		while(true){
			{
				unique_lock<mutex> lock(threadMutex);

				//This condition is checked on the first iteration of the InfiniteLoopFunction
				//and whenever condition variable is notified. Condition variable is notified as
				//soon as a function is pushed to tasks. The predicate guarantees that lost wakeup
				//or spurious wakeup does not cause any problems.
				//In the wildest of scenarios, if a double lost wakeup takes place, then
				//there would always be a task in the function that was not executed.(I can
				//safely neglect this possibility as it would probably never happen).
				condition.wait(lock,[this](){
					return (!tasks.empty() || terminate_thread);
				});

				if(terminate_thread){
					//At this point the tasks queue is empty and we need to break out of this loop.
					break;
				}

				func = tasks.front();
				tasks.pop();
			}
			func();
		}
	}

public:
	/**
	 * Default  constructor
	 */
	ScopedThread(){
		counter++;
		id = counter;		//ids start from 1,2,3...  and so on.
		t = move(thread(&ScopedThread::InfiniteLoopFunction,this));
		//Log here.
	}

	//Threads shouldn't be copy constructible or copy assignable
	ScopedThread(const ScopedThread&) = delete;
	ScopedThread& operator=(const ScopedThread&) = delete;

	/**
	 * I can see no use cases where move constructor would be required.
	 */
	ScopedThread(ScopedThread&& scoped_thread)=delete;

	/**
	 * If move constructor isn't required there is no need for move assignment.
	 */
	ScopedThread& operator=(ScopedThread&& other)=delete;

	/**
	 * Destructor  automatically checks if the thread was joinable. If it was then shutdown() is
	 * automatically called.
	 */
	~ScopedThread(){
		if(isJoinable){
			shutdown();
		}
		//Log here
	}

	/**
	 * Can post lambdas, Function Objects as well as functions. The syntax is exactly the same
	 * as if passing to std::thread. There is one exception to this rule.
	 * The callable is internally bound to a  function<void()> so the type cannot be moveable.
	 * Thus you cannot pass parameters using move semantics (rvalue references).
	 * Throws bad_thread exception if shutdown or join was already called.
	 * If you want to return a value from the callable it is recommended to use submit().
	 */
	template<typename Function, typename ...Args>
	void post(Function&& f, Args&&... args){
		if(isJoinable){
			function<void()> func = bind(forward<Function>(f),forward<Args>(args)...);
			{
				unique_lock<mutex> lock(threadMutex);
				tasks.push(func);
			}
			condition.notify_one();
		}else{
			bad_thread e("Cannot call post() on a thread that is no longer joinable.");
			throw e;
		}

	}

	/**
	 * Works exactly the same as post(). But with callables that return a value. The callable is
	 * internally bound to a std::packaged_task and a future is returned. User can access the return
	 * value of the callable once by calling the get() method of std::future.
	 * The calling thread will not block unless get() is called on the future returned by the submit().
	 */
	template<typename Function, typename ...Args>
	auto submit(Function&& f, Args&&... args) -> future<decltype(f(args...))> {

		if(!isJoinable){
			bad_thread e("Cannot call submit() on a thread that is no longer joinable");
			throw e;
		}

	    // Create a function with bounded parameters ready to execute
	    function<decltype(f(args...))()> func = bind(forward<Function>(f), forward<Args>(args)...);
	    // Encapsulate it into a shared ptr in order to be able to copy construct / assign
	    auto task_ptr = make_shared<packaged_task<decltype(f(args...))()>>(func);

	    // Wrap packaged task into void function
	    function<void()> wrapper_func = [task_ptr]() {
	      (*task_ptr)();
	    };

	    {
	    	unique_lock<mutex> lock(threadMutex);
	        tasks.push(wrapper_func);
	    }

	    condition.notify_one();

	    // Return future from promise
	    return task_ptr->get_future();
	 }

	/**
	 * Completes the current job and terminates the thread.
	 * Whenever we call shutdown there are two possibilities:
	 * 1. The thread is currently performing a job and the threadMutex is unlocked.
	 * 2. There is no task under execution and the thread is waiting for notification
	 * 	  of the condition variable. The threadMutex is unlocked but the thread is blocked(can't join it).
	 *
	 * In case 1, we simply  acquire the threadMutex, pop all the tasks and join the thread.
	 *
	 * In case 2, also acquire the threadMutex and pop all the tasks.
	 * Then we notify the condition variable by terminate_thread
	 *
	 * Calls bad_thread exception if the Scoped Thread was no longer joinable.
	 */
	void shutdown(){

		if(!isJoinable){	//if join or shutdown was already called on this thread
			bad_thread e("Cannot call shutdown() on a thread that is no longer joinable.");
			throw e;
		}
		isJoinable = false;

		{
			unique_lock<mutex> lock(threadMutex);

			while(!tasks.empty()){
				//Access gained when tasks is not empty.
				//A particular task would be under execution by the thread as of this moment.
				//We empty  out the rest of the tasks meaning on freeing of this mutex,
				//there would be no tasks to execute.
				tasks.pop();
			}

			terminate_thread = true;

			condition.notify_one();
		}

		//Ensure completion of current task under execution
		//Thread must be joined otherwise it would be terminated.
		if(t.joinable()){
			t.join();
		}
	}

	/**
	 * Executes all the pending tasks in queue. The calling thread is blocked until all
	 * tasks in queue are executed.
	 * Throws bad_thread exception if shutdown() or join() was already called.
	 */
	void join(){

		if(!isJoinable){	//if join or shutdown was already called on this thread
			bad_thread e("Cannot call join() on a thread that is no longer joinable.");
			throw e;
		}
		isJoinable = false;

		while(true){	//blocks the caller thread until tasks is empty.
			{
				unique_lock<mutex> lock(threadMutex);
				if(tasks.empty()){
					terminate_thread = true;
					break;
				}
			}	//free the lock to avoid deadlock
		}

		condition.notify_one();

		//Ensure completion of the InfiniteLoopFunction.
		//Thread must be joined otherwise it would be terminated.
		if(t.joinable()){
			t.join();
		}
	}

	/**
	 * Return the unique id of the ScopedThread.
	 * This id is not equal to the thread::getId() of the implicit thread.
	 */
	unsigned int getId() const{
		return id;
	}
};

unsigned int ScopedThread::counter = 0;

#endif
