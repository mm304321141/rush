#pragma once

#include "rcode.h"

struct rstrw
{
	rbuf<ushort> buf;

	~rstrw()
	{
	}

	rstrw()
	{
	}

	rstrw(const rstrw& s)
	{
		buf.alloc_not_change(s.buf.max_count);
		buf.cur_count=s.count();
		xf::memcpy(begin(),s.begin(),s.size());
	}

	rstrw(const char* p)
	{
		rstr tmp=rcode::trans_utf8_to_utf16(rstr(p));//todo:
		set_size(tmp.size());
		xf::memcpy(begin(),tmp.begin(),tmp.size());
	}

	rstrw(const rstr& s)
	{
		rstr tmp=rcode::trans_utf8_to_utf16(s);
		set_size(tmp.size());
		xf::memcpy(begin(),tmp.begin(),tmp.size());
	}

#ifdef SUPPORT_MOVE
	rstrw(rstrw&& s)
	{
		buf.move(s.buf);
	}
#endif

	rstr torstr() const
	{
		return rcode::trans_utf16_to_utf8(rstr((char*)begin(),size()));
	}

	int toint()
	{
		return this->torstr().toint();
	}

	ushort* cstrw()
	{
		this->push((ushort)0);
		this->pop();
		return begin();
	}

	wchar* cstrw_t()
	{
		return (wchar*)(cstrw());
	}

	void operator=(const rstrw& a)
	{
		buf=a.buf;
	}

#ifdef SUPPORT_MOVE
	void operator=(rstrw&& a)
	{
		buf.free();
		buf.move(a.buf);
	}
#endif

	ushort& operator[](int num) const 
	{
		return buf[num];
	}

	friend rbool operator==(const rstrw& a,const rstrw& b)
	{
		return a.buf==b.buf;
	}

	friend rbool operator!=(const rstrw& a,const rstrw& b)
	{
		return a.buf!=b.buf;
	}

	friend rstrw operator+(const rstrw& a,const rstrw& b)
	{
		rstrw ret;
		ret.buf=a.buf+b.buf;
		return r_move(ret);
	}

	void operator+=(const rstrw& a)
	{
		buf+=a.buf;
	}

#ifdef SUPPORT_MOVE
	void operator+=(rstrw&& a)
	{
		buf+=r_move(a.buf);
	}
#endif

	void operator+=(ushort ch)
	{
		buf+=ch;
	}

	friend rbool operator<(const rstrw& a,const rstrw& b)
	{
		for(int i=0;i<a.count()&&i<b.count();i++)
		{
			if(a[i]!=b[i])
			{
				return a[i]<b[i];
			}
		}
		return a.count()<b.count();
	}

	friend rbool operator>(const rstrw& a,const rstrw& b)
	{
		return b<a;
	}

	friend rbool operator<=(const rstrw& a,const rstrw& b)
	{
		return !(a>b);
	}

	friend rbool operator>=(const rstrw& a,const rstrw& b)
	{
		return !(a<b);
	}

	ushort* set_count(int count)
	{
		buf.realloc_n(count);
		return begin();
	}

	ushort* set_size(int size)
	{
		return set_count(size/2);
	}

	void set_w(const wchar* p)
	{
		int len=xf::strlenw(p);
		buf.realloc_n_not_change(rbuf<ushort>::get_extend_num(len));
		buf.cur_count=len;
		xf::memcpy(buf.begin(),p,len*2);
	}

	void push(ushort ch)
	{
		buf.push(ch);
	}

	void push_front(ushort ch)
	{
		buf.push_front(ch);
	}

	ushort pop()
	{
		return buf.pop();
	}

	ushort pop_front()
	{
		return buf.pop_front();
	}

	int count() const 
	{
		return buf.count();
	}

	int size() const
	{
		return count()*2;
	}

	ushort* begin() const 
	{
		return buf.begin();
	}

	ushort* end() const 
	{
		return buf.end();
	}

	rbool empty() const 
	{
		return buf.empty();
	}

	void clear()
	{
		buf.clear();
	}

	static ushort* next(void* p)
	{
		return ((ushort*)p)+1;
	}

	static ushort* prev(void* p)
	{
		return ((ushort*)p)-1;
	}

	ushort* pos(int num) const
	{
		return begin()+num;
	}

	int pos(void* p) const
	{
		return int((ushort*)p-begin());
	}

	rstrw sub(int begin) const
	{
		return sub(begin,count());
	}

	rstrw sub(int begin,int end) const
	{
		rstrw ret;
		ret.buf=buf.sub(begin,end);
		return r_move(ret);
	}

	rbool erase(int begin,int end)
	{
		return buf.erase(begin,end);
	}

	rbool erase(int i)
	{
		return erase(i,i+1);
	}

	rbool insert(int pos,const rstrw& a)
	{
		return buf.insert(pos,a.buf);
	}

	ushort get(int i) const
	{
		if(i>=count())
		{
			return (ushort)0;
		}
		else
		{
			return buf[i];
		}
	}

	ushort get_top() const
	{
		if(count()>0)
		{
			return buf.point[count()-1];
		}
		return (ushort)0;
	}

	ushort get_bottom() const
	{
		if(count()>0)
		{
			return buf.point[0];
		}
		return (ushort)0;
	}

	ushort get_left() const
	{
		return get_bottom();
	}

	ushort get_right() const
	{
		return get_top();
	}

	rbool is_number() const
	{
		if(empty())
		{
			return false;
		}
		for(int i=0;i<count();i++)
		{
			if(!is_number(buf[i]))
			{
				return false;
			}
		}
		return true;
	}

	static rbool is_number(ushort ch)
	{
		return ch>=r_char('0')&&ch<=r_char('9');
	}

	int find(rstrw s,int begin=0) const
	{
		return r_find<rstrw>(*this,s,begin);
	}

	int find_last(ushort ch) const
	{
		return buf.find_last(ch);
	}
};
