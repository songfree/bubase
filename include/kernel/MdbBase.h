// MdbBase.h: interface for the CMdbBase class.
//
//////////////////////////////////////////////////////////////////////

/***********************************************************************************
  编码日志记录
  编码者: 王明松
  编码日期 2010-4-26 9:43:25
  版本: V1.00 
  说明: 使用STL构建的一个内存数据模板类，简单的内存库，没有事务
 ***********************************************************************************/


#if !defined(AFX_MDBBASE_H__B2EF1D82_10E5_4C0B_B6EF_A98703B25A7C__INCLUDED_)
#define AFX_MDBBASE_H__B2EF1D82_10E5_4C0B_B6EF_A98703B25A7C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "BF_Tools.h"
#include "typedefine.h"
#include <set>
#include <vector>
#include <stack>
#include <list>
#include <queue>
#include <stdarg.h>

#include <unordered_set>
#include <functional>	

#include <string>
#include <cstring>
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <algorithm>
#include <atomic>

#ifdef _WIN32
#include <windows.h>
#else
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

 // from boost (functional/hash):
 // see http://www.boost.org/doc/libs/1_35_0/doc/html/hash/combine.html
template <typename T>
inline void hash_combine(std::size_t& seed, const T& val)
{
    seed ^= std::hash<T>()(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

// auxiliary generic functions to create a hash value using a seed
template <typename T>
inline void hash_val(std::size_t& seed, const T& val)
{
    hash_combine(seed, val);
}
template <typename T, typename... Types>
inline void hash_val(std::size_t& seed,
    const T& val, const Types&... args)
{
    hash_combine(seed, val);
    hash_val(seed, args...);
}

// auxiliary generic function to create a hash value out of a heterogeneous list of arguments
template <typename... Types>
inline std::size_t hash_val(const Types&... args)
{
    std::size_t seed = 0;
    hash_val(seed, args...);
    return seed;
}





#if defined(WIN32)
#include <io.h>
#else
#define O_BINARY 0
#endif



#ifdef WIN32
#pragma warning(disable:4786) 
#endif


//不重复的索引基类
template<typename Record,typename Compare>
class CUniqueIndexBase
{
public:
	typedef std::set<Record,Compare> SET_;
	typedef typename SET_::iterator LP_SET;
	typedef typename SET_::reverse_iterator LP_SET_R;
	SET_      m_index;//索引数据，Record不可能重复
	LP_SET    m_current;//当前索引
	LP_SET_R  m_current_r;//反向当前索引
    bool         m_bDroped;	 //是否删除操作
	bool      m_bIsreverse;	 //是否反向迭代
public:
	CUniqueIndexBase()
	{
		m_bDroped = false;
		m_bIsreverse = false;
	}
	~CUniqueIndexBase()
	{
		Clear();
	}
	void Clear()
	{
		m_index.clear();
	}
	//第一个元素
	bool First(Record& rt)
	{
		m_bIsreverse = false;
		m_bDroped = false;
		m_current = m_index.begin();
		if(m_current == m_index.end())
		{
			return false;
		}
		else
		{
			rt = *m_current;
			return true;
		}
	}
	//向下一个元素和First配合使用
	bool Next(Record& rt)
	{
        if (!m_bDroped)
        {
            m_current++;
        }
        else
        {
            m_bDroped = false;
        }
        if (m_current == m_index.end())
        {
            return false;
        }
        else
        {
            rt = *m_current;
            return true;
        }
	}
	//最后一个元素
	bool End(Record& rt)
	{
		m_bIsreverse = true;
		m_bDroped = false;
		m_current_r = m_index.rbegin();
        if (m_current_r == m_index.rend())
        {
            return false;
        }
        else
        {
            rt = *m_current_r;
            return true;
        }
	}
	//向上一个元素，和END配合使用
	bool Last(Record& rt)
	{
        if (!m_bDroped)
        {
			m_current_r++;
        }
        else
        {
            m_bDroped = false;
        }
        if (m_current_r == m_index.rend())
        {
            return false;
        }
        else
        {
            rt = *m_current_r;
            return true;
        }

	}
	bool Find(Record& rt)
	{
		m_bIsreverse = false;
		m_bDroped = false;
		m_current = m_index.find(rt);
		if(m_current == m_index.end())
		{
			return false;
		}
		else
		{
			rt = *m_current;
			return true;
		}
	}
	bool Add(Record& rt)
	{
// #ifdef _GCC
// 		Erase(rt);
// #endif
		std::pair<LP_SET,bool> r = m_index.insert(rt);
		return r.second;
// #ifndef _GCC
// 		if(!r.second) *r.first = rt;
// #endif
	}
	//反向迭代暂不能删除
    void EraseOne()
    {
		if (m_bIsreverse)
		{
			//m_index.erase(m_current_r++);//反向迭代暂不能删除
		}
		else
		{
			m_index.erase(m_current++);
			m_bDroped = true;
		}
        
		
    }
	int Erase(Record& rt)
	{
		return m_index.erase(rt);
	}
	int Size()
	{
		return m_index.size();
	}
};
//不重复的索引基类
template<typename Record,typename HashSet>
class CUniqueIndexBaseUnordered
{
public:
    typedef std::unordered_set<Record, HashSet> SET_;
    typedef typename SET_::iterator LP_SET;
    SET_      m_index;//索引数据，Record不可能重复
    LP_SET    m_current;//当前索引
    bool         m_bDroped;	 //是否删除操作
public:
	CUniqueIndexBaseUnordered()
    {
        m_bDroped = false;
    }
    ~CUniqueIndexBaseUnordered()
    {
        Clear();
    }
    void Clear()
    {
        m_index.clear();
    }
    //第一个元素
    bool First(Record& rt)
    {
        m_bDroped = false;
        m_current = m_index.begin();
        if (m_current == m_index.end())
        {
            return false;
        }
        else
        {
            rt = *m_current;
            return true;
        }
    }
    //向下一个元素和First配合使用
    bool Next(Record& rt)
    {
        if (!m_bDroped)
        {
            m_current++;
        }
        else
        {
            m_bDroped = false;
        }
        if (m_current == m_index.end())
        {
            return false;
        }
        else
        {
            rt = *m_current;
            return true;
        }
    }
    
    bool Find(Record& rt)
    {
        m_bDroped = false;
        m_current = m_index.find(rt);
        if (m_current == m_index.end())
        {
            return false;
        }
        else
        {
            rt = *m_current;
            return true;
        }
    }
    bool Add(Record& rt)
    {
        // #ifdef _GCC
        // 		Erase(rt);
        // #endif
        std::pair<LP_SET, bool> r = m_index.insert(rt);
        return r.second;
        // #ifndef _GCC
        // 		if(!r.second) *r.first = rt;
        // #endif
    }
    //反向迭代暂不能删除
    void EraseOne()
    {
        m_index.erase(m_current++);
        m_bDroped = true;
    }
    int Erase(Record& rt)
    {
        return m_index.erase(rt);
    }
    int Size()
    {
        return m_index.size();
    }
};

//重复的索引基类
template<typename Record,typename Compare>
class CRepeatIndexBase
{
public:
	typedef std::multiset<Record,Compare> MULTISET_;
	typedef typename MULTISET_::iterator LP_MULTISET;
	typedef typename MULTISET_::reverse_iterator LP_MULTISET_R;
	MULTISET_     m_index;   //索引数据 Record可能有多个
	LP_MULTISET   m_current;  //当前索引
	LP_MULTISET_R m_current_r;//反向当前索引
	LP_MULTISET   m_end;		   //查找或最后一条记录
	bool          m_bDroped;	   //删除操作
	bool          m_bIsreverse;	 //是否反向迭代
public:
	CRepeatIndexBase()
	{
		m_bDroped=false;
		m_bIsreverse = false;
	}
	~CRepeatIndexBase()
	{
		Clear();
	}
	void Clear()
	{
		m_index.clear();
	}
	//第一个元素
	bool First(Record& rt)
	{
		m_bIsreverse = false;
		m_current = m_index.begin();
		m_end=m_index.end();
		if(m_current == m_end)
		{
			return false;
		}
		else
		{
			rt = *m_current;
			return true;
		}
	}
	//下一个元素 和 First配合使用
	bool Next(Record& rt)
	{
		if (!m_bDroped)
		{
			m_current++;
		}
		else
		{
			m_bDroped=false;
		}
		if(m_current == m_end)
		{
			return false;
		}
		else
		{
			rt = *m_current;
			return true;
		}
	}
	//最后一个元素
    bool End(Record& rt)
    {
		m_bIsreverse = true;
        m_bDroped = false;
        m_current_r = m_index.rbegin();
        if (m_current_r == m_index.rend())
        {
            return false;
        }
        else
        {
            rt = *m_current_r;
            return true;
        }
    }
	//上一个元素  和End配合使用
    bool Last(Record& rt)
    {
        if (!m_bDroped)
        {
            m_current_r++;
        }
        else
        {
            m_bDroped = false;
        }
        if (m_current_r == m_index.rend())
        {
            return false;
        }
        else
        {
            rt = *m_current_r;
            return true;
        }
    }
	//查找第一个的位置和最后一个大于此值的位置，NEXT可以在这之间取得多个相同值的数据
	bool Find(Record& rt)
	{
		m_bIsreverse = false;
		m_bDroped=false;
		m_current = m_index.find(rt);
		m_end=m_index.upper_bound(rt);//第一个大于此值的位置，以后可以Next使用
		if(m_current == m_end)
		{
			return false;
		}
		else
		{
			if (m_current == m_index.end())
			{
				return false;
			}
			else
			{
				rt = *m_current;
				return true;
			}
		}
	}
	//查找第一个的位置，NEXT可以继续找，只到最后结束
	bool FindFirst(Record& rt)
	{
		m_bIsreverse = false;
		m_bDroped=false;
		m_current = m_index.find(rt);
		m_end= m_index.end();
		if(m_current == m_end)
		{
			return false;
		}
		else
		{
			if (m_current == m_index.end())
			{
				return false;
			}
			else
			{
				rt = *m_current;
				return true;
			}
		}
		
	}
	void Add(Record& rt)
	{
		m_index.insert(rt);
	}

	void EraseOne()	  //反向迭代暂不能删除 
	{
		if (m_bIsreverse)
		{
			//m_index.erase(m_current_r++);//反向迭代暂不能删除
		}
		else
		{
			m_index.erase(m_current++);
			m_bDroped = true;
		}
	}

	int Erase(Record& rt)
	{
		return m_index.erase(rt);
	}
	int Size()
	{
		return m_index.size();
	}
};

//整型ROWID数据,不重复  要排序，针对索引删除后使用，如果直接使用数据，请直接用vector保存rowid
class CInt
{
public:
	std::set<int>             m_intdata;
	std::set<int>::iterator   m_current;
	std::set<int>::iterator   m_end;
	std::set<int>::iterator   m_begin;
public:
	CInt()
	{

	}
	~CInt()
	{
		Clear();
	}
	void Add(int id)
	{
		m_intdata.insert(id);
	}
	bool First(int &id)
	{
		m_current=m_intdata.begin();
		m_end=m_intdata.end();
		if (m_current == m_end)
		{
			return false;
		}
		else
		{
			id=*m_current;
			return true;
		}
	}
	bool Next(int &id)
	{
		m_current++;
		if (m_current == m_end)
		{
			return false;
		}
		else
		{
			id=*m_current;
			return true;
		}
	}
	bool End(int &id)
	{
		m_current=m_intdata.end();
		m_begin=m_intdata.begin();
		if (m_current == m_end)
		{
			return false;
		}
		else
		{
			m_current--;
			id=*m_current;
			return true;
		}
	}
	bool Last(int &id)
	{
		if (m_current == m_begin)
		{
			return false;
		}
		else
		{
			m_current--;
			id=*m_current;
			return true;
		}
	}
	bool Find(int id)
	{
		std::set<int>::iterator p;
		p = m_intdata.find(id);
		if (p == m_intdata.end())
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	int Delete(int id)
	{
		return m_intdata.erase(id);
	}
	void Clear()
	{
		m_intdata.clear();
	}
	int Size()
	{
		return m_intdata.size();
	}
};

//整型ROWID数据,不重复 要排序，针对索引删除后使用，如果直接使用数据，请直接用vector保存rowid
class CUInt
{
public:
	std::set<unsigned int>             m_intdata;
	std::set<unsigned int>::iterator   m_current;
	std::set<unsigned int>::iterator   m_end;
	std::set<unsigned int>::iterator   m_begin;
public:
	CUInt()
	{
		
	}
	~CUInt()
	{
		Clear();
	}
	void Add(unsigned int id)
	{
	    //pair<std::set<unsigned int>::iterator,bool> r = m_intdata.insert(id);
		m_intdata.insert(id);
	}
	bool First(unsigned int &id)
	{
		m_current=m_intdata.begin();
		m_end=m_intdata.end();
		if (m_current == m_end)
		{
			return false;
		}
		else
		{
			id=*m_current;
			return true;
		}
	}
	bool Next(unsigned int &id)
	{
		m_current++;
		if (m_current == m_end)
		{
			return false;
		}
		else
		{
			id=*m_current;
			return true;
		}
	}
	bool End(unsigned int &id)
	{
		m_begin=m_intdata.begin();
		m_current=m_intdata.end();
		if (m_current == m_begin)
		{
			return false;
		}
		else
		{
			m_current--;
			id=*m_current;
			return true;
		}
	}
	bool Last(unsigned int &id)
	{
		
		if (m_current == m_begin)
		{
			return false;
		}
		else
		{
			m_current--;
			id=*m_current;
			return true;
		}
	}
	bool Find(unsigned int id)
	{
		std::set<unsigned int>::iterator p;
		p = m_intdata.find(id);
		if (p == m_intdata.end())
		{
			return false;
		}
		else
		{
			return true;
		}
	}
    int Delete(unsigned int id)
    {
        return m_intdata.erase(id);
    }
	void Clear()
	{
		m_intdata.clear();
	}
	int Size()
	{
		return m_intdata.size();
	}
};
//多个整型字段的索引，可重复
template<int count>
class CIndexInt
{
public:
	struct Int_
	{
		int	k[count];    //多个整型索引
		int id;          //对应表的ID，相当于rowid
	};
	struct Int_lt
	{
		bool operator()(const Int_ f1, const Int_ f2) const
		{
			int r=0;
			int i;

			for (i=0; i<count; i++)
			{
                if (f1.k[i] == f2.k[i])
                {
                    continue;
                }
                else
                {
                    if (f1.k[i] > f2.k[i])
                    {
                        r = 1;
                    }
                    else
                    {
                        r = -1;
                    }
                    break;
                }
				//r=f1.k[i]-f2.k[i];
				//if (r != 0)
				//{
				//	break;
				//}
			}
			return r<0;
		}
	};

	CRepeatIndexBase<Int_,Int_lt> m_index;//索引数据，可重复

public:
	CIndexInt()
	{
	}
	~CIndexInt()
	{
	}
	//增加一条索引:rowid,索引...
	void Add(int id,...)
	{
		Int_ a_;
		va_list	intp;
		int i;

		a_.id=id;
		va_start(intp, id);
		for (i=0; i<count; i++)
		{
			a_.k[i]=va_arg(intp, int);
		}
		va_end(intp);    
		m_index.Add(a_);
		return;
	}
	//查找指定索引的rowid，查找完成后直接使用数据，不删除，用此方法
	bool Select(std::vector<int> &iset,int k,...)
	{
		Int_ a_;
		bool bret_,bret2_;
		int i;
		va_list	intp;
		iset.clear();
		a_.k[0]=k;
		va_start(intp, k);
		for (i=0; i<count-1; i++)
		{
			a_.k[i+1]=va_arg(intp, int);
		}
		va_end(intp);    
		bret2_=bret_=m_index.Find(a_);
		while (bret_)
		{
			iset.push_back(a_.id);
			bret_=m_index.Next(a_);
		}
		return bret2_;
	}
	//查找指定索引的rowid  查找后按rowid排序，删除时使用
	bool Select(CInt &iset,int k,...)
	{
		Int_ a_;
		bool bret_,bret2_;
		int i;
		va_list	intp;
		
		iset.Clear();
		a_.k[0]=k;
		va_start(intp, k);
		for (i=0; i<count-1; i++)
		{
			a_.k[i+1]=va_arg(intp, int);
		}
		va_end(intp);    
		bret2_=bret_=m_index.Find(a_);
		while (bret_)
		{
			iset.Add(a_.id);
			bret_=m_index.Next(a_);
		}
		return bret2_;
	}
	//查找指定索引的数据是否存在，m_end置为最后一个大于此值的位置，可以Next取后续相同的值
	bool Find(int &rowid,int k,...)
	{
		Int_ a_;
		bool bret_;
		int i;
		va_list	intp;
		
		a_.k[0]=k;
		va_start(intp, k);
		for (i=0; i<count-1; i++)
		{
			a_.k[i+1]=va_arg(intp, int);
		}
		va_end(intp);    
		bret_=m_index.Find(a_);
		rowid = a_.id;
		return bret_;
	}
	//查找指定索引的数据是否存在，m_end为最后，可以用Next直到最后来比较查找，如取范围的值
	bool FindFirst(int &rowid,int k,...)
	{
		Int_ a_;
		bool bret_;
		int i;
		va_list	intp;
		
		a_.k[0]=k;
		va_start(intp, k);
		for (i=0; i<count-1; i++)
		{
			a_.k[i+1]=va_arg(intp, int);
		}
		va_end(intp);    
		bret_=m_index.FindFirst(a_);
		rowid = a_.id;
		return bret_;
	}
	//删除指定索引
	int Delete(int k,...)
	{
		Int_ a_;
		va_list	intp;
		int i;

		a_.k[0]=k;
		va_start(intp, k);
		for (i=0; i<count-1; i++)
		{
			a_.k[i+1]=va_arg(intp, int);
		}
		va_end(intp);    
		return m_index.Erase(a_);
	}
	//根据索引查找rowid，若rowid在iset中存在则删除索引
	void Delete(CInt iset,int k,...)
	{
		Int_ a_;
		bool bret_;
		va_list	intp;
		int i;
		
		a_.k[0]=k;
		va_start(intp, k);
		for (i=0; i<count-1; i++)
		{
			a_.k[i+1]=va_arg(intp, int);
		}
		va_end(intp);    
		bret_=m_index.Find(a_);
		while (bret_)
		{
			if (iset.Find(a_.id))
			{
				m_index.EraseOne();
			}
			bret_=m_index.Next(a_);
		}
	}
	//扫描全表，删除rowid
	void Delete(CInt iset)
	{
		Int_ a_;
		bool bret_;
		bret_=m_index.First(a_);
		while (bret_)
		{
			if (iset.Find(a_.id))
			{
				m_index.EraseOne();
			}
			bret_=m_index.Next(a_);
		}
	}
	bool First(int &id)
	{
		Int_ aa;
		if (!m_index.First(aa))
		{
			return false;
		}
		id = aa.id;
		return true;
	}
	bool Next(int &id)
	{
		Int_ aa;
		if (!m_index.Next(aa))
		{
			return false;
		}
		id = aa.id;
		return true;
	}
	bool End(int &id)
	{
		Int_ aa;
		if (!m_index.End(aa))
		{
			return false;
		}
		id = aa.id;
		return true;
	}
	bool Last(int &id)
	{
		Int_ aa;
		if (!m_index.Last(aa))
		{
			return false;
		}
		id = aa.id;
		return true;
	}
	void Clear()
	{
		m_index.Clear();
	}
	int Size()
	{
		return m_index.Size();
	}
};

//多个整型字段的key，不可重复
template<int count>
class CPkeyInt
{
public:
	struct Int_
	{
		int	k[count];    //多个整型索引
		int id;          //对应表的ID，相当于rowid
	};
	struct Int_lt
	{
		bool operator()(const Int_ f1, const Int_ f2) const
		{
			int r=0;
			int i;

			for (i=0; i<count; i++)
			{
                if (f1.k[i] == f2.k[i])
                {
                    continue;
                }
                else
                {
                    if (f1.k[i] > f2.k[i])
                    {
                        r = 1;
                    }
                    else
                    {
                        r = -1;
                    }
                    break;
                }
				//r=f1.k[i]-f2.k[i];
				//if (r != 0)
				//{
				//	break;
				//}
			}
			return r<0;
		}
	};

	CUniqueIndexBase<Int_,Int_lt> m_key;//key数据，不可重复

public:
	CPkeyInt()
	{
	}
	~CPkeyInt()
	{
	}
	//增加一条索引:rowid,索引...
	void Add(int id,...)
	{
		Int_ a_;
		va_list	intp;
		int i;

		a_.id=id;
		va_start(intp, id);
		for (i=0; i<count; i++)
		{
			a_.k[i]=va_arg(intp, int);
		}
		va_end(intp);    
		m_key.Add(a_);
		return;
	}
	//查找指定key的rowid  删除时使用
	bool Select(CInt &iset,int k,...)
	{
		Int_ a_;
		bool bret_;
		int i;
		va_list	intp;
		
		iset.Clear();
		a_.k[0]=k;
		va_start(intp, k);
		for (i=0; i<count-1; i++)
		{
			a_.k[i+1]=va_arg(intp, int);
		}
		va_end(intp);    
		bret_=m_key.Find(a_);
		if (bret_)
		{
			iset.Add(a_.id);
		}
		return bret_;
	}
	bool Select(int &rid,int k,...)
	{
		Int_ a_;
		bool bret_;
		int i;
		va_list	intp;
		
		a_.k[0]=k;
		va_start(intp, k);
		for (i=0; i<count-1; i++)
		{
			a_.k[i+1]=va_arg(intp, int);
		}
		va_end(intp);    
		bret_=m_key.Find(a_);
		if (bret_)
		{
			rid=a_.id;
		}
		return bret_;
	}
	//查找指定key的数据是否存在
	bool Find(int k,...)
	{
		Int_ a_;
		bool bret_;
		int i;
		va_list	intp;
		
		a_.k[0]=k;
		va_start(intp, k);
		for (i=0; i<count-1; i++)
		{
			a_.k[i+1]=va_arg(intp, int);
		}
		va_end(intp);    
		bret_=m_key.Find(a_);
		return bret_;
	}
	//删除指定key
	int Delete(int k,...)
	{
		Int_ a_;
		va_list	intp;
		int i;

		a_.k[0]=k;
		va_start(intp, k);
		for (i=0; i<count-1; i++)
		{
			a_.k[i+1]=va_arg(intp, int);
		}
		va_end(intp);    
		return m_key.Erase(a_);
	}
    //扫描全表，删除rowid
    void Delete(CInt iset)
    {
		Int_ a_;
        bool bret_;
        bret_ = m_key.First(a_);
        while (bret_)
        {
            if (iset.Find(a_.id))
            {
                m_key.EraseOne();
            }
            bret_ = m_key.Next(a_);
        }
    }
	bool First(int &id)
	{
		Int_ aa;
		if (!m_key.First(aa))
		{
			return false;
		}
		id = aa.id;
		return true;
	}
	bool Next(int &id)
	{
		Int_ aa;
		if (!m_key.Next(aa))
		{
			return false;
		}
		id = aa.id;
		return true;
	}
	bool End(int &id)
	{
		Int_ aa;
		if (!m_key.End(aa))
		{
			return false;
		}
		id = aa.id;
		return true;
	}
	bool Last(int &id)
	{
		Int_ aa;
		if (!m_key.Last(aa))
		{
			return false;
		}
		id = aa.id;
		return true;
	}
	void Clear()
	{
		m_key.Clear();
	}
	int Size()
	{
		return m_key.Size();
	}
};


//多个整型字段的key，不可重复
template<int count>
class CPkeyIntUnordered
{
public:
    struct Int_
    {
        int	k[count];    //多个整型索引
        int id;          //对应表的ID，相当于rowid
        bool operator == (const Int_& t) const
        {
            for (int i = 0; i < count; i++)
            {
                if (t.k[i] != k[i])
                {
                    return false;
                }
            }
            return true;
        }
    };
    struct Int_hash 
    {
        std::size_t operator() (const Int_ &f1) const
        {
			switch (count)
			{
			    case 1:
					return hash_val(f1.k[0]);
                case 2:
                    return hash_val(f1.k[0], f1.k[1]);
                case 3:
                    return hash_val(f1.k[0], f1.k[1], f1.k[2]);
                case 4:
                    return hash_val(f1.k[0], f1.k[1], f1.k[2], f1.k[3]);
                case 5:
					return hash_val(f1.k[0], f1.k[1], f1.k[2], f1.k[3], f1.k[4]);
                case 6:
					return hash_val(f1.k[0], f1.k[1], f1.k[2], f1.k[3], f1.k[4], f1.k[5]);
                case 7:
					return hash_val(f1.k[0], f1.k[1], f1.k[2], f1.k[3], f1.k[4], f1.k[5], f1.k[6]);
                case 8:
					return hash_val(f1.k[0], f1.k[1], f1.k[2], f1.k[3], f1.k[4], f1.k[5], f1.k[6], f1.k[7]);
                case 9:
					return hash_val(f1.k[0], f1.k[1], f1.k[2], f1.k[3], f1.k[4], f1.k[5], f1.k[6], f1.k[7], f1.k[8]);
                case 10:
					return hash_val(f1.k[0], f1.k[1], f1.k[2], f1.k[3], f1.k[4], f1.k[5], f1.k[6], f1.k[7], f1.k[8], f1.k[9]);
				default:
					return hash_val(f1.k[0], f1.k[1], f1.k[2], f1.k[3], f1.k[4], f1.k[5], f1.k[6], f1.k[7], f1.k[8], f1.k[9]);
			}
        }
    };

    CUniqueIndexBaseUnordered<Int_, Int_hash> m_key;//key数据，不可重复

public:
	CPkeyIntUnordered()
    {
		
    }
    ~CPkeyIntUnordered()
    {
    }
    //增加一条索引:rowid,索引...
    void Add(int id, ...)
    {
        Int_ a_;
        va_list	intp;
        int i;

        a_.id = id;
        va_start(intp, id);
        for (i = 0; i < count; i++)
        {
            a_.k[i] = va_arg(intp, int);
        }
        va_end(intp);
        m_key.Add(a_);
        return;
    }
    //查找指定key的rowid  删除时使用
    bool Select(CInt& iset, int k, ...)
    {
        Int_ a_;
        bool bret_;
        int i;
        va_list	intp;

        iset.Clear();
        a_.k[0] = k;
        va_start(intp, k);
        for (i = 0; i < count - 1; i++)
        {
            a_.k[i + 1] = va_arg(intp, int);
        }
        va_end(intp);
        bret_ = m_key.Find(a_);
        if (bret_)
        {
            iset.Add(a_.id);
        }
        return bret_;
    }
    bool Select(int& rid, int k, ...)
    {
        Int_ a_;
        bool bret_;
        int i;
        va_list	intp;

        a_.k[0] = k;
        va_start(intp, k);
        for (i = 0; i < count - 1; i++)
        {
            a_.k[i + 1] = va_arg(intp, int);
        }
        va_end(intp);
        bret_ = m_key.Find(a_);
        if (bret_)
        {
            rid = a_.id;
        }
        return bret_;
    }
    //查找指定key的数据是否存在
    bool Find(int k, ...)
    {
        Int_ a_;
        bool bret_;
        int i;
        va_list	intp;

        a_.k[0] = k;
        va_start(intp, k);
        for (i = 0; i < count - 1; i++)
        {
            a_.k[i + 1] = va_arg(intp, int);
        }
        va_end(intp);
        bret_ = m_key.Find(a_);
        return bret_;
    }
    //删除指定key
    int Delete(int k, ...)
    {
        Int_ a_;
        va_list	intp;
        int i;

        a_.k[0] = k;
        va_start(intp, k);
        for (i = 0; i < count - 1; i++)
        {
            a_.k[i + 1] = va_arg(intp, int);
        }
        va_end(intp);
        return m_key.Erase(a_);
    }
    //扫描全表，删除rowid
    void Delete(CInt iset)
    {
        Int_ a_;
        bool bret_;
        bret_ = m_key.First(a_);
        while (bret_)
        {
            if (iset.Find(a_.id))
            {
                m_key.EraseOne();
            }
            bret_ = m_key.Next(a_);
        }
    }
    bool First(int& id)
    {
        Int_ aa;
        if (!m_key.First(aa))
        {
            return false;
        }
        id = aa.id;
        return true;
    }
    bool Next(int& id)
    {
        Int_ aa;
        if (!m_key.Next(aa))
        {
            return false;
        }
        id = aa.id;
        return true;
    }
    bool End(int& id)
    {
        Int_ aa;
        if (!m_key.End(aa))
        {
            return false;
        }
        id = aa.id;
        return true;
    }
    bool Last(int& id)
    {
        Int_ aa;
        if (!m_key.Last(aa))
        {
            return false;
        }
        id = aa.id;
        return true;
    }
    void Clear()
    {
        m_key.Clear();
    }
    int Size()
    {
        return m_key.Size();
    }
};


//多个整型字段的索引，可重复
template<int count>
class CIndexUInt
{
public:
	struct Int_
	{
		unsigned int	k[count];    //多个整型索引
		int id;          //对应表的ID，相当于rowid
	};
	struct Int_lt
	{
		bool operator()(const Int_ f1, const Int_ f2) const
		{
			int r=0;
			int i;

			for (i=0; i<count; i++)
			{
				if (f1.k[i] == f2.k[i])
				{
					continue;
				}
				else
				{
					if (f1.k[i] > f2.k[i])
					{
						r = 1;
					}
					else 
					{
						r = -1;
					}
					break;
				}
				//r=f1.k[i]-f2.k[i];
				//if (r != 0)
				//{
				//	break;
				//}
			}
			return r<0;
		}
	};

	CRepeatIndexBase<Int_,Int_lt> m_index;//索引数据，可重复

public:
	CIndexUInt()
	{
	}
	~CIndexUInt()
	{
	}
	//增加一条索引:rowid,索引...
	void Add(int id,...)
	{
		Int_ a_;
		va_list	intp;
		int i;

		a_.id=id;
		va_start(intp, id);
		for (i=0; i<count; i++)
		{
			a_.k[i]=va_arg(intp, unsigned int);
		}
		va_end(intp);    
		m_index.Add(a_);
		return;
	}
	//查找指定索引的rowid  查找后直接使用数据，按索引排序。
	bool Select(std::vector<int> &iset,unsigned int k,...)
	{
		Int_ a_;
		bool bret_,bret2_;
		int i;
		va_list	intp;
		
		iset.clear();
		a_.k[0]=k;
		va_start(intp, k);
		for (i=0; i<count-1; i++)
		{
			a_.k[i+1]=va_arg(intp, unsigned int);
		}
		va_end(intp);    
		bret2_=bret_=m_index.Find(a_);
		while (bret_)
		{
			iset.push_back(a_.id);
			bret_=m_index.Next(a_);
		}
		return bret2_;
	}
	//查找指定索引的rowid 删除时使用，按rowid排序
	bool Select(CInt &iset,unsigned int k,...)
	{
		Int_ a_;
		bool bret_,bret2_;
		int i;
		va_list	intp;
		
		iset.Clear();
		a_.k[0]=k;
		va_start(intp, k);
		for (i=0; i<count-1; i++)
		{
			a_.k[i+1]=va_arg(intp, unsigned int);
		}
		va_end(intp);    
		bret2_=bret_=m_index.Find(a_);
		while (bret_)
		{
			iset.Add(a_.id);
			bret_=m_index.Next(a_);
		}
		return bret2_;
	}
	//查找指定索引的数据是否存在，m_end置为最后一个大于此值的位置，可以Next取后续相同的值
	bool Find(int &rowid,unsigned int k,...)
	{
		Int_ a_;
		bool bret_;
		int i;
		va_list	intp;
		
		a_.k[0]=k;
		va_start(intp, k);
		for (i=0; i<count-1; i++)
		{
			a_.k[i+1]=va_arg(intp,unsigned int);
		}
		va_end(intp);    
		bret_=m_index.Find(a_);
		rowid = a_.id;
		return bret_;
	}
	//查找指定索引的数据是否存在，m_end为最后一值，NEXT取只要最后，可以按范围取值
	bool FindFirst(int &rowid,unsigned int k,...)
	{
		Int_ a_;
		bool bret_;
		int i;
		va_list	intp;
		
		a_.k[0]=k;
		va_start(intp, k);
		for (i=0; i<count-1; i++)
		{
			a_.k[i+1]=va_arg(intp,unsigned int);
		}
		va_end(intp);    
		bret_=m_index.FindFirst(a_);
		rowid = a_.id;
		return bret_;
	}
	//删除指定索引
	int Delete(unsigned int k,...)
	{
		Int_ a_;
		va_list	intp;
		int i;

		a_.k[0]=k;
		va_start(intp, k);
		for (i=0; i<count-1; i++)
		{
			a_.k[i+1]=va_arg(intp,unsigned int);
		}
		va_end(intp);    
		return m_index.Erase(a_);
	}
	//根据索引查找rowid，若rowid在iset中存在则删除索引
	void Delete(CInt iset,unsigned int k,...)
	{
		Int_ a_;
		bool bret_;
		va_list	intp;
		int i;
		
		a_.k[0]=k;
		va_start(intp, k);
		for (i=0; i<count-1; i++)
		{
			a_.k[i+1]=va_arg(intp,unsigned int);
		}
		va_end(intp);    
		bret_=m_index.Find(a_);
		while (bret_)
		{
			if (iset.Find(a_.id))
			{
				m_index.EraseOne();
			}
			bret_=m_index.Next(a_);
		}
	}
	//扫描全表，删除rowid
	void Delete(CInt iset)
	{
		Int_ a_;
		bool bret_;
		bret_=m_index.First(a_);
		while (bret_)
		{
			if (iset.Find(a_.id))
			{
				m_index.EraseOne();
			}
			bret_=m_index.Next(a_);
		}
	}
	bool First(int &id)
	{
		Int_ aa;
		if (!m_index.First(aa))
		{
			return false;
		}
		id = aa.id;
		return true;
	}
	bool Next(int &id)
	{
		Int_ aa;
		if (!m_index.Next(aa))
		{
			return false;
		}
		id = aa.id;
		return true;
	}
	bool End(int &id)
	{
		Int_ aa;
		if (!m_index.End(aa))
		{
			return false;
		}
		id = aa.id;
		return true;
	}
	bool Last(int &id)
	{
		Int_ aa;
		if (!m_index.Last(aa))
		{
			return false;
		}
		id = aa.id;
		return true;
	}
	void Clear()
	{
		m_index.Clear();
	}
	int Size()
	{
		return m_index.Size();
	}
};

//多个整型字段的key，不可重复
template<int count>
class CPkeyUInt
{
public:
	struct Int_
	{
		unsigned int	k[count];    //多个整型索引
		int id;          //对应表的ID，相当于rowid
	};
	struct Int_lt
	{
		bool operator()(const Int_ f1, const Int_ f2) const
		{
			int r=0;
			int i;

			for (i=0; i<count; i++)
			{
                if (f1.k[i] == f2.k[i])
                {
                    continue;
                }
                else
                {
                    if (f1.k[i] > f2.k[i])
                    {
                        r = 1;
                    }
                    else
                    {
                        r = -1;
                    }
                    break;
                }
				//r=f1.k[i]-f2.k[i];
				//if (r != 0)
				//{
				//	break;
				//}
			}
			return r<0;
		}
	};

	CUniqueIndexBase<Int_,Int_lt> m_key;//key数据，不可重复

public:
	CPkeyUInt()
	{
	}
	~CPkeyUInt()
	{
	}
	//增加一条索引:rowid,索引...
	void Add(int id,...)
	{
		Int_ a_;
		va_list	intp;
		int i;

		a_.id=id;
		va_start(intp, id);
		for (i=0; i<count; i++)
		{
			a_.k[i]=va_arg(intp,unsigned int);
		}
		va_end(intp);    
		m_key.Add(a_);
		return;
	}
	//查找指定key的rowid 删除时使用
	bool Select(CInt &iset,unsigned int k,...)
	{
		Int_ a_;
		bool bret_;
		int i;
		va_list	intp;
		
		iset.Clear();
		a_.k[0]=k;
		va_start(intp, k);
		for (i=0; i<count-1; i++)
		{
			a_.k[i+1]=va_arg(intp,unsigned int);
		}
		va_end(intp);    
		bret_=m_key.Find(a_);
		if (bret_)
		{
			iset.Add(a_.id);
		}
		return bret_;
	}
	bool Select(int &rid,unsigned int k,...)
	{
		Int_ a_;
		bool bret_;
		int i;
		va_list	intp;
		
		a_.k[0]=k;
		va_start(intp, k);
		for (i=0; i<count-1; i++)
		{
			a_.k[i+1]=va_arg(intp,unsigned int);
		}
		va_end(intp);    
		bret_=m_key.Find(a_);
		if (bret_)
		{
			rid = a_.id;
		}
		return bret_;
	}
	//查找指定key的数据是否存在
	bool Find(unsigned int k,...)
	{
		Int_ a_;
		bool bret_;
		int i;
		va_list	intp;
		
		a_.k[0]=k;
		va_start(intp, k);
		for (i=0; i<count-1; i++)
		{
			a_.k[i+1]=va_arg(intp,unsigned int);
		}
		va_end(intp);    
		bret_=m_key.Find(a_);
		return bret_;
	}
	//删除指定key
	int Delete(unsigned int k,...)
	{
		Int_ a_;
		va_list	intp;
		int i;

		a_.k[0]=k;
		va_start(intp, k);
		for (i=0; i<count-1; i++)
		{
			a_.k[i+1]=va_arg(intp,unsigned int);
		}
		va_end(intp);    
		return m_key.Erase(a_);
	}
    //扫描全表，删除rowid
    void Delete(CInt iset)
    {
        Int_ a_;
        bool bret_;
        bret_ = m_key.First(a_);
        while (bret_)
        {
            if (iset.Find(a_.id))
            {
				m_key.EraseOne();
            }
            bret_ = m_key.Next(a_);
        }
    }
	bool First(int &id)
	{
		Int_ aa;
		if (!m_key.First(aa))
		{
			return false;
		}
		id = aa.id;
		return true;
	}
	bool Next(int &id)
	{
		Int_ aa;
		if (!m_key.Next(aa))
		{
			return false;
		}
		id = aa.id;
		return true;
	}
	bool End(int &id)
	{
		Int_ aa;
		if (!m_key.End(aa))
		{
			return false;
		}
		id = aa.id;
		return true;
	}
	bool Last(int &id)
	{
		Int_ aa;
		if (!m_key.Last(aa))
		{
			return false;
		}
		id = aa.id;
		return true;
	}
	void Clear()
	{
		m_key.Clear();
	}
	int Size()
	{
		return m_key.Size();
	}
};
//多个整型字段的key，不可重复
template<int count>
class CPkeyUIntUnordered
{
public:
    struct Int_
    {
        unsigned int k[count];    //多个整型索引
        int id;          //对应表的ID，相当于rowid
        bool operator == (const Int_& t) const
        {
            for (int i = 0; i < count; i++)
            {
                if (t.k[i] != k[i])
                {
                    return false;
                }
            }
            return true;
        }
    };
    struct Int_hash
    {
        std::size_t operator() (const Int_& f1) const
        {
            switch (count)
            {
            case 1:
                return hash_val(f1.k[0]);
            case 2:
                return hash_val(f1.k[0], f1.k[1]);
            case 3:
                return hash_val(f1.k[0], f1.k[1], f1.k[2]);
            case 4:
                return hash_val(f1.k[0], f1.k[1], f1.k[2], f1.k[3]);
            case 5:
                return hash_val(f1.k[0], f1.k[1], f1.k[2], f1.k[3], f1.k[4]);
            case 6:
                return hash_val(f1.k[0], f1.k[1], f1.k[2], f1.k[3], f1.k[4], f1.k[5]);
            case 7:
                return hash_val(f1.k[0], f1.k[1], f1.k[2], f1.k[3], f1.k[4], f1.k[5], f1.k[6]);
            case 8:
                return hash_val(f1.k[0], f1.k[1], f1.k[2], f1.k[3], f1.k[4], f1.k[5], f1.k[6], f1.k[7]);
            case 9:
                return hash_val(f1.k[0], f1.k[1], f1.k[2], f1.k[3], f1.k[4], f1.k[5], f1.k[6], f1.k[7], f1.k[8]);
            case 10:
                return hash_val(f1.k[0], f1.k[1], f1.k[2], f1.k[3], f1.k[4], f1.k[5], f1.k[6], f1.k[7], f1.k[8], f1.k[9]);
            default:
                return hash_val(f1.k[0], f1.k[1], f1.k[2], f1.k[3], f1.k[4], f1.k[5], f1.k[6], f1.k[7], f1.k[8], f1.k[9]);
            }
        }
    };
    CUniqueIndexBaseUnordered<Int_, Int_hash> m_key;//key数据，不可重复

public:
	CPkeyUIntUnordered()
    {
    }
    ~CPkeyUIntUnordered()
    {
    }
    //增加一条索引:rowid,索引...
    void Add(int id, ...)
    {
        Int_ a_;
        va_list	intp;
        int i;

        a_.id = id;
        va_start(intp, id);
        for (i = 0; i < count; i++)
        {
            a_.k[i] = va_arg(intp, unsigned int);
        }
        va_end(intp);
        m_key.Add(a_);
        return;
    }
    //查找指定key的rowid 删除时使用
    bool Select(CInt& iset, unsigned int k, ...)
    {
        Int_ a_;
        bool bret_;
        int i;
        va_list	intp;

        iset.Clear();
        a_.k[0] = k;
        va_start(intp, k);
        for (i = 0; i < count - 1; i++)
        {
            a_.k[i + 1] = va_arg(intp, unsigned int);
        }
        va_end(intp);
        bret_ = m_key.Find(a_);
        if (bret_)
        {
            iset.Add(a_.id);
        }
        return bret_;
    }
    bool Select(int& rid, unsigned int k, ...)
    {
        Int_ a_;
        bool bret_;
        int i;
        va_list	intp;

        a_.k[0] = k;
        va_start(intp, k);
        for (i = 0; i < count - 1; i++)
        {
            a_.k[i + 1] = va_arg(intp, unsigned int);
        }
        va_end(intp);
        bret_ = m_key.Find(a_);
        if (bret_)
        {
            rid = a_.id;
        }
        return bret_;
    }
    //查找指定key的数据是否存在
    bool Find(unsigned int k, ...)
    {
        Int_ a_;
        bool bret_;
        int i;
        va_list	intp;

        a_.k[0] = k;
        va_start(intp, k);
        for (i = 0; i < count - 1; i++)
        {
            a_.k[i + 1] = va_arg(intp, unsigned int);
        }
        va_end(intp);
        bret_ = m_key.Find(a_);
        return bret_;
    }
    //删除指定key
    int Delete(unsigned int k, ...)
    {
        Int_ a_;
        va_list	intp;
        int i;

        a_.k[0] = k;
        va_start(intp, k);
        for (i = 0; i < count - 1; i++)
        {
            a_.k[i + 1] = va_arg(intp, unsigned int);
        }
        va_end(intp);
        return m_key.Erase(a_);
    }
    //扫描全表，删除rowid
    void Delete(CInt iset)
    {
        Int_ a_;
        bool bret_;
        bret_ = m_key.First(a_);
        while (bret_)
        {
            if (iset.Find(a_.id))
            {
                m_key.EraseOne();
            }
            bret_ = m_key.Next(a_);
        }
    }
    bool First(int& id)
    {
        Int_ aa;
        if (!m_key.First(aa))
        {
            return false;
        }
        id = aa.id;
        return true;
    }
    bool Next(int& id)
    {
        Int_ aa;
        if (!m_key.Next(aa))
        {
            return false;
        }
        id = aa.id;
        return true;
    }
    bool End(int& id)
    {
        Int_ aa;
        if (!m_key.End(aa))
        {
            return false;
        }
        id = aa.id;
        return true;
    }
    bool Last(int& id)
    {
        Int_ aa;
        if (!m_key.Last(aa))
        {
            return false;
        }
        id = aa.id;
        return true;
    }
    void Clear()
    {
        m_key.Clear();
    }
    int Size()
    {
        return m_key.Size();
    }
};

//多个字符字段的索引,可重复
template<int length,int count>
class CIndexCharF
{
public:
	struct Char_
	{
		char p[count][length];//多个字符索引
		int  id;              //内存表的rowid
	};
	struct Char_lt
	{
		bool operator()(const Char_ f1, const Char_ f2) const
		{
			int r=0;
			int i;

			for (i=0; i<count; i++)
			{
				r=strcmp(f1.p[i],f2.p[i]);
				if (r != 0)
				{
					break;
				}
			}
			return r<0;
		}
	};

	CRepeatIndexBase<Char_,Char_lt> m_index;  //索引数据，可重复

public:
	CIndexCharF()
	{
	}
	~CIndexCharF()
	{
	}
	//增加一条索引:rowid,索引...
	void Add(int id,char *p,...)
	{
		Char_ a_;
		va_list	charp;
		int i;

		a_.id=id;
		CBF_Tools::StringCopy(a_.p[0],length-1,p);
		va_start(charp,p);
		for (i=0; i<count-1; i++)
		{
			CBF_Tools::StringCopy(a_.p[i+1],length-1,va_arg(charp,char*));
		}
		va_end(charp);
		m_index.Add(a_);
		return;
	}
	//查找指定索引的rowid  直接使用查找的数据时使用，按索引排序
	bool Select(std::vector<int> &iset,char *p,...)
	{
		Char_ a_;
		bool bret_,bret2_;
		va_list charp;
		int i;
		
		iset.clear();
		CBF_Tools::StringCopy(a_.p[0],length-1,p);
		va_start(charp,p);
		for (i=0; i<count-1; i++)
		{
			CBF_Tools::StringCopy(a_.p[i+1],length-1,va_arg(charp,char*));
		}
		va_end(charp);
		bret2_=bret_=m_index.Find(a_);
		while (bret_)
		{
			iset.push_back(a_.id);
			bret_=m_index.Next(a_);
		}
		return bret2_;
	}
	//查找指定索引的rowid  删除时使用
	bool Select(CInt &iset,char *p,...)
	{
		Char_ a_;
		bool bret_,bret2_;
		va_list charp;
		int i;

		iset.Clear();
		CBF_Tools::StringCopy(a_.p[0],length-1,p);
		va_start(charp,p);
		for (i=0; i<count-1; i++)
		{
			CBF_Tools::StringCopy(a_.p[i+1],length-1,va_arg(charp,char*));
		}
		va_end(charp);
		bret2_=bret_=m_index.Find(a_);
		while (bret_)
		{
			iset.Add(a_.id);
			bret_=m_index.Next(a_);
		}
		return bret2_;
	}
	//查找指定索引的数据是否存在，m_end置为最后一个大于此值的位置，可以Next取后续相同的值
	bool Find(int &rowid,char *p,...)
	{
		Char_ a_;
		bool bret_;
		va_list charp;
		int i;

		CBF_Tools::StringCopy(a_.p[0],length-1,p);
		va_start(charp,p);
		for (i=0; i<count-1; i++)
		{
			CBF_Tools::StringCopy(a_.p[i+1],length-1,va_arg(charp,char*));
		}
		va_end(charp);
		bret_=m_index.Find(a_);
		rowid = a_.id;
		return bret_;
	}
	//查找指定索引的数据是否存在，m_end为最后一值，NEXT取只要最后，可以按范围取值
	bool FindFirst(int &rowid,char *p,...)
	{
		Char_ a_;
		bool bret_;
		va_list charp;
		int i;
		
		CBF_Tools::StringCopy(a_.p[0],length-1,p);
		va_start(charp,p);
		for (i=0; i<count-1; i++)
		{
			CBF_Tools::StringCopy(a_.p[i+1],length-1,va_arg(charp,char*));
		}
		va_end(charp);
		bret_=m_index.FindFirst(a_);
		rowid = a_.id;
		return bret_;
	}
	//删除指定索引
	int Delete(char *p,...)
	{
		Char_ a_;
		va_list charp;
		int i;

		CBF_Tools::StringCopy(a_.p[0],length-1,p);
		va_start(charp,p);
		for (i=0; i<count-1; i++)
		{
			CBF_Tools::StringCopy(a_.p[i+1],length-1,va_arg(charp,char*));
		}
		va_end(charp);
		return m_index.Erase(a_);
	}
	//根据索引查找rowid，若rowid在iset中存在则删除索引
	void Delete(CInt iset,char *p,...)
	{
		Char_ a_;
		bool bret_;
		va_list charp;
		int i;
		
		CBF_Tools::StringCopy(a_.p[0],length-1,p);
		va_start(charp,p);
		for (i=0; i<count-1; i++)
		{
			CBF_Tools::StringCopy(a_.p[i+1],length-1,va_arg(charp,char*));
		}
		va_end(charp);
		bret_=m_index.Find(a_);
		while (bret_)
		{
			if (iset.Find(a_.id))
			{
				m_index.EraseOne();
			}
			bret_=m_index.Next(a_);
		}
	}
	//扫描全表，删除rowid
	void Delete(CInt iset)
	{
		Char_ a_;
		bool bret_;
		
		bret_=m_index.First(a_);
		while (bret_)
		{
			if (iset.Find(a_.id))
			{
				m_index.EraseOne();
			}
			bret_=m_index.Next(a_);
		}
	}
	bool First(int &id)
	{
		Char_ aa;
		if (!m_index.First(aa))
		{
			return false;
		}
		id = aa.id;
		return true;
	}
	bool Next(int &id)
	{
		Char_ aa;
		if (!m_index.Next(aa))
		{
			return false;
		}
		id = aa.id;
		return true;
	}
	bool End(int &id)
	{
		Char_ aa;
		if (!m_index.End(aa))
		{
			return false;
		}
		id = aa.id;
		return true;
	}
	bool Last(int &id)
	{
		Char_ aa;
		if (!m_index.Last(aa))
		{
			return false;
		}
		id = aa.id;
		return true;
	}
	void Clear()
	{
		m_index.Clear();
	}
	int Size()
	{
		return m_index.Size();
	}
};

//// 多个字符字段的key,不可重复
template<int length,int count>
class CPkeyCharF
{
public:
	struct Char_
	{
		char p[count][length];//多个字符索引
		int  id;              //内存表的rowid
	};
	struct Char_lt
	{
		bool operator()(const Char_ f1, const Char_ f2) const
		{
			int r=0;
			int i;

			for (i=0; i<count; i++)
			{
				r=strcmp(f1.p[i],f2.p[i]);
				if (r != 0)
				{
					break;
				}
			}
			return r<0;
		}
	};

	CUniqueIndexBase<Char_,Char_lt> m_key;  //key数据，不可重复

public:
	CPkeyCharF()
	{
	}
	~CPkeyCharF()
	{
	}
	//增加一条索引:id=rowid,索引...
	void Add(int id,char *p,...)
	{
		Char_ a_;
		va_list	charp;
		int i;

		a_.id=id;
		CBF_Tools::StringCopy(a_.p[0],length-1,p);
		va_start(charp,p);
		for (i=0; i<count-1; i++)
		{
			CBF_Tools::StringCopy(a_.p[i+1],length-1,va_arg(charp,char*));
		}
		va_end(charp);
		m_key.Add(a_);
		return;
	}

	//查找指定key的rowid 删除时使用
	bool Select(CInt &iset,char *p,...)
	{
		Char_ a_;
		bool bret_;
		va_list charp;
		int i;

		iset.Clear();
		CBF_Tools::StringCopy(a_.p[0],length-1,p);
		va_start(charp,p);
		for (i=0; i<count-1; i++)
		{
			CBF_Tools::StringCopy(a_.p[i+1],length-1,va_arg(charp,char*));
		}
		va_end(charp);
		bret_=m_key.Find(a_);
		if (bret_)
		{
			iset.Add(a_.id);
		}
		return bret_;
	}
	//查找指定key的rowid
	bool Select(int &rowid,char *p,...)
	{
		Char_ a_;
		bool bret_;
		va_list charp;
		int i;
		
		CBF_Tools::StringCopy(a_.p[0],length-1,p);
		va_start(charp,p);
		for (i=0; i<count-1; i++)
		{
			CBF_Tools::StringCopy(a_.p[i+1],length-1,va_arg(charp,char*));
		}
		va_end(charp);
		bret_=m_key.Find(a_);
		if (bret_)
		{
			rowid =a_.id;
		}
		return bret_;
	}
	//查找指定key的rowid
	bool GetRowId(int &rid,char *p,...)
	{
		Char_ a_;
		bool bret_;
		va_list charp;
		int i;
		
		CBF_Tools::StringCopy(a_.p[0],length-1,p);
		va_start(charp,p);
		for (i=0; i<count-1; i++)
		{
			CBF_Tools::StringCopy(a_.p[i+1],length-1,va_arg(charp,char*));
		}
		va_end(charp);
		bret_=m_key.Find(a_);
		if (bret_)
		{
			rid =a_.id;
		}
		return bret_;
	}
	//查找指定key是否存在
	bool Find(char *p,...)
	{
		Char_ a_;
		bool bret_;
		va_list charp;
		int i;

		CBF_Tools::StringCopy(a_.p[0],length-1,p);
		va_start(charp,p);
		for (i=0; i<count-1; i++)
		{
			CBF_Tools::StringCopy(a_.p[i+1],length-1,va_arg(charp,char*));
		}
		va_end(charp);
		bret_=m_key.Find(a_);
		return bret_;
	}
	//删除指定key
	int Delete(char *p,...)
	{
		Char_ a_;
		va_list charp;
		int i;

		CBF_Tools::StringCopy(a_.p[0],length-1,p);
		va_start(charp,p);
		for (i=0; i<count-1; i++)
		{
			CBF_Tools::StringCopy(a_.p[i+1],length-1,va_arg(charp,char*));
		}
		va_end(charp);
		return m_key.Erase(a_);
	}
    //扫描全表，删除rowid
    void Delete(CInt iset)
    {
        Char_ a_;
        bool bret_;

        bret_ = m_key.First(a_);
        while (bret_)
        {
            if (iset.Find(a_.id))
            {
				m_key.EraseOne();
            }
            bret_ = m_key.Next(a_);
        }
    }
	bool First(int &id)
	{
		Char_ aa;
		if (!m_key.First(aa))
		{
			return false;
		}
		id = aa.id;
		return true;
	}
	bool Next(int &id)
	{
		Char_ aa;
		if (!m_key.Next(aa))
		{
			return false;
		}
		id = aa.id;
		return true;
	}
	bool End(int &id)
	{
		Char_ aa;
		if (!m_key.End(aa))
		{
			return false;
		}
		id = aa.id;
		return true;
	}
	bool Last(int &id)
	{
		Char_ aa;
		if (!m_key.Last(aa))
		{
			return false;
		}
		id = aa.id;
		return true;
	}
	void Clear()
	{
		m_key.Clear();
	}
	int Size()
	{
		return m_key.Size();
	}
};
//// 多个字符字段的key,不可重复
template<int length, int count>
class CPkeyCharFUnordered
{
public:
    struct Char_
    {
        char p[count][length];//多个字符索引
        int  id;              //内存表的rowid
        bool operator == (const Char_& t) const
        {
            for (int i = 0; i < count; i++)
            {
                if (strncmp(t.p[i],p[i],length)!=0)
                {
                    return false;
                }
            }
            return true;
        }
    };
    struct Char_hash
    {
        std::size_t operator() (const Char_& f1) const
        {
            switch (count)
            {
            case 1:
                return hash_val(std::string(f1.p[0]));
            case 2:
                return hash_val(std::string(f1.p[0]), std::string(f1.p[1]));
            case 3:
                return hash_val(std::string(f1.p[0]), std::string(f1.p[1]), std::string(f1.p[2]));
            case 4:
                return hash_val(std::string(f1.p[0]), std::string(f1.p[1]), std::string(f1.p[2]), std::string(f1.p[3]));
            case 5:
                return hash_val(std::string(f1.p[0]), std::string(f1.p[1]), std::string(f1.p[2]), std::string(f1.p[3]), std::string(f1.p[4]));
            case 6:
                return hash_val(std::string(f1.p[0]), std::string(f1.p[1]), std::string(f1.p[2]), std::string(f1.p[3]), std::string(f1.p[4]), std::string(f1.p[5]));
            case 7:
                return hash_val(std::string(f1.p[0]), std::string(f1.p[1]), std::string(f1.p[2]), std::string(f1.p[3]), std::string(f1.p[4]), std::string(f1.p[5]), std::string(f1.p[6]));
            case 8:
                return hash_val(std::string(f1.p[0]), std::string(f1.p[1]), std::string(f1.p[2]), std::string(f1.p[3]), std::string(f1.p[4]), std::string(f1.p[5]), std::string(f1.p[6]), std::string(f1.p[7]));
            case 9:
                return hash_val(std::string(f1.p[0]), std::string(f1.p[1]), std::string(f1.p[2]), std::string(f1.p[3]), std::string(f1.p[4]), std::string(f1.p[5]), std::string(f1.p[6]), std::string(f1.p[7]), std::string(f1.p[8]));
            case 10:
                return hash_val(std::string(f1.p[0]), std::string(f1.p[1]), std::string(f1.p[2]), std::string(f1.p[3]), std::string(f1.p[4]), std::string(f1.p[5]), std::string(f1.p[6]), std::string(f1.p[7]), std::string(f1.p[8]), std::string(f1.p[9]));
            default:
                return hash_val(std::string(f1.p[0]), std::string(f1.p[1]), std::string(f1.p[2]), std::string(f1.p[3]), std::string(f1.p[4]), std::string(f1.p[5]), std::string(f1.p[6]), std::string(f1.p[7]), std::string(f1.p[8]), std::string(f1.p[9]));
            }
        }
    };
    CUniqueIndexBaseUnordered<Char_, Char_hash> m_key;  //key数据，不可重复

public:
	CPkeyCharFUnordered()
    {
    }
    ~CPkeyCharFUnordered()
    {
    }
    //增加一条索引:id=rowid,索引...
    void Add(int id, char* p, ...)
    {
        Char_ a_;
        va_list	charp;
        int i;

        a_.id = id;
        CBF_Tools::StringCopy(a_.p[0], length - 1, p);
        va_start(charp, p);
        for (i = 0; i < count - 1; i++)
        {
            CBF_Tools::StringCopy(a_.p[i + 1], length - 1, va_arg(charp, char*));
        }
        va_end(charp);
        m_key.Add(a_);
        return;
    }

    //查找指定key的rowid 删除时使用
    bool Select(CInt& iset, char* p, ...)
    {
        Char_ a_;
        bool bret_;
        va_list charp;
        int i;

        iset.Clear();
        CBF_Tools::StringCopy(a_.p[0], length - 1, p);
        va_start(charp, p);
        for (i = 0; i < count - 1; i++)
        {
            CBF_Tools::StringCopy(a_.p[i + 1], length - 1, va_arg(charp, char*));
        }
        va_end(charp);
        bret_ = m_key.Find(a_);
        if (bret_)
        {
            iset.Add(a_.id);
        }
        return bret_;
    }
    //查找指定key的rowid
    bool Select(int& rowid, char* p, ...)
    {
        Char_ a_;
        bool bret_;
        va_list charp;
        int i;

        CBF_Tools::StringCopy(a_.p[0], length - 1, p);
        va_start(charp, p);
        for (i = 0; i < count - 1; i++)
        {
            CBF_Tools::StringCopy(a_.p[i + 1], length - 1, va_arg(charp, char*));
        }
        va_end(charp);
        bret_ = m_key.Find(a_);
        if (bret_)
        {
            rowid = a_.id;
        }
        return bret_;
    }
    //查找指定key的rowid
    bool GetRowId(int& rid, char* p, ...)
    {
        Char_ a_;
        bool bret_;
        va_list charp;
        int i;

        CBF_Tools::StringCopy(a_.p[0], length - 1, p);
        va_start(charp, p);
        for (i = 0; i < count - 1; i++)
        {
            CBF_Tools::StringCopy(a_.p[i + 1], length - 1, va_arg(charp, char*));
        }
        va_end(charp);
        bret_ = m_key.Find(a_);
        if (bret_)
        {
            rid = a_.id;
        }
        return bret_;
    }
    //查找指定key是否存在
    bool Find(char* p, ...)
    {
        Char_ a_;
        bool bret_;
        va_list charp;
        int i;

        CBF_Tools::StringCopy(a_.p[0], length - 1, p);
        va_start(charp, p);
        for (i = 0; i < count - 1; i++)
        {
            CBF_Tools::StringCopy(a_.p[i + 1], length - 1, va_arg(charp, char*));
        }
        va_end(charp);
        bret_ = m_key.Find(a_);
        return bret_;
    }
    //删除指定key
    int Delete(char* p, ...)
    {
        Char_ a_;
        va_list charp;
        int i;

        CBF_Tools::StringCopy(a_.p[0], length - 1, p);
        va_start(charp, p);
        for (i = 0; i < count - 1; i++)
        {
            CBF_Tools::StringCopy(a_.p[i + 1], length - 1, va_arg(charp, char*));
        }
        va_end(charp);
        return m_key.Erase(a_);
    }
    //扫描全表，删除rowid
    void Delete(CInt iset)
    {
        Char_ a_;
        bool bret_;

        bret_ = m_key.First(a_);
        while (bret_)
        {
            if (iset.Find(a_.id))
            {
                m_key.EraseOne();
            }
            bret_ = m_key.Next(a_);
        }
    }
    bool First(int& id)
    {
        Char_ aa;
        if (!m_key.First(aa))
        {
            return false;
        }
        id = aa.id;
        return true;
    }
    bool Next(int& id)
    {
        Char_ aa;
        if (!m_key.Next(aa))
        {
            return false;
        }
        id = aa.id;
        return true;
    }
    bool End(int& id)
    {
        Char_ aa;
        if (!m_key.End(aa))
        {
            return false;
        }
        id = aa.id;
        return true;
    }
    bool Last(int& id)
    {
        Char_ aa;
        if (!m_key.Last(aa))
        {
            return false;
        }
        id = aa.id;
        return true;
    }
    void Clear()
    {
        m_key.Clear();
    }
    int Size()
    {
        return m_key.Size();
    }
};
//多个字符字段的主键，注意主键里的键值指向表里的对应字段，同时表必须用deque，因为vector地址会变

template<int count>
class CPkeyCharV
{
public:
	struct Char_
	{
		char *p[count];//多个字符索引
		int  id;              //内存表的rowid
	};
	struct Char_lt
	{
		bool operator()(const Char_ f1, const Char_ f2) const
		{
			int r=0;
			int i;

			for (i=0; i<count; i++)
			{
				r=strcmp(f1.p[i],f2.p[i]);
				if (r != 0)
				{
					break;
				}
			}
			return r<0;
		}
	};

	CUniqueIndexBase<Char_,Char_lt> m_key;  //key数据，不可重复

public:
	CPkeyCharV()
	{
	}
	~CPkeyCharV()
	{
	}
	//增加一条索引:id=rowid,索引...
	void Add(int id,char *p,...)
	{
		Char_ a_;
		va_list	charp;
		int i;

		a_.id=id;
		a_.p[0]=p;
		va_start(charp,p);
		for (i=0; i<count-1; i++)
		{
			a_.p[i+1] = va_arg(charp,char*);
		}
		va_end(charp);
		m_key.Add(a_);
		return;
	}
	//查找指定key的rowid  删除时使用
	bool Select(CInt &iset,char *p,...)
	{
		Char_ a_;
		bool bret_;
		va_list charp;
		int i;

		iset.Clear();
		a_.p[0] =p;
		va_start(charp,p);
		for (i=0; i<count-1; i++)
		{
			a_.p[i+1]= va_arg(charp,char*);
		}
		va_end(charp);
		bret_=m_key.Find(a_);
		if (bret_)
		{
			iset.Add(a_.id);
		}
		return bret_;
	}
	//查找指定key的rowid
	bool Select(int &rid,char *p,...)
	{
		Char_ a_;
		bool bret_;
		va_list charp;
		int i;
		
		a_.p[0] =p;
		va_start(charp,p);
		for (i=0; i<count-1; i++)
		{
			a_.p[i+1]= va_arg(charp,char*);
		}
		va_end(charp);
		bret_=m_key.Find(a_);
		if (bret_)
		{
			rid = a_.id;
		}
		return bret_;
	}
	//查找指定key的rowid
	bool GetRowId(int &rid,char *p,...)
	{
		Char_ a_;
		bool bret_;
		va_list charp;
		int i;
		
		a_.p[0] =p;
		va_start(charp,p);
		for (i=0; i<count-1; i++)
		{
			a_.p[i+1] = va_arg(charp,char*);
		}
		va_end(charp);
		bret_=m_key.Find(a_);
		if (bret_)
		{
			rid =a_.id;
		}
		return bret_;
	}
	//查找指定key是否存在
	bool Find(char *p,...)
	{
		Char_ a_;
		bool bret_;
		va_list charp;
		int i;

		a_.p[0] = p;
		va_start(charp,p);
		for (i=0; i<count-1; i++)
		{
			a_.p[i+1] = va_arg(charp,char*);
		}
		va_end(charp);
		bret_=m_key.Find(a_);
		return bret_;
	}
	//删除指定key
	int Delete(char *p,...)
	{
		Char_ a_;
		va_list charp;
		int i;

		a_.p[0]=p;
		va_start(charp,p);
		for (i=0; i<count-1; i++)
		{
			a_.p[i+1]=va_arg(charp,char*);
		}
		va_end(charp);
		return m_key.Erase(a_);
	}
    //扫描全表，删除rowid
    void Delete(CInt iset)
    {
        Char_ a_;
        bool bret_;

        bret_ = m_key.First(a_);
        while (bret_)
        {
            if (iset.Find(a_.id))
            {
                m_key.EraseOne();
            }
            bret_ = m_key.Next(a_);
        }
    }
	bool First(int &id)
	{
		Char_ aa;
		if (!m_key.First(aa))
		{
			return false;
		}
		id = aa.id;
		return true;
	}
	bool Next(int &id)
	{
		Char_ aa;
		if (!m_key.Next(aa))
		{
			return false;
		}
		id = aa.id;
		return true;
	}
	char *FirstKey(int &id)
	{
		Char_ aa;
		if (!m_key.First(aa))
		{
			return NULL;
		}
		id = aa.id;
		return aa.p[0];
	}
	char *NextKey(int &id)
	{
		Char_ aa;
		if (!m_key.Next(aa))
		{
			return NULL;
		}
		id = aa.id;
		return aa.p[0];
	}
	bool End(int &id)
	{
		Char_ aa;
		if (!m_key.End(aa))
		{
			return false;
		}
		id = aa.id;
		return true;
	}
	bool Last(int &id)
	{
		Char_ aa;
		if (!m_key.Last(aa))
		{
			return false;
		}
		id = aa.id;
		return true;
	}
	void Clear()
	{
		m_key.Clear();
	}
	int Size()
	{
		return m_key.Size();
	}
};
//多个字符字段的主键，注意主键里的键值指向表里的对应字段，同时表必须用deque，因为vector地址会变

template<int count>
class CPkeyCharVUnordered
{
public:
    struct Char_
    {
        char* p[count];//多个字符索引
        int  id;              //内存表的rowid
        bool operator == (const Char_& t) const
        {
            for (int i = 0; i < count; i++)
            {
                if (strcmp(t.p[i],p[i])!=0)
                {
                    return false;
                }
            }
            return true;
        }
    };
    struct Char_hash
    {
        std::size_t operator() (const Char_& f1) const
        {
            switch (count)
            {
            case 1:
                return hash_val(std::string(f1.p[0]));
            case 2:
                return hash_val(std::string(f1.p[0]), std::string(f1.p[1]));
            case 3:
                return hash_val(std::string(f1.p[0]), std::string(f1.p[1]), std::string(f1.p[2]));
            case 4:
                return hash_val(std::string(f1.p[0]), std::string(f1.p[1]), std::string(f1.p[2]), std::string(f1.p[3]));
            case 5:
                return hash_val(std::string(f1.p[0]), std::string(f1.p[1]), std::string(f1.p[2]), std::string(f1.p[3]), std::string(f1.p[4]));
            case 6:
                return hash_val(std::string(f1.p[0]), std::string(f1.p[1]), std::string(f1.p[2]), std::string(f1.p[3]), std::string(f1.p[4]), std::string(f1.p[5]));
            case 7:
                return hash_val(std::string(f1.p[0]), std::string(f1.p[1]), std::string(f1.p[2]), std::string(f1.p[3]), std::string(f1.p[4]), std::string(f1.p[5]), std::string(f1.p[6]));
            case 8:
                return hash_val(std::string(f1.p[0]), std::string(f1.p[1]), std::string(f1.p[2]), std::string(f1.p[3]), std::string(f1.p[4]), std::string(f1.p[5]), std::string(f1.p[6]), std::string(f1.p[7]));
            case 9:
                return hash_val(std::string(f1.p[0]), std::string(f1.p[1]), std::string(f1.p[2]), std::string(f1.p[3]), std::string(f1.p[4]), std::string(f1.p[5]), std::string(f1.p[6]), std::string(f1.p[7]), std::string(f1.p[8]));
            case 10:
                return hash_val(std::string(f1.p[0]), std::string(f1.p[1]), std::string(f1.p[2]), std::string(f1.p[3]), std::string(f1.p[4]), std::string(f1.p[5]), std::string(f1.p[6]), std::string(f1.p[7]), std::string(f1.p[8]), std::string(f1.p[9]));
            default:
                return hash_val(std::string(f1.p[0]), std::string(f1.p[1]), std::string(f1.p[2]), std::string(f1.p[3]), std::string(f1.p[4]), std::string(f1.p[5]), std::string(f1.p[6]), std::string(f1.p[7]), std::string(f1.p[8]), std::string(f1.p[9]));
            }
        }
    };

    CUniqueIndexBaseUnordered<Char_, Char_hash> m_key;  //key数据，不可重复

public:
	CPkeyCharVUnordered()
    {
    }
    ~CPkeyCharVUnordered()
    {
    }
    //增加一条索引:id=rowid,索引...
    void Add(int id, char* p, ...)
    {
        Char_ a_;
        va_list	charp;
        int i;

        a_.id = id;
        a_.p[0] = p;
        va_start(charp, p);
        for (i = 0; i < count - 1; i++)
        {
            a_.p[i + 1] = va_arg(charp, char*);
        }
        va_end(charp);
        m_key.Add(a_);
        return;
    }
    //查找指定key的rowid  删除时使用
    bool Select(CInt& iset, char* p, ...)
    {
        Char_ a_;
        bool bret_;
        va_list charp;
        int i;

        iset.Clear();
        a_.p[0] = p;
        va_start(charp, p);
        for (i = 0; i < count - 1; i++)
        {
            a_.p[i + 1] = va_arg(charp, char*);
        }
        va_end(charp);
        bret_ = m_key.Find(a_);
        if (bret_)
        {
            iset.Add(a_.id);
        }
        return bret_;
    }
    //查找指定key的rowid
    bool Select(int& rid, char* p, ...)
    {
        Char_ a_;
        bool bret_;
        va_list charp;
        int i;

        a_.p[0] = p;
        va_start(charp, p);
        for (i = 0; i < count - 1; i++)
        {
            a_.p[i + 1] = va_arg(charp, char*);
        }
        va_end(charp);
        bret_ = m_key.Find(a_);
        if (bret_)
        {
            rid = a_.id;
        }
        return bret_;
    }
    //查找指定key的rowid
    bool GetRowId(int& rid, char* p, ...)
    {
        Char_ a_;
        bool bret_;
        va_list charp;
        int i;

        a_.p[0] = p;
        va_start(charp, p);
        for (i = 0; i < count - 1; i++)
        {
            a_.p[i + 1] = va_arg(charp, char*);
        }
        va_end(charp);
        bret_ = m_key.Find(a_);
        if (bret_)
        {
            rid = a_.id;
        }
        return bret_;
    }
    //查找指定key是否存在
    bool Find(char* p, ...)
    {
        Char_ a_;
        bool bret_;
        va_list charp;
        int i;

        a_.p[0] = p;
        va_start(charp, p);
        for (i = 0; i < count - 1; i++)
        {
            a_.p[i + 1] = va_arg(charp, char*);
        }
        va_end(charp);
        bret_ = m_key.Find(a_);
        return bret_;
    }
    //删除指定key
    int Delete(char* p, ...)
    {
        Char_ a_;
        va_list charp;
        int i;

        a_.p[0] = p;
        va_start(charp, p);
        for (i = 0; i < count - 1; i++)
        {
            a_.p[i + 1] = va_arg(charp, char*);
        }
        va_end(charp);
        return m_key.Erase(a_);
    }
    //扫描全表，删除rowid
    void Delete(CInt iset)
    {
        Char_ a_;
        bool bret_;

        bret_ = m_key.First(a_);
        while (bret_)
        {
            if (iset.Find(a_.id))
            {
                m_key.EraseOne();
            }
            bret_ = m_key.Next(a_);
        }
    }
    bool First(int& id)
    {
        Char_ aa;
        if (!m_key.First(aa))
        {
            return false;
        }
        id = aa.id;
        return true;
    }
    bool Next(int& id)
    {
        Char_ aa;
        if (!m_key.Next(aa))
        {
            return false;
        }
        id = aa.id;
        return true;
    }
    char* FirstKey(int& id)
    {
        Char_ aa;
        if (!m_key.First(aa))
        {
            return NULL;
        }
        id = aa.id;
        return aa.p[0];
    }
    char* NextKey(int& id)
    {
        Char_ aa;
        if (!m_key.Next(aa))
        {
            return NULL;
        }
        id = aa.id;
        return aa.p[0];
    }
    bool End(int& id)
    {
        Char_ aa;
        if (!m_key.End(aa))
        {
            return false;
        }
        id = aa.id;
        return true;
    }
    bool Last(int& id)
    {
        Char_ aa;
        if (!m_key.Last(aa))
        {
            return false;
        }
        id = aa.id;
        return true;
    }
    void Clear()
    {
        m_key.Clear();
    }
    int Size()
    {
        return m_key.Size();
    }
};
//多个字符字段的索引，注意索引里的值指向表里的对应字段
template<int count>
class CIndexVarChar
{
public:
	struct Char_
	{
		char *p[count];//多个字符索引
		int  id;              //内存表的rowid
	};
	struct Char_lt
	{
		bool operator()(const Char_ f1, const Char_ f2) const
		{
			int r=0;
			int i;

			for (i=0; i<count; i++)
			{
				r=strcmp(f1.p[i],f2.p[i]);
				if (r != 0)
				{
					break;
				}
			}
			return r<0;
		}
	};

	CRepeatIndexBase<Char_,Char_lt> m_index;  //索引数据，可重复

public:
	CIndexVarChar()
	{
	}
	~CIndexVarChar()
	{
	}
	//增加一条索引:id=rowid,索引...
	void Add(int id,char *p,...)
	{
		Char_ a_;
		va_list	charp;
		int i;

		a_.id=id;
		a_.p[0]=p;
		va_start(charp,p);
		for (i=0; i<count-1; i++)
		{
			a_.p[i+1] = va_arg(charp,char*);
		}
		va_end(charp);
		m_index.Add(a_);
		return;
	}
	//查找指定的rowid  直接使用数据时使用
	bool Select(std::vector<int> &iset,char *p,...)
	{
		Char_ a_;
		bool bret_,bret2_;
		va_list charp;
		int i;
		
		iset.clear();
		a_.p[0] =p;
		va_start(charp,p);
		for (i=0; i<count-1; i++)
		{
			a_.p[i+1]= va_arg(charp,char*);
		}
		va_end(charp);
		bret2_=bret_=m_index.Find(a_);
		while (bret_)
		{
			iset.push_back(a_.id);
			bret_=m_index.Next(a_);
		}
		return bret2_;
	}
	//查找指定的rowid  删除时使用
	bool Select(CInt &iset,char *p,...)
	{
		Char_ a_;
		bool bret_,bret2_;
		va_list charp;
		int i;

		iset.Clear();
		a_.p[0] =p;
		va_start(charp,p);
		for (i=0; i<count-1; i++)
		{
			a_.p[i+1]= va_arg(charp,char*);
		}
		va_end(charp);
		bret2_=bret_=m_index.Find(a_);
		while (bret_)
		{
			iset.Add(a_.id);
			bret_=m_index.Next(a_);
		}
		return bret2_;
	}

	//删除指定
	int Delete(char *p,...)
	{
		Char_ a_;
		va_list charp;
		int i;

		a_.p[0]=p;
		va_start(charp,p);
		for (i=0; i<count-1; i++)
		{
			a_.p[i+1]=va_arg(charp,char*);
		}
		va_end(charp);
		return m_index.Erase(a_);
	}
	//删除索引，根据索引查找rowid,若rowid在iset中存在则删除索引
	void Delete(CInt iset,char *p,...)
	{
		Char_ a_;
		bool bret_;
		va_list	intp;
		int i;
		
		a_.p[0]=p;
		va_start(intp, p);
		for (i=0; i<count-1; i++)
		{
			a_.p[i+1]=va_arg(intp, char *);
		}
		va_end(intp);    
		bret_=m_index.Find(a_);
		while (bret_)
		{
			if (iset.Find(a_.id))
			{
				m_index.EraseOne();
			}
			bret_=m_index.Next(a_);
		}
	}
	//扫描全表，删除rowid
	void Delete(CInt iset)
	{
		Char_ a_;
		bool bret_;
		bret_=m_index.First(a_);
		while (bret_)
		{
			if (iset.Find(a_.id))
			{
				m_index.EraseOne();
			}
			bret_=m_index.Next(a_);
		}
	}
	//查找指定索引的数据是否存在，m_end置为最后一个大于此值的位置，可以Next取后续相同的值
	bool Find(int &rowid,char *p,...)
	{
		Char_ a_;
		bool bret_;
		va_list charp;
		int i;
		
		a_.p[0] = p;
		va_start(charp,p);
		for (i=0; i<count-1; i++)
		{
			a_.p[i+1] = va_arg(charp,char*);
		}
		va_end(charp);
		bret_=m_index.Find(a_);
		rowid = a_.id;
		return bret_;
	}
	//查找指定索引的数据是否存在，m_end为最后一值，NEXT取只要最后，可以按范围取值
	bool FindFirst(int &rowid,char *p,...)
	{
		Char_ a_;
		bool bret_;
		va_list charp;
		int i;
		
		a_.p[0] = p;
		va_start(charp,p);
		for (i=0; i<count-1; i++)
		{
			a_.p[i+1] = va_arg(charp,char*);
		}
		va_end(charp);
		bret_=m_index.FindFirst(a_);
		rowid = a_.id;
		return bret_;
	}
	bool First(int &id)
	{
		Char_ aa;
		if (!m_index.First(aa))
		{
			return false;
		}
		id = aa.id;
		return true;
	}
	bool Next(int &id)
	{
		Char_ aa;
		if (!m_index.Next(aa))
		{
			return false;
		}
		id = aa.id;
		return true;
	}
	bool End(int &id)
	{
		Char_ aa;
		if (!m_index.End(aa))
		{
			return false;
		}
		id = aa.id;
		return true;
	}
	bool Last(int &id)
	{
		Char_ aa;
		if (!m_index.Last(aa))
		{
			return false;
		}
		id = aa.id;
		return true;
	}
	void Clear()
	{
		m_index.Clear();
	}
	int Size()
	{
		return m_index.Size();
	}
};
//一个字符字段的索引
class CIndexCharV
{
public:
	struct Char_
	{
		char *p;  //字符索引值
		int  id;  //rowid
	};
	struct Char_lt
	{
		bool operator()(const Char_ f1, const Char_ f2) const
		{
			int r=0;

			r=strcmp(f1.p,f2.p);
			return r<0;
		}
	};

	CRepeatIndexBase<Char_,Char_lt> m_index;////索引数据，可重复

public:
	CIndexCharV()
	{
	}
	~CIndexCharV()
	{
	}
	//增加一条索引
	void Add(int id,char *p)
	{
		Char_ a_;
		
		a_.id=id;
		a_.p=p;
		m_index.Add(a_);
		return;
	}
	//根据索引查找rowid,放在iset中  直接使用数据时使用
	bool Select(std::vector<int> &iset,char *p)
	{
		Char_ a_;
		bool bret_,bret2_;
		
		iset.clear();
		a_.p=p;
		bret2_=bret_=m_index.Find(a_);
		while (bret_)
		{
			iset.push_back(a_.id);
			bret_=m_index.Next(a_);
		}
		return bret2_;
	}
	//根据索引查找rowid,放在iset中  删除时使用
	bool Select(CInt &iset,char *p)
	{
		Char_ a_;
		bool bret_,bret2_;
		
		iset.Clear();
		a_.p=p;
		bret2_=bret_=m_index.Find(a_);
		while (bret_)
		{
			iset.Add(a_.id);
			bret_=m_index.Next(a_);
		}
		return bret2_;
	}
	//查找指定索引的数据是否存在，m_end置为最后一个大于此值的位置，可以Next取后续相同的值
	bool Find(int &rowid,char *p)
	{
		Char_ a_;
		bool bret_;
		
		a_.p=p;
		bret_=m_index.Find(a_);
		rowid = a_.id;
		return bret_;
	}

	//查找指定索引的数据是否存在，m_end为最后一值，NEXT取只要最后，可以按范围取值
	bool FindFirst(int &rowid,char *p)
	{
		Char_ a_;
		bool bret_;
		
		a_.p=p;
		bret_=m_index.FindFirst(a_);
		rowid = a_.id;
		return bret_;
	}
	//删除索引
	int Delete(char *p)
	{
		Char_ a_;
		a_.p=p;
		return m_index.Erase(a_);
	}
	//删除索引，根据索引查找rowid,若rowid在iset中存在则删除索引
	void Delete(CInt iset,char *p)
	{
		Char_ a_;
		bool bret_;
		
		a_.p=p;
		bret_=m_index.Find(a_);
		while (bret_)
		{
			if (iset.Find(a_.id))
			{
				m_index.EraseOne();
			}
			bret_=m_index.Next(a_);
		}
	}
	//扫描全表，删除rowid
	void Delete(CInt iset)
	{
		Char_ a_;
		bool bret_;
		
		bret_=m_index.First(a_);
		while (bret_)
		{
			if (iset.Find(a_.id))
			{
				m_index.EraseOne();
			}
			bret_=m_index.Next(a_);
		}
	}
	bool First(int &id)
	{
		Char_ aa;
		if (!m_index.First(aa))
		{
			return false;
		}
		id = aa.id;
		return true;
	}
	bool Next(int &id)
	{
		Char_ aa;
		if (!m_index.Next(aa))
		{
			return false;
		}
		id = aa.id;
		return true;
	}
	bool End(int &id)
	{
		Char_ aa;
		if (!m_index.End(aa))
		{
			return false;
		}
		id = aa.id;
		return true;
	}
	bool Last(int &id)
	{
		Char_ aa;
		if (!m_index.Last(aa))
		{
			return false;
		}
		id = aa.id;
		return true;
	}
	void Clear()
	{
		m_index.Clear();
	}
	int Size()
	{
		return m_index.Size();
	}
};

//多个64位无符号整型字段的索引，可重复
template<int count>
class CIndexUint64
{
public:
	struct Int64_
	{
		UINT64_	k[count];    //多个整型索引
		int id;          //对应表的ID，相当于rowid
	};
	struct Int64_lt
	{
		bool operator()(const Int64_ f1, const Int64_ f2) const
		{
			INT64_ r=0;
			int i;

			for (i=0; i<count; i++)
			{
				r=f1.k[i]-f2.k[i];
				if (r != 0)
				{
					break;
				}
			}
			return r<0;
		}
	};

	CRepeatIndexBase<Int64_,Int64_lt> m_index;//索引数据，可重复

public:
	CIndexUint64()
	{
	}
	~CIndexUint64()
	{
	}
	//增加一条索引:rowid,索引...
	void Add(int id,UINT64_ ind,...)
	{
		Int64_ a_;
		va_list	intp;
		int i;

		a_.id=id;
		a_.k[0]=  ind;
		va_start(intp, ind);
		for (i=0; i<count-1; i++)
		{
			a_.k[i+1]=va_arg(intp, UINT64_);
		}
		va_end(intp);    
		m_index.Add(a_);
		return;
	}
	//查找指定索引的rowid  直接使用数据时使用
	bool Select(std::vector<int> &iset,UINT64_ k,...)
	{
		Int64_ a_;
		bool bret_,bret2_;
		int i;
		va_list	intp;
		
		iset.clear();
		a_.k[0]=k;
		va_start(intp, k);
		for (i=0; i<count-1; i++)
		{
			a_.k[i+1]=va_arg(intp, UINT64_);
		}
		va_end(intp);    
		bret2_=bret_=m_index.Find(a_);
		while (bret_)
		{
			iset.push_back(a_.id);
			bret_=m_index.Next(a_);
		}
		return bret2_;
	}
	//查找指定索引的rowid  删除时使用
	bool Select(CInt &iset,UINT64_ k,...)
	{
		Int64_ a_;
		bool bret_,bret2_;
		int i;
		va_list	intp;
		
		iset.Clear();
		a_.k[0]=k;
		va_start(intp, k);
		for (i=0; i<count-1; i++)
		{
			a_.k[i+1]=va_arg(intp, UINT64_);
		}
		va_end(intp);    
		bret2_=bret_=m_index.Find(a_);
		while (bret_)
		{
			iset.Add(a_.id);
			bret_=m_index.Next(a_);
		}
		return bret2_;
	}
	//查找指定索引的数据是否存在，m_end置为最后一个大于此值的位置，可以Next取后续相同的值
	bool Find(int &rowid,UINT64_ k,...)
	{
		Int64_ a_;
		a_.id=-1;
		bool bret_;
		int i;
		va_list	intp;
		
		a_.k[0]=k;
		va_start(intp, k);
		for (i=0; i<count-1; i++)
		{
			a_.k[i+1]=va_arg(intp, UINT64_);
		}
		va_end(intp);    
		bret_=m_index.Find(a_);
		rowid = a_.id;
		return bret_;
	}
	//查找指定索引的数据是否存在，m_end为最后一值，NEXT取只要最后，可以按范围取值
	bool FindFirst(int &rowid,UINT64_ k,...)
	{
		Int64_ a_;
		bool bret_;
		int i;
		va_list	intp;
		
		a_.k[0]=k;
		va_start(intp, k);
		for (i=0; i<count-1; i++)
		{
			a_.k[i+1]=va_arg(intp, UINT64_);
		}
		va_end(intp);    
		bret_=m_index.FindFirst(a_);
		rowid = a_.id;
		return bret_;
	}
	//删除指定索引
	int Delete(UINT64_ k,...)
	{
		Int64_ a_;
		va_list	intp;
		int i;

		a_.k[0]=k;
		va_start(intp, k);
		for (i=0; i<count-1; i++)
		{
			a_.k[i+1]=va_arg(intp, UINT64_);
		}
		va_end(intp);    
		return m_index.Erase(a_);
	}
	//根据索引查找rowid，若rowid在iset中存在则删除索引
	void Delete(CInt iset,UINT64_ k,...)
	{
		Int64_ a_;
		bool bret_;
		va_list	intp;
		int i;
		
		a_.k[0]=k;
		va_start(intp, k);
		for (i=0; i<count-1; i++)
		{
			a_.k[i+1]=va_arg(intp, UINT64_);
		}
		va_end(intp);    
		bret_=m_index.Find(a_);
		while (bret_)
		{
			if (iset.Find(a_.id))
			{
				m_index.EraseOne();
			}
			bret_=m_index.Next(a_);
		}
	}
	//扫描全表，删除rowid
	void Delete(CInt iset)
	{
		Int64_ a_;
		bool bret_;
		
		bret_=m_index.First(a_);
		while (bret_)
		{
			if (iset.Find(a_.id))
			{
				m_index.EraseOne();
			}
			bret_=m_index.Next(a_);
		}
	}
	bool First(int &id)
	{
		Int64_ aa;
		if (!m_index.First(aa))
		{
			return false;
		}
		id = aa.id;
		return true;
	}
	bool Next(int &id)
	{
		Int64_ aa;
		if (!m_index.Next(aa))
		{
			return false;
		}
		id = aa.id;
		return true;
	}
	bool End(int& rid)
	{
        Int64_ aa;
        if (!m_index.End(aa))
        {
            return false;
        }
        rid = aa.id;
        return true;
	}
    bool Last(int& id)
    {
		Int64_ aa;
        if (!m_index.Last(aa))
        {
            return false;
        }
        id = aa.id;
        return true;
    }
	void Clear()
	{
		m_index.Clear();
	}
	int Size()
	{
		return m_index.Size();
	}
};


//多个64位无符号整型字段的key，不可重复
template<int count>
class CPkeyUint64
{
public:
	struct Int64_
	{
		UINT64_	k[count];    //多个整型索引
		int id;          //对应表的ID，相当于rowid
	};
	struct Int64_lt
	{
		bool operator()(const Int64_ f1, const Int64_ f2) const
		{
			INT64_ r=0;
			int i;

			for (i=0; i<count; i++)
			{
                if (f1.k[i] == f2.k[i])
                {
                    continue;
                }
                else
                {
                    if (f1.k[i] > f2.k[i])
                    {
                        r = 1;
                    }
                    else
                    {
                        r = -1;
                    }
                    break;
                }
				//r=f1.k[i]-f2.k[i];
				//if (r != 0)
				//{
				//	break;
				//}
			}
			return r<0;
		}
	};

	CUniqueIndexBase<Int64_,Int64_lt> m_key;//key数据，不可重复

public:
	CPkeyUint64()
	{
	}
	~CPkeyUint64()
	{
	}
	//增加一条索引:rowid,索引...
	void Add(int id,UINT64_ ind,...)
	{
		Int64_ a_;
		va_list	intp;
		int i;

		a_.id=id;
		a_.k[0]=ind;
		va_start(intp, ind);
		for (i=0; i<count-1; i++)
		{
			a_.k[i+1]=va_arg(intp, UINT64_);
		}
		va_end(intp);    
		m_key.Add(a_);
		return;
	}
	//查找指定key的rowid  删除时使用
	bool Select(CInt &iset,UINT64_ k,...)
	{
		Int64_ a_;
		bool bret_;
		int i;
		va_list	intp;
		
		iset.Clear();
		a_.k[0]=k;
		va_start(intp, k);
		for (i=0; i<count-1; i++)
		{
			a_.k[i+1]=va_arg(intp, UINT64_);
		}
		va_end(intp);    
		bret_=m_key.Find(a_);
		if (bret_)
		{
			iset.Add(a_.id);
		}
		return bret_;
	}
	//查找指定key的rowid
	bool Select(int  &rid,UINT64_ k,...)
	{
		Int64_ a_;
		bool bret_;
		int i;
		va_list	intp;
		
		a_.k[0]=k;
		va_start(intp, k);
		for (i=0; i<count-1; i++)
		{
			a_.k[i+1]=va_arg(intp, UINT64_);
		}
		va_end(intp);    
		bret_=m_key.Find(a_);
		if (bret_)
		{
			rid =a_.id;
		}
		return bret_;
	}
	//查找指定key的数据是否存在
	bool Find(UINT64_ k,...)
	{
		Int64_ a_;
		bool bret_;
		int i;
		va_list	intp;
		
		a_.k[0]=k;
		va_start(intp, k);
		for (i=0; i<count-1; i++)
		{
			a_.k[i+1]=va_arg(intp, UINT64_);
		}
		va_end(intp);    
		bret_=m_key.Find(a_);
		return bret_;
	}
	//删除指定key
	int Delete(UINT64_ k,...)
	{
		Int64_ a_;
		va_list	intp;
		int i;

		a_.k[0]=k;
		va_start(intp, k);
		for (i=0; i<count-1; i++)
		{
			a_.k[i+1]=va_arg(intp, UINT64_);
		}
		va_end(intp);    
		return m_key.Erase(a_);
	}
    //扫描全表，删除rowid
    void Delete(CInt iset)
    {
		Int64_ a_;
        bool bret_;
        bret_ = m_key.First(a_);
        while (bret_)
        {
            if (iset.Find(a_.id))
            {
                m_key.Erase(a_);
            }
            bret_ = m_key.Next(a_);
        }
    }
	bool First(int &id)
	{
		Int64_ aa;
		if (!m_key.First(aa))
		{
			return false;
		}
		id = aa.id;
		return true;
	}
	bool Next(int &id)
	{
		Int64_ aa;
		if (!m_key.Next(aa))
		{
			return false;
		}
		id = aa.id;
		return true;
	}
    bool End(int& rid)
    {
        Int64_ aa;
        if (!m_key.End(aa))
        {
        return false;
        }
        rid = aa.id;
        return true;
    }
    bool Last(int& id)
    {
        Int64_ aa;
        if (!m_key.Last(aa))
        {
            return false;
        }
        id = aa.id;
        return true;
    }
	void Clear()
	{
		m_key.Clear();
	}
	int Size()
	{
		return m_key.Size();
	}
};
//多个64位无符号整型字段的key，不可重复
template<int count>
class CPkeyUint64Unordered
{
public:
    struct Int64_
    {
        UINT64_	k[count];    //多个整型索引
        int id;          //对应表的ID，相当于rowid
        bool operator == (const Int64_& t) const
        {
            for (int i = 0; i < count; i++)
            {
                if (t.k[i] != k[i])
                {
                    return false;
                }
            }
            return true;
        }
    };
    struct Int_hash
    {
        std::size_t operator() (const Int64_& f1) const
        {
            switch (count)
            {
            case 1:
                return hash_val(f1.k[0]);
            case 2:
                return hash_val(f1.k[0], f1.k[1]);
            case 3:
                return hash_val(f1.k[0], f1.k[1], f1.k[2]);
            case 4:
                return hash_val(f1.k[0], f1.k[1], f1.k[2], f1.k[3]);
            case 5:
                return hash_val(f1.k[0], f1.k[1], f1.k[2], f1.k[3], f1.k[4]);
            case 6:
                return hash_val(f1.k[0], f1.k[1], f1.k[2], f1.k[3], f1.k[4], f1.k[5]);
            case 7:
                return hash_val(f1.k[0], f1.k[1], f1.k[2], f1.k[3], f1.k[4], f1.k[5], f1.k[6]);
            case 8:
                return hash_val(f1.k[0], f1.k[1], f1.k[2], f1.k[3], f1.k[4], f1.k[5], f1.k[6], f1.k[7]);
            case 9:
                return hash_val(f1.k[0], f1.k[1], f1.k[2], f1.k[3], f1.k[4], f1.k[5], f1.k[6], f1.k[7], f1.k[8]);
            case 10:
                return hash_val(f1.k[0], f1.k[1], f1.k[2], f1.k[3], f1.k[4], f1.k[5], f1.k[6], f1.k[7], f1.k[8], f1.k[9]);
            default:
                return hash_val(f1.k[0], f1.k[1], f1.k[2], f1.k[3], f1.k[4], f1.k[5], f1.k[6], f1.k[7], f1.k[8], f1.k[9]);
            }
        }
    };

    CUniqueIndexBaseUnordered<Int64_, Int_hash> m_key;//key数据，不可重复

public:
	CPkeyUint64Unordered()
    {
    }
    ~CPkeyUint64Unordered()
    {
    }
    //增加一条索引:rowid,索引...
    void Add(int id, UINT64_ ind, ...)
    {
        Int64_ a_;
        va_list	intp;
        int i;

        a_.id = id;
        a_.k[0] = ind;
        va_start(intp, ind);
        for (i = 0; i < count - 1; i++)
        {
            a_.k[i + 1] = va_arg(intp, UINT64_);
        }
        va_end(intp);
        m_key.Add(a_);
        return;
    }
    //查找指定key的rowid  删除时使用
    bool Select(CInt& iset, UINT64_ k, ...)
    {
        Int64_ a_;
        bool bret_;
        int i;
        va_list	intp;

        iset.Clear();
        a_.k[0] = k;
        va_start(intp, k);
        for (i = 0; i < count - 1; i++)
        {
            a_.k[i + 1] = va_arg(intp, UINT64_);
        }
        va_end(intp);
        bret_ = m_key.Find(a_);
        if (bret_)
        {
            iset.Add(a_.id);
        }
        return bret_;
    }
    //查找指定key的rowid
    bool Select(int& rid, UINT64_ k, ...)
    {
        Int64_ a_;
        bool bret_;
        int i;
        va_list	intp;

        a_.k[0] = k;
        va_start(intp, k);
        for (i = 0; i < count - 1; i++)
        {
            a_.k[i + 1] = va_arg(intp, UINT64_);
        }
        va_end(intp);
        bret_ = m_key.Find(a_);
        if (bret_)
        {
            rid = a_.id;
        }
        return bret_;
    }
    //查找指定key的数据是否存在
    bool Find(UINT64_ k, ...)
    {
        Int64_ a_;
        bool bret_;
        int i;
        va_list	intp;

        a_.k[0] = k;
        va_start(intp, k);
        for (i = 0; i < count - 1; i++)
        {
            a_.k[i + 1] = va_arg(intp, UINT64_);
        }
        va_end(intp);
        bret_ = m_key.Find(a_);
        return bret_;
    }
    //删除指定key
    int Delete(UINT64_ k, ...)
    {
        Int64_ a_;
        va_list	intp;
        int i;

        a_.k[0] = k;
        va_start(intp, k);
        for (i = 0; i < count - 1; i++)
        {
            a_.k[i + 1] = va_arg(intp, UINT64_);
        }
        va_end(intp);
        return m_key.Erase(a_);
    }
    //扫描全表，删除rowid
    void Delete(CInt iset)
    {
        Int64_ a_;
        bool bret_;
        bret_ = m_key.First(a_);
        while (bret_)
        {
            if (iset.Find(a_.id))
            {
                m_key.Erase(a_);
            }
            bret_ = m_key.Next(a_);
        }
    }
    bool First(int& id)
    {
        Int64_ aa;
        if (!m_key.First(aa))
        {
            return false;
        }
        id = aa.id;
        return true;
    }
    bool Next(int& id)
    {
        Int64_ aa;
        if (!m_key.Next(aa))
        {
            return false;
        }
        id = aa.id;
        return true;
    }
    void Clear()
    {
        m_key.Clear();
    }
    int Size()
    {
        return m_key.Size();
    }
};

//内存表
template<typename Record>
class CMemTableNew  
{
public:
	//songfree 20180419 改为deque是因为vector存储时，增加元素时原来的地址为变，使用PKEYCHARV等指针作为索引时不可用。
	//typedef std::vector<Record> VECTOR_;       //定义保存内存表数据的类型，vector的下标相当于rowid
	typedef std::deque<Record> VECTOR_;       //定义保存内存表数据的类型，deque的下标相当于rowid
	std::list<int>    m_rowidpool;              //保存删除的内存表数据ID，相当于rowid，下次增加时直接使用         
	VECTOR_      m_table;                  //表数据
	std::deque<bool> m_inuseid;                //rowid对应的数据是否删除
	int  m_current;
public:
	
	CMemTableNew()
	{
		m_current = 0;
	}
	~CMemTableNew()
	{
		Clear();
	}
	//当记录删除时，过滤掉
    Record *At(int index)
	{
		if (index <0 || index >m_table.size()-1)
		{
			return NULL;
		}
		if (m_inuseid[index]) //没有删除
		{
			return &(m_table.at(index));
		}
		else
		{
			return NULL;
		}
		
	}
	void Clear()
	{
		m_table.clear();
		m_inuseid.clear();
		m_rowidpool.clear();
	}
	int Add(Record& rt)
	{
		int id;
		if (m_rowidpool.size() > 0)
		{
			id=m_rowidpool.front();
			m_rowidpool.pop_front();
			m_table[id]=rt;
			m_inuseid[id]=true;
		}
		else
		{
			id=m_table.size();
			m_table.push_back(rt);
			m_inuseid.push_back(true);
		}
		return id;
	}
	void Update(int id,Record& rt,bool logged=false)
	{
		m_table[id]=rt;
	}
	void GetById(int id,Record& rt)
	{
		rt=m_table[id];
	}
	void Delete(int id)
	{
		if (m_inuseid[id] != false)
		{
			m_rowidpool.push_back(id);
		}
		m_inuseid[id]=false;
	}
	bool First(Record& rt)
	{
		m_current = 0;
		for (unsigned int i=0;i<m_table.size();i++)
		{
			if (m_inuseid[i])
			{
				rt = m_table[i];
				m_current = i;
				return true;
			}
		}
		return false;
	}
	Record *First()
	{
		m_current = 0;
		for (unsigned int i=0;i<m_table.size();i++)
		{
			if (m_inuseid[i])
			{
				m_current = i;
				return &(m_table[i]);
			}
		}
		return NULL;
	}
	bool Next(Record& rt)
	{
		m_current++;
		for (;m_current<m_table.size();m_current++)
		{
			if (m_inuseid[m_current])
			{
				rt = m_table[m_current];
				return true;
			}
		}
		return false;
	}
	Record *Next()
	{
		m_current++;
		for (;m_current<m_table.size();m_current++)
		{
			if (m_inuseid[m_current])
			{
				return &(m_table[m_current]);
			}
		}
		return NULL;
	}
	//包括删除的记录
	int AllSize()
	{
		return m_table.size();
	}
	//不包括删除的记录
	int RealSize()
	{
		return m_table.size()-m_rowidpool.size();
	}
	//不包括删除的记录
	int Size()
	{
		return m_table.size()-m_rowidpool.size();
	}
};
////内存表
//template<typename Record>
//class CMemTable  
//{
//public:
//	
//	typedef std::vector<Record> VECTOR_;       //定义保存内存表数据的类型，vector的下标相当于rowid
////	typedef std::deque<Record> VECTOR_;       //定义保存内存表数据的类型，vector的下标相当于rowid
//	std::list<int>    m_rowidpool;              //保存删除的内存表数据ID，相当于rowid，下次增加时直接使用         
//	VECTOR_      m_table;                  //表数据
//	std::vector<bool> m_inuseid;                //rowid对应的数据是否删除
//	unsigned int  m_current;
//public:
//	
//	CMemTable()
//	{
//		m_current = 0;
//	}
//	~CMemTable()
//	{
//		Clear();
//	}
//	//当记录删除时，过滤掉
//    Record *At(int index)
//	{
//		if (index <0 || index >m_table.size()-1)
//		{
//			return NULL;
//		}
//		if (m_inuseid[index]) //没有删除
//		{
//			return &(m_table.at(index));
//		}
//		else
//		{
//			return NULL;
//		}
//		
//	}
//	void Clear()
//	{
//		m_table.clear();
//		m_inuseid.clear();
//		m_rowidpool.clear();
//	}
//	int Add(Record& rt)
//	{
//		int id;
//		if (m_rowidpool.size() > 0)
//		{
//			id=m_rowidpool.front();
//			m_rowidpool.pop_front();
//			m_table[id]=rt;
//			m_inuseid[id]=true;
//		}
//		else
//		{
//			id=m_table.size();
//			m_table.push_back(rt);
//			m_inuseid.push_back(true);
//		}
//		return id;
//	}
//	void Update(int id,Record& rt,bool logged=false)
//	{
//		m_table[id]=rt;
//	}
//	void GetById(int id,Record& rt)
//	{
//		rt=m_table[id];
//	}
//	void Delete(int id)
//	{
//		if (m_inuseid[id] != false)
//		{
//			m_rowidpool.push_back(id);
//		}
//		m_inuseid[id]=false;
//	}
//	bool First(Record& rt)
//	{
//		m_current = 0;
//		for (unsigned int i=0;i<m_table.size();i++)
//		{
//			if (m_inuseid[i])
//			{
//				rt = m_table[i];
//				m_current = i;
//				return true;
//			}
//		}
//		return false;
//	}
//	Record *First()
//	{
//		m_current = 0;
//		for (int i=0;i<m_table.size();i++)
//		{
//			if (m_inuseid[i])
//			{
//				m_current = i;
//				return &(m_table[i]);
//			}
//		}
//		return NULL;
//	}
//	bool Next(Record& rt)
//	{
//		m_current++;
//		for (;m_current<m_table.size();m_current++)
//		{
//			if (m_inuseid[m_current])
//			{
//				rt = m_table[m_current];
//				return true;
//			}
//		}
//		return false;
//	}
//	Record *Next()
//	{
//		m_current++;
//		for (;m_current<m_table.size();m_current++)
//		{
//			if (m_inuseid[m_current])
//			{
//				return &(m_table[m_current]);
//			}
//		}
//		return NULL;
//	}
//	//包括删除的记录
//	int AllSize()
//	{
//		return m_table.size();
//	}
//	//不包括删除的记录
//	int RealSize()
//	{
//		return m_table.size()-m_rowidpool.size();
//	}
//	//不包括删除的记录
//	int Size()
//	{
//		return m_table.size()-m_rowidpool.size();
//	}
//};

//索引字段基类
class CIndex_Field_Base
{
public:
	int   m_nRowId;//rowid，表的rowid

	//需要实现的字段索引比较方法
	virtual bool operator<(const CIndex_Field_Base& field) const
	{
		return false;
	}
};
//c++11用的索引主键模板类
template<typename Record>
class Index_tuple : public CIndex_Field_Base
{
public:
    Record data;
    virtual bool operator<(const Index_tuple& field) const
    {
        return data<field.data;
    }
};


//Index_tuple使用例子
//CKeyField<Index_tuple<tuple<UINT64_,std::string>>> m_key; 
//Index_tuple<tuple<UINT64_,std::string>> aa ;
//        aa.data = {info.user_id,info.mac};
//        aa.data = std::make_tuple(info.user_id,info.mac);
//        if (m_key.Find(aa)) {



//不重复的索引基类
template<typename Record>
class CUniqueFieldIndexBase
{
public:
	typedef std::set<Record> SET_;
	typedef typename SET_::iterator LP_SET;
	SET_    m_index;//索引数据，Record不可能重复
	LP_SET  m_current;//当前索引
	LP_SET  m_end;
	LP_SET  m_begin;
    bool         m_bDroped;
public:
	CUniqueFieldIndexBase()
	{
		m_bDroped = false;
	}
	~CUniqueFieldIndexBase()
	{
		Clear();
	}
	void Clear()
	{
		m_index.clear();
	}
    bool End(Record& rt)
    {
        m_begin = m_index.begin();
        m_current = m_index.end();
        if (m_current == m_end)
        {
            return false;
        }
        else
        {
            m_current--;
            rt = *m_current;
            return true;
        }
    }
    bool Last(Record& rt)
    {
        if (!m_bDroped)
        {
            m_current--;
        }
        else
        {
            m_bDroped = false;
        }
        if (m_current == m_begin)
        {
            return false;
        }
        else
        {
            rt = *m_current;
            return true;
        }
    }
    bool First(Record& rt)
    {
        m_current = m_index.begin();
        m_end = m_index.end();
        if (m_current == m_end)
        {
            return false;
        }
        else
        {
            rt = *m_current;
            return true;
        }
    }
    bool Next(Record& rt)
    {
        if (!m_bDroped)
        {
            m_current++;
        }
        else
        {
            m_bDroped = false;
        }
        if (m_current == m_end)
        {
            return false;
        }
        else
        {
            rt = *m_current;
            return true;
        }
    }
	bool Find(Record& rt)
	{
		m_current = m_index.find(rt);
		if(m_current == m_index.end())
		{
			return false;
		}
		else
		{
			rt = *m_current;
			return true;
		}
	}
	void Add(Record& rt)
	{
// #ifdef _GCC
// 		Erase(rt);
// #endif
		//pair<LP_SET,bool> r = m_index.insert(rt);
		m_index.insert(rt);
// #ifndef _GCC
// 		if(!r.second) *r.first = rt;
// #endif
	}
	void Erase(Record& rt)
	{
		m_index.erase(rt);
	}
	void EraseOne()
	{
		LP_SET tmp = m_current;
		m_current++;
		m_index.erase(tmp);
		m_bDroped = true;
	}
	int Size()
	{
		return m_index.size();
	}
};

//不重复的索引基类
template<typename Record, typename HashSet>
class CUniqueFieldIndexBaseUnordered
{
public:
    typedef std::unordered_set<Record, HashSet> SET_;
    typedef typename SET_::iterator LP_SET;
    SET_    m_index;//索引数据，Record不可能重复
    LP_SET  m_current;//当前索引
    LP_SET  m_end;
    LP_SET  m_begin;
    bool         m_bDroped;
public:
	CUniqueFieldIndexBaseUnordered()
    {
        m_bDroped = false;
    }
    ~CUniqueFieldIndexBaseUnordered()
    {
        Clear();
    }
    void Clear()
    {
        m_index.clear();
    }
    bool End(Record& rt)
    {
        m_begin = m_index.begin();
        m_current = m_index.end();
        if (m_current == m_end)
        {
            return false;
        }
        else
        {
            m_current--;
            rt = *m_current;
            return true;
        }
    }
    bool Last(Record& rt)
    {
        if (!m_bDroped)
        {
            m_current--;
        }
        else
        {
            m_bDroped = false;
        }
        if (m_current == m_begin)
        {
            return false;
        }
        else
        {
            rt = *m_current;
            return true;
        }
    }
    bool First(Record& rt)
    {
        m_current = m_index.begin();
        m_end = m_index.end();
        if (m_current == m_end)
        {
            return false;
        }
        else
        {
            rt = *m_current;
            return true;
        }
    }
    bool Next(Record& rt)
    {
        if (!m_bDroped)
        {
            m_current++;
        }
        else
        {
            m_bDroped = false;
        }
        if (m_current == m_end)
        {
            return false;
        }
        else
        {
            rt = *m_current;
            return true;
        }
    }
    bool Find(Record& rt)
    {
        m_current = m_index.find(rt);
        if (m_current == m_index.end())
        {
            return false;
        }
        else
        {
            rt = *m_current;
            return true;
        }
    }
    void Add(Record& rt)
    {
        // #ifdef _GCC
        // 		Erase(rt);
        // #endif
                //pair<LP_SET,bool> r = m_index.insert(rt);
        m_index.insert(rt);
        // #ifndef _GCC
        // 		if(!r.second) *r.first = rt;
        // #endif
    }
    void Erase(Record& rt)
    {
        m_index.erase(rt);
    }
    void EraseOne()
    {
        LP_SET tmp = m_current;
        m_current++;
        m_index.erase(tmp);
        m_bDroped = true;
    }
    int Size()
    {
        return m_index.size();
    }
};

//重复的索引基类
template<typename Record>
class CRepeatFieldIndexBase
{
public:
	typedef std::multiset<Record> MULTISET_;
	typedef typename MULTISET_::iterator LP_MULTISET;
	MULTISET_    m_index;   //索引数据 Record可能有多个
	LP_MULTISET  m_current; 
	LP_MULTISET  m_end;
	LP_MULTISET  m_begin;
	bool         m_bDroped;

public:
	CRepeatFieldIndexBase()
	{
		m_bDroped=false;
	}
	~CRepeatFieldIndexBase()
	{
		Clear();
	}
	void Clear()
	{
		m_index.clear();
	}
	bool End(Record& rt)
	{
		m_begin = m_index.begin();
		m_current = m_index.end();
		if(m_current == m_end)
		{
			return false;
		}
		else 
		{
			m_current--;
			rt = *m_current;
			return true;
		}
	}
	bool Last(Record& rt)
	{
		if (!m_bDroped)
		{
			m_current--;
		}
		else
		{
			m_bDroped=false;
		}
		if(m_current == m_begin)
		{
			return false;
		}
		else
		{
			rt = *m_current;
			return true;
		}
	}
	bool First(Record& rt)
	{
		m_current = m_index.begin();
		m_end=m_index.end();
		if(m_current == m_end)
		{
			return false;
		}
		else
		{
			rt = *m_current;
			return true;
		}
	}
	bool Next(Record& rt)
	{
		if (!m_bDroped)
		{
			m_current++;
		}
		else
		{
			m_bDroped=false;
		}
		if(m_current == m_end)
		{
			return false;
		}
		else
		{
			rt = *m_current;
			return true;
		}
	}
	//查找第一个的位置和最后一个大于此值的位置，NEXT可以在这之间取得多个相同值的数据
	bool Find(Record& rt)
	{
		m_bDroped=false;
		m_current = m_index.find(rt);
		m_end=m_index.upper_bound(rt);//第一个大于此值的位置，以后可以Next使用
		if(m_current == m_end)
		{
			return false;
		}
		else
		{
			if (m_current == m_index.end())
			{
				return false;
			}
			else
			{
				rt = *m_current;
				return true;
			}
		}
	}
	//查找第一个的位置，NEXT可以继续找，只要最后结束
	bool FindFirst(Record& rt)
	{
		m_bDroped=false;
		m_current = m_index.find(rt);
		m_end= m_index.end();
		if(m_current == m_end)
		{
			return false;
		}
		else
		{
			if (m_current == m_index.end())
			{
				return false;
			}
			else
			{
				rt = *m_current;
				return true;
			}
		}
		
	}
	void Add(Record& rt)
	{
		m_index.insert(rt);
	}

	void EraseOne()
	{
		LP_MULTISET tmp= m_current;
		m_current++;
		m_index.erase(tmp);
		m_bDroped=true;
	}
	void Erase(Record& rt)
	{
		m_index.erase(rt);
	}
	int Size()
	{
		return m_index.size();
	}
};

template<typename Record>
class CKeyField
{
public:
	CUniqueFieldIndexBase<Record> m_key;  //key数据，不可重复

public:
	CKeyField()
	{
	}
	~CKeyField()
	{
	}
	//增加一条索引:id=rowid,索引...
	void Add(Record &rf)
	{
		m_key.Add(rf);
		return;
	}
	bool Select(Record &rf)
	{
		return m_key.Find(rf);
	}
	
	//查找指定key是否存在
	bool Find(Record &rf)
	{
		return m_key.Find(rf);
	}
    //扫描全表，删除rowid
    void Delete(CInt iset)
    {
        Record a_;
        bool bret_;
        bret_ = m_key.First(a_);
        while (bret_)
        {
            if (iset.Find(a_.m_nRowId))
            {
				m_key.EraseOne();
            }
            bret_ = m_key.Next(a_);
        }
    }
	//删除指定key
	void Delete(Record &rf)
	{
		m_key.Erase(rf);
		return;
	}
	void EraseOne()
	{
		m_key.EraseOne();
		return;
	}
	bool First(Record &rf)
	{
		if (!m_key.First(rf))
		{
			return false;
		}
		return true;
	}
	bool Next(Record &rf)
	{
		if (!m_key.Next(rf))
		{
			return false;
		}
		return true;
	}
	bool End(Record &rf)
	{
		
		if (!m_key.End(rf))
		{
			return false;
		}
		return true;
	}
	bool Last(Record &rf)
	{
		if (!m_key.Last(rf))
		{
			return false;
		}
		return true;
	}
	void Clear()
	{
		m_key.Clear();
	}
	int Size()
	{
		return m_key.Size();
	}
};

template<typename Record,typename HashSet>
class CKeyFieldUnordered
{
public:
    CUniqueFieldIndexBaseUnordered<Record, HashSet> m_key;  //key数据，不可重复

public:
	CKeyFieldUnordered()
    {
    }
    ~CKeyFieldUnordered()
    {
    }
    //增加一条索引:id=rowid,索引...
    void Add(Record& rf)
    {
        m_key.Add(rf);
        return;
    }
    bool Select(Record& rf)
    {
        return m_key.Find(rf);
    }

    //查找指定key是否存在
    bool Find(Record& rf)
    {
        return m_key.Find(rf);
    }
    //扫描全表，删除rowid
    void Delete(CInt iset)
    {
        Record a_;
        bool bret_;
        bret_ = m_key.First(a_);
        while (bret_)
        {
            if (iset.Find(a_.m_nRowId))
            {
                m_key.EraseOne();
            }
            bret_ = m_key.Next(a_);
        }
    }
    //删除指定key
    void Delete(Record& rf)
    {
        m_key.Erase(rf);
        return;
    }
    void EraseOne()
    {
        m_key.EraseOne();
        return;
    }
    bool First(Record& rf)
    {
        if (!m_key.First(rf))
        {
            return false;
        }
        return true;
    }
    bool Next(Record& rf)
    {
        if (!m_key.Next(rf))
        {
            return false;
        }
        return true;
    }
    
    void Clear()
    {
        m_key.Clear();
    }
    int Size()
    {
        return m_key.Size();
    }
};

template<typename Record>
class CIndexField
{
public:
	CRepeatFieldIndexBase<Record> m_index;  //索引数据，可重复

public:
	CIndexField()
	{
	}
	~CIndexField()
	{
	}
	//增加一条索引:rowid,索引...
	void Add(Record &rf)
	{
		m_index.Add(rf);
		return;
	}
	
	//查找指定索引的数据是否存在，m_end置为最后一个大于此值的位置，可以Next取后续相同的值
	bool Find(Record &rf)
	{
		return m_index.Find(rf);
	}
	//查找指定索引的数据是否存在，m_end为最后一值，NEXT取只到最后，可以按范围取值
	bool FindFirst(Record &rf)
	{
		return m_index.FindFirst(rf);
	}
    //扫描全表，删除rowid
    void Delete(CInt iset)
    {
		Record a_;
        bool bret_;

        bret_ = m_index.First(a_);
        while (bret_)
        {
            if (iset.Find(a_.m_nRowId))
            {
                m_index.EraseOne();
            }
            bret_ = m_index.Next(a_);
        }
    }
	//删除指定索引
	void Delete(Record &rf)
	{
		Record tmp;
		bool bret = Find(rf);
		while (bret)
		{
			m_index.EraseOne();
			bret = Next(tmp);
		}
		return;
	}
    void Delete(CInt iset, Record& rf)
    {
		Record tmp =rf;
        bool bret = Find(tmp);
        while (bret)
        {
            if (iset.Find(tmp.m_nRowId))
            {
                m_index.EraseOne();
            }
            bret = Next(tmp);
        }
        return;
    }

	void EraseOne()
	{
		m_index.EraseOne();
	}
	bool First(Record &rf)
	{
		if (!m_index.First(rf))
		{
			return false;
		}
		return true;
	}
	bool Next(Record &rf)
	{
		if (!m_index.Next(rf))
		{
			return false;
		}
		return true;
	}
	bool End(Record &rf)
	{
		if (!m_index.End(rf))
		{
			return false;
		}
		return true;
	}
	bool Last(Record &rf)
	{
//		CIndex_Field_Base rf;
		if (!m_index.Last(rf))
		{
			return false;
		}
		return true;
	}
	void Clear()
	{
		m_index.Clear();
	}
	int Size()
	{
		return m_index.Size();
	}

};


template<typename Record>
class CInstanceList
{
public:
	CInstanceList()
	{
		
	}
	virtual ~CInstanceList()
	{
		
	}
	int Size()
	{
		return m_table.RealSize();
	}
	Record * Find(int id)
	{
		CInt iset;
		if (!m_key.Select(iset,id))
		{
			return NULL;
		}
		int rid;
		iset.First(rid);
		return m_table.m_table[id];
	}
	bool Delete(int id)
	{
		CInt iset;
		if (!m_key.Select(iset,id))
		{
			return false;
		}
		m_key.Delete(id);
		m_table.Delete(id);
		return true;
	}
	int  Insert(Record *point)
	{
		int id = m_table.Add(point);
		CInt iset;
		if (!m_key.Select(iset,id))
		{
			m_key.Add(id,id);
		}
		else
		{
			m_key.Delete(id);
			m_key.Add(id,id);
		}
		return id;
	}
protected:
	
	CMemTableNew <Record *> m_table;
	CPkeyInt<1>                     m_key;   //rowid索引
};




class CMTblSample
{
public:
	CMTblSample();
	virtual ~CMTblSample();
	
	struct S_TBL_SAMPLE
	{
		unsigned short nNodeId;        
		char           cNodePrivateId; 
		int            data;
	};
public:
	
	void Test()
	{
		CMTblSample dbl;
		S_TBL_SAMPLE dd;
		int i;
		for (i=0;i<100;i++)
		{
			dd.nNodeId = i;
			dd.cNodePrivateId = i;
			dbl.Insert(dd);
		}
		for (i=0;i<100;i++)
		{
			dd.nNodeId = i;
			dd.cNodePrivateId = 0;
			dbl.Insert(dd);
		}
		std::vector<S_TBL_SAMPLE>dlist;
		dbl.SelectByNode(1,dlist);
		
		for (int j=0;j<(int)dlist.size();j++)
		{
			printf("NODEID=%d PRIVATEID=%d\n",dlist[j].nNodeId,dlist[j].cNodePrivateId);
		}
		dbl.SelectPrivateid(1,1,dd);
		printf("NODEID=%d PRIVATEID=%d\n",dd.nNodeId,dd.cNodePrivateId);
	}
	bool Update(S_TBL_SAMPLE &drtp)
	{
		CInt iset;
		int rid;
		if (!m_pkey.Select(rid,drtp.nNodeId,drtp.cNodePrivateId))
		{
			return false;
		}
		
		m_table.m_table[rid] = drtp;
		return true;
	}
	bool Delete(int nodeid,int privateid)
	{
		CInt iset;
		if (!m_pkey.Select(iset,nodeid,privateid))
		{
			return false;
		}
		int id;
		iset.First(id);
		m_pkey.Delete(nodeid,privateid);
		m_index_node.Delete(iset,nodeid);
		m_table.Delete(id);
		return true;
	}
	bool SelectPrivateid(int nodeid,int privateid,S_TBL_SAMPLE &drtp)
	{
		CInt iset;
		if (!m_pkey.Select(iset,nodeid,privateid))
		{
			return false;
		}
		int id;
		iset.First(id);
		drtp = m_table.m_table[id];
		return true;
	}
	//这个取值，按索引的排序来保存数据
	bool SelectByNode(int nodeid, std::vector<S_TBL_SAMPLE *> &drtp)
	{
		std::vector<int> iset;
		if (!m_index_node.Select(iset,nodeid))
		{
			return false;
		}
		for (unsigned int i=0; i<iset.size();i++)
		{
			drtp.push_back(&(m_table.m_table[iset[i]]));
		}
		return true;
	}
	//这个取值后按rowid排序，可能不是想要的
	bool SelectByNode(int nodeid, std::vector<S_TBL_SAMPLE> &drtp)
	{
		CInt iset;
		if (!m_index_node.Select(iset,nodeid))
		{
			return false;
		}
		int id;
		bool bRet;
		bRet = iset.First(id);
		while (bRet)
		{
			drtp.push_back(m_table.m_table[id]);
			bRet = iset.Next(id);
		}
		return true;
	}
	bool Insert(S_TBL_SAMPLE drtp)
	{
		int id;
		//通过主键查找，不存在则增加
		if (!m_pkey.Find(drtp.nNodeId,drtp.cNodePrivateId))
		{
			id = m_table.Add(drtp);//增加到表
			m_pkey.Add(id,drtp.nNodeId,drtp.cNodePrivateId);//增加主键
			m_index_node.Add(id,drtp.nNodeId);//增加索引
			return true;
		}
		return false;
	}
protected:
	
	CMemTableNew <S_TBL_SAMPLE> m_table;        //内存表
	std::list<int>                m_queryResult;  //查询结果rowid列表
	CIndexInt<1>             m_index_node;   //id索引
	CPkeyInt<2>              m_pkey;         //id+私有id索引，唯一索引
	
};


typedef struct 
{
	char sKey[41];
}S_CHAR_KEY;

class  CTbl_Char_Key
{
protected:
	
	CMemTableNew <S_CHAR_KEY> m_table;
	CPkeyCharF<40,1>          m_pkey; 
public:
	CTbl_Char_Key()
	{
		
	}
	virtual ~CTbl_Char_Key()
	{
		
	}
	void Clear()
	{
		m_pkey.Clear();
		m_table.Clear();
	}
	int Insert(char *key)
	{
		if (m_pkey.Find(key))
		{
			return -1;
		}
		S_CHAR_KEY tmp;
		strcpy(tmp.sKey,key);
		
		int rid=m_table.Add(tmp);
		m_pkey.Add(rid,m_table.m_table[rid].sKey);
		return rid;
	}
	char *First()
	{
		int rid;
		if (m_pkey.First(rid))
		{
			return m_table.m_table[rid].sKey;
		}
		return NULL;
	}
	char *Next()
	{
		int rid;
		if (m_pkey.Next(rid))
		{
			return m_table.m_table[rid].sKey;
		}
		return NULL;
	}
};


//CKeyFieldUnordered  CKeyField使用例子
class index_field_sample : public CIndex_Field_Base
{
public:
    UINT64_ account_id = 0;//资金账号
    UINT64_ portfolio_id = 0;//策略ID
    char    code[33] = { 0 };//代码
    virtual bool operator<(const index_field_sample& field) const
    {
        int ret = account_id - field.account_id;
        if (ret < 0)
        {
            return true;
        }
        else if (ret == 0)
        {
            int ret2 = portfolio_id - field.portfolio_id;
            if (ret2 < 0) 
			{
                return true;
            }
            else if (ret2 == 0)
            {
                ret = strcmp(code, field.code);
                if (ret < 0) 
				{
                    return true;
                }
            }
            return false;
        }
        return false;
    }
    //unordered使用
    virtual bool operator == (const index_field_sample& field) const
    {
        if (account_id != field.account_id)
        {
            return false;
        }
        else if (portfolio_id != field.portfolio_id)
        {
            return false;
        }
        else if (strcmp(code, field.code) != 0)
        {
            return false;
        }
        return true;
    }
};
//使用CKeyFieldUnordered CKeyField的表的例子
class CTbl_Table_Sample 
{
protected:
    struct _hash
    {
        std::size_t operator() (const index_field_sample& f1) const
        {
            return hash_val(f1.account_id, f1.portfolio_id, (std::string)f1.code);
        }
    };
	CKeyField<index_field_sample>   m_key1; //账户+策略+code 主键
    CKeyFieldUnordered<index_field_sample, _hash>   m_key2; //账户+策略+code 主键

	CIndexField<index_field_sample>   m_index1; //账户+策略+code索引 

public:
	CTbl_Table_Sample() {

    }
    virtual ~CTbl_Table_Sample() {

    }
   
};


//linux makefile 要加： -D_FILE_OFFSET_BITS = 64
// 编译期校验：T 必须是 trivially copyable 且 standard layout
  // CFileMapping.h

// ============================================================
// 映射模式
// ============================================================
enum class MapMode
{
    DirectWrite,   // 模式1：MAP_SHARED / PAGE_READWRITE，Flush 才确保落盘，无 LRU，适合生成文件
    CopyOnWrite    // 模式2：MAP_PRIVATE / PAGE_WRITECOPY，COW + LRU，退出不落盘
};

// ============================================================
// CFileMapping<T>：跨平台文件映射，支持模式1生成、模式2 COW 处理
// ============================================================
template <typename T>
class CFileMapping
{
public:
    // --------------------------------------------------------
    // 构造 / 析构 / 拷贝 / 移动
    // --------------------------------------------------------

    /// @brief 默认构造：初始化所有句柄为空，不打开任何文件。
    CFileMapping() { InitHandles(); }

    /// @brief 析构：调用 Close() 释放映射和句柄。
    ~CFileMapping() { Close(); }

    /// @brief 禁止拷贝构造（映射句柄不可复制）。
    CFileMapping(const CFileMapping&) = delete;

    /// @brief 禁止拷贝赋值（映射句柄不可复制）。
    CFileMapping& operator=(const CFileMapping&) = delete;

    /// @brief 移动构造：接管源对象的所有资源，源对象被置空。
    CFileMapping(CFileMapping&& o) noexcept
    {
        InitHandles();
        MoveFrom(o);
    }

    /// @brief 移动赋值：先 Close() 自身，再接管源对象资源。
    CFileMapping& operator=(CFileMapping&& o) noexcept
    {
        if (this != &o)
        {
            Close();
            InitHandles();
            MoveFrom(o);
        }
        return *this;
    }

    // --------------------------------------------------------
    // 打开接口
    // --------------------------------------------------------

    /// @brief 模式1：打开/创建文件并映射 byte_size 字节（MAP_SHARED）。
    /// @param path 文件路径
    /// @param byte_size 映射字节数；>0 时创建或调整文件到该大小，==0 时使用文件现有大小
    /// @return 成功 true，失败 false（错误信息见 Error()）
    bool OpenDirect(const std::string& path, size_t byte_size)
    {
        Close();
        m_eMode = MapMode::DirectWrite;
        m_sError.clear();
#ifdef _WIN32
        return OpenDirectWindows(path, byte_size);
#else
        return OpenDirectPosix(path, byte_size);
#endif
    }

    /// @brief 模式2：只读打开文件并映射为 COW（MAP_PRIVATE）。
    /// @param path 文件路径
    /// @return 成功 true，失败 false（错误信息见 Error()）
    bool OpenCow(const std::string& path)
    {
        Close();
        m_eMode = MapMode::CopyOnWrite;
        m_sError.clear();
#ifdef _WIN32
        return OpenCowWindows(path);
#else
        return OpenCowPosix(path);
#endif
    }

    // --------------------------------------------------------
    // 模式1 的两种收尾方式
    // --------------------------------------------------------

    /// @brief 模式1 生成完成后：Flush → 按 actual_count 截断 → 关闭。仅生成不处理时使用。
    /// @param actual_count 实际写入的元素个数，必须 <= Count()
    /// @return 成功 true，失败 false
    bool CloseDirect(size_t actual_count)
    {
        if (m_eMode != MapMode::DirectWrite)
        {
            m_sError = "CloseDirect: not in DirectWrite mode";
            return false;
}
        if (actual_count > m_nCount)
        {
            m_sError = "CloseDirect: actual_count exceeds mapped count";
            return false;
        }

        size_t actual_bytes = actual_count * sizeof(T);
        Flush();

#ifdef _WIN32
        if (m_pBase) { UnmapViewOfFile(m_pBase); m_pBase = nullptr; }
        if (m_hMap) { CloseHandle(m_hMap); m_hMap = NULL; }
        if (m_hFile != INVALID_HANDLE_VALUE)
        {
            LARGE_INTEGER li;
            li.QuadPart = static_cast<LONGLONG>(actual_bytes);
            SetFilePointerEx(m_hFile, li, NULL, FILE_BEGIN);
            SetEndOfFile(m_hFile);
        }
#else
        if (m_pBase) { munmap(m_pBase, m_nSize); m_pBase = nullptr; }
        if (m_nFd >= 0)
            ftruncate(m_nFd, static_cast<off_t>(actual_bytes));
#endif

        ResetHandles();
        m_nSize = 0;
        m_nCount = 0;
        ClearLruState();
        return true;
    }
    /// @brief 模式1：显式调整映射大小（扩容/收缩）。仅 DirectWrite 模式有效。
/// @param new_count 新的元素个数
/// @return 成功 true；失败 false（尽力回滚到原大小）
/// @note 收缩时超出 new_count 的数据被丢弃；扩容后新区域在 Linux 上读为零，
///       Windows 上不保证为零。每次调整都会 munmap + mmap，频繁调用开销较大。
/// @warning 不可与任何访问并发调用。
    bool Resize(size_t new_count)
    {
        if (m_eMode != MapMode::DirectWrite)
        {
            m_sError = "Resize: only valid in DirectWrite mode";
            return false;
        }

        size_t new_bytes = new_count * sizeof(T);
        if (new_bytes == m_nSize)
            return true;

        // 收缩前先 Flush，避免截断丢弃尚未写回的数据
        if (new_bytes < m_nSize)
            Flush();

        size_t old_size = m_nSize;

#ifdef _WIN32
        if (m_pBase) { UnmapViewOfFile(m_pBase); m_pBase = nullptr; }
        if (m_hMap) { CloseHandle(m_hMap); m_hMap = NULL; }

        LARGE_INTEGER li;
        li.QuadPart = static_cast<LONGLONG>(new_bytes);
        if (!SetFilePointerEx(m_hFile, li, NULL, FILE_BEGIN) ||
            !SetEndOfFile(m_hFile))
        {
            m_sError = "Resize: SetEndOfFile failed: " +
                std::to_string(GetLastError());
            RestoreFileSizeWindows(old_size);
            RemapDirectWindows(old_size);
            return false;
        }

        if (new_bytes == 0)
        {
            m_nSize = 0;
            m_nCount = 0;
            return true;
        }

        if (!RemapDirectWindows(new_bytes))
        {
            // 回滚：文件大小恢复，重新映射旧大小
            RestoreFileSizeWindows(old_size);
            RemapDirectWindows(old_size);
            m_sError = "Resize: remap failed";
            return false;
        }
#else
        if (m_pBase) { munmap(m_pBase, m_nSize); m_pBase = nullptr; }

        if (ftruncate(m_nFd, static_cast<off_t>(new_bytes)) != 0)
        {
            m_sError = std::string("Resize: ftruncate failed: ") +
                std::strerror(errno);
            RemapDirectPosix(old_size);
            return false;
        }

        if (new_bytes == 0)
        {
            m_nSize = 0;
            m_nCount = 0;
            return true;
        }

        void* p = mmap(nullptr, new_bytes, PROT_READ | PROT_WRITE,
            MAP_SHARED, m_nFd, 0);
        if (p == MAP_FAILED)
        {
            m_sError = std::string("Resize: mmap failed: ") +
                std::strerror(errno);
            // 回滚文件大小并重新映射
            ftruncate(m_nFd, static_cast<off_t>(old_size));
            RemapDirectPosix(old_size);
            return false;
        }
        m_pBase = p;
        m_nSize = new_bytes;
        m_nCount = new_count;
#endif
        return true;
    }
    /// @brief 模式1 生成完成后：Flush → 按 actual_count 截断 → 就地重新映射为 COW。
    ///        不重新打开文件，复用文件句柄。
    /// @param actual_count 实际写入的元素个数，必须 <= Count()
    /// @return 成功 true；成功后 Mode()==CopyOnWrite，Count()==actual_count，LRU/dirty 状态已清空
    bool SwitchToCow(size_t actual_count)
    {
        if (m_eMode != MapMode::DirectWrite)
        {
            m_sError = "SwitchToCow: not in DirectWrite mode";
            return false;
        }
        if (actual_count > m_nCount)
        {
            m_sError = "SwitchToCow: actual_count exceeds mapped count";
            return false;
        }

        size_t actual_bytes = actual_count * sizeof(T);

        // 1) 先 Flush，确保数据落盘后再截断，避免丢数据
        Flush();

        // 2) 释放旧映射，保留文件句柄
#ifdef _WIN32
        if (m_pBase) { UnmapViewOfFile(m_pBase); m_pBase = nullptr; }
        if (m_hMap) { CloseHandle(m_hMap); m_hMap = NULL; }

        // 3) 截断到实际大小
        LARGE_INTEGER li;
        li.QuadPart = static_cast<LONGLONG>(actual_bytes);
        if (!SetFilePointerEx(m_hFile, li, NULL, FILE_BEGIN) ||
            !SetEndOfFile(m_hFile))
        {
            m_sError = "SetEndOfFile failed: " + std::to_string(GetLastError());
            ResetHandles();
            m_nSize = 0; m_nCount = 0;
            return false;
        }

        if (actual_bytes == 0)
        {
            m_eMode = MapMode::CopyOnWrite;
            m_nSize = 0; m_nCount = 0;
            ClearLruState();
            return true;
        }

        // 4) 重新映射为 COW
        m_hMap = CreateFileMappingA(
            m_hFile, NULL, PAGE_WRITECOPY,
            static_cast<DWORD>(static_cast<uint64_t>(actual_bytes) >> 32),
            static_cast<DWORD>(static_cast<uint64_t>(actual_bytes) & 0xFFFFFFFFULL),
            NULL);
        if (!m_hMap)
        {
            m_sError = "CreateFileMapping(PAGE_WRITECOPY) failed: " +
                std::to_string(GetLastError());
            ResetHandles();
            m_nSize = 0; m_nCount = 0;
            return false;
        }
        void* p = MapViewOfFile(m_hMap, FILE_MAP_COPY, 0, 0, 0);
        if (!p)
        {
            m_sError = "MapViewOfFile(FILE_MAP_COPY) failed: " +
                std::to_string(GetLastError());
            ResetHandles();
            m_nSize = 0; m_nCount = 0;
            return false;
        }
        m_pBase = p;
#else
        if (m_pBase) { munmap(m_pBase, m_nSize); m_pBase = nullptr; }

        if (ftruncate(m_nFd, static_cast<off_t>(actual_bytes)) != 0)
        {
            m_sError = std::string("ftruncate failed: ") + std::strerror(errno);
            ResetHandles();
            m_nSize = 0; m_nCount = 0;
            return false;
        }

        if (actual_bytes == 0)
        {
            m_eMode = MapMode::CopyOnWrite;
            m_nSize = 0; m_nCount = 0;
            ClearLruState();
            return true;
        }

        void* p = mmap(nullptr, actual_bytes, PROT_READ | PROT_WRITE,
            MAP_PRIVATE, m_nFd, 0);
        if (p == MAP_FAILED)
        {
            m_sError = std::string("mmap(MAP_PRIVATE) failed: ") +
                std::strerror(errno);
            ResetHandles();
            m_nSize = 0; m_nCount = 0;
            return false;
        }
        m_pBase = p;
#endif

        m_eMode = MapMode::CopyOnWrite;
        m_nSize = actual_bytes;
        m_nCount = actual_count;
        ClearLruState();
        return true;
        }

    /// @brief 关闭映射并释放所有句柄和 LRU 状态。不主动 Flush（COW 修改丢弃；DirectWrite 依赖 OS 异步写回）。
    void Close()
    {
        if (m_pBase)
        {
#ifdef _WIN32
            UnmapViewOfFile(m_pBase);
#else
            munmap(m_pBase, m_nSize);
#endif
        }
        m_pBase = nullptr;
        m_nSize = 0;
        m_nCount = 0;
        ResetHandles();
        ClearLruState();
    }

    // --------------------------------------------------------
    // 基本访问
    // --------------------------------------------------------

    /// @brief 是否已成功映射（空文件返回 false）。
    bool IsOpen() const { return m_pBase != nullptr; }

    /// @brief 映射的字节数。
    size_t ByteSize() const { return m_nSize; }

    /// @brief 映射的元素个数（ByteSize() / sizeof(T)）。
    size_t Count() const { return m_nCount; }

    /// @brief 最近一次错误信息。
    const std::string& Error() const { return m_sError; }

    /// @brief 当前映射模式。
    MapMode Mode() const { return m_eMode; }

    /// @brief 非 const 数据指针。COW 下调用会禁用 LRU 淘汰（无法跟踪写），所有页常驻。
    T* Data()
    {
        if (m_eMode == MapMode::CopyOnWrite)
            MarkAllDirty();
        return static_cast<T*>(m_pBase);
    }

    /// @brief const 数据指针。不改变 LRU 状态。
    const T* Data() const
    {
        return static_cast<const T*>(m_pBase);
    }

    /// @brief 非 const 按索引访问。COW 下自动 MarkDirty 该页，返回可写指针。
    /// @return 越界返回 nullptr
    T* At(size_t idx)
    {
        if (idx >= m_nCount) return nullptr;
        TouchPage(idx, true);
        return &static_cast<T*>(m_pBase)[idx];
    }

    /// @brief const 按索引访问。走正常 LRU，不标记 dirty。
    /// @return 越界返回 nullptr
    const T* At(size_t idx) const
    {
        if (idx >= m_nCount) return nullptr;
        TouchPage(idx, false);
        return &static_cast<const T*>(m_pBase)[idx];
    }

    /// @brief 非 const 下标访问。COW 下自动 MarkDirty 该页。
    /// @warning 不检查越界，调用方保证 idx < Count()
    T& operator[](size_t idx)
    {
        TouchPage(idx, true);
        return static_cast<T*>(m_pBase)[idx];
    }

    /// @brief const 下标访问。走正常 LRU，不标记 dirty。
    /// @warning 不检查越界，调用方保证 idx < Count()
    const T& operator[](size_t idx) const
    {
        TouchPage(idx, false);
        return static_cast<const T*>(m_pBase)[idx];
    }

    // --------------------------------------------------------
    // dirty 标记（COW 模式下）
    // --------------------------------------------------------

    /// @brief 显式声明 idx 所在页会被修改。该页移出 LRU、常驻内存，修改永不丢弃。
    /// @note 必须在写之前调用；非 const 的 At()/operator[] 会自动调用。
    void MarkDirty(size_t idx)
    {
        if (m_eMode != MapMode::CopyOnWrite || !m_pBase || idx >= m_nCount)
            return;
        size_t page_idx = (idx * sizeof(T)) / m_nPageSize;
        std::lock_guard<std::mutex> lk(m_mtxLru);
        MarkDirtyLocked(page_idx);
    }

    /// @brief 批量标记 [begin, end) 覆盖的所有页为 dirty。
    void MarkDirtyRange(size_t begin, size_t end)
    {
        if (m_eMode != MapMode::CopyOnWrite || !m_pBase) return;
        if (begin >= m_nCount) return;
        if (end > m_nCount) end = m_nCount;
        if (begin >= end) return;

        size_t p0 = (begin * sizeof(T)) / m_nPageSize;
        size_t p1 = ((end - 1) * sizeof(T)) / m_nPageSize;

        std::lock_guard<std::mutex> lk(m_mtxLru);
        for (size_t p = p0; p <= p1; ++p)
            MarkDirtyLocked(p);
    }

    // --------------------------------------------------------
    // reload：确保 idx 所在页当前在内存中
    // --------------------------------------------------------

    /// @brief 确保 idx 所在页当前在内存。
    ///        dirty 页：修改保留，no-op。
    ///        clean 页在 LRU：移到 LRU 头部。
    ///        clean 页不在 LRU：加入 LRU，淘汰最旧页，从文件预读加载。
    /// @return 越界或模式不对返回 false
    bool Reload(size_t idx)
    {
        if (!m_pBase || idx >= m_nCount || m_eMode != MapMode::CopyOnWrite)
            return false;

        size_t page_idx = (idx * sizeof(T)) / m_nPageSize;

        bool need_load = false;
        {
            std::lock_guard<std::mutex> lk(m_mtxLru);

            if (m_bAllDirty) return true;                     // 淘汰已禁用
            if (m_setDirty.count(page_idx)) return true;      // dirty 页常驻

            auto it = m_mapLru.find(page_idx);
            if (it != m_mapLru.end())
            {
                m_listLru.splice(m_listLru.begin(), m_listLru, it->second);
                return true;
            }

            if (m_bLruEnabled.load(std::memory_order_relaxed))
            {
                m_listLru.push_front(page_idx);
                m_mapLru[page_idx] = m_listLru.begin();
                while (m_mapLru.size() > m_nLruCapacity)
                    EvictOldestLocked();
            }
            need_load = true;
        }

        if (need_load) PreloadPage(page_idx);
        return true;
        }

    /// @brief 批量 Reload [begin, end)。
    void ReloadRange(size_t begin, size_t end)
    {
        if (!m_pBase || m_eMode != MapMode::CopyOnWrite) return;
        if (begin >= m_nCount) return;
        if (end > m_nCount) end = m_nCount;
        if (begin >= end) return;

        size_t per_page = m_nPageSize / sizeof(T);
        if (per_page == 0) per_page = 1;

        size_t p0 = (begin * sizeof(T)) / m_nPageSize;
        size_t p1 = ((end - 1) * sizeof(T)) / m_nPageSize;
        for (size_t p = p0; p <= p1; ++p)
            Reload(p * per_page);
    }

    // --------------------------------------------------------
    // 刷盘（仅 DirectWrite 有效）
    // --------------------------------------------------------

    /// @brief 将 DirectWrite 模式的修改同步到磁盘。COW 模式下为 no-op。
    void Flush()
    {
        if (!m_pBase || m_eMode != MapMode::DirectWrite) return;
#ifdef _WIN32
        FlushViewOfFile(m_pBase, m_nSize);
        if (m_hFile != INVALID_HANDLE_VALUE)
            FlushFileBuffers(m_hFile);
#else
        msync(m_pBase, m_nSize, MS_SYNC);
#endif
    }

    // --------------------------------------------------------
    // 预取 / 缓存建议
    // --------------------------------------------------------

    /// @brief 预取 idx 所在页到内存（只读提示，走 clean 路径）。
    void Prefetch(size_t idx) const
    {
        if (idx >= m_nCount || !m_pBase) return;
        TouchPage(idx, false);
        const void* addr = static_cast<const char*>(m_pBase) + idx * sizeof(T);
#ifdef _WIN32
        WIN32_MEMORY_RANGE_ENTRY range;
        range.VirtualAddress = const_cast<void*>(addr);
        range.NumberOfBytes = sizeof(T);
        PrefetchVirtualMemory(GetCurrentProcess(), 1, &range, 0);
#else
        off_t off = static_cast<off_t>(idx) * static_cast<off_t>(sizeof(T));
        posix_fadvise(m_nFd, off, static_cast<off_t>(sizeof(T)), POSIX_FADV_WILLNEED);
#endif
    }

    /// @brief 建议内核按随机访问模式处理（Linux only）。
    void AdviseRandom()
    {
#ifdef __linux__
        if (m_pBase) madvise(m_pBase, m_nSize, MADV_RANDOM);
#endif
    }

    /// @brief 建议内核使用大页（Linux only）。
    void AdviseHugePage()
    {
#ifdef __linux__
        if (m_pBase) madvise(m_pBase, m_nSize, MADV_HUGEPAGE);
#endif
    }

    // --------------------------------------------------------
    // LRU 接口
    // --------------------------------------------------------

    /// @brief 设置 LRU 容量（只限制 clean 页；dirty 页常驻不受限制）。0 表示禁用 LRU。
    void SetLruCapacity(size_t max_pages)
    {
        std::lock_guard<std::mutex> lk(m_mtxLru);
        m_nLruCapacity = max_pages;
        m_bLruEnabled.store(max_pages > 0, std::memory_order_relaxed);
        if (max_pages == 0)
        {
            m_listLru.clear();
            m_mapLru.clear();
    }
        else
        {
            while (m_mapLru.size() > m_nLruCapacity)
                EvictOldestLocked();
        }
    }

    /// @brief 返回当前 LRU 容量。
    size_t LruCapacity() const
    {
        std::lock_guard<std::mutex> lk(m_mtxLru);
        return m_nLruCapacity;
    }

    /// @brief 返回当前 LRU 中缓存的 clean 页数量。
    size_t LruCachedPages() const
    {
        std::lock_guard<std::mutex> lk(m_mtxLru);
        return m_mapLru.size();
    }

    /// @brief 返回当前被标记 dirty 的页数量。
    size_t LruDirtyPages() const
    {
        std::lock_guard<std::mutex> lk(m_mtxLru);
        return m_setDirty.size();
    }

    /// @brief 是否因调用 Data() 而禁用了 LRU 淘汰。
    bool LruEvictionDisabled() const
    {
        std::lock_guard<std::mutex> lk(m_mtxLru);
        return m_bAllDirty;
    }

private:
    // --------------------------------------------------------
    // 内部工具
    // --------------------------------------------------------

    /// @brief 初始化句柄和所有成员为默认值。
    void InitHandles()
    {
#ifdef _WIN32
        m_hFile = INVALID_HANDLE_VALUE;
        m_hMap = NULL;
#else
        m_nFd = -1;
#endif
        m_pBase = nullptr;
        m_nSize = 0;
        m_nCount = 0;
        m_eMode = MapMode::DirectWrite;
        m_nPageSize = QueryPageSize();
        m_bAllDirty = false;
        m_nLruCapacity = 0;
        m_bLruEnabled.store(false, std::memory_order_relaxed);
    }

    /// @brief 查询系统页大小（字节）。
    static size_t QueryPageSize()
    {
#ifdef _WIN32
        SYSTEM_INFO si; GetSystemInfo(&si);
        return static_cast<size_t>(si.dwPageSize);
#else
        long ps = sysconf(_SC_PAGESIZE);
        return ps > 0 ? static_cast<size_t>(ps) : 4096;
#endif
    }

    /// @brief 关闭并重置文件/映射句柄。
    void ResetHandles()
    {
#ifdef _WIN32
        if (m_hMap) { CloseHandle(m_hMap); m_hMap = NULL; }
        if (m_hFile != INVALID_HANDLE_VALUE)
        {
            CloseHandle(m_hFile); m_hFile = INVALID_HANDLE_VALUE;
        }
#else
        if (m_nFd >= 0) { ::close(m_nFd); m_nFd = -1; }
#endif
    }

    /// @brief 清空 LRU / dirty 相关状态。
    void ClearLruState()
    {
        std::lock_guard<std::mutex> lk(m_mtxLru);
        m_listLru.clear();
        m_mapLru.clear();
        m_setDirty.clear();
        m_bAllDirty = false;
    }

    /// @brief 从源对象接管所有资源，源对象被置空。移动/赋值用。
    void MoveFrom(CFileMapping& o)
    {
        m_pBase = o.m_pBase;
        m_nSize = o.m_nSize;
        m_nCount = o.m_nCount;
        m_eMode = o.m_eMode;
        m_nPageSize = o.m_nPageSize;
        m_sError.swap(o.m_sError);
#ifdef _WIN32
        m_hFile = o.m_hFile; m_hMap = o.m_hMap;
        o.m_hFile = INVALID_HANDLE_VALUE; o.m_hMap = NULL;
#else
        m_nFd = o.m_nFd; o.m_nFd = -1;
#endif
        // 移动/赋值不应与访问并发，这里不加锁
        m_nLruCapacity = o.m_nLruCapacity;
        m_listLru = std::move(o.m_listLru);
        m_mapLru = std::move(o.m_mapLru);
        m_setDirty = std::move(o.m_setDirty);
        m_bAllDirty = o.m_bAllDirty;
        m_bLruEnabled.store(o.m_bLruEnabled.load(std::memory_order_relaxed),
            std::memory_order_relaxed);

        o.m_nLruCapacity = 0;
        o.m_bAllDirty = false;
        o.m_bLruEnabled.store(false, std::memory_order_relaxed);
        o.m_pBase = nullptr;
        o.m_nSize = 0;
        o.m_nCount = 0;
        o.m_eMode = MapMode::DirectWrite;
    }

    /// @brief 在持锁状态下把 page_idx 标记为 dirty（移出 LRU、加入 dirty 集合）。
    /// @note 调用方必须持有 m_mtxLru。
    void MarkDirtyLocked(size_t page_idx) const
    {
        if (m_bAllDirty) return;
        auto it = m_mapLru.find(page_idx);
        if (it != m_mapLru.end())
        {
            m_listLru.erase(it->second);
            m_mapLru.erase(it);
        }
        m_setDirty.insert(page_idx);
    }

    /// @brief 访问 idx 所在页：is_write=true 时自动 MarkDirty；false 时走 LRU 刷新/加入。
    void TouchPage(size_t idx, bool is_write) const
    {
        if (m_eMode != MapMode::CopyOnWrite) return;
        if (!m_pBase || idx >= m_nCount) return;

        size_t page_idx = (idx * sizeof(T)) / m_nPageSize;

        if (is_write)
        {
            std::lock_guard<std::mutex> lk(m_mtxLru);
            MarkDirtyLocked(page_idx);
            return;
        }

        if (!m_bLruEnabled.load(std::memory_order_relaxed)) return;

        std::lock_guard<std::mutex> lk(m_mtxLru);
        if (m_bAllDirty) return;
        if (m_setDirty.count(page_idx)) return;

        auto it = m_mapLru.find(page_idx);
        if (it != m_mapLru.end())
        {
            m_listLru.splice(m_listLru.begin(), m_listLru, it->second);
            return;
        }
        m_listLru.push_front(page_idx);
        m_mapLru[page_idx] = m_listLru.begin();
        while (m_mapLru.size() > m_nLruCapacity)
            EvictOldestLocked();
    }

    /// @brief 淘汰 LRU 尾部（最旧）的 clean 页。Linux 用 MADV_DONTNEED 释放；Windows 仅统计移除。
    /// @note 调用方必须持有 m_mtxLru。
    void EvictOldestLocked() const
    {
        if (m_bAllDirty) return;
        if (m_listLru.empty()) return;
        size_t page_idx = m_listLru.back();
        m_listLru.pop_back();
        m_mapLru.erase(page_idx);

        // dirty 页从不进入 LRU 列表，这里淘汰的一定是 clean 页
#ifdef __linux__
        if (!m_pBase) return;
        size_t off = page_idx * m_nPageSize;
        if (off >= m_nSize) return;
        size_t len = std::min(m_nPageSize, m_nSize - off);
        madvise(static_cast<char*>(m_pBase) + off, len, MADV_DONTNEED);
#endif
        // Windows：无等价 API，clean 文件页由 OS 自然回收
    }

    /// @brief 禁用 LRU 淘汰（COW 下调用非 const Data() 后无法跟踪写，安全优先）。
    void MarkAllDirty() const
    {
        std::lock_guard<std::mutex> lk(m_mtxLru);
        m_bAllDirty = true;
        m_listLru.clear();
        m_mapLru.clear();
    }

    /// @brief 锁外调用：触发一页从文件加载到内存。
    void PreloadPage(size_t page_idx) const
    {
        if (!m_pBase) return;
        size_t off = page_idx * m_nPageSize;
        if (off >= m_nSize) return;
        size_t len = MIN(m_nPageSize, m_nSize - off);

#ifdef _WIN32
        volatile char c = static_cast<char*>(m_pBase)[off];
        (void)c;
#else
        madvise(static_cast<char*>(m_pBase) + off, len, MADV_WILLNEED);
#endif
    }

    // --------------------------------------------------------
    // 平台相关：模式1
    // --------------------------------------------------------
#ifdef _WIN32
    /// @brief Windows 模式1：创建/打开文件到 byte_size，映射为 PAGE_READWRITE。
    bool OpenDirectWindows(const std::string& path, size_t byte_size)
    {
        m_hFile = CreateFileA(path.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ, NULL,
            OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (m_hFile == INVALID_HANDLE_VALUE)
        {
            m_sError = "CreateFile failed: " + std::to_string(GetLastError());
            return false;
        }

        LARGE_INTEGER sz;
        if (!GetFileSizeEx(m_hFile, &sz))
        {
            m_sError = "GetFileSizeEx failed";
            ResetHandles();
            return false;
        }
        size_t actual = static_cast<size_t>(sz.QuadPart);
        size_t target = (byte_size > 0) ? byte_size : actual;
        if (target != actual)
        {
            LARGE_INTEGER ns; ns.QuadPart = static_cast<LONGLONG>(target);
            if (!SetFilePointerEx(m_hFile, ns, NULL, FILE_BEGIN) ||
                !SetEndOfFile(m_hFile))
            {
                m_sError = "SetEndOfFile failed: " + std::to_string(GetLastError());
                ResetHandles();
                return false;
            }
        }
        if (target == 0)
        {
            m_nSize = 0; m_nCount = 0; m_pBase = NULL;
            return true;
        }
        if (target % sizeof(T) != 0)
            m_sError = "warning: file size not multiple of record size";

        m_hMap = CreateFileMappingA(m_hFile, NULL, PAGE_READWRITE,
            static_cast<DWORD>(static_cast<uint64_t>(target) >> 32),
            static_cast<DWORD>(static_cast<uint64_t>(target) & 0xFFFFFFFFULL),
            NULL);
        if (!m_hMap)
        {
            m_sError = "CreateFileMapping failed: " + std::to_string(GetLastError());
            ResetHandles();
            return false;
        }
        void* p = MapViewOfFile(m_hMap, FILE_MAP_WRITE, 0, 0, 0);
        if (!p)
        {
            m_sError = "MapViewOfFile failed: " + std::to_string(GetLastError());
            ResetHandles();
            return false;
        }
        m_pBase = p;
        m_nSize = target;
        m_nCount = target / sizeof(T);
        return true;
    }

    /// @brief Windows 模式2：只读打开文件，映射为 PAGE_WRITECOPY（COW）。
    bool OpenCowWindows(const std::string& path)
    {
        m_hFile = CreateFileA(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL,
            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (m_hFile == INVALID_HANDLE_VALUE)
        {
            m_sError = "CreateFile failed: " + std::to_string(GetLastError());
            return false;
        }
        LARGE_INTEGER sz;
        if (!GetFileSizeEx(m_hFile, &sz))
        {
            m_sError = "GetFileSizeEx failed";
            ResetHandles();
            return false;
        }
        if (sz.QuadPart == 0)
        {
            m_nSize = 0; m_nCount = 0; m_pBase = NULL;
            return true;
        }
        if (sz.QuadPart % static_cast<LONGLONG>(sizeof(T)) != 0)
            m_sError = "warning: file size not multiple of record size";

        m_hMap = CreateFileMappingA(m_hFile, NULL, PAGE_WRITECOPY,
            static_cast<DWORD>(sz.QuadPart >> 32),
            static_cast<DWORD>(sz.QuadPart & 0xFFFFFFFFULL),
            NULL);
        if (!m_hMap)
        {
            m_sError = "CreateFileMapping(PAGE_WRITECOPY) failed: " +
                std::to_string(GetLastError());
            ResetHandles();
            return false;
        }
        void* p = MapViewOfFile(m_hMap, FILE_MAP_COPY, 0, 0, 0);
        if (!p)
        {
            m_sError = "MapViewOfFile(FILE_MAP_COPY) failed: " +
                std::to_string(GetLastError());
            ResetHandles();
            return false;
        }
        m_pBase = p;
        m_nSize = static_cast<size_t>(sz.QuadPart);
        m_nCount = m_nSize / sizeof(T);
        return true;
    }

    HANDLE m_hFile;   ///< 文件句柄（Windows）
    HANDLE m_hMap;    ///< 文件映射句柄（Windows）
#else
    /// @brief POSIX 模式1：open(O_RDWR|O_CREAT) + ftruncate 到 byte_size + mmap(MAP_SHARED)。
    bool OpenDirectPosix(const std::string& path, size_t byte_size)
    {
        m_nFd = ::open(path.c_str(), O_RDWR | O_CREAT, 0644);
        if (m_nFd < 0)
        {
            m_sError = std::string("open failed: ") + std::strerror(errno);
            return false;
        }
        struct stat st;
        if (fstat(m_nFd, &st) != 0)
        {
            m_sError = std::string("fstat failed: ") + std::strerror(errno);
            ResetHandles();
            return false;
        }
        size_t actual = static_cast<size_t>(st.st_size);
        size_t target = (byte_size > 0) ? byte_size : actual;
        if (target != actual)
        {
            if (ftruncate(m_nFd, static_cast<off_t>(target)) != 0)
            {
                m_sError = std::string("ftruncate failed: ") + std::strerror(errno);
                ResetHandles();
                return false;
            }
        }
        if (target == 0)
        {
            m_nSize = 0; m_nCount = 0; m_pBase = nullptr;
            return true;
        }
        if (target % sizeof(T) != 0)
            m_sError = "warning: file size not multiple of record size";

        void* p = mmap(nullptr, target, PROT_READ | PROT_WRITE,
            MAP_SHARED, m_nFd, 0);
        if (p == MAP_FAILED)
        {
            m_sError = std::string("mmap failed: ") + std::strerror(errno);
            ResetHandles();
            return false;
        }
        m_pBase = p;
        m_nSize = target;
        m_nCount = target / sizeof(T);
        return true;
    }

    /// @brief POSIX 模式2：open(O_RDONLY) + mmap(MAP_PRIVATE)，写触发 COW 不落盘。
    bool OpenCowPosix(const std::string& path)
    {
        m_nFd = ::open(path.c_str(), O_RDONLY);
        if (m_nFd < 0)
        {
            m_sError = std::string("open failed: ") + std::strerror(errno);
            return false;
        }
        struct stat st;
        if (fstat(m_nFd, &st) != 0)
        {
            m_sError = std::string("fstat failed: ") + std::strerror(errno);
            ResetHandles();
            return false;
        }
        if (st.st_size == 0)
        {
            m_nSize = 0; m_nCount = 0; m_pBase = nullptr;
            return true;
        }
        if (st.st_size % static_cast<off_t>(sizeof(T)) != 0)
            m_sError = "warning: file size not multiple of record size";

        void* p = mmap(nullptr, static_cast<size_t>(st.st_size),
            PROT_READ | PROT_WRITE,
            MAP_PRIVATE, m_nFd, 0);
        if (p == MAP_FAILED)
        {
            m_sError = std::string("mmap failed: ") + std::strerror(errno);
            ResetHandles();
            return false;
        }
        m_pBase = p;
        m_nSize = static_cast<size_t>(st.st_size);
        m_nCount = m_nSize / sizeof(T);
        return true;
    }
#ifdef _WIN32
    /// @brief 把文件大小恢复到指定字节数（Resize 失败回滚用）。
    void RestoreFileSizeWindows(size_t bytes)
    {
        LARGE_INTEGER li;
        li.QuadPart = static_cast<LONGLONG>(bytes);
        SetFilePointerEx(m_hFile, li, NULL, FILE_BEGIN);
        SetEndOfFile(m_hFile);
    }

    /// @brief 用现有文件句柄重新建立 DirectWrite 映射（不 open）。
    /// @param bytes 映射字节数
    bool RemapDirectWindows(size_t bytes)
    {
        if (bytes == 0)
        {
            m_pBase = NULL;
            m_nSize = 0;
            m_nCount = 0;
            return true;
        }
        m_hMap = CreateFileMappingA(
            m_hFile, NULL, PAGE_READWRITE,
            static_cast<DWORD>(static_cast<uint64_t>(bytes) >> 32),
            static_cast<DWORD>(static_cast<uint64_t>(bytes) & 0xFFFFFFFFULL),
            NULL);
        if (!m_hMap) return false;

        void* p = MapViewOfFile(m_hMap, FILE_MAP_WRITE, 0, 0, 0);
        if (!p)
        {
            CloseHandle(m_hMap);
            m_hMap = NULL;
            return false;
        }
        m_pBase = p;
        m_nSize = bytes;
        m_nCount = bytes / sizeof(T);
        return true;
    }
#else
    /// @brief 用现有 fd 重新建立 DirectWrite 映射（不 open、不改文件大小）。
    bool RemapDirectPosix(size_t bytes)
    {
        if (bytes == 0)
        {
            m_pBase = nullptr;
            m_nSize = 0;
            m_nCount = 0;
            return true;
        }
        void* p = mmap(nullptr, bytes, PROT_READ | PROT_WRITE,
            MAP_SHARED, m_nFd, 0);
        if (p == MAP_FAILED) return false;
        m_pBase = p;
        m_nSize = bytes;
        m_nCount = bytes / sizeof(T);
        return true;
    }
#endif

    int m_nFd;   ///< 文件描述符（POSIX）
#endif

    // --------------------------------------------------------
    // 通用成员变量
    // --------------------------------------------------------

    void* m_pBase;      ///< 映射基地址（nullptr 表示未映射）
    size_t      m_nSize;      ///< 映射字节数
    size_t      m_nCount;     ///< 映射元素个数（m_nSize / sizeof(T)）
    MapMode     m_eMode;      ///< 当前映射模式
    std::string m_sError;     ///< 最近一次错误信息

    // --------------------------------------------------------
    // LRU / dirty 状态（由 m_mtxLru 保护）
    // --------------------------------------------------------

    mutable std::mutex m_mtxLru;   ///< 保护以下所有 LRU/dirty 状态
    mutable std::list<size_t> m_listLru;   ///< LRU 双向链表，头部最新、尾部最旧（存 page_idx）
    mutable std::unordered_map<size_t, std::list<size_t>::iterator> m_mapLru;  ///< page_idx → m_listLru 迭代器
    mutable std::unordered_set<size_t> m_setDirty;  ///< 被标记 dirty 的 page_idx 集合（常驻、不淘汰）
    mutable bool m_bAllDirty;      ///< true 表示禁用淘汰（COW 下调用 Data() 后置位）
    size_t m_nLruCapacity;         ///< LRU 容量（clean 页数上限；0 表示禁用 LRU）
    std::atomic<bool> m_bLruEnabled;  ///< LRU 是否启用（m_nLruCapacity > 0）
    size_t m_nPageSize;            ///< 系统页大小（字节）
};

#ifdef BUBASE_TEST

        struct Record { int id; double value; };

        void generate_only()
        {
            const size_t MAX_N = 10'000'000;
            size_t actual = 0;

            CFileMapping<Record> m;
            if (!m.open_direct("data.bin", MAX_N * sizeof(Record))) {
                std::fprintf(stderr, "open_direct failed: %s\n", m.error().c_str());
                return;
            }

            for (size_t i = 0; i < MAX_N; ++i) {
                if (i >= 3'000'000) break;
                m[i] = Record{ (int)i, i * 1.0 };
                ++actual;
            }

            // flush 落盘 + 截断到 actual 条 + 关闭
            if (!m.close_direct(actual)) {
                std::fprintf(stderr, "close_direct failed: %s\n", m.error().c_str());
            }
        }
        void generate_then_cow_process()
        {
            const size_t MAX_N = 10'000'000;
            size_t actual = 0;

            CFileMapping<Record> m;
            if (!m.open_direct("data.bin", MAX_N * sizeof(Record))) {
                std::fprintf(stderr, "open_direct failed: %s\n", m.error().c_str());
                return;
            }

            // ===== 阶段1：生成 =====
            for (size_t i = 0; i < MAX_N; ++i) {
                if (i >= 3'000'000) break;
                m[i] = Record{ (int)i, i * 1.0 };
                ++actual;
            }

            // ===== 就地转 COW：flush → 截断到 3M 条 → 重新映射 =====
            if (!m.switch_to_cow(actual)) {
                std::fprintf(stderr, "switch_to_cow failed: %s\n", m.error().c_str());
                return;
            }
            // 此时 m.mode() == CopyOnWrite，m.count() == 3000000

            // ===== 阶段2：COW 处理 =====
            m.set_lru_capacity(1024);   // 最多 1024 个 clean 页常驻

            // 第一轮：顺序读 + 条件写
            for (size_t i = 0; i < m.count(); ++i) {
                const CFileMapping<Record>& cm = m;
                if (cm[i].value > 1'500'000.0) {
                    m.mark_dirty(i);
                    m[i].value *= 2.0;
                }
            }

            // 第二轮：应用逻辑回看某些数据
            // reload 确保在内存：dirty 页保留修改，clean 页被淘汰则从文件加载
            for (size_t i = 0; i < m.count(); i += 1000) {
                m.reload(i);
                const CFileMapping<Record>& cm = m;
                if (cm[i].value > 3'000'000.0) {
                    // 之前改过，dirty 页常驻，读到修改后的值
                }
            }

            std::printf("count=%zu clean=%zu dirty=%zu\n",
                m.count(), m.lru_cached_pages(), m.lru_dirty_pages());

            // 退出：close 不 msync，修改丢弃
        }

        int main()
        {
            generate_then_cow_process();
            return 0;
        }
        void process_existing_file()
        {
            CFileMapping<Record> m;
            m.set_lru_capacity(512);
            if (!m.open_cow("data.bin")) {
                std::fprintf(stderr, "open_cow failed: %s\n", m.error().c_str());
                return;
            }

            for (size_t i = 0; i < m.count(); ++i) {
                const CFileMapping<Record>& cm = m;
                if (cm[i].id % 100 == 0) {
                    m.mark_dirty(i);
                    m[i].value = -1.0;
                }
            }
            // 所有修改退出时丢弃，data.bin 不变
        }

        void GenerateWithResize()
        {
            CFileMapping<Record> m;
            // 初始只映射 1024 条，远小于最终规模
            if (!m.OpenDirect("data.bin", 1024 * sizeof(Record))) {
                std::fprintf(stderr, "%s\n", m.Error().c_str());
                return;
            }

            size_t actual = 0;
            const size_t FINAL_N = 3'000'000;

            for (size_t i = 0; i < FINAL_N; ++i)
            {
                // 写之前确保容量够
                if (i >= m.Count())
                {
                    size_t new_cap = m.Count() * 2;
                    if (new_cap <= i) new_cap = i + 1;
                    if (!m.Resize(new_cap)) {
                        std::fprintf(stderr, "Resize failed: %s\n", m.Error().c_str());
                        return;
                    }
                }
                m[i] = Record{ (int)i, i * 1.0 };
                ++actual;
            }

            // 收尾：Flush + 截断到实际大小 + 关闭
            m.CloseDirect(actual);
        }
        //三、详细说明
        //    1. 两种模式的数据流
        //    阶段	调用	映射方式	写入落盘？	LRU
        //    模式1 生成	open_direct(path, max_bytes)	MAP_SHARED / PAGE_READWRITE	不主动；flush() 才确保	无
        //    模式1 收尾（只生成）	close_direct(actual_count)	—	flush() 后截断关闭	—
        //    模式1 收尾（转处理）	switch_to_cow(actual_count)	变成 MAP_PRIVATE / PAGE_WRITECOPY	之后修改不落盘	可启用
        //    模式2 处理	open_cow(path) 或上一步 switch_to_cow	COW	不落盘	支持
        //    2. switch_to_cow 的步骤
        //    flush()：先确保生成的数据落盘，否则截断可能丢未写回数据。

        //    释放旧映射：munmap / UnmapViewOfFile + CloseHandle(hMap)，保留文件句柄。

        //    截断：ftruncate(fd, actual_bytes) / SetEndOfFile(actual_bytes)。

        //    重新映射为 COW：mmap(MAP_PRIVATE) / CreateFileMapping(PAGE_WRITECOPY) + MapViewOfFile(FILE_MAP_COPY)。

        //    重置状态：m_nSize_、m_nCount_ 更新，LRU / dirty 状态清空。

        //    为什么不重新 open：复用文件句柄，避免路径被改名 / 删除导致失败，也省一次 open 的开销。

        //    3. COW 语义（Linux / Windows 一致）
        //    读：缺页时从文件读入内存。

        //    写：第一次写某页触发 COW，内核复制该页为私有匿名页，后续写只改私有页。

        //    munmap / UnmapViewOfFile：私有页释放，文件不变。

        //    4. dirty / clean 页的区分
        //    用户态无法精确查询 COW 页是否已被写过。所以：

        //    mark_dirty(idx)：显式契约，应用声明"我可能要写这一页"。该页从 LRU 移除，常驻内存，永不淘汰。

        //    非 const operator[] / at()：自动调用 mark_dirty，作为安全兜底。

        //    const 访问：正常参与 LRU，可被淘汰。

        //    非 const data()：无法跟踪写，保守地禁用 LRU 淘汰（m_all_dirty_ = true），所有页常驻。

        //    关键：mark_dirty 必须在写之前调用。如果先写后标记，且该页恰好被 LRU 淘汰，修改会丢。

        //    5. reload 的语义
        //    确保 idx 所在页当前在内存，不丢弃任何修改：

        //    页状态	行为
        //    dirty 页	常驻，修改保留，直接返回
        //    clean 页在 LRU	移到 LRU 头部（刷新热度）
        //    clean 页不在 LRU	加入 LRU，淘汰最旧页，从文件预读
        //    m_all_dirty_	淘汰已禁用，直接返回
        //    Linux 用 madvise(MADV_WILLNEED) 异步预读；Windows 用 volatile 读触发缺页。

        //    6. LRU 淘汰（仅 clean 页）
        //    每次 const 访问，touch_page 把页移到 LRU 头部。

        //    LRU 超容量时，从尾部淘汰，Linux 上 madvise(MADV_DONTNEED)。

        //    dirty 页从不进入 LRU 列表，所以淘汰的一定是 clean 页，安全。

        //    7. 平台差异
        //    操作	Linux	Windows
        //    模式1 映射	mmap(MAP_SHARED)	CreateFileMapping(PAGE_READWRITE)
        //    模式2 映射	mmap(MAP_PRIVATE)	CreateFileMapping(PAGE_WRITECOPY)
        //    flush	msync(MS_SYNC)	FlushViewOfFile + FlushFileBuffers
        //    LRU 淘汰 clean 页	madvise(MADV_DONTNEED)，实际释放页	仅从统计移除，OS 自然回收
        //    reload 预读	madvise(MADV_WILLNEED) 异步	volatile 读触发缺页
        //    退出丢修改	munmap	UnmapViewOfFile
        //    Windows 上 LRU 的"淘汰"只影响统计，clean 文件页由 OS 在内存压力下回收，语义正确。

        //    8. 线程安全
        //    at / operator[] / mark_dirty / reload / prefetch：可多线程并发，内部有 m_lru_mtx_ 保护。

        //    open_ * / close / switch_to_cow / close_direct / set_lru_capacity / 移动赋值：不应与任何访问并发，由调用方保证。

        //    9. 限制与契约
        //    mark_dirty 必须在写之前。顺序：mark_dirty(i) → 写。

        //    非 const 读也会标 dirty（C++ 无法区分读 / 写）。纯读请用 const 引用：const CFileMapping<T>&cm = m; cm[i]。

        //    data() 在 COW 下禁用 LRU。裸指针写无法跟踪，安全优先。

        //    模式1 的"不调用不写"是程序语义。MAP_SHARED 的 dirty page 最终会被内核异步写回，程序无法完全阻止。flush() 是"确保已落盘"的语义。

        //    switch_to_cow / close_direct 内部会 flush。如果生成阶段数据量很大，flush 可能耗时较长，属正常。

        //    actual_count 不能超过 m_nCount_（open_direct 时映射的大小），否则返回 false。

        //    10. 编译
        //    bash
        //    # Linux
        //    g++ - std = c++11 - O2 - pthread main.cpp - o main

        //    # Windows(MSVC)
        //    cl / std:c++14 / O2 main.cpp
        //    - pthread 在 Linux 下必加（std::mutex 依赖）。C++11 即可，无需 C++17。
#endif BUBASE_TEST

#endif // !defined(AFX_MDBBASE_H__B2EF1D82_10E5_4C0B_B6EF_A98703B25A7C__INCLUDED_)
