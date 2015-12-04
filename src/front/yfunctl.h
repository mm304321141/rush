#pragma once

#include "ymemb.h"
#include "yclasstl.h"

//函数模板处理
struct yfunctl
{
	static rbool replace_ftl(tsh& sh,tclass& tci,rbuf<tword>& v,tclass* ptci)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v[i].val!=rsoptr(c_tbk_l))
			{
				continue;
			}
			int left=i;
			rstr name=v.get(left-1).val;
			tclass* cur_ptci=ptci;
			if(cur_ptci==null)
			{
				if(v.get(left-2).val==rsoptr(c_dot))
				{
					cur_ptci=yfind::find_class(sh,v.get(left-3).val);
					if(cur_ptci==null)
					{
						continue;
					}	
				}
				else
				{
					cur_ptci=&tci;
				}
			}
			tfunc* ptfi=yfind::find_ftl(*cur_ptci,name);
			if(ptfi==null)
			{
				cur_ptci=sh.pmain;
				ptfi=yfind::find_ftl(*cur_ptci,name);
			}
			if(null==ptfi)
			{
				continue;
			}
			int right=ybase::find_symm_tbk(sh,v,left);
			if(right>=v.count())
			{
				rserror(v[left-1],"func template miss >");
				return false;
			}
			rbuf<rbuf<tword> > vparam=ybase::split_comma<tword>(
				sh,v.sub(left+1,right));
			if(vparam.count()!=ptfi->vtl.count())
			{
				rserror(v[left-1],"func template not match");
				return false;
			}
			for(int j=left;j<=right;j++)
			{
				v[left-1].val+=v[j].val;
				v[j].val.clear();
			}
			//模板函数不可能重载，因此只要判断一下函数名是否相同即可
			if(yfind::find_func(*cur_ptci,v[left-1].val)!=null)
			{
				continue;
			}
			tfunc item=*ptfi;
			item.vtl.free();
			yclasstl::replace_vtl(item.vword,ptfi->vtl,vparam);
			ifn(replace_type(sh,item.vword))
			{
				return false;
			}
			ifn(combine_ftl_name(sh,item.vword,ptfi->name))
			{
				return false;
			}
			ifn(ymemb::add_func(sh,*cur_ptci,item.vword,false))
			{
				return false;
			}
			i=right;
		}
		ybase::arrange(v);
		return true;
	}
	
	//处理函数模板中又出现新类型的情况
	static rbool replace_type(tsh& sh,rbuf<tword>& v)
	{
		int cur=sh.s_class.count();
		ifn(yclasstl::replace_type(sh,v))
		{
			return false;
		}
		if(sh.s_class.count()>cur)
		{
			tclass* p;
			for_set(p,sh.s_class)
			{
				ifn(proc_class(sh,*p))
				{
					return false;
				}
			}
		}
		return true;
	}
	
	static rbool proc_class(tsh& sh,tclass& tci)
	{
		ifn(ymemb::parse_class(sh,tci))
		{
			return false;
		}
		ifn(ymemb::obtain_size_recursion(sh,tci))
		{
			return false;
		}
		ymemb::obtain_size_func_all(sh,tci);
		return true;
	}

	static rbool combine_ftl_name(const tsh& sh,rbuf<tword>& v,const rstr& name)
	{
		for(int i=1;i<v.count();i++)
		{
			if(v[i]!=rsoptr(c_tbk_l))
			{
				continue;
			}
			if(name!=v[i-1].val)
			{
				continue;
			}
			int left=i;
			int right=ybase::find_symm_tbk(sh,v,left);
			if(right>=v.count())
			{
				rserror(v[left-1],"miss >");
				return false;
			}
			for(int j=left;j<=right;j++)
			{
				v[left-1].val+=v[j].val;
				v[j].val.clear();
			}
			i=right;
		}
		ybase::arrange(v);
		return true;
	}
};
