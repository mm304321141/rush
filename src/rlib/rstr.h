#pragma once

#include "ralgo.h"

struct rstr
{
	rbuf<uchar> buf;

	~rstr()
	{
	}

	rstr()
	{
	}

#ifndef _RS
	rstr(const void* p)
	{
		set(p);
	}
#else
	rstr(const char* p)
	{
		set(p);
	}
#endif

	rstr(void* p,int len)
	{
		buf.alloc(len);
		xf::memcpy(begin(),p,len);
	}

	rstr(const void* p,const void* q)
	{
		int len=(int)((uchar*)q-(uchar*)p);
		buf.alloc(len);
		xf::memcpy(begin(),p,len);
	}

	rstr(int i)
	{
		char tmp[32];
		xf::itoa(tmp,i);
		set(tmp);
	}

	rstr(uint i)
	{
		char tmp[32];
		xf::utoa(tmp,i);
		set(tmp);
	}

	rstr(double i)
	{
		char tmp[128];
		xf::dtoa(tmp,i);
		set(tmp);
	}

	rstr(int64 i)
	{
		char tmp[128];
		xf::int64toa(tmp,i);
		set(tmp);
	}

	rstr(const rstr& s)
	{
		buf.alloc_not_change(s.buf.max_count);
		buf.cur_count=s.count();
		xf::memcpy(begin(),s.begin(),s.count());
	}

#ifdef SUPPORT_MOVE
	rstr(rstr&& s)
	{
		buf.move(s.buf);
	}
#endif

	//todo: 改为const
	int toint()
	{
		return xf::atoi(cstr_t());
	}

	uint touint()
	{
		return xf::atou(cstr_t());
	}

	double todouble()
	{
		return xf::atod(cstr_t());
	}

	int64 toint64()
	{
		int64 result;
		xf::atoint64(cstr_t(),&result);
		return result;
	}

	rstr torstr() const
	{
		return *this;
	}

	uchar* cstr()
	{
		this->push((uchar)0);//todo:
		this->pop();
		return begin();
	}

	char* cstr_t()
	{
		return (char*)cstr();
	}

	ushort* cstrw()
	{
		this->push((uchar)0);
		this->push((uchar)0);
		this->pop();
		this->pop();
		return (ushort*)begin();
	}

	wchar* cstrw_t()
	{
		return (wchar*)cstrw();
	}
	
	//赋值函数千万不要让编译器自动生成
	void operator=(const rstr& s)
	{
		buf.realloc_n_not_change(s.buf.max_count);
		buf.cur_count=s.count();
		xf::memcpy(begin(),s.begin(),s.count());
		//buf=a.buf;
	}

#ifdef SUPPORT_MOVE
	void operator=(rstr&& s)
	{
		buf.free();
		buf.move(s.buf);
	}
#endif

	uchar& operator[](int num) const 
	{
		return buf[num];
	}

	friend rbool operator==(const rstr& a,const rstr& b)
	{
		return a.buf==b.buf;
	}

	friend rbool operator!=(const rstr& a,const rstr& b)
	{
		return a.buf!=b.buf;
	}

	//有待优化
	friend rstr operator+(const rstr& a,const rstr& b)
	{
		int total=a.count()+b.count();
		int max=r_max(a.buf.max_count,b.buf.max_count);
		rstr ret;
		if(total>max)
		{
			ret.buf.alloc_not_change(rbuf<char>::get_extend_num(total));
		}
		else
		{
			ret.buf.alloc_not_change(max);
		}
		ret.buf.cur_count=total;
		xf::memcpy(ret.begin(),a.begin(),a.buf.cur_count);
		xf::memcpy(ret.begin()+a.buf.cur_count,b.begin(),b.buf.cur_count);
		return r_move(ret);
		//rstr ret;
		//ret.buf=a.buf+b.buf;
		//return ret;
	}

	void operator+=(const rstr& a)
	{
		buf+=a.buf;
	}

#ifdef SUPPORT_MOVE
	void operator+=(rstr&& a)
	{
		buf+=r_move(a.buf);
	}
#endif

	void operator+=(uchar ch)
	{
		buf+=ch;
	}

	friend rbool operator<(const rstr& a,const rstr& b)
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

	friend rbool operator>(const rstr& a,const rstr& b)
	{
		return b<a;
	}

	friend rbool operator<=(const rstr& a,const rstr& b)
	{
		return !(a>b);
		//return a<b||a==b;
	}

	friend rbool operator>=(const rstr& a,const rstr& b)
	{
		return !(a<b);
		//return a>b||a==b;
	}

	uchar* set_size(int size)
	{
		buf.realloc_n(size);
		return begin();
	}

	void set(const void* p)
	{
		//todo:如果字符串长度为0没必要分配内存
		int len=xf::strlen((const char*)p);
		buf.realloc_n_not_change(rbuf<uchar>::get_extend_num(len));
		buf.cur_count=len;
		xf::memcpy(buf.begin(),p,len);
	}

	void push(uchar ch)
	{
		buf.push(ch);
	}

