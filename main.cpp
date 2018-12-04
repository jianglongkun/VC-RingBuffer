#include "ThreadImpl.h"
#include "RingBufferImpl.h"
#include <fcntl.h>
#include <io.h>
#include <Shlwapi.h>
#pragma comment(lib,"Shlwapi.lib")

void InConsoleWindow()
{
	try
	{
		int nCrt = 0;
		FILE* fp;

#if 0
		AllocConsole();
		nCrt = _open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
		fp = _fdopen(nCrt, "w");
#else
		char LogFileName[1024] = { 0 };
		SYSTEMTIME Time = { 0 };//获取本地当前时间
		GetLocalTime(&Time);
		char NowFileName[256] = { 0 };
		if (!PathFileExistsA(".\\RunLog"))
		{
			CreateDirectoryA(".\\Runlog", 0);
		}
		sprintf(NowFileName, "%04d%02d%02d%02d%02d%02d", Time.wYear, Time.wMonth, Time.wDay, Time.wHour, Time.wMinute, Time.wSecond);
		strcpy(LogFileName, ".\\RunLog\\");
		strcat(LogFileName, NowFileName);
		strcat(LogFileName, ".log");
		fp = fopen(LogFileName, "a");
#endif

		*stdout = *fp;
		setvbuf(stdout, NULL, _IONBF, 0);
	}
	catch (...)
	{
		printf("%s->%d\n", __FUNCTION__, __LINE__);
	}
}


class WriteThreadDelegate :public ThreadDelegate
{
public:
	bool ThreadProcess(const ThreadDelegateParameterDescribe *pThreadDelegateParameterDescribe)
	{
		RingBufferImpl *pRingBufferImpl = (RingBufferImpl *)pThreadDelegateParameterDescribe->pUserObject;

		char Buffer[32 * 20] = { 0 };

		int nRet = pRingBufferImpl->GetWriteAvailable();
		if (nRet < 20)
		{
			int ElementCount = pRingBufferImpl->GetElementCount();

			int ElementSizeBytes = pRingBufferImpl->GetElementSizeBytes();

			int WriteOffset = pRingBufferImpl->GetWriteOffset();

			int ReadOffset = pRingBufferImpl->GetReadOffset();
			
			printf("[ThreadName:[%s]]->Buffer is full:[ElementCount:%d][ElementSizeBytes:%d][WriteOffset:%d][ReadOffset:%d][ReminWriteElement:%d]\n", pThreadDelegateParameterDescribe->ThreadName.c_str(), ElementCount, ElementSizeBytes, WriteOffset, ReadOffset,nRet);
		}
		else
		{
			int nWRet = pRingBufferImpl->Write(Buffer, 20);
			if (nWRet != 20)
			{
				int ElementCount = pRingBufferImpl->GetElementCount();

				int ElementSizeBytes = pRingBufferImpl->GetElementSizeBytes();

				int WriteOffset = pRingBufferImpl->GetWriteOffset();

				int ReadOffset = pRingBufferImpl->GetReadOffset();

				printf("[ThreadName:[%s]]->Buffer is full:[ElementCount:%d][ElementSizeBytes:%d][WriteOffset:%d][ReadOffset:%d][WriteElement:%d]\n", pThreadDelegateParameterDescribe->ThreadName.c_str(), ElementCount, ElementSizeBytes, WriteOffset, ReadOffset, nWRet);
			}
			else
			{
				int ElementCount = pRingBufferImpl->GetElementCount();

				int ElementSizeBytes = pRingBufferImpl->GetElementSizeBytes();

				int WriteOffset = pRingBufferImpl->GetWriteOffset();

				int ReadOffset = pRingBufferImpl->GetReadOffset();
				printf("[ThreadName:[%s]]->Write Success:[ElementCount:%d][ElementSizeBytes:%d][WriteOffset:%d][ReadOffset:%d][WriteElement:%d]\n", pThreadDelegateParameterDescribe->ThreadName.c_str(), ElementCount, ElementSizeBytes, WriteOffset, ReadOffset, nWRet);
			}
		}
		return true;
	}
};

