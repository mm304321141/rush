#pragma once

#include "yfind.h"

//自动增加类成员函数
struct yautof
{
	static rbool add_func(const tsh& sh,tclass& tci)
	{
		ifn(tconf::c_auto_addfunc)
		{
			return true;
		}
		if(ybase::is_basic_type(sh,tci.name)||
			tci.is_friend)
		{
			return true;
		}
		tpos pos;
		pos.line=1;
		pos.file=null;
		ifn(tci.s_func.empty())
		{
			pos=tci.s_func.begin()->pos;
		}
		if(yfind::find_destruct(tci)==null)
		{
			tfunc tfi;
			tfi.ptci=&tci;
			tfi.pos=pos;
			tfi.name=rsoptr(c_destruct)+tci.name;

			tfi.param.push(tdata(tci.name+rsoptr(c_addr),rskey(c_this)));
			tfi.retval=tdata(rskey(c_void),rstr());

			tfi.name_dec=tfi.get_dec();
			tci.s_func.insert(tfi);
		}
		if(yfind::find_empty_struct(tci)==null)
		{
			tfunc tfi;
			tfi.ptci=&tci;
			tfi.pos=pos;

			tfi.name=tci.name;

			tfi.param.push(tdata(tci.name+rsoptr(c_addr),rskey(c_this)));
			tfi.retval=tdata(rskey(c_void),rstr());

			tfi.name_dec=tfi.get_dec();
			tci.s_func.insert(tfi);
		}
		if(yfind::find_copy_struct(tci)==null)
		{
			tfunc tfi;
			tfi.ptci=&tci;
			tfi.pos=pos;

			tfi.name=tci.name;

			tfi.param.push(tdata(tci.name+rsoptr(c_addr),rskey(c_this)));
			tfi.param.push(tdata(tci.name+rsoptr(c_addr),rskey(c_rvara)));
			tfi.retval=tdata(rskey(c_void),rstr());

			add_copy_sent(sh,tfi);
			tfi.name_dec=tfi.get_dec();
			tci.s_func.insert(tfi);
		}
		add_copystruct_func(tci);
		if(yfind::find_func(tci,rsoptr(c_equal),tci.name+rsoptr(c_addr),
			tci.name+rsoptr(c_addr))==null)
		{
			tfunc tfi;
			tfi.ptci=&tci;
			tfi.pos=pos;

			tfi.name=rsoptr(c_equal);

			tfi.param.push(tdata(tci.name+rsoptr(c_addr),rskey(c_this)));
			tfi.param.push(tdata(tci.name+rsoptr(c_addr),rskey(c_rvara)));
			tfi.retval=tdata(rskey(c_void),rstr());

			add_copy_sent(sh,tfi);
			tfi.name_dec=tfi.get_dec();
			tci.s_func.insert(tfi);
		}
		if(yfind::find_func(tci,rsoptr(c_equal),
			tci.name+rsoptr(c_addr),tci.name)==null)
		{
			tfunc tfi=*yfind::find_func(
				tci,rsoptr(c_equal),tci.name+rsoptr(c_addr),
				tci.name+rsoptr(c_addr));
			tfi.ptci=&tci;
			tfi.param[1].type=tci.name;
			tfi.name_dec=tfi.get_dec();
			tci.s_func.insert(tfi);
		}
		if(yfind::find_func(tci,rsoptr(c_addr))==null)
		{
			tfunc tfi;
			tfi.ptci=&tci;
			tfi.pos=pos;

			tfi.name=rsoptr(c_addr);

			tfi.param.push(tdata(tci.name+rsoptr(c_addr),rskey(c_this)));
			tfi.retval=tdata("rp<"+tci.name+">",rskey(c_s_ret));

			tword twi;
			twi.pos=tfi.pos;
			if(sh.mode==tsh::c_gpp)
			{
				//todo:
				tfi.is_final=true;
				ybase::push_twi(tfi.vword,twi,"return");
				ybase::push_twi(tfi.vword,twi,"&");
				ybase::push_twi(tfi.vword,twi,rskey(c_this));
				ybase::push_twi(tfi.vword,twi,";");
			}
			else
			{
				ybase::push_twi(tfi.vword,twi,rskey(c_mov));
				ybase::push_twi(tfi.vword,twi,rskey(c_s_ret));
				ybase::push_twi(tfi.vword,twi,rsoptr(c_comma));
				ybase::push_twi(tfi.vword,twi,rskey(c_this));
			}
			tfi.name_dec=tfi.get_dec();
			if(yfind::is_class(sh,tfi.retval.type))
			{
				tci.s_func.insert(tfi);
			}
		}
		return true;
	}

	static void add_copystruct_func(tclass& tci)
	{
		rbuf<tfunc> temp;
		tfunc* p;
		for_set(p,tci.s_func)
		{
			if(tci.name!=p->name)
			{
				continue;
			}
			if(p->param.count()<2)
			{
				continue;
			}
			int i;
			for(i=0;i<p->param.count();i++)
			{
				ifn(ybase::is_quote(p->param[i].type))
				{
					break;
				}
			}
			if(i<p->param.count())
			{
				continue;
			}
			tfunc* q;
			for_set(q,tci.s_func)
			{
				if(q->name!=p->name||q->param.count()!=p->param.count())
				{
					continue;
				}
				ifn(ybase::is_quote(q->param.get(0).type))
				{
					continue;
				}
				int j;
				for(j=1;j<q->param.count();j++)
				{
					if(ybase::is_quote(q->param[j].type))
					{
						break;
					}
					if(ybase::get_tname(p->param[j].type)!=q->param[j].type)
					{
						break;
					}
				}
				if(j>=q->param.count())
				{
					break;
				}
			}
			if(q!=tci.s_func.end())
			{
				continue;
			}
			tfunc tfi=*p;
			tfi.ptci=&tci;
			for(int i=1;i<tfi.param.count();i++)
			{
				tfi.param[i].type=ybase::get_tname(tfi.param[i].type);
			}
			tfi.name_dec=tfi.get_dec();
			temp.push(tfi);
		}
		for(int k=0;k<temp.count();k++)
		{
			tci.s_func.insert(temp[k]);
		}
	}

	static void add_copy_sent(const tsh& sh,tfunc& tfi)
	{
		tclass& tci=*tfi.ptci;
		tword twi;
		twi.pos=tfi.pos;
		for(int i=0;i<tci.vdata.count();i++)
		{
			if(ybase::is_basic_type(sh,tci.vdata[i].type)||
				ybase::is_quote(tci.vdata[i].type))
			{
				continue;
			}
			ybase::push_twi(tfi.vword,twi,rskey(c_this));
			ybase::push_twi(tfi.vword,twi,rsoptr(c_dot));
			ybase::push_twi(tfi.vword,twi,tci.vdata[i].name);
			ybase::push_twi(tfi.vword,twi,rsoptr(c_equal));
			ybase::push_twi(tfi.vword,twi,rskey(c_rvara));
			ybase::push_twi(tfi.vword,twi,rsoptr(c_dot));
			ybase::push_twi(tfi.vword,twi,tci.vdata[i].name);
			ybase::push_twi(tfi.vword,twi,rsoptr(c_semi));
		}
	}
};
