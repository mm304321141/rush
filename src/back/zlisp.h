#pragma once

#include "../front/yjs.h"

struct var;

struct tobj
{
	int type;
	int count;
	int64 v_long;
	rstr v_str;
	rdic<var> v_table;
};

extern tsh*& r_zjitf_get_psh();

struct var
{
	tobj* obj;

	enum
	{
		c_void,
		c_long,
		c_double,
		c_func,
		c_point,
		c_table,
		c_str,
	};

	~var()
	{
	}

	var()
	{
		obj=null;
	}

	var(int64 a)
	{
		obj=r_new<tobj>();
		r_zjitf_get_psh()->s_obj.insert(obj);
		obj->type=c_long;
		obj->count=0;
		ref_long()=a;
	}

	var(double a)
	{
		obj=r_new<tobj>();
		r_zjitf_get_psh()->s_obj.insert(obj);
		obj->type=c_double;
		obj->count=0;
		ref_double()=a;
	}

	var(const rstr& a)
	{
		obj=r_new<tobj>();
		r_zjitf_get_psh()->s_obj.insert(obj);
		obj->type=c_str;
		obj->count=0;
		ref_str()=a;
	}

	var(const rdic<var>& a)
	{
		obj=r_new<tobj>();
		r_zjitf_get_psh()->s_obj.insert(obj);
		obj->type=c_table;
		obj->count=0;
		ref_table()=a;
	}

	var(const rstr& a,const rdic<var>& b)
	{
		obj=r_new<tobj>();
		r_zjitf_get_psh()->s_obj.insert(obj);
		obj->type=c_func;
		obj->count=0;
		ref_str()=a;
		ref_table()=b;
	}

	var(const var& a)
	{
		obj=a.obj;
	}

	void operator=(const var& a)
	{
		obj=a.obj;
	}

	int get_type() const
	{
		ifn(empty())
		{
			return obj->type;
		}
		return c_void;
	}

	rbool empty() const
	{
		return obj==null;
	}

	int64& ref_long() const
	{
		return obj->v_long;
	}

	double& ref_double() const
	{
		return *(double*)&obj->v_long;
	}

	rstr& ref_str() const
	{
		return obj->v_str;
	}

	rdic<var>& ref_table() const
	{
		return obj->v_table;
	}

	static var add(var a,var b)
	{
		if(a.get_type()==c_long)
		{
			return var(a.ref_long()+b.ref_long());
		}
		if(a.get_type()==c_double)
		{
			return var(a.ref_double()+b.ref_double());
		}
		return var(a.ref_str()+b.ref_str());
	}

	static var sub(var a,var b)
	{
		if(a.get_type()==c_double)
		{
			return var(a.ref_double()-b.ref_double());
		}
		return var(a.ref_long()-b.ref_long());
	}

	static var mul(var a,var b)
	{
		if(a.get_type()==c_double)
		{
			return var(a.ref_double()*b.ref_double());
		}
		return var(a.ref_long()*b.ref_long());
	}

	static var div(var a,var b)
	{
		if(a.get_type()==c_double)
		{
			return var(a.ref_double()/b.ref_double());
		}
		return var(a.ref_long()/b.ref_long());
	}

	static var mod(var a,var b)
	{
		return var(a.ref_long()%b.ref_long());
	}

	static var equal_equal(var a,var b)
	{
		if(a.get_type()==c_long)
		{
			return var(int64(a.ref_long()==b.ref_long()));
		}
		if(a.get_type()==c_double)
		{
			return var(int64(a.ref_double()==b.ref_double()));
		}
		return var(int64(a.ref_str()==b.ref_str()));
	}

	static var not_equal(var a,var b)
	{
		if(a.get_type()==c_long)
		{
			return var(int64(a.ref_long()!=b.ref_long()));
		}
		if(a.get_type()==c_double)
		{
			return var(int64(a.ref_double()!=b.ref_double()));
		}
		return var(int64(a.ref_str()!=b.ref_str()));
	}

	static var less_equal(var a,var b)
	{
		if(a.get_type()==c_long)
		{
			return var(int64(a.ref_long()<=b.ref_long()));
		}
		if(a.get_type()==c_double)
		{
			return var(int64(a.ref_double()<=b.ref_double()));
		}
		return var(int64(a.ref_str()<=b.ref_str()));
	}

	static var great_equal(var a,var b)
	{
		if(a.get_type()==c_long)
		{
			return var(int64(a.ref_long()>=b.ref_long()));
		}
		if(a.get_type()==c_double)
		{
			return var(int64(a.ref_double()>=b.ref_double()));
		}
		return var(int64(a.ref_str()>=b.ref_str()));
	}

	static var less(var a,var b)
	{
		if(a.get_type()==c_long)
		{
			return var(int64(a.ref_long()<b.ref_long()));
		}
		if(a.get_type()==c_double)
		{
			return var(int64(a.ref_double()<b.ref_double()));
		}
		return var(int64(a.ref_str()<b.ref_str()));
	}

	static var great(var a,var b)
	{
		if(a.get_type()==c_long)
		{
			return var(int64(a.ref_long()>b.ref_long()));
		}
		if(a.get_type()==c_double)
		{
			return var(int64(a.ref_double()>b.ref_double()));
		}
		return var(int64(a.ref_str()>b.ref_str()));
	}

	static var neg(var a)
	{
		if(a.get_type()==c_long)
		{
			return var(-a.ref_long());
		}
		return var(-a.ref_double());
	}
};

struct fstate
{
	var env;
	rbuf<rbuf<tword> > v;
	int pc;
	rdic<int> dic_tag;

	var& ref_result()
	{
		return env.ref_table()["_result"];
	}
};

fstate g_state;
rdic<int> g_op;

struct zlisp
{
	enum
	{
		c_add,
		c_sub,
		c_mul,
		c_div,
		c_mod,

		c_equal_equal,
		c_not_equal,
		c_less_equal,
		c_great_equal,
		c_less,
		c_great,

		c_and,
		c_or,
		c_not,

		c_neg,
		c_colon,
		c_jebxz,
		c_jebxnz,
		c_jmp,
		c_putsl,
		c_equal,
		c_function,
		c_mbk_l,
		c_json,
		c_dot,
		c_stringify,
		c_code_to_data,
		c_eval,
		c_cond,
		c_call_cps,
		c_string,
		c_long,
		c_double,
		c_typeof,
		c_length,
		c_string_sub,
	};

	static void init_op()
	{
		g_op["+"]=c_add;
		g_op["-"]=c_sub;
		g_op["*"]=c_mul;
		g_op["/"]=c_div;
		g_op["%"]=c_mod;

		g_op["=="]=c_equal_equal;
		g_op["!="]=c_not_equal;
		g_op["<="]=c_less_equal;
		g_op[">="]=c_great_equal;
		g_op["<"]=c_less;
		g_op[">"]=c_great;

		g_op["&&"]=c_and;
		g_op["||"]=c_or;
		g_op["!"]=c_not;

		g_op["neg"]=c_neg;
		g_op[":"]=c_colon;
		g_op["jebxz"]=c_jebxz;
		g_op["jebxnz"]=c_jebxnz;
		g_op["jmp"]=c_jmp;
		g_op["putsl"]=c_putsl;
		g_op["="]=c_equal;
		g_op["function"]=c_function;
		g_op["["]=c_mbk_l;
		g_op["_JSON"]=c_json;
		g_op["."]=c_dot;
		g_op["stringify"]=c_stringify;
		g_op["code_to_data"]=c_code_to_data;
		g_op["eval"]=c_eval;
		g_op["cond"]=c_cond;
		g_op["call_cps"]=c_call_cps;
		g_op["rstr"]=c_string;
		g_op["int64"]=c_long;
		g_op["double"]=c_double;
		g_op["typeof"]=c_typeof;
		g_op["length"]=c_length;
		g_op["string_sub"]=c_string_sub;
	}

	static void mark(var& env)
	{
		ifn(env.empty())
		{
			if(env.obj->count==1)
			{
				return;
			}
			env.obj->count=1;
			var* p;
			for_set(p,env.ref_table())
			{
				mark(*p);
			}
		}
	}

	static void clear()
	{
		rset<tobj*>& s_obj=r_zjitf_get_psh()->s_obj;
		tobj** p=s_obj.begin();
		while(p!=s_obj.end())
		{
			tobj& item=**p;
			if(item.count==1)
			{
				item.count=0;
				p=s_obj.next(p);
			}
			else
			{
				tobj** next=s_obj.next(p);
				r_delete<tobj>(*p);
				s_obj.erase(*p);
				p=next;
			}
		}
	}

	static void gc()
	{
		if(r_zjitf_get_psh()->s_obj.count()<100000)
		{
			return;
		}
		mark(g_state.env);
		clear();
	}

	static void obtain_tag(tsh& sh,fstate& state)
	{
		rbuf<rbuf<tword> >& v=state.v;
		for(int i=0;i<v.count();i++)
		{
			if(v[i].count()==4&&v[i].get(1).val==rsoptr(c_colon))
			{
				state.dic_tag[state.v[i].get(2).val]=i+1;
			}
		}
		state.dic_tag["_func_end"]=v.count();
	}

