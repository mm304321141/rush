#pragma once

#include "yfind.h"
#include "yconsteval.h"

//进行类模板替换，增加了一些新类
struct yclasstl
{
	static rbool proc(tsh& sh)
	{
		rbuf<tclass*> vtmp;
		tclass* p;
		for_set(p,sh.s_class)
		{
			vtmp.push(p);
		}
		for(int i=0;i<vtmp.count();i++)
		{
			ifn(replace_type(sh,vtmp[i]->vword,0))
			{
				return false;
			}
		}
		vtmp.clear();
		for_set(p,sh.s_class)
		{
			vtmp.push(p);
		}
		for(int i=0;i<vtmp.count();i++)
		{
			ifn(add_point_declare(sh,*vtmp[i]))
			{
				return false;
			}
		}
		return true;
	}

	static rbool add_point_declare(tsh& sh,tclass& tci)
	{
		if(ybase::is_basic_type(sh,tci.name)||tci.is_friend||ybase::is_point(tci.name))
		{
			return true;
		}
		rbuf<tword> temp;
		temp+=rskey(c_rp);
		temp+=rsoptr(c_tbk_l);
		temp+=tci.name;
		temp+=rsoptr(c_tbk_r);
		ifn(replace_type(sh,temp))
		{
			return false;
		}
		return true;
	}

	static rbool replace_type(tsh& sh,rbuf<tword>& v,int level=0)
	{
		if(level>c_rs_deep)
		{
			rserror("type level overflow");
			return false;
		}
		level++;
		rbool need=replace_point(sh,v);
		for(int i=1;i<v.count();i++)
		{
			if(v[i]!=rsoptr(c_tbk_l))
			{
				continue;
			}
			tclass* pctl=yfind::find_classtl(sh,v[i-1].val);
			if(pctl==null)
			{
				continue;
			}
			int left=i;
			int right=ybase::find_symm_tbk(sh,v,left);
			if(right>=v.count())
			{
				rserror(v[i-1],"miss >");
				return false;
			}
			rbuf<rbuf<tword> > vparam=ybase::split_comma<tword>(
				sh,v.sub(left+1,right));
			if(vparam.count()!=pctl->vtl.count())
			{
				rserror(v[i-1],"template not match");
				return false;
			}
			ifn(is_type(sh,vparam))
			{
				continue;
			}
			for(int j=left;j<=right;j++)
			{
				v[left-1].val+=v[j].val;
				v[j].val.clear();
			}
			i=right;
			if(yfind::is_class(sh,v[left-1].val))
			{
				continue;
			}
			tclass item=*pctl;
			item.name=v[left-1].val;
			item.vtl.clear();
			replace_vtl(item.vword,pctl->vtl,vparam);
			sh.s_class.insert(item);
			tclass* ptci=yfind::find_class(sh,item.name);
			if(ptci==null)
			{
				rserror(v.get(i),"can't find class "+item.name);
				return false;
			}
			ifn(replace_type(sh,ptci->vword,level))
			{
				return false;
			}
		}
		ybase::arrange(v);
		need=replace_point(sh,v);
		if(need)
		{
			ifn(replace_type(sh,v,level))
			{
				return false;
			}
		}
		combine_quote(sh,v);
		ifn(yconsteval::optimize_const_exp(sh,v,false))
		{
			return false;
		}
		return true;
	}
	
	//类模板只能替换类型，否则与函数模板冲突
	static rbool is_type(const tsh& sh,const rbuf<rbuf<tword> >& vparam)
	{
		for(int i=0;i<vparam.count();i++)
		{
			ifn(is_type(sh,vparam[i]))
			{
				return false;
			}
		}
		return true;
	}

	static rbool is_type(const tsh& sh,const rbuf<tword>& v)
	{
		if(v.count()==1)
		{
			return yfind::is_class_t(sh,v[0].val);
		}
		if(v.count()<4)
		{
			return false;
		}
		if(v[1]!=rsoptr(c_tbk_l))
		{
			return false;
		}
		ifn(yfind::is_classtl(sh,v[0].val))
		{
			return false;
		}
		return is_type(sh,v.sub(2,v.count()-1));
		//v.count()==1&&v[0].val.is_number();//todo:
	}

	static void replace_vtl(rbuf<tword>& result,rbuf<ttl>& vtl,
		rbuf<rbuf<tword> >& vparam)
	{
		for(int j=0;j<vparam.count();j++)
		{
			replace_str(result,vtl[j].name,ybase::trans_vword_to_vstr(vparam[j]));
		}
		ybase::arrange(result);
	}

	static void replace_str(rbuf<tword>& v,const rstr& src,const rbuf<rstr>& vstr)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v[i].val==src)
			{
				v[i].val.clear();
				v[i].multi=vstr;
			}
		}
	}

	//返回是否需要重排
	static rbool replace_point(const tsh& sh,rbuf<tword>& v)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v.get(i+1).val!=rsoptr(c_mul)||
				!yfind::is_class(sh,v[i].val))
			{
				continue;
			}
			if(v.get(i+2).val==rsoptr(c_mul))
			{
				if(v.get(i+3)==rsoptr(c_sbk_l))
				{
					continue;
				}
				rbuf<tword> vword;
				tword word;
				word.pos=v[i].pos;
				ybase::push_twi(vword,word,rskey(c_rp));
				ybase::push_twi(vword,word,rsoptr(c_tbk_l));
				ybase::push_twi(vword,word,rskey(c_rp));
				ybase::push_twi(vword,word,rsoptr(c_tbk_l));
				ybase::push_twi(vword,word,v[i].val);
				ybase::push_twi(vword,word,rsoptr(c_tbk_r));
				ybase::push_twi(vword,word,rsoptr(c_tbk_r));
				v[i].multi=ybase::trans_vword_to_vstr(vword);
				v[i].val.clear();
				v[i+1].val.clear();
				v[i+2].val.clear();
			}
			else
			{
				if(v.get(i+2)==rsoptr(c_sbk_l))
				{
					continue;
				}
				rbuf<tword> vword;
				tword word;
				word.pos=v[i].pos;
				ybase::push_twi(vword,word,rskey(c_rp));
				ybase::push_twi(vword,word,rsoptr(c_tbk_l));
				ybase::push_twi(vword,word,v[i].val);
				ybase::push_twi(vword,word,rsoptr(c_tbk_r));

				v[i].multi=ybase::trans_vword_to_vstr(vword);
				v[i].val.clear();
				v[i+1].val.clear();
			}
		}
		return ybase::arrange(v);
	}

	static void combine_quote(const tsh& sh,rbuf<tword>& v)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v.get(i+1).val==rsoptr(c_addr)&&
				yfind::is_class(sh,v[i].val))
			{
				for(int j=i+1;j<v.count();j++)
				{
					if(v[j].val!=rsoptr(c_addr))
					{
						break;
					}
					v[i].val+=v[j].val;
					v[j].clear();
				}
			}
			elif((v[i].val==rsoptr(c_dot)||v[i]==rsoptr(c_arrow_r))&&
				v.get(i+1).val==rsoptr(c_destruct)&&
				yfind::is_class(sh,v.get(i+2).val))
			{
				v[i+1].val+=v[i+2].val;
				v[i+2].clear();
			}
		}
		ybase::arrange(v);
	}
};
