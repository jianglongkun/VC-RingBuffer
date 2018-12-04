#pragma once
#ifndef __THREADIMPL__H_
#define __THREADIMPL__H_

#include <windows.h>
#include <process.h>
#include <tchar.h>
#include <string>
#include <stdint.h>

using namespace std;

#ifdef UNICODE
typedef wstring TSTRING;
#else
typedef string TSTRING;
#endif

class ThreadDelegate
{
public:
	typedef struct ThreadDelegateParameterDescribe
	{
		uint32_t ThreadID;
		TSTRING ThreadName;
		void *pUserObject;
		ThreadDelegateParameterDescribe() :
			ThreadID(0),
			ThreadName(_T("")),
			pUserObject(NULL) {}
	};
public:
	virtual bool ThreadProcess(const ThreadDelegateParameterDescribe *pThreadDelegateParameterDescribe)=0;
};

class ThreadImpl
{
public:
	ThreadImpl();

	~ThreadImpl();

	void SetThreadDelegate(ThreadDelegate *pThreadDelegate,void *pUserObject,TSTRING ThreadName);

	bool Start();

	bool Stop();

	bool IsStop();

	bool IsStart();

	void Terminate();
private:
	static uint32_t __stdcall ThreadProcessFunction(void *lParam);
private:
	ThreadDelegate *m_pThreadDelegate;

	HANDLE m_ThreadHandle;

	bool m_bIsRuning;

	ThreadDelegate::ThreadDelegateParameterDescribe m_ThreadDelegateParameterDescribe;
};

#endif