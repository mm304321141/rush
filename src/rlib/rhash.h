#pragma once

#include "rstr.h"

template<typename T>
struct rhash_i
{
	T val;
	rstr name;
	rhash_i<T>* pre;
	rhash_i<T>* next;
};

template<typename T>
struct rhash
{
	rbuf<rhash_i<T>*> vp;
	int num;

	~rhash<T>()
	{
		clear();
	}

	rhash<T>()
	{
		init();
	}

	rhash<T>(rhash<T>& a)
	{
		init();
		T* p;
		for_set(p,a)
		{
			insert(*p);
		}
	}

	void operator=(rhash<T>& a)
	{
		clear();
		T* p;
		for_set(p,a)
		{
			insert(*p);
		}
	}

	T& operator[](const rstr& name)
	{
		T* p=(T*)(find(name));
		if(p!=null)
		{
			return *p;
		}
		//找不到则插入一个空值
		T a;
		insert(a,name);
		p=(T*)(find(name));
		return *p;
	}

	void init()
	{
		//两个字节消耗256K内存，如果用3个字节需要64M内存
		vp.realloc_n(65536);
		for(int i=0;i<vp.count();i++)
		{
			vp[i]=null;
		}
		num=0;
	}

	void clear()
	{
		if(num==0)
		{
			return;
		}
		for(int i=0;i<vp.count();i++)
		{
			if(vp[i]!=null)
			{
				rhash_i<T>* p=vp[i];
				while(p!=null)
				{
					rhash_i<T>* temp=p;
					p=p->next;
					r_delete<rhash_i<T> >(temp);
				}
			}
		}
		init();
	}

	rbool exist(const rstr& name) const
	{
		return find(name)!=null;
	}

	T* find(const rstr& name) const
	{
		int addr=get_addr(name);
		rhash_i<T>* p=vp[addr];
		while(p!=null)
		{
			if(p->name==name)
			{
				return (T*)p;
			}
			p=p->next;
		}
		return null;
	}
	
	//如果重复则不插入
	void insert_c(T& a,rstr name=rstr())
	{
		if(exist(name))
		{
			return;
		}
		insert(a,name);
	}

	void insert(T& a,rstr name=rstr())
	{
		int addr=get_addr(name);
		rhash_i<T>* p=r_new<rhash_i<T> >();
		p->name=name;
		p->next=vp[addr];
		p->pre=null;
		p->val=a;
		if(vp[addr]!=null)
		{
			vp[addr]->pre=p;
		}
		vp[addr]=p;
		num++;
	}

	rbool erase(rstr name=rstr())
	{
		rhash_i<T>* p=(rhash_i<T>*)(find(name));
		if(p==null)
		{
			return false;
		}
		if(p->pre!=null)
		{
			p->pre->next=p->next;
			if(p->next!=null)
			{
				p->next->pre=p->pre;
			}
		}
		else
		{
			vp[get_addr(name)]=p->next;
			if(p->next!=null)
			{
				p->next->pre=null;
			}
		}
		r_delete<rhash_i<T> >(p);
		num--;
		return true;
	}

	T* begin() const
	{
		for(int i=0;i<vp.count();i++)
		{
			if(vp[i]!=null)
			{
				return (T*)(vp[i]);
			}
		}
		return null;
	}

	T* rend() const
	{
		for(int i=vp.count()-1;i>=0;i--)
		{
			if(vp[i]!=null)
			{
				rhash_i<T>* p=vp[i];
				while(p!=null&&p->next!=null)
				{
					p=p->next;
				}
				return (T*)p;
			}
		}
		return null;
	}
	
	//这个next有点慢，能不能把一个桶的最后一个元素的next指向下一个桶？
	T* next(T* cur) const
	{
		rhash_i<T>* p=(rhash_i<T>*)cur;
		if(p->next!=null)
		{
			return (T*)(p->next);
		}
		int addr=get_addr(p->name);
		for(int i=addr+1;i<vp.count();i++)
		{
			if(vp[i]!=null)
			{
				return (T*)(vp[i]);
			}
		}
		return null;
	}

	static int get_addr(const rstr& name)
	{
		return (int)(bkdr_hash16(name.begin(),name.count()));
	}

	static int bkdr_hash32(const void* data,int len)
	{
		uint seed=131; // 31 131 1313 13131 131313 etc..
		uint hash=0;
		for(int i=0;i<len;++i)
		{
			hash=(hash*seed)+((const uchar*)data)[i];
		}
		return (int)hash;
	}

	static ushort bkdr_hash16(const void* data,int len)
	{
		int val=bkdr_hash32(data,len);
		return (ushort)(val%65536);
	}
};