	void push_front(uchar ch)
	{
		buf.push_front(ch);
	}

	uchar pop()
	{
		return buf.pop();
	}

	uchar pop_front()
	{
		return buf.pop_front();
	}

	int count() const 
	{
		return buf.count();
	}

	int size() const
	{
		return count();
	}

	uchar* begin() const 
	{
		return buf.begin();
	}

	uchar* end() const 
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

	static uchar* next(void* p)
	{
		return ((uchar*)p)+1;
	}

	static uchar* prev(void* p)
	{
		return ((uchar*)p)-1;
	}

	uchar* pos(int num) const
	{
		return begin()+num;
	}

	int pos(void* p) const
	{
		return int((uchar*)p-begin());
	}

	rstr sub_tail(int num) const
	{
		return sub(count()-num);
	}

	rstr sub_trim(int num) const
	{
		return sub(0,count()-num);
	}

	rstr sub(int begin) const
	{
		return sub(begin,count());
	}

	rstr sub(int begin,int end) const
	{
		rstr ret;
		ret.buf=buf.sub(begin,end);
		return r_move(ret);
	}

	rstr reverse() const
	{
		rstr result=(*this);
		r_reverse<uchar>(result.buf);
		return r_move(result);
	}

	rbool erase(int begin,int end)
	{
		return buf.erase(begin,end);
	}

	rbool erase(int i)
	{
		return erase(i,i+1);
	}

	rbool insert(int pos,const rstr& a)
	{
		return buf.insert(pos,a.buf);
	}

	uchar get(int i) const
	{
		if(i>=count())
		{
			return (uchar)0;
		}
		else
		{
			return buf[i];
		}
	}

	uchar get_top() const
	{
		if(count()>0)
		{
			return buf.point[count()-1];
		}
		return (uchar)0;
	}

	uchar get_bottom() const
	{
		if(count()>0)
		{
			return buf.point[0];
		}
		return (uchar)0;
	}

	uchar get_left() const
	{
		return get_bottom();
	}

	uchar get_right() const
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
			ifn(is_number(buf[i]))
			{
				return false;
			}
		}
		return true;
	}

	int find(const rstr& s,int begin=0) const
	{
		return r_find<rstr>(*this,s,begin);
	}

	int find(uchar ch,int begin=0) const
	{
		return buf.find(ch,begin);
	}

	int find_last(uchar ch) const
	{
		return buf.find_last(ch);
	}

	rbool exist(const rstr& s) const
	{
		return find(s)<count();
	}

	static rbool is_alpha(uchar ch)
	{
		return (ch>=r_char('a')&&ch<=r_char('z')||
			ch>=r_char('A')&&ch<=r_char('Z'));
	}

	static rbool is_number(uchar ch)
	{
		return ch>=r_char('0')&&ch<=r_char('9');
	}

	static uchar trans_char_to_up(uchar ch)
	{
		return r_cond(ch>=r_char('a')&&ch<=r_char('z'),ch-32,ch);
	}

	static int trans_upchar_to_num(uchar ch)
	{
		return r_cond(ch>=r_char('A'),ch-r_char('A')+10,ch-r_char('0'));
	}

	static int trans_char_to_num(uchar ch)
	{
		return trans_upchar_to_num(trans_char_to_up(ch));
	}

	static uchar trans_hex_to_byte(rstr s)
	{
		return (uchar)(trans_char_to_num(s.get(0))*16+trans_char_to_num(s.get(1)));
	}

	static rstr trans_hex_to_dec(rstr s)
	{
		uint a;
		xf::sscanf(s.cstr_t(),"%x",&a);
		return rstr(a);
	}

	static rstr trans_bin_to_dec(rstr s)
	{
		uint sum=0;
		uint pro=1;
		for(int i=s.count()-1;i>=0;i--)
		{
			if(s[i]==r_char('1'))
			{
				sum+=pro;
			}
			pro*=2;
		}
		return rstr(sum);
	}

	template<typename T>
	static rstr join(const rbuf<T>& v,const rstr& s)
	{
		rstr ret;
		for(int i=0;i<v.count();i++)
		{
			if(i!=0)
			{
				ret+=s;
			}
			ret+=v[i].torstr();
		}
		return r_move(ret);
	}

	void print()
	{
		xf::print(cstr_t());
	}

	void printl() const
	{
		xf::print((*this+"\n").cstr_t());
	}

#ifndef _RS
	static rstr format(const char* f,...)
	{
		char buf[1024];
		va_list arg_list;
		va_start(arg_list,f);
		xf::vsnprintf(buf,r_size(buf),f,(char*)arg_list);
		va_end(arg_list);
		return rstr(buf);
	}
#else
	static rstr format(const char* f,int a)
	{
		rbuf<char> buf(1024);
		xf::vsnprintf(buf.begin(),buf.count(),f,(char*)(&a));
		return rstr(buf.begin());
	}

	rstrw torstrw()
	{
		return rstrw(*this);
	}
#endif
};
