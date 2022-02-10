// BF_AIO_WorkBase.cpp: implementation of the CBF_AIO_WorkBase class.
//
//////////////////////////////////////////////////////////////////////

#include "BF_AIO_WorkBase.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBF_AIO_WorkBase::CBF_AIO_WorkBase()
{
	m_pAioMethod = NULL;
	m_pAioPool = NULL;
}

CBF_AIO_WorkBase::~CBF_AIO_WorkBase()
{

}

void CBF_AIO_WorkBase::SetParameter(CBF_AIO_Base *p,CBF_AIO_Pool   *pool)
{
	m_pAioMethod = p;
	m_pAioPool = pool;
}
