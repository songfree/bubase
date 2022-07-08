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
	void Erase(Record& rt)
	{
		m_index.erase(rt);
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
    void Erase(Record& rt)
    {
        m_index.erase(rt);
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

	void Erase(Record& rt)
	{
		m_index.erase(rt);
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
	void Delete(int id)
	{
		m_intdata.erase(id);
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
				r=f1.k[i]-f2.k[i];
				if (r != 0)
				{
					break;
				}
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
	void Delete(int k,...)
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
		m_index.Erase(a_);
		return;
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
				r=f1.k[i]-f2.k[i];
				if (r != 0)
				{
					break;
				}
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
	void Delete(int k,...)
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
		m_key.Erase(a_);
		return;
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
    void Delete(int k, ...)
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
        m_key.Erase(a_);
        return;
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
				r=f1.k[i]-f2.k[i];
				if (r != 0)
				{
					break;
				}
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
	void Delete(unsigned int k,...)
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
		m_index.Erase(a_);
		return;
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
				r=f1.k[i]-f2.k[i];
				if (r != 0)
				{
					break;
				}
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
	void Delete(unsigned int k,...)
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
		m_key.Erase(a_);
		return;
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
    void Delete(unsigned int k, ...)
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
        m_key.Erase(a_);
        return;
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
	void Delete(char *p,...)
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
		m_index.Erase(a_);
		return;
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
	void Delete(char *p,...)
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
		m_key.Erase(a_);
		return;
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
    void Delete(char* p, ...)
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
        m_key.Erase(a_);
        return;
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
	void Delete(char *p,...)
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
		m_key.Erase(a_);
		return;
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
    void Delete(char* p, ...)
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
        m_key.Erase(a_);
        return;
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
	void Delete(char *p,...)
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
		m_index.Erase(a_);
		return;
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
	void Delete(char *p)
	{
		Char_ a_;

		a_.p=p;
		m_index.Erase(a_);
		return;
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
	void Delete(UINT64_ k,...)
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
		m_index.Erase(a_);
		return;
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
				r=f1.k[i]-f2.k[i];
				if (r != 0)
				{
					break;
				}
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
	void Delete(UINT64_ k,...)
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
		m_key.Erase(a_);
		return;
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
    void Delete(UINT64_ k, ...)
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
        m_key.Erase(a_);
        return;
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
		m_rowidpool.push_back(id);
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
//内存表
template<typename Record>
class CMemTable  
{
public:
	
	typedef std::vector<Record> VECTOR_;       //定义保存内存表数据的类型，vector的下标相当于rowid
//	typedef std::deque<Record> VECTOR_;       //定义保存内存表数据的类型，vector的下标相当于rowid
	std::list<int>    m_rowidpool;              //保存删除的内存表数据ID，相当于rowid，下次增加时直接使用         
	VECTOR_      m_table;                  //表数据
	std::vector<bool> m_inuseid;                //rowid对应的数据是否删除
	unsigned int  m_current;
public:
	
	CMemTable()
	{
		m_current = 0;
	}
	~CMemTable()
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
		m_rowidpool.push_back(id);
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
		for (int i=0;i<m_table.size();i++)
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
	
	CMemTable <Record *> m_table;
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
	
	CMemTable <S_TBL_SAMPLE> m_table;        //内存表
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
	
	CMemTable <S_CHAR_KEY> m_table;
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




#endif // !defined(AFX_MDBBASE_H__B2EF1D82_10E5_4C0B_B6EF_A98703B25A7C__INCLUDED_)
