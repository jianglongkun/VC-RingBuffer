#pragma once
#ifndef __RINGBUFFERIMPL__H_
#define __RINGBUFFERIMPL__H_

#include "UtilsDefine.h"

using namespace Utils;

class RingBufferImpl
{
public:
	RingBufferImpl();

	~RingBufferImpl();

	bool Initialize(int32_t ElementSizeBytes, int32_t ElementCount);

	void Unitialize();

	void Reset();

	int32_t GetElementCount();

	int32_t GetElementSizeBytes();

	int32_t GetWriteOffset();

	int32_t GetReadOffset();

	int32_t GetWriteAvailable();

	int32_t GetReadAvailable();

	int32_t Write(void *pData, int32_t ElementCount);

	int32_t Read(void *pData, int32_t ElementCount);
private:
	bool IsRingBufferValid();

	bool UpdateWriteOffset(int32_t ElementCount);

	bool UpdateReadOffset(int32_t ElementCount);
private:
	typedef struct RingBufferInfoDescribe
	{
		int32_t ElementCount;//Ԫ�ص�������
		int32_t ElementSizeBytes;//ÿ��Ԫ�ص��ֽ���
		volatile int32_t WriteOffset;//дƫ��
		volatile int32_t ReadOffset;//��ƫ��
		char *pBuffer;//���ݻ�����
		RingBufferInfoDescribe() ://�ṹ��ʼ��
			ElementCount(0),
			ElementSizeBytes(0),
			WriteOffset(0),
			ReadOffset(0),
			pBuffer(NULL) {}
	}RINGBUFFERINFODESCRIBE,*LPRINGBUFFERINFODESCRIBE;

	RINGBUFFERINFODESCRIBE m_RingBufferInfoDescribe;
};

#endif