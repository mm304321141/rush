#pragma once

#include "rnew.h"

template<typename T>
struct rbuf
{
	T* point;
	int cur_count;
	int max_count;
	uchar block[16];
	//当分配小内存时，point指向block，无需new可提高效率

	~rbuf<T>()
	{
		free_x();
	}

	rbuf<T>()
	{
		init();
	}

	rbuf<T>(int num)
	{
		init();
		alloc(num);
	}

	rbuf<T>(int num,const T& a)
	{
		init();
		alloc(num);
		for(int i=0;i<num;i++)
		{
			point[i]=a;
		}
	}

	rbuf<T>(const rbuf<T>& a)
	{
		init();
		copy(a);
	}

#ifdef SUPPORT_MOVE
	rbuf<T>(rbuf<T>&& a)
	{
		move(a);
	}
#endif

	void move(rbuf<T>& a)
	{
#ifndef _RS
		if(a.point==(T*)(a.block))
		{
			xf::memcpy(this,&a,r_size(rbuf<T>));
			point=(T*)block;
		}
		else
		{
			point=a.point;
			cur_count=a.cur_count;
			max_count=a.max_count;
		}
#else
		if(a.point==&a.block)
		{
			xf::memcpy(this,&a,r_size(rbuf<T>));
			point=&block;
		}
		else
		{
			point=a.point;
			cur_count=a.cur_count;
			max_count=a.max_count;
		}
#endif
		a.init();//如果move后对象不再使用可以只置空a.point
	}

	//如果复制指针一定要处理好等于号，不支持自己复制自己
	void operator=(const rbuf<T>& a)
	{
		if(a.point!=point)
		{
			copy(a);
		}
	}

#ifdef SUPPORT_MOVE
	void operator=(rbuf<T>&& a)
	{
		free_x();
		move(a);
	}
#endif

	friend rbuf<T> operator+(const rbuf<T>& a,const rbuf<T>& b)
	{
		int total=a.count()+b.count();
		int max=r_max(a.max_count,b.max_count);
		rbuf<T> ret;
		if(total>max)
		{
			ret.alloc_not_change(get_extend_num(total));
		}
		else
		{
			ret.alloc_not_change(max);
		}
		int i;
		for(i=0;i<a.count();i++)
		{
			ret[i]=a[i];
		}
		for(i=0;i<b.count();i++)
		{
			ret[i+a.count()]=b[i];
		}
		ret.cur_count=total;
		return r_move(ret);
	}

	void operator+=(const rbuf<T>& a)
	{
		int total=a.count()+count();
		if(total>max_count)
		{
			realloc_not_change(get_extend_num(total));
		}
		for(int i=count();i<total;i++)
		{
			point[i]=a[i-count()];
		}
		cur_count=total;
	}

#ifdef SUPPORT_MOVE
	void operator+=(rbuf<T>&& a)
	{
		int total=a.count()+count();
		if(total>max_count)
		{
			realloc_not_change(get_extend_num(total));
		}
		for(int i=count();i<total;i++)
		{
			point[i]=r_move(a[i-count()]);
		}
		cur_count=total;
	}

	void operator+=(T&& a)
	{
		this->push(a);
	}
#endif

	void operator+=(const T& a)
	{
		this->push(a);
	}

	T& operator[](int num) const
	{
		return point[num];
	}

	friend rbool operator==(const rbuf<T>& a,const rbuf<T>& b)
	{
		if(a.count()!=b.count())
		{
			return false;
		}
		for(int i=0;i<a.count();++i)
		{
			if(a[i]!=b[i])
			{
				return false;
			}
		}
		return true;
	}

	friend rbool operator!=(const rbuf<T>& a,const rbuf<T>& b)
	{
		return !(a==b);
	}

	void init()
	{
		point=null;
		cur_count=0;
		max_count=0;
	}

	static T* next(void* p)
	{
		return (T*)p+1;
	}

	static T* prev(void* p)
	{
		return (T*)p-1;
	}

	int size() const 
	{
		return cur_count*r_size(T);
	}

	int count() const
	{
		return cur_count;
	}

	T* begin() const 
	{
		return point;
	}

	T* end() const 
	{
		return point+count();
	}

	T* rbegin() const 
	{
		if(0==cur_count)
		{
			return null;
		}
		else
		{
			return point+count()-1;
		}
	}