class ReadThreadDelegate :public ThreadDelegate
{
public:
	bool ThreadProcess(const ThreadDelegateParameterDescribe *pThreadDelegateParameterDescribe)
	{
		RingBufferImpl *pRingBufferImpl = (RingBufferImpl *)pThreadDelegateParameterDescribe->pUserObject;
		char Buffer[32 * 20] = { 0 };

		int ReadyElementCount = pRingBufferImpl->GetElementCount();

		int ReadyElementSizeBytes = pRingBufferImpl->GetElementSizeBytes();

		int ReadyWriteOffset = pRingBufferImpl->GetWriteOffset();

		int ReadyReadOffset = pRingBufferImpl->GetReadOffset();

		printf("[ThreadName:[%s]]->Ready Read 20 ElementCount:[ElementCount:%d][ElementSizeBytes:%d][WriteOffset:%d][ReadOffset:%d]\n", pThreadDelegateParameterDescribe->ThreadName.c_str(), ReadyElementCount, ReadyElementSizeBytes, ReadyWriteOffset, ReadyReadOffset);

		int iReadAvailableRet = pRingBufferImpl->GetReadAvailable();

		if (iReadAvailableRet < 20)
		{
			int GetElementCount = pRingBufferImpl->GetElementCount();

			int GetElementSizeBytes = pRingBufferImpl->GetElementSizeBytes();

			int GetWriteOffset = pRingBufferImpl->GetWriteOffset();

			int GetReadOffset = pRingBufferImpl->GetReadOffset();

			printf("[ThreadName:[%s]]->Get 20 ElementCount fail:[ElementCount:%d][ElementSizeBytes:%d][WriteOffset:%d][ReadOffset:%d][ReminReadElement:%d]\n", pThreadDelegateParameterDescribe->ThreadName.c_str(), GetElementCount, GetElementSizeBytes, GetWriteOffset, GetReadOffset, iReadAvailableRet);
		}
		else
		{
			int iReadRet = pRingBufferImpl->Read(Buffer, 20);
			if (iReadRet != 20)
			{
				int ElementCount = pRingBufferImpl->GetElementCount();

				int ElementSizeBytes = pRingBufferImpl->GetElementSizeBytes();

				int WriteOffset = pRingBufferImpl->GetWriteOffset();

				int ReadOffset = pRingBufferImpl->GetReadOffset();

				printf("[ThreadName:[%s]]->Read 20 ElementCount fail:[ElementCount:%d][ElementSizeBytes:%d][WriteOffset:%d][ReadOffset:%d][ReadElement:%d]\n", pThreadDelegateParameterDescribe->ThreadName.c_str(), ElementCount, ElementSizeBytes, WriteOffset, ReadOffset, iReadRet);
			}
			else
			{
				int SuccessElementCount = pRingBufferImpl->GetElementCount();

				int SuccessElementSizeBytes = pRingBufferImpl->GetElementSizeBytes();

				int SuccessWriteOffset = pRingBufferImpl->GetWriteOffset();

				int SuccessReadOffset = pRingBufferImpl->GetReadOffset();
				printf("[ThreadName:[%s]]->Read 20 ElementCount Success:[ElementCount:%d][ElementSizeBytes:%d][WriteOffset:%d][ReadOffset:%d][ReadElement:%d]\n", pThreadDelegateParameterDescribe->ThreadName.c_str(), SuccessElementCount, SuccessElementSizeBytes, SuccessWriteOffset, SuccessReadOffset, iReadRet);
			}
		}
		return true;
	}
};

int main()
{
	InConsoleWindow();

	RingBufferImpl *pRingBufferImpl = new RingBufferImpl;

	pRingBufferImpl->Initialize(32, 100);

	for (int i = 0; i < 2; i++)
	{
		ThreadImpl *pThreadImpl = new ThreadImpl;

		WriteThreadDelegate *pWriteThreadDelegate = new WriteThreadDelegate;

		char ThreadName[256] = { 0 };
		sprintf(ThreadName, "WriteThread%d", i + 1);

		pThreadImpl->SetThreadDelegate(pWriteThreadDelegate, pRingBufferImpl, ThreadName);

		pThreadImpl->Start();
	}

	for (int i = 0; i < 2; i++)
	{
		ThreadImpl *pThreadImpl = new ThreadImpl;

		ReadThreadDelegate *pReadThreadDelegate = new ReadThreadDelegate;

		char ThreadName[256] = { 0 };
		sprintf(ThreadName, "ReadThread%d", i + 1);

		pThreadImpl->SetThreadDelegate(pReadThreadDelegate, pRingBufferImpl, ThreadName);

		pThreadImpl->Start();
	}

	while (1)
	{
		Sleep(1);
	}
	return 0;
}