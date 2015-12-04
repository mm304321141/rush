#pragma once

#include "ralgo.h"

template<typename T>
struct rlist_i
{
	T val;
	rlist_i<T>* prev;
	rlist_i<T>* next;
};

template<typename T>
struct rlist
{
	rlist_i<T>* front;
	rlist_i<T>* rear;
	int num;

	~rlist<T>()
	{
		clear();
	}

	rlist<T>()
	{
		init();
	}

	rlist<T>(rlist<T>& a)
	{
		init();
		T* p;
		for_set(p,a)
		{
			this->push(*p);
		}
	}

	void operator=(rlist<T>& a)
	{
		clear();
		T* p;
		for_set(p,a)
		{
			this->push(*p);
		}
	}

	void init()
	{
		front=null;
		rear=null;
		num=0;
	}

	void clear()
	{
		while(front!=null)
		{
			rear=front->next;
			r_delete<rlist_i<T> >(front);
			front=rear;
		}
		init();
	}

	rbool empty() const
	{
		return num==0;
	}

	int count() const
	{
		return num;
	}

	T& top() const
	{
		return *(T*)rear;
	}

	void push(const T& a)
	{
		rlist_i<T>* p=r_new<rlist_i<T> >();
		p->val=a;
		p->next=null;
		p->prev=rear;
		if(empty())
		{
			front=p;
			rear=p;
		}
		else
		{
			rear->next=p;
			rear=p;
		}
		num++;
	}

	T pop()
	{
		num--;
		rlist_i<T>* p=rear;
		T ret=p->val;
		rear=p->prev;
		if(rear!=null)
		{
			rear->next=null;
		}
		if(empty())
		{
			front=null;
		}
		r_delete<rlist_i<T> >(p);
		return ret;
	}

	void push_front(const T& a)
	{
		rlist_i<T>* p=r_new<rlist_i<T> >();
		p->val=a;
		p->next=front;
		p->prev=null;
		if(empty())
		{
			front=p;
			rear=p;
		}
		else
		{
			front->prev=p;
			front=p;
		}
		num++;
	}

	T pop_front()
	{
		num--;
		rlist_i<T>* p=front;
		T ret=p->val;
		front=p->next;
		if(front!=null)
		{
			front->prev=null;
		}
		if(empty())
		{
			rear=null;
		}
		r_delete<rlist_i<T> >(p);
		return ret;
	}
	
	//前插
	void insert(T* pos,const T& a)
	{
		if(pos==null)
		{
			push(a);
			return;
		}
		rlist_i<T>* cur=(rlist_i<T>*)pos;
		rlist_i<T>* p=r_new<rlist_i<T> >();
		p->val=a;
		p->next=cur;
		p->prev=cur->prev;
		if(cur->prev!=null)
		{
			cur->prev->next=p;
		}
		cur->prev=p;
		if(p->prev==null)
		{
			front=p;
		}
		if(p->next==null)
		{
			rear=p;
		}
		num++;
	}

	void insert(int pos,const T& a)
	{
		insert(index(pos),a);
	}

	void erase(T* pos)
	{
		if(pos==null)
		{
			return;
		}
		rlist_i<T>* cur=(rlist_i<T>*)pos;
		if(cur->prev==null)
		{
			front=cur->next;
		}
		if(cur->next==null)
		{
			rear=cur->prev;
		}
		if(cur->prev!=null)
		{
			cur->prev->next=cur->next;
		}
		if(cur->next!=null)
		{
			cur->next->prev=cur->prev;
		}
		r_delete<rlist_i<T> >(cur);
		num--;
	}

	void erase(int pos)
	{
		erase(index(pos));
	}

	T* index(int n) const
	{
		rlist_i<T>* p=front;
		for(;n;n--)
		{
			if(p==null)
			{
				return null;
			}
			p=p->next;
		}
		return (T*)p;
	}

	T& operator[](int n) const
	{
		T* p=index(n);
		return *p;
	}

	T* begin() const
	{
		return (T*)front;
	}

	static T* end()
	{
		return null;
	}

	T* next(void* p) const
	{
		return (T*)(((rlist_i<T>*)p)->next);
	}

	T* prev(void* p) const
	{
		return (T*)(((rlist_i<T>*)p)->prev);
	}
};