	static int get_index(const rstr& s)
	{
		int* p_op=g_op.find(s);
		if(p_op!=null)
		{
			return *p_op;
		}
		return -1;
	}

	static var* find_val(const rstr& name,rdic<var>& table)
	{
		if(table.exist(name))
		{
			return &table[name];
		}
		ifn(table["_fenv"].empty())
		{
			return find_val(name,table["_fenv"].ref_table());
		}
		return null;
	}

	static var* find_val(const rstr& name,fstate& state)
	{
		return find_val(name,state.env.ref_table());
	}

	static var* find_proto(const rstr& name,rdic<var>& table)
	{
		if(table.exist(name))
		{
			return &table[name];
		}
		if(table.exist("prototype"))
		{
			return find_proto(name,table["prototype"].ref_table());
		}
		return null;
	}

	static var eval_func(tsh& sh,fstate& state,const rbuf<rbuf<tword> >& param,var fval)
	{
		rbuf<var> v_var;
		for(int i=0;i<param.count();i++)
		{
			v_var.push(eval(sh,state,param[i]));
		}
		return eval_func(sh,state,v_var,fval);
	}

	static var eval_func(tsh& sh,fstate& state,const rbuf<var>& param,var fval)
	{
		fstate cur_state;
		cur_state.env=var(rdic<var>());
		cur_state.env.ref_table()["_upenv"]=state.env;
		state.env.ref_table()["_downenv"]=cur_state.env;
		cur_state.env.ref_table()["_fenv"]=fval.ref_table()["_fenv"];
		rbuf<rbuf<tword> > vexp=ybase::split_sexp(sh,yjs::trans_s_to_v(sh,fval.ref_str()).sub_trim_s(1));

		rbuf<tword> vparam=vexp.get(1).sub_trim_s(1);
		if(fval.ref_table().exist("this"))
		{
			cur_state.env.ref_table()["this"]=fval.ref_table()["this"];
		}
		for(int i=0;i<vparam.count();i++)
		{
			cur_state.env.ref_table()[vparam[i].val]=param[i];
		}

		cur_state.v=vexp.sub(2);
		obtain_tag(sh,cur_state);
		cur_state.pc=0;
		state.ref_result()=eval_multi(sh,cur_state);
		cur_state.env.ref_table().erase_key("_upenv");
		state.env.ref_table().erase_key("_downenv");
		gc();
		return state.ref_result();
	}

	static var eval_str(tsh& sh,fstate& state,const rstr& s)
	{
		rbuf<tword> v=yjs::trans_s_to_v(sh,s);
		ifn(yjs::replace_control(sh,v))
		{
			rserror();
			return var();
		}
		v=yjs::trans_fdef_to_sexp(sh,v);
		return eval(sh,state,v);
	}

	static var eval_multi(tsh& sh,fstate& state)
	{
		while(state.pc<state.v.count())
		{
			rbuf<tword>& cur=state.v[state.pc];
			if(cur.count()==4)
			{
				int index=get_index(cur[1].val);
				switch (index)
				{
				case c_colon:
					state.pc++;
					continue;
				case c_jebxz:
					if(state.ref_result().ref_long()==0)
					{
						state.pc=state.dic_tag[cur[2].val];
						gc();
						continue;
					}
					state.pc++;
					continue;
				case c_jebxnz:
					if(state.ref_result().ref_long()!=0)
					{
						state.pc=state.dic_tag[cur[2].val];
						gc();
						continue;
					}
					state.pc++;
					continue;
				case c_jmp:
					state.pc=state.dic_tag[cur[2].val];
					gc();
					continue;
				}
			}
			state.ref_result()=eval(sh,state,cur);
			state.pc++;
		}
		return state.ref_result();
	}

