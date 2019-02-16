#ifndef _KOMMANDER_TOOL_SINGLETON_H_
#define _KOMMANDER_TOOL_SINGLETON_H_

#include <functional>
#include <mutex>
using namespace std;

//#include <QQueue>

//typedef std::function<void()> DelSingletonAction;
//typedef QQueue<DelSingletonAction> SingletonDeleteQueue;

//void pushSingletonAction(DelSingletonAction delAction);
//void clearSingleton();

template<class T>
class KSingleton
{
public:
	template<typename... Args>
	static T*	instance(Args&&... args)
	{
		static T* _instance = nullptr;
		static std::once_flag oc;
		std::call_once(oc, [&] {
			_instance = new T(std::forward<Args>(args)...);
		});
		return _instance;
	}

private:
	KSingleton() {}
	virtual ~KSingleton() {}
	KSingleton(const KSingleton& st) {}
	KSingleton &operator=(const KSingleton& st) {}
};

#define SINGLETON(x,...) KSingleton<x>::instance(__VA_ARGS__)
#define SINGLETONDELETE(x) { delete SINGLETON(x); }
#define DECLARESINGLETON(clsname) friend class KSingleton<clsname>;
#define CLEARSINGLETON	{ clearSingleton(); }
#endif


