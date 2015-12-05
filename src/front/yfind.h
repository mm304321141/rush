﻿#pragma once

#include "ybase.h"

struct yfind
{
	//获取类型大小
	static int get_type_size(const tsh& sh,const rstr& s)
	{
		if(ybase::is_point_quote(s))
		{
			return sh.point_size;
		}
		tclass* ptci=find_class_t(sh,s);
		if(ptci!=null)
		{
			return ptci->size;
		}
		return 0;
	}

	static int get_func_local_size(const tfunc& tfi)
	{
		int ret=0;
		for(int i=0;i<tfi.local.count();++i)
		{
			ret+=get_ceil_space(tfi.local[i]);
		}
		return ret;
	}

	static int get_func_param_size(const tfunc& tfi)
	{
		int ret=0;
		for(int i=0;i<tfi.param.count();++i)
		{
			ret+=get_ceil_space(tfi.param[i]);
		}
		return ret;
	}

	//不包括返回值
	static int get_func_stack_size(const tfunc& tfi)
	{
		return get_func_local_size(tfi)+get_func_param_size(tfi)+c_point_size*2;
	}

	//32位栈空间按4字节排列，64位栈空间按8字节排列，因为amd64没有32位push指令
	static int get_ceil_space(const tdata& tdi)
	{
		return r_ceil_div(tdi.size,c_point_size)*c_point_size;
	}

	static rbool is_type_mebx(const tsh& sh,const rstr& type)
	{
		//这里不用is_point_quote可略微提高效率
		return type==rskey(c_int)||ybase::is_point(type)||ybase::is_quote(type);
	}

	static rbool is_op_pass_type(const tsh& sh,const rstr& s)
	{
		return s==rskey(c_int)||s==rskey(c_uint)||ybase::is_point(s);
	}

	static rbool is_rstr_optr(const tsh& sh,const rstr& s)
	{
		return (s==rsoptr(c_plus)||s==rsoptr(c_equal_equal)||
			s==rsoptr(c_not_equal)||s==rsoptr(c_great_equal)||
			s==rsoptr(c_less_equal));
		//大于和小于最好不要加上，防止与尖括号作用冲突
	}

	static rbool is_end_part_optr(const tsh& sh,const rstr& s)
	{
		ifn(sh.optr.is_optr(s))
		{
			return false;
		}
		return (s==rsoptr(c_dot)||s==rsoptr(c_sbk_r)||
			s==rsoptr(c_mbk_r)||s==rsoptr(c_plus));
		//还有很多这样的运算符，暂时不写
	}

	static rbool is_empty_struct_type(const tsh& sh,const rstr& s)
	{
		return (s==rskey(c_int)||s==rskey(c_char)||
			s==rskey(c_uint)||s==rskey(c_int64)||
			s==rskey(c_double)||s==rskey(c_rd32)||
			s=="float"||ybase::is_point_quote(s));
	}

	static rbool is_class(const tsh& sh,const rstr& s)
	{
		return find_class(sh,s)!=null;
	}

	static rbool is_class_t(const tsh& sh,const rstr& s)
	{
		return find_class_t(sh,s)!=null;
	}

	static rbool is_classtl(const tsh& sh,const rstr& s)
	{
		return find_classtl(sh,s)!=null;
	}

	static tclass* find_class_t(const tsh& sh,const rstr& name)
	{
		return find_class(sh,ybase::get_tname(name));
	}

	static tclass* find_class(const tsh& sh,const rstr& name)
	{
		tclass item;
		item.name=name;
		return sh.s_class.find(item);
	}

	static tclass* find_classtl(const tsh& sh,const rstr& name)
	{
		tclass item;
		item.name=name;
		return sh.s_class_tl.find(item);
	}

	//先搜索友元空参，再搜索非友元空参
	static tfunc* find_func_empty_param_m(const tclass& tci,
		const rstr& fname,rbool is_friend)
	{
		//友元只能调用友元
		if(is_friend)
		{
			return find_func_empty_param_f(tci,fname);
		}
		tfunc* ptfi=find_func_empty_param_f(tci,fname);
		if(null==ptfi)
		{
			ptfi=find_func_empty_param_n(tci,fname);
		}
		return ptfi;
	}
	
	//先搜索友元多参，再搜索非友元多参
	static tfunc* find_func_multi_param_m(const tclass& tci,
		const rstr& fname,rbool is_friend)
	{
		if(is_friend)
		{
			return find_func_multi_param_f(tci,fname);
		}
		tfunc* ptfi=find_func_multi_param_f(tci,fname);
		if(null==ptfi)
		{
			ptfi=find_func_multi_param_n(tci,fname);
		}
		return ptfi;
	}
	
	//搜索非友元参数大于等于1个的函数（实际大于等于2）
	static tfunc* find_func_multi_param_n(const tclass& tci,const rstr& fname)
	{
		tfunc* p;
		for_set(p,tci.s_func)
		{
			if(fname==p->name&&
				!p->is_friend&&
				p->param.count()>=2&&
				p->param[0].type==tci.name+rstr("&"))
			{
				return p;
			}
		}
		return null;
	}