	T* rend() const 
	{
		if(0==cur_count)
		{
			return null;
		}
		else
		{
			return point-1;
		}
	}

	rbool empty() const 
	{
		return 0==cur_count;
	}

	void clear()
	{
		cur_count=0;
	}

	static int get_extend_num(int num)
	{
		return r_cond(num<16,16,num*2);
	}
	
	//应判断num是否太大,否则在64位上num++有可能归零
	void push(const T& a)
	{
		if(cur_count>=max_count)
		{
			realloc_not_change(get_extend_num(cur_count));
		}
		point[count()]=a;
		cur_count++;
	}

#ifdef SUPPORT_MOVE
	void push(T&& a)
	{
		if(cur_count>=max_count)
		{
			realloc_not_change(get_extend_num(cur_count));
		}
		point[count()]=a;
		cur_count++;
	}
#endif

	T pop()
	{
		cur_count--;
		return point[count()];
	}

	T pop_front()
	{
		T ret=r_move(point[0]);
		erase(0);
		return r_move(ret);
	}

	//没必要增加push_front的move版本
	void push_front(const T& a)
	{
		insert(0,a);
	}

	T& top() const 
	{
		return point[count()-1];
	}

	T& bottom() const 
	{
		return point[0];
	}

	T get_top() const 
	{
		if(count()>0)
		{
			return point[count()-1];
		}
		return T();
	}

	T get_right() const 
	{
		return get_top();
	}

	T get_bottom() const 
	{
		if(count()>0)
		{
			return point[0];
		}
		return T();
	}

	T get_left() const 
	{
		return get_bottom();
	}

	rbool erase(int num)
	{
		if(num>=count()||num<0)
		{
			return false;
		}
		for(int i=num;i<count()-1;++i)
		{
			*(point+i)=r_move(*(point+i+1));
		}
		cur_count--;
		return true;
	}

	rbool erase(int begin,int end)
	{
		if(begin<0||end>count()||begin>=end)
		{
			return false;
		}
		for(int i=0;i<count()-end;i++)//count()-(end-begin)-begin
		{
			point[i+begin]=r_move(point[end+i]);//删除的元素稍后析构
		}
		cur_count-=end-begin;
		return true;
	}

	rbool insert(int pos,const T& a)
	{
		if(pos>count()||pos<0)
		{
			return false;
		}
		if(cur_count>=max_count)
		{
			realloc_not_change(get_extend_num(cur_count));
		}
		for(int i=count();i>pos;--i)
		{
			point[i]=r_move(point[i-1]);
		}
		point[pos]=a;
		cur_count++;
		return true;
	}

	rbool insert(int pos,const rbuf<T>& a)
	{
		if(pos>count()||pos<0||0==a.count())
		{
			return false;
		}
		if(max_count<cur_count+a.count())
		{
			realloc_not_change(a.count()+count());
		}
		for(int i=0;i<count()-pos;i++)
		{
			point[a.count()+count()-1-i]=r_move(point[count()-1-i]);
		}
		for(int i=0;i<a.count();i++)
		{
			point[pos+i]=a[i];
		}
		cur_count+=a.count();
		return true;
	}

	T* v_new(int num) const
	{
#ifdef _RGPP
		return r_new<T>(num);//GPP模式不能使用此优化技巧
#endif
		if(num*r_size(T)<=16)
		{
#ifndef _RS
			return (T*)block;
#else
			return &block;
#endif
		}
		return r_new<T>(num);
	}

	void v_delete()
	{
#ifdef _RGPP
		r_delete<T>(point);
		return;
#endif
#ifndef _RS
		if(point!=(T*)block)
		{
			r_delete<T>(point);
		}
#else
		if(point!=&block)
		{
			r_delete<T>(point);
		}
#endif
	}

	//这里的重复代码可用宏或者模板简化
	void alloc(int num)
	{
		if(point!=null)
		{
			return;
		}
		if(num<=0)
		{
			init();
			return;
		}
		point=v_new(num);
		max_count=num;
		cur_count=max_count;
	}

	void alloc_not_change(int num)
	{
		if(point!=null)
		{
			return;
		}
		if(num<=0)
		{
			init();
			return;
		}
		point=v_new(num);
		max_count=num;
	}