	static var eval(tsh& sh,fstate& state,const rbuf<tword>& v)
	{
		if(v.count()==0)
		{
			return state.ref_result();
		}
		if(v.count()==1)
		{
			rstr first=v[0].val;
			if(tword::is_cstr_in(first))
			{
				return var(first.sub(1).sub_trim(1));
			}
			elif(first.is_number())
			{
				return var(first.toint64());
			}
			elif(tword::is_cdouble_in(first))
			{
				return var(first.todouble());
			}
			else
			{
				var* pval=find_val(first,state);
				if(pval==null)
				{
					ybase::print_vword(v);
					rserror();
					return var();
				}
				return *pval;
			}
		}
		rbuf<rbuf<tword> > vexp=ybase::split_sexp(sh,v.sub_trim_s(1));
		rstr first=vexp.get(0).get(0).val;
		int index=get_index(first);
		switch(index)
		{
		case c_add:
			return var::add(eval(sh,state,vexp.get(1)),eval(sh,state,vexp.get(2)));
		case c_sub:
			return var::sub(eval(sh,state,vexp.get(1)),eval(sh,state,vexp.get(2)));
		case c_mul:
			return var::mul(eval(sh,state,vexp.get(1)),eval(sh,state,vexp.get(2)));
		case c_div:
			return var::div(eval(sh,state,vexp.get(1)),eval(sh,state,vexp.get(2)));
		case c_mod:
			return var::mod(eval(sh,state,vexp.get(1)),eval(sh,state,vexp.get(2)));

		case c_equal_equal:
			return var::equal_equal(eval(sh,state,vexp.get(1)),eval(sh,state,vexp.get(2)));
		case c_not_equal:
			return var::not_equal(eval(sh,state,vexp.get(1)),eval(sh,state,vexp.get(2)));
		case c_less_equal:
			return var::less_equal(eval(sh,state,vexp.get(1)),eval(sh,state,vexp.get(2)));
		case c_great_equal:
			return var::great_equal(eval(sh,state,vexp.get(1)),eval(sh,state,vexp.get(2)));
		case c_less:
			return var::less(eval(sh,state,vexp.get(1)),eval(sh,state,vexp.get(2)));
		case c_great:
			return var::great(eval(sh,state,vexp.get(1)),eval(sh,state,vexp.get(2)));

		case c_and:
			return var(int64(eval(sh,state,vexp.get(1)).ref_long()!=0&&
				eval(sh,state,vexp.get(2)).ref_long()!=0));
		case c_or:
			return var(int64(eval(sh,state,vexp.get(1)).ref_long()!=0||
				eval(sh,state,vexp.get(2)).ref_long()!=0));
		case c_not:
			return var(int64(eval(sh,state,vexp.get(1)).ref_long()==0));

		case c_neg:
			if(vexp.count()==2)
			{
				return var::neg(eval(sh,state,vexp.get(1)));
			}
			return var::sub(eval(sh,state,vexp.get(1)),eval(sh,state,vexp.get(2)));
		case c_putsl:
			state.ref_result()=eval(sh,state,vexp.get(1));
			if(state.ref_result().get_type()==var::c_long)
			{
				rf::printl(state.ref_result().ref_long());
			}
			elif(state.ref_result().get_type()==var::c_double)
			{
				rf::printl(state.ref_result().ref_double());
			}
			else
			{
				rf::printl(state.ref_result().ref_str());
			}
			return state.ref_result();
		case c_equal:
			if(vexp.count()!=3)
			{
				rserror();
				return var();
			}
			if(vexp[1].count()>1)
			{
				var val=eval(sh,state,vexp[2]);
				var key=eval(sh,state,vexp[1]);
				if(key.obj==null)
				{
					rserror();
					return var();
				}
				*key.obj=*val.obj;
				return val;
			}
			else
			{
				state.ref_result()=eval(sh,state,vexp[2]);
				rstr name=vexp[1].get(0).val;
				var* pval=find_val(name,state);
				if(pval==null)
				{
					state.env.ref_table()[name]=state.ref_result();
				}
				else
				{
					*pval=state.ref_result();
				}
				return state.ref_result();
			}
		case c_function:
			{
				rdic<var> table;
				table["_fenv"]=state.env;
				return var(ybase::trans_v_to_s(v),table);
			}
		case c_mbk_l:
			state.ref_result()=eval(sh,state,vexp.get(0));
			if(state.ref_result().get_type()!=var::c_func)
			{
				return var();
			}
			return eval_func(sh,state,vexp.sub(1),state.ref_result());
		case c_json:
			{
				rdic<var> table;
				for(int i=1;i<vexp.count();i+=2)
				{
					var key=eval(sh,state,vexp[i]);
					var val=eval(sh,state,vexp.get(i+1));
					if(key.get_type()==var::c_long)
					{
						table[rstr(key.ref_long())]=val;
					}
					else
					{
						table[key.ref_str()]=val;
					}
				}
				return var(table);
			}
		case c_dot:
			{
				var obj=eval(sh,state,vexp.get(1));
				var name=eval(sh,state,vexp.get(2));
				rstr name_s;
				if(name.get_type()==var::c_long)
				{
					name_s=rstr(name.ref_long());
				}
				else
				{
					name_s=name.ref_str();
				}
				var* pval=find_proto(name_s,obj.ref_table());
				if(pval==null)
				{
					obj.ref_table()[name_s]=var((int64)0);
					pval=find_proto(name_s,obj.ref_table());
				}
				if(pval->get_type()==var::c_func)
				{
					pval->ref_table()["this"]=obj;
				}
				return *pval;
			}
		case c_stringify:
			return var(stringify(eval(sh,state,vexp.get(1))));
		case c_code_to_data:
			return code_to_data(vexp.get(1));//todo
		case c_eval:
			return eval_str(sh,state,eval(sh,state,vexp.get(1)).ref_str());
		case c_cond:
			{
				var cond=eval(sh,state,vexp.get(1));
				if(cond.ref_long()!=0)
				{
					return eval(sh,state,vexp.get(2));
				}
				else
				{
					return eval(sh,state,vexp.get(3));
				}
			}
		case c_call_cps:
			return var();
		case c_string:
			state.ref_result()=eval(sh,state,vexp.get(1));
			if(state.ref_result().get_type()==var::c_long)
			{
				return var(rstr(state.ref_result().ref_long()));
			}
			if(state.ref_result().get_type()==var::c_double)
			{
				return var(rstr(state.ref_result().ref_double()));
			}
			rserror();
			return var();
		case c_long:
			state.ref_result()=eval(sh,state,vexp.get(1));
			if(state.ref_result().get_type()==var::c_str)
			{
				return var(state.ref_result().ref_str().toint64());
			}
			if(state.ref_result().get_type()==var::c_double)
			{
				return var((int64)state.ref_result().ref_double());
			}
			rserror();
			return var();
		case c_double:
			state.ref_result()=eval(sh,state,vexp.get(1));
			if(state.ref_result().get_type()==var::c_str)
			{
				return var(state.ref_result().ref_str().todouble());
			}
			if(state.ref_result().get_type()==var::c_long)
			{
				return var((double)state.ref_result().ref_long());
			}
			rserror();
			return var();
		case c_typeof:
			state.ref_result()=eval(sh,state,vexp.get(1));
			if(state.ref_result().get_type()==var::c_long)
			{
				return var(rstr("long"));
			}
			if(state.ref_result().get_type()==var::c_double)
			{
				return var(rstr("double"));
			}
			if(state.ref_result().get_type()==var::c_table)
			{
				return var(rstr("table"));
			}
			if(state.ref_result().get_type()==var::c_str)
			{
				return var(rstr("string"));
			}
			if(state.ref_result().get_type()==var::c_func)
			{
				return var(rstr("function"));
			}
			rserror();
			return var();
		case c_length:
			state.ref_result()=eval(sh,state,vexp.get(1));
			if(state.ref_result().get_type()==var::c_str)
			{
				return var((int64)state.ref_result().ref_str().count());
			}
			return var((int64)state.ref_result().ref_table().count());
		case c_string_sub:
			state.ref_result()=eval(sh,state,vexp.get(1));
			return var(state.ref_result().ref_str().sub(
				(int)eval(sh,state,vexp.get(2)).ref_long(),
				(int)eval(sh,state,vexp.get(3)).ref_long()));
		default:
			{
				var* pval=find_val(first,state);
				if(pval==null)
				{
					ybase::print_vword(v);
					rserror();
					return var();
				}
				if(pval->get_type()==var::c_func)
				{
					return eval_func(sh,state,vexp.sub(1),*pval);
				}
				rserror();
				return var();
			}
		}
	}

	static var code_to_data(const rbuf<tword>& v)
	{
		var t=var(rdic<var>());
		for(int i=0;i<v.count();i++)
		{
			t.ref_table()[i]=var(v[i].val);
		}
		return t;
	}

	static rstr stringify(var t)
	{
		if(t.get_type()==var::c_long)
		{
			return t.ref_long();
		}
		if(t.get_type()==var::c_double)
		{
			return t.ref_double();
		}
		if(t.get_type()==var::c_str)
		{
			return ybase::add_quote(t.ref_str());
		}
		if(t.get_type()==var::c_func)
		{
			return t.ref_str();
		}
		if(t.get_type()==var::c_table)
		{
			rstr s="{";
			var* p;
			for_set(p,t.ref_table())
			{
				if(p!=t.ref_table().begin())
				{
					s+=",";
				}
				s+="\""+t.ref_table().get_key(p)+"\":";
				s+=stringify(*p);
			}
			s+="}";
			return s;
		}
		return rstr();
	}
};