	//搜索非友元空参函数（类成员空参函数）
	static tfunc* find_func_empty_param_n(const tclass& tci,const rstr& fname)
	{
		tfunc* p;
		for_set(p,tci.s_func)
		{
			if(fname==p->name&&
				!p->is_friend&&
				p->param.count()==1&&
				p->param[0].type==tci.name+rstr("&"))
			{
				return p;
			}
		}
		return null;
	}

	//搜索友元参数大于等于1个的函数
	static tfunc* find_func_multi_param_f(const tclass& tci,const rstr& fname)
	{
		tfunc* p;
		for_set(p,tci.s_func)
		{
			if(fname==p->name&&
				p->is_friend&&
				p->param.count()>=1)
			{
				return p;
			}
		}
		return null;
	}

	//搜索友元空参函数
	static tfunc* find_func_empty_param_f(const tclass& tci,const rstr& fname)
	{
		tfunc* p;
		for_set(p,tci.s_func)
		{
			if(fname==p->name&&
				p->is_friend&&
				p->param.empty())
			{
				return p;
			}
		}
		return null;
	}

	//类型完全相同
	static tfunc* find_func_same(const tclass& tci,const rstr& fname,
			const rbuf<rstr>& vtype)
	{
		tfunc tfi;
		tfi.name=fname;
		for(int i=0;i<vtype.count();i++)
		{
			tfi.param.push(vtype[i]);
		}
		tfi.name_dec=tfi.get_dec();
		return tci.s_func.find(tfi);
	}

	static tfunc* find_func_dec(const tclass& tci,const rstr& func_dec)
	{
		tfunc tfi;
		tfi.name_dec=func_dec;
		return tci.s_func.find(tfi);
	}
	
	//根据函数名搜索函数
	static tfunc* find_func(const tclass& tci,const rstr& fname)
	{
		tfunc* p;
		for_set(p,tci.s_func)
		{
			if(fname==p->name)
			{
				return p;
			}
		}
		return null;
	}

	static tfunc* find_func_infer(const tclass& tci,const rstr& fname)
	{
		tfunc* p;
		for_set(p,tci.s_func_infer)
		{
			if(fname==p->name)
			{
				return p;
			}
		}
		return null;
	}

	static tfunc* find_func(const tclass& tci,const rstr& fname,
		const rstr& a,const rstr& b)
	{
		tfunc item;
		item.name=fname;
		item.param.push(a);
		item.param.push(b);
		item.name_dec=item.get_dec();
		return tci.s_func.find(item);
	}

	static tfunc* find_func(const tclass& tci,const rstr& fname,const rstr& a)
	{
		tfunc item;
		item.name=fname;
		item.param.push(a);
		item.name_dec=item.get_dec();
		return tci.s_func.find(item);
	}

	static tfunc* find_ftl(const tclass& tci,const rstr& name)
	{
		tfunc item;
		item.name=name;
		item.name_dec=item.get_dec();
		return tci.s_func_tl.find(item);
	}

	static tfunc* find_destruct(const tclass& tci)
	{
		return find_func(tci,"~"+tci.name,tci.name+"&");
	}

	static rbool is_destruct(const tsh& sh,const tfunc& tfi)
	{
		tclass& tci=*tfi.ptci;
		return (rsoptr(c_destruct)+tci.name==tfi.name&&
				tfi.param.count()==1&&
				tfi.param[0].type==tci.name+rsoptr(c_addr)&&
				tfi.retval.type==rskey(c_void));
	}

	static tfunc* find_empty_struct(const tclass& tci)
	{
		return find_func(tci,tci.name,tci.name+"&");
	}

	static rbool is_empty_struct(const tsh& sh,const tfunc& tfi)
	{
		tclass& tci=*tfi.ptci;
		return (tci.name==tfi.name&&
				tfi.param.count()==1&&
				tfi.param[0].type==tci.name+rsoptr(c_addr)&&
				tfi.retval.type==rskey(c_void));
	}

	static tfunc* find_copy_struct(const tclass& tci)
	{
		return find_func(tci,tci.name,tci.name+"&",tci.name+"&");
	}

	static tdata* find_data_member(const tclass& tci,const rstr& name)
	{
		for(int i=0;i<tci.vdata.count();i++)
		{
			if(name==tci.vdata[i].name)
			{
				return &tci.vdata[i];
			}
		}
		return null;
	}

	static tdata* find_local(const tfunc& tfi,const rstr& name)
	{
		for(int i=0;i<tfi.local.count();++i)
		{
			if(name==tfi.local[i].name)
			{
				return &tfi.local[i];
			}
		}
		for(int i=0;i<tfi.param.count();++i)
		{
			if(name==tfi.param[i].name)
			{
				return &tfi.param[i];
			}
		}
		if(tfi.retval.name==name)
		{
			return (tdata*)(&tfi.retval);
		}
		return null;
	}
};
