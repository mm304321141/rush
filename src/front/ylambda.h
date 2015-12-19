#pragma once

#include "yfind.h"
#include "ymemb.h"

struct ylambda
{
	//闭包处理，todo:闭包暂时不能使用类型推断,不是线程安全
	static void pack(tsh& sh,const rbuf<tword>& vhead,
		const rbuf<tword>& vbody,const tfunc& tfi,tfunc& lambda)
	{
		tclass item;
		for(int i=0;i<vbody.count();i++)
		{
			ifn(vbody[i].is_name())
			{
				continue;
			}
			if(yfind::is_class_t(sh,vbody[i].val))
			{
				continue;
			}
			//todo 未判断重复
			tdata* ptdi=yfind::find_local(tfi,vbody[i].val);
			tdata tdi;
			if(null==ptdi)
			{
				int pos=vhead.find(vbody[i]);
				if(pos>=vhead.count()||pos==0)
				{
					continue;
				}
				ifn(yfind::is_class_t(sh,vhead[pos-1].val))
				{
					continue;
				}
				ptdi=&tdi;
				ptdi->type=vhead[pos-1].val;
			}
			else
			{
				tdi.type=ptdi->type;
				ptdi=&tdi;
			}
			ptdi->name=vbody[i].val;
			ptdi->size=yfind::get_type_size(sh,ptdi->type);
			if(ptdi->size==0)
			{
				continue;
			}
			item.vdata.push(*ptdi);
		}
		if(item.vdata.empty())
		{
			return;
		}
		item.is_friend=true;
		item.name=lambda.name+"_C";
		for(int i=0;i<item.vdata.count();i++)
		{
			item.vdata[i].off=item.size;
			item.size+=item.vdata[i].size;
		}
		lambda.lambda_data.set_size(item.size);
		sh.s_class.insert_c(item);
	}

	static rbool replace(tsh& sh,tfunc& tfi)
	{
		tclass& tci=*tfi.ptci;
		rbuf<tword>& v=tfi.vword;
		for(int i=0;i<v.count();i++)
		{
			if(v[i]!=rskey(c_lambda))
			{
				continue;
			}
			if(v.get(i+1)!=rsoptr(c_sbk_l))
			{
				continue;
			}
			int right=ybase::find_symm_sbk(sh,v,i+1);
			if(right>=v.count())
			{
				continue;
			}
			int left=right+1;
			if(v.get(left)!=rsoptr(c_bbk_l))
			{
				continue;
			}
			right=ybase::find_symm_bbk(sh,v,left);
			if(right>=v.count())
			{
				continue;
			}
			v[i].val=rstr("_LAMBDA")+tci.s_func.count();
			ifn(ymemb::add_func(sh,tci,v.sub(i,right+1),false))
			{
				return false;
			}
			tfunc* ptfi=yfind::find_func(tci,v[i].val);
			if(ptfi!=null)
			{
				pack(sh,v.sub(0,i),
					v.sub(left+1,right),tfi,*ptfi);
			}
			v[i].multi.push("(");
			v[i].multi.push("&");
			v[i].multi.push(tci.name);
			v[i].multi.push(".");
			v[i].multi.push(v[i].val);
			v[i].multi.push(")");
			ybase::clear_word_val(v,i,right+1);
			i=right;
		}
		ybase::arrange(v);
		return true;
	}

	static rstr get(const rbuf<tword>& v)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v[i].val.sub(0,7)=="_LAMBDA")
			{
				return v[i].val;
			}
		}
		return rstr();
	}

	static void push_type(const rstr& name,tfunc& tfi)
	{
		tdata item;
		item.name=name+"_C_P";
		item.type=name+"_C&";
		item.size=4;
		tfi.local.push(item);
	}

	static void proc(const tsh& sh,tfunc& tfi)
	{
		tclass& tci=*tfi.ptci;
		rbuf<tsent>& v=tfi.vsent;
		for(int i=0;i<v.count();i++)
		{
			rstr name=get(v[i].vword);
			if(name.empty())
			{
				continue;
			}
			tfunc* ptfi=yfind::find_func(tci,name);
			if(ptfi==null||ptfi->lambda_data.empty())
			{
				continue;
			}
			tclass* ptci=yfind::find_class(sh,name+"_C");
			if(ptci==null||ptci->vdata.empty())
			{
				continue;
			}
			rstr data_name=name+"_C_P";
			push_type(name,tfi);
			tword word;
			word.pos=v[i].vword.get_bottom().pos;
			rbuf<tword> vtemp;
			ybase::push_twi(vtemp,word,"mov");
			ybase::push_twi(vtemp,word,data_name);
			ybase::push_twi(vtemp,word,",");
			if(sh.mode==tsh::c_vm||sh.mode==tsh::c_jit)
			{
				ybase::push_twi(vtemp,word,
					rstr((uint)(ptfi->lambda_data.begin())));
			}
			else
			{
				ybase::push_twi(vtemp,word,ptfi->name+"_C_P_A");
			}
			ybase::push_twi(vtemp,word,";");
			for(int j=0;j<ptci->vdata.count();j++)
			{
				ybase::push_twi(vtemp,word,data_name);
				ybase::push_twi(vtemp,word,".");
				ybase::push_twi(vtemp,word,ptci->vdata[j].name);
				ybase::push_twi(vtemp,word,"=");
				ybase::push_twi(vtemp,word,ptci->vdata[j].name);
				ybase::push_twi(vtemp,word,";");
			}
			vtemp+=v[i].vword;
			v[i].vword=r_move(vtemp);
		}
		ybase::part_vsent(v);
	}

	static void add_init_asm(const tsh& sh,tfunc& tfi)
	{
		if(tfi.lambda_data.empty())
		{
			return;
		}
		tsent sent;
		sent.pos=tfi.pos;
		sent.vword.push(tword(rstr("mov")));//tword没有设置pos
		sent.vword.push(tword(tfi.name+"_C_P"));
		sent.vword.push(tword(rstr(",")));
		if(sh.mode==tsh::c_vm||sh.mode==tsh::c_jit)
		{
			sent.vword.push(tword(rstr((uint)(tfi.lambda_data.begin()))));
		}
		else
		{
			sent.vword.push(tword(tfi.name+"_C_P_A"));
		}
		tfi.vsent.push_front(r_move(sent));
	}

	static void replace_var(const tsh& sh,tfunc& tfi)
	{
		if(tfi.lambda_data.empty())
		{
			return;
		}
		tclass* ptci=yfind::find_class(sh,tfi.name+"_C");
		if(ptci==null)
		{
			return;
		}
		push_type(tfi.name,tfi);
		rstr data_name=tfi.name+"_C_P";
		for(int i=0;i<tfi.vword.count();i++)
		{
			if(yfind::find_data_member(*ptci,tfi.vword[i].val)!=null)
			{
				tfi.vword[i].multi.push(data_name);
				tfi.vword[i].multi.push(".");
				tfi.vword[i].multi.push(tfi.vword[i].val);
				tfi.vword[i].val.clear();
			}
		}
		ybase::arrange(tfi.vword);
	}
};