	void realloc(int num)
	{
		if(null==point)
		{
			alloc(num);
			return;
		}
		if(num<=0)
		{
			free();
			return;
		}
		realloc_not_change_in(num);
		cur_count=max_count;
	}

	void realloc_not_change(int num)
	{
		if(null==point)
		{
			alloc_not_change(num);
			return;
		}
		if(num<=0)
		{
			free();
			return;
		}
		realloc_not_change_in(num);
	}

	void realloc_not_change_in(int num)
	{
		T* p=v_new(num);
		int copy_size=r_min(num,cur_count);
		for(int i=0;i<copy_size;i++)
		{
			p[i]=r_move(point[i]);
		}
		v_delete();
		point=p;
		max_count=num;
	}
	
	//不复制原有的元素
	void realloc_n(int num)
	{
		if(null==point)
		{
			alloc(num);
			return;
		}
		if(num<=0)
		{
			free();
			return;
		}
		v_delete();
		point=v_new(num);
		max_count=num;
		cur_count=max_count;
	}

	void realloc_n_not_change(int num)
	{
		if(null==point)
		{
			alloc_not_change(num);
			return;
		}
		if(num<=0)
		{
			free();
			return;
		}
		v_delete();
		point=v_new(num);
		max_count=num;
	}
	
	void free_x()
	{
		if(null!=point)//point为空时rbuf必须保证cur_count和max_count也为空
		{
			v_delete();
		}
	}

	void free()
	{
		free_x();
		init();
	}

	void copy(const rbuf<T>& a)
	{
		if(max_count<a.count())
		{
			realloc_n_not_change(a.max_count);
		}
		for(int i=0;i<a.count();i++)
		{
			point[i]=a[i];
		}
		cur_count=a.count();
	}

	T get(int i) const
	{
		if((uint)i>=(uint)(count()))//补码判断
		{
			return T();
		}
		else 
		{
			return point[i];
		}
	}

	rbuf<T> sub(int begin,int end) const 
	{
		if(end>count())
		{
			end=count();
		}
		if(begin<0)
		{
			begin=0;
		}
		int temp=end-begin;
		rbuf<T> ret(temp);//alloc会判断temp<0的情况，所以不需要判断begin>=end
		for(int i=0;i<temp;i++)
		{
			ret[i]=point[begin+i];
		}
		return r_move(ret);
	}

	rbuf<T> sub(int begin) const 
	{
		return sub(begin,count());
	}

	rbuf<T> sub_trim(int num) const
	{
		return sub(0,count()-num);
	}

	rbool exist(const T& a) const
	{
		for(int i=0;i<count();i++)
		{
			if(a==point[i])
			{
				return true;
			}
		}
		return false;
	}

	int find(const T& a,int begin=0) const
	{
		for(int i=begin;i<count();i++)
		{
			if(a==point[i])
			{
				return i;
			}
		}
		return count();
	}

	int find_last(const T& a) const
	{
		for(int i=count()-1;i>=0;i--)
		{
			if(a==point[i])
			{
				return i;
			}
		}
		return count();
	}

	void print() const 
	{
		for(int i=0;i<count();i++)
		{
			point[i].print();
		}
	}

	void printl() const 
	{
		for(int i=0;i<count();i++)
		{
			point[i].printl();
		}
	}
#ifdef _RS
	rstr join(rstr s) const
	{
		rstr ret
		for i=0;i<count;i++
			if i!=0
				ret+=s
			ret+=point[i].torstr
		return ret
	}
	
	rbuf<T> map(void* f) const
	{
		rbuf<T> ret
		for i=0;i<count;i++
			ret.push(T[f,point[i]])
		return ret
	}
	
	void parallel(int num,void* f)
	{
		rmutex* pmu=r_new<rmutex>()
		int* pcur=r_new<int>()
		rbuf<T>* pthis=this
		rbuf<int> arr_thr
		*pcur=0
		for i=0;i<num;i++
			arr_thr.push(xf.create_thr(lambda(void* param){
				for
					pmu->enter
					if *pcur>=pthis->count
						pmu->leave
						return
					index=*pcur
					(*pcur)++
					pmu->leave
					void[f,&(*pthis)[index]]
			}))
		for i in arr_thr
			xf.wait_thr(arr_thr[i])
			xf.CloseHandle(arr_thr[i])
		r_delete<int>(pcur)
		r_delete<rmutex>(pmu)
	}
#endif
};
