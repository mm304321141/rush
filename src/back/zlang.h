#pragma once

#include "../front/yword.h"

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
		return var(a.ref_str()+b.ref_str());
	}

	static var sub(var a,var b)
	{
		return var(a.ref_long()-b.ref_long());
	}

	static var equal(var a,var b)
	{
		return var(a.ref_long()==b.ref_long());
	}
};

struct fstate
{
	var env;
	rbuf<tword> v;

	var& ref_result()
	{
		return env.ref_table()["_result"];
	}
};

fstate g_state;

struct zlang
{
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
		mark(g_state.env);
		clear();
	}

	static rbuf<tword> trans_s_to_v(tsh& sh,rstr s)
	{
		rbuf<tword> v;
		ifn(yword::parse(sh,s,v,null))
		{
			return rbuf<tword>();
		}
		return r_move(v);
	}

	static rstr trans_v_to_s(const rbuf<tword>& v)
	{
		rstr s;
		for(int i=0;i<v.count();i++)
		{
			s+=v[i].val+" ";
		}
		return r_move(s);
	}

	static rbuf<tword> trans_vstr_to_vword(const rbuf<rstr>& vstr)
	{
		rbuf<tword> v;
		for(int i=0;i<vstr.count();i++)
		{
			v+=tword(vstr[i]);
		}
		return r_move(v);
	}

	static var eval(tsh& sh,fstate& state,const rstr& s)
	{
		return eval(sh,state,trans_s_to_v(sh,s));
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

	static var eval_json(tsh& sh,fstate& state,const rbuf<tword>& v)
	{
		int bbk_right=ybase::find_symm_bbk(sh,v,0);
		rbuf<rbuf<tword> > vtemp=ybase::split_comma_b(sh,v.sub(1,bbk_right));
		var temp=var(rdic<var>());
		for(int i=0;i<vtemp.count();i++)
		{
			rbuf<tword>& item=vtemp[i];
			if(item.get(1).val==":")
			{
				rstr key=item.get(0).val;
				if(tword::is_cstr_in(key))
				{
					key=key.sub(1).sub_trim(1);
				}
				temp.ref_table()[key]=eval(sh,state,item.sub(2));
			}
			else
			{
				temp.ref_table()[i]=eval(sh,state,item);
			}
		}
		state.ref_result()=temp;
		if(bbk_right!=v.count()-1)
		{
			state.ref_result()=eval(sh,state,v.sub(bbk_right+1));
		}
		return state.ref_result();
	}

	static var eval_mbk(tsh& sh,fstate& state,const rbuf<tword>& v)
	{
		int mbk_right=ybase::find_symm_mbk(sh,v,0);
		var key=eval(sh,state,v.sub(1,mbk_right));
		rdic<var>& table=state.ref_result().ref_table();
		rstr s;
		if(key.get_type()==var::c_long)
		{
			s=key.ref_long();
		}
		else
		{
			s=key.ref_str();
		}
		if(s=="_count")
		{
			state.ref_result()=var((int64)table.count());
		}
		else
		{
			state.ref_result()=table[s];
		}
		if(mbk_right!=v.count()-1)
		{
			state.ref_result()=eval(sh,state,v.sub(mbk_right+1));
		}
		return state.ref_result();
	}

	static var eval_dot(tsh& sh,fstate& state,const rbuf<tword>& v)
	{
		rstr name=v.get(1).val;
		rdic<var>& table=state.ref_result().ref_table();
		int sbk_right;
		if(v.get(2).val!="(")
		{
			state.ref_result()=table[name];
			sbk_right=1;
			if(sbk_right!=v.count()-1)
			{
				state.ref_result()=eval(sh,state,v.sub(sbk_right+1));
			}
			return state.ref_result();
		}
		sbk_right=ybase::find_symm_sbk(sh,v,2);
		rbuf<rbuf<tword> > param=ybase::split_comma_b(sh,v.sub(3,sbk_right));
		rbuf<var> v_var;
		for(int i=0;i<param.count();i++)
		{
			v_var.push(eval(sh,state,param[i]));
		}
		v_var.push_front(state.ref_result());
		state.ref_result()=eval_func(sh,state,v_var,table[name]);
		if(sbk_right!=v.count()-1)
		{
			state.ref_result()=eval(sh,state,v.sub(sbk_right+1));
		}
		return state.ref_result();
	}

	static rstr stringify(var t)
	{
		if(t.get_type()==var::c_long)
		{
			return t.ref_long();
		}
		if(t.get_type()==var::c_str)
		{
			return t.ref_str();
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

	static var code_to_data(rbuf<tword>& v)
	{
		var t=var(rdic<var>());
		for(int i=0;i<v.count();i++)
		{
			t.ref_table()[i]=var(v[i].val);
		}
		return t;
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
		cur_state.v=trans_s_to_v(sh,fval.ref_str());
		cur_state.env.ref_table()["_upenv"]=state.env;
		state.env.ref_table()["_downenv"]=cur_state.env;
		cur_state.env.ref_table()["_fenv"]=fval.ref_table()["_fenv"];

		int left=cur_state.v.find(rstr("("));
		int right=ybase::find_symm_sbk(sh,cur_state.v,left);
		rbuf<rbuf<tword> > v=ybase::split_comma_b(sh,cur_state.v.sub(left+1,right));
		int start=0;
		if(param.count()==v.count()+1)
		{
			start++;
			cur_state.env.ref_table()["this"]=param[0];
		}
		for(int i=0;i<v.count();i++)
		{
			cur_state.env.ref_table()[v[i].get(0).val]=param[i+start];
		}
		state.ref_result()=eval(sh,cur_state,cur_state.v.sub(right+2).sub_trim(1));
		cur_state.env.ref_table().erase_key("_upenv");
		state.env.ref_table().erase_key("_downenv");
		gc();
		return state.ref_result();
	}

	static var eval(tsh& sh,fstate& state,const rbuf<tword>& v)
	{
		if(v.count()==0)
		{
			return var();
		}
		rstr first=v.get(0).val;
		if(v.count()==1)
		{
			if(tword::is_name_in(first))
			{
				var* pval=find_val(first,state);
				if(pval==null)
				{
					rserror(first);
					return var();
				}
				return *pval;
			}
			if(tword::is_cstr_in(first))
			{
				return var(first.sub(1).sub_trim(1));
			}
			return var(first.toint64());
		}
		if(first=="("&&state.ref_result().get_type()==var::c_func)
		{
			int sbk_right=ybase::find_symm_sbk(sh,v,0);
			state.ref_result()=eval_func(sh,state,ybase::split_comma_b(sh,
				v.sub(1,sbk_right)),state.ref_result());
			if(sbk_right!=v.count()-1)
			{
				state.ref_result()=eval(sh,state,v.sub(sbk_right+1));
			}
			return state.ref_result();
		}
		if(first=="["&&state.ref_result().get_type()==var::c_table)
		{
			return eval_mbk(sh,state,v);
		}
		if(first=="."&&state.ref_result().get_type()==var::c_table)
		{
			return eval_dot(sh,state,v);
		}
		if(first=="{")
		{
			return eval_json(sh,state,v);
		}
		if(first=="rf"&&v.get(2).val=="printl")
		{
			return eval(sh,state,v.sub(2));
		}
		int right=ybase::find_symm_sbk(sh,v,1);
		rbuf<tword> param=v.sub(2,right);
		if(first=="printl")
		{
			state.ref_result()=eval(sh,state,param);
			if(state.ref_result().get_type()==var::c_long)
			{
				rf::printl(state.ref_result().ref_long());
			}
			else
			{
				rf::printl(state.ref_result().ref_str());
			}
		}
		elif(first=="stringify")
		{
			state.ref_result()=var(stringify(eval(sh,state,param)));
		}
		elif(first=="code_to_data")
		{
			state.ref_result()=code_to_data(param);
		}
		elif(first=="get_cur_env")
		{
			state.ref_result()=state.env;
		}
		elif(first=="cond")
		{
			rbuf<rbuf<tword> > vtemp=ybase::split_comma_b(sh,param);
			if(vtemp.count()!=3)
			{
				rserror();
				return var();
			}
			var cond=eval(sh,state,vtemp[0]);
			if(cond.ref_long()!=0)
			{
				state.ref_result()=eval(sh,state,vtemp[1]);
			}
			else
			{
				state.ref_result()=eval(sh,state,vtemp[2]);
			}
		}
		elif(first=="return")
		{
			rbuf<rbuf<tword> > vtemp=ybase::split_comma_b(sh,param);
			return eval(sh,state,vtemp.get(0));
		}
		elif(first=="function")
		{
			if(v.get(1).val!="(")
			{
				rstr name=v.get(1).val;
				right=ybase::find_symm_sbk(sh,v,2);
				int bbk_right=ybase::find_symm_bbk(sh,v,right+1);
				if(bbk_right>=v.count())
				{
					rserror();
					return var();
				}
				state.ref_result()=eval(sh,state,trans_vstr_to_vword(
					rf::vstr("=","(",name,",","function"))+
					v.sub(2,bbk_right+1)+trans_vstr_to_vword(rf::vstr(")")));
				right=bbk_right;
			}
			else
			{
				int bbk_right=ybase::find_symm_bbk(sh,v,right+1);
				if(bbk_right>=v.count())
				{
					rserror();
					return var();
				}
				rdic<var> table;
				table["_fenv"]=state.env;
				state.ref_result()=var(trans_v_to_s(v.sub(0,bbk_right+1)),table);
				right=bbk_right;
			}
		}
		elif(first=="eval")
		{
			rbuf<rbuf<tword> > vtemp=ybase::split_comma_b(sh,param);
			var temp=eval(sh,state,vtemp.get(0));
			state.ref_result()=eval(sh,state,temp.ref_str());
		}
		elif(first=="=")
		{
			rbuf<rbuf<tword> > vtemp=ybase::split_comma_b(sh,param);
			if(vtemp.count()!=2)
			{
				rserror();
				return var();
			}
			if(vtemp[0].count()>1)
			{
				var temp=eval(sh,state,vtemp.get(1));
				var key=eval(sh,state,vtemp[0]);
				*key.obj=*temp.obj;
				state.ref_result()=temp;
			}
			else
			{
				state.ref_result()=eval(sh,state,vtemp.get(1));
				rstr name=vtemp.get(0).get(0).val;
				var* pval=find_val(name,state);
				if(pval==null)
				{
					state.env.ref_table()[name]=state.ref_result();
				}
				else
				{
					*pval=state.ref_result();
				}
			}
		}
		elif(first=="+")
		{
			rbuf<rbuf<tword> > vtemp=ybase::split_comma_b(sh,param);
			state.ref_result()=var::add(eval(sh,state,vtemp.get(0)),eval(sh,state,vtemp.get(1)));
		}
		elif(first=="-")
		{
			rbuf<rbuf<tword> > vtemp=ybase::split_comma_b(sh,param);
			state.ref_result()=var::sub(eval(sh,state,vtemp.get(0)),eval(sh,state,vtemp.get(1)));
		}
		elif(first=="==")
		{
			rbuf<rbuf<tword> > vtemp=ybase::split_comma_b(sh,param);
			state.ref_result()=var::equal(eval(sh,state,vtemp.get(0)),eval(sh,state,vtemp.get(1)));
		}
		else
		{
			var* pval=find_val(first,state);
			if(pval!=null&&pval->get_type()==var::c_func)
			{
				state.ref_result()=eval_func(sh,state,ybase::split_comma_b(sh,param),*pval);
			}
			elif(pval!=null&&pval->get_type()==var::c_table)
			{
				state.ref_result()=*pval;
				right=0;
			}
			else
			{
				rserror();
				return var();
			}
		}
		if(right!=v.count()-1)
		{
			state.ref_result()=eval(sh,state,v.sub(right+1));
		}
		return state.ref_result();
	}
};
