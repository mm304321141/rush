#pragma once

#include "rstr.h"
#include "rset.h"

template<typename T>
struct rdic_i
{
	T val;//val必须前置
	rstr key;

	friend rbool operator<(const rdic_i<T>& a,const rdic_i<T>& b)
	{
		return a.key<b.key;
	}
};

template<typename T>
struct rdic
{
	rset<rdic_i<T> > set;

	void clear()
	{
		set.clear();
	}

	T* begin() const
	{
		return (T*)(set.begin());
	}

	T* end() const
	{
		return (T*)(set.end());
	}

	T* next(T* p) const
	{
		return (T*)(set.next((rdic_i<T>*)p));
	}

	rstr get_key(T* p) const
	{
		return ((rdic_i<T>*)p)->key;
	}

	rbool empty() const
	{
		return set.empty();
	}

	int count() const
	{
		return set.count();
	}

	rbool exist(const rstr& key) const
	{
		rdic_i<T> item;
		item.key=key;
		return set.exist(item);
	}

	T* find(const rstr& key) const
	{
		rdic_i<T> item;
		item.key=key;
		return (T*)(set.find(item));
	}

	void insert(const rstr& key,const T& val)
	{
		rdic_i<T> item;
		item.key=key;
		item.val=val;
		set.insert(item);
	}

	T& operator[](const rstr& key)
	{
		rdic_i<T> item;
		item.key=key;
		set.insert_c(item);
		rdic_i<T>* p=set.find(item);
		return p->val;
	}
};
