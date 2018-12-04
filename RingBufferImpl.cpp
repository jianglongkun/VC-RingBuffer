#include "RingBufferImpl.h"

RingBufferImpl::RingBufferImpl()
{

}

RingBufferImpl::~RingBufferImpl()
{
	Unitialize();
}

bool RingBufferImpl::Initialize(int32_t ElementSizeBytes, int32_t ElementCount)
{
	if (ElementCount <= 0)
		return false;

	if (ElementSizeBytes <= 0)
		return false;

	Unitialize();
	m_RingBufferInfoDescribe.ElementCount = ElementCount;
	m_RingBufferInfoDescribe.ElementSizeBytes = ElementSizeBytes;
	m_RingBufferInfoDescribe.pBuffer = new char[ElementCount * ElementSizeBytes];
	if (m_RingBufferInfoDescribe.pBuffer == NULL)
		return false;
	return true;
}

void RingBufferImpl::Unitialize()
{
	Reset();
	m_RingBufferInfoDescribe.ElementCount = 0;
	m_RingBufferInfoDescribe.ElementSizeBytes = 0;
	if (m_RingBufferInfoDescribe.pBuffer)
	{
		delete[]m_RingBufferInfoDescribe.pBuffer;
		m_RingBufferInfoDescribe.pBuffer = NULL;
	}
}

void RingBufferImpl::Reset()
{
	m_RingBufferInfoDescribe.WriteOffset = m_RingBufferInfoDescribe.ReadOffset = 0;
}

int32_t RingBufferImpl::GetElementCount()
{
	return m_RingBufferInfoDescribe.ElementCount;
}

int32_t RingBufferImpl::GetElementSizeBytes()
{
	return m_RingBufferInfoDescribe.ElementSizeBytes;
}

int32_t RingBufferImpl::GetWriteOffset()
{
	return m_RingBufferInfoDescribe.WriteOffset;
}

int32_t RingBufferImpl::GetReadOffset()
{
	return m_RingBufferInfoDescribe.ReadOffset;
}

int32_t RingBufferImpl::GetWriteAvailable()
{
	return m_RingBufferInfoDescribe.ElementCount - GetReadAvailable();
}

int32_t RingBufferImpl::GetReadAvailable()
{
	if (m_RingBufferInfoDescribe.WriteOffset >= m_RingBufferInfoDescribe.ReadOffset)
		return m_RingBufferInfoDescribe.WriteOffset - m_RingBufferInfoDescribe.ReadOffset;
	else
		return m_RingBufferInfoDescribe.ElementCount - m_RingBufferInfoDescribe.ReadOffset + m_RingBufferInfoDescribe.WriteOffset;
}

int32_t RingBufferImpl::Write(void *pData, int32_t ElementCount)
{
	if (IsRingBufferValid() == false)
		return 0;

	if (pData == NULL || ElementCount <= 0 || ElementCount>m_RingBufferInfoDescribe.ElementCount)
		return 0;

	int WriteAvailableElement = GetWriteAvailable();//获取当前可写的元素数量

	if (WriteAvailableElement <= 0)//当前没有可写的元素数量(缓冲区已满)
		return 0;

	if (WriteAvailableElement < ElementCount)//当前可写的元素数量小于要写入的元素数量
		return 0;

	int WriteOffset = m_RingBufferInfoDescribe.WriteOffset;//获取当前写的偏移位置
	FullMemoryBarrier();
	//不连续内存写入
	if ((WriteOffset + ElementCount) > m_RingBufferInfoDescribe.ElementCount)
	{
		int iLeftElementCount = m_RingBufferInfoDescribe.ElementCount - WriteOffset;//获取左边的缓冲区大小

		memcpy(&m_RingBufferInfoDescribe.pBuffer[WriteOffset * m_RingBufferInfoDescribe.ElementSizeBytes], pData, iLeftElementCount*m_RingBufferInfoDescribe.ElementSizeBytes);

		memcpy(&m_RingBufferInfoDescribe.pBuffer[0], (char *)pData + iLeftElementCount*m_RingBufferInfoDescribe.ElementSizeBytes, (ElementCount - iLeftElementCount)*m_RingBufferInfoDescribe.ElementSizeBytes);
	}
	else
	{
		memcpy(&m_RingBufferInfoDescribe.pBuffer[WriteOffset * m_RingBufferInfoDescribe.ElementSizeBytes], pData, ElementCount*m_RingBufferInfoDescribe.ElementSizeBytes);
	}

	UpdateWriteOffset(ElementCount);
	return ElementCount;
}

int32_t RingBufferImpl::Read(void *pData, int32_t ElementCount)
{
	if (IsRingBufferValid() == false)
		return 0;

	if (pData == NULL || ElementCount <= 0 || ElementCount>m_RingBufferInfoDescribe.ElementCount)
		return 0;

	int ReadAvailableElement = GetReadAvailable();//获取当前可读的元素数量

	if (ReadAvailableElement <= 0)//当前没有可读的元素数量(缓冲区已满)
		return 0;

	if (ReadAvailableElement < ElementCount)//当前可读的元素数量小于要读入的元素数量
		return 0;
	ReadMemoryBarrier();
	int ReadOffset = m_RingBufferInfoDescribe.ReadOffset;

	if ((ReadOffset + ElementCount) > m_RingBufferInfoDescribe.ElementCount)
	{
		int iLeftElementCount = m_RingBufferInfoDescribe.ElementCount - ReadOffset;//获取左边的缓冲区大小

		memcpy(pData, &m_RingBufferInfoDescribe.pBuffer[ReadOffset * m_RingBufferInfoDescribe.ElementSizeBytes], iLeftElementCount*m_RingBufferInfoDescribe.ElementSizeBytes);

		memcpy((char *)pData + (iLeftElementCount*m_RingBufferInfoDescribe.ElementSizeBytes), &m_RingBufferInfoDescribe.pBuffer[0], (ElementCount - iLeftElementCount)*m_RingBufferInfoDescribe.ElementSizeBytes);
	}
	else
	{
		memcpy(pData, &m_RingBufferInfoDescribe.pBuffer[ReadOffset * m_RingBufferInfoDescribe.ElementSizeBytes], ElementCount*m_RingBufferInfoDescribe.ElementSizeBytes);
	}

	UpdateReadOffset(ElementCount);
	return ElementCount;
}

bool RingBufferImpl::IsRingBufferValid()
{
	if (m_RingBufferInfoDescribe.ElementCount <= 0)
		return false;

	if (m_RingBufferInfoDescribe.ElementSizeBytes <= 0)
		return false;

	if (m_RingBufferInfoDescribe.pBuffer == NULL)
		return false;
	return true;
}

bool RingBufferImpl::UpdateWriteOffset(int32_t ElementCount)
{
	WriteMemoryBarrier();
	m_RingBufferInfoDescribe.WriteOffset = (m_RingBufferInfoDescribe.WriteOffset + ElementCount) % m_RingBufferInfoDescribe.ElementCount;
	return true;
}

bool RingBufferImpl::UpdateReadOffset(int32_t ElementCount)
{
	FullMemoryBarrier();
	m_RingBufferInfoDescribe.ReadOffset = (m_RingBufferInfoDescribe.ReadOffset + ElementCount) % m_RingBufferInfoDescribe.ElementCount;
	return true;
}