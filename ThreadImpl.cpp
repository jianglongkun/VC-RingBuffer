#include "ThreadImpl.h"

ThreadImpl::ThreadImpl():
	m_pThreadDelegate(NULL),
	m_ThreadHandle(NULL),
	m_bIsRuning(false)
{

}

ThreadImpl::~ThreadImpl()
{
	Stop();
}

void ThreadImpl::SetThreadDelegate(ThreadDelegate *pThreadDelegate, void *pUserObject, TSTRING ThreadName)
{
	m_pThreadDelegate = pThreadDelegate;
	m_ThreadDelegateParameterDescribe.pUserObject = pUserObject;
	m_ThreadDelegateParameterDescribe.ThreadName = ThreadName;
}

bool ThreadImpl::Start()
{
	Stop();

	m_bIsRuning = true;

	m_ThreadHandle = (HANDLE)_beginthreadex(NULL, NULL, ThreadProcessFunction, this, NULL, &m_ThreadDelegateParameterDescribe.ThreadID);

	if (m_ThreadHandle == NULL)
	{
		m_bIsRuning = false;
		return false;
	}
	return true;
}

bool ThreadImpl::Stop()
{
	if (m_bIsRuning != true)
		return true;

	m_bIsRuning = false;

	if (m_ThreadHandle)
	{
		WaitForSingleObject(m_ThreadHandle, INFINITE);
		CloseHandle(m_ThreadHandle);
		m_ThreadHandle = NULL;
		m_ThreadDelegateParameterDescribe.ThreadID = 0;
	}
	return false;
}

bool ThreadImpl::IsStop()
{
	return m_bIsRuning;
}

bool ThreadImpl::IsStart()
{
	return m_bIsRuning;
}

void ThreadImpl::Terminate()
{
	if (m_bIsRuning != true)
		return;

	if (m_ThreadHandle)
	{
		TerminateThread(m_ThreadHandle, 0);
		CloseHandle(m_ThreadHandle);
		m_ThreadHandle = NULL;
		m_ThreadDelegateParameterDescribe.ThreadID = 0;
	}
	m_bIsRuning = false;
}

uint32_t __stdcall ThreadImpl::ThreadProcessFunction(void *lParam)
{
	ThreadImpl *pThreadImpl = (ThreadImpl *)lParam;

	while (pThreadImpl->m_bIsRuning)
	{
		if (pThreadImpl->m_pThreadDelegate)
		{
			if (pThreadImpl->m_pThreadDelegate->ThreadProcess(&(pThreadImpl->m_ThreadDelegateParameterDescribe)) == false)
			{
				break;
			}
		}
		Sleep(1);
	}
	pThreadImpl->m_bIsRuning = false;
	CloseHandle(pThreadImpl->m_ThreadHandle);
	pThreadImpl->m_ThreadHandle = NULL;
	pThreadImpl->m_ThreadDelegateParameterDescribe.ThreadID = 0;
	return 0;
}