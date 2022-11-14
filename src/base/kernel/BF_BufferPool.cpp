// BF_BufferPool.cpp: implementation of the CBF_BufferPool class.
//
//////////////////////////////////////////////////////////////////////

#include "BF_BufferPool.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CBF_BufferPool::CBF_BufferPool(int mallocsize)
{
	m_size=mallocsize;
	m_total=0;
	m_poolmalloc = NULL;
	m_nInitSize = 0;
	m_nMaxFreeSize = 100;

}

CBF_BufferPool::~CBF_BufferPool()
{
// 	while(m_pTotalBuffer.size() > 0)
// 	{
// 		m_poolmalloc=m_pTotalBuffer.top();
// 		m_pTotalBuffer.pop();
// 		if (m_poolmalloc != NULL)
// 		{
// 			free(m_poolmalloc);
// 		}
// 	}
	bool bRet = m_pTotalBuffer_new.First(m_poolmalloc);
	while (bRet)
	{
		if (m_poolmalloc != NULL)
		{
			free(m_poolmalloc);
		}
		bRet = m_pTotalBuffer_new.Next(m_poolmalloc);
	}
	m_poolmalloc = NULL;
}

bool CBF_BufferPool::Init(int size,int mallocsize,int maxfreesize)
{
	int i;
	m_size = mallocsize;
	m_nInitSize = size;
	
	if (maxfreesize == 0 || maxfreesize <size)
	{
		m_nMaxFreeSize = size*100;
	}
	else 
	{
		m_nMaxFreeSize = maxfreesize;
	}
	
	int rid=0;
	m_total=0;
	for (i=0; i<size; i++)
	{
		m_poolmalloc=(char *)malloc(m_size);
		if (m_poolmalloc == NULL)
		{
			return false;
		}
		bzero(m_poolmalloc,m_size);
		m_total++;
		m_stack.push(m_poolmalloc);
//		m_pTotalBuffer.push(m_poolmalloc);
		rid = m_pTotalBuffer_new.Add(m_poolmalloc);
		m_pKeyAddr.Add(rid,(UINT64_)m_poolmalloc);
// #ifdef _WINDOWS
// 		printf("CBF_BufferPool Add %I64d\n",(UINT64_)m_poolmalloc);
// #else
// 		printf("CBF_BufferPool Add %lld\n",(UINT64_)m_poolmalloc);
// #endif		

	}
	return true;
}

void *CBF_BufferPool::PoolMalloc()
{
	CBF_PMutex pp(&m_mutex);
	//判断是否要释放一些空间
	int rid=0;
	int nFreeNum=m_stack.size();

	nFreeNum = nFreeNum - m_nMaxFreeSize;
	if (nFreeNum >0)
	{
		CInt iset;
		for (int i=0; i< nFreeNum; i++)
		{
			m_poolmalloc=m_stack.top();
			m_stack.pop();
			iset.Clear();
			m_total--;
			if (m_pKeyAddr.Select(iset,(UINT64_)m_poolmalloc))
			{
				if (iset.First(rid))
				{
					m_pTotalBuffer_new.Delete(rid);
					m_pKeyAddr.Delete((UINT64_)m_poolmalloc);
					free(m_poolmalloc);
					
				}
				else
				{
#ifdef _WINDOWS
					printf("CBF_BufferPool free First error %I64d\n",(UINT64_)m_poolmalloc);
#else
					printf("CBF_BufferPool free First error %lld\n",(UINT64_)m_poolmalloc);
#endif
				}
				
			}
			else
			{
#ifdef _WINDOWS
				printf("CBF_BufferPool free Select error %I64d\n",(UINT64_)m_poolmalloc);
#else
				printf("CBF_BufferPool free Select error %lld\n",(UINT64_)m_poolmalloc);
#endif
			}
		}
	}
	

	if (m_stack.size() > 0)
	{
		m_poolmalloc=m_stack.top();
		m_stack.pop();
		//memset(m_poolmalloc,0,m_size); //很耗时
	}
	else
	{
		m_poolmalloc=(char *)malloc(m_size);
		if (m_poolmalloc != NULL)
		{
			//memset(m_poolmalloc,0,m_size);//很耗时
			m_total++;
//			m_pTotalBuffer.push(m_poolmalloc);
			rid = m_pTotalBuffer_new.Add(m_poolmalloc);
			m_pKeyAddr.Add(rid,(UINT64_)m_poolmalloc);
			
		}
		else
		{
			return NULL;
		}
	}
	return m_poolmalloc;
}

void CBF_BufferPool::PoolFree(void *trunck)
{
	CBF_PMutex pp(&m_mutex);
	m_stack.push(trunck);
}

void CBF_BufferPool::PoolInfo(int &total,int &unused)
{
	CBF_PMutex pp(&m_mutex);
	total=m_total;
	unused=m_stack.size();
}

int  CBF_BufferPool::PoolInuse()
{
	CBF_PMutex pp(&m_mutex);
	return m_total-m_stack.size();
}

