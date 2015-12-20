#pragma once

#include "yword.h"
#include "ycontrol.h"
#include "yrep.h"

struct yjs
{
	static rbuf<tword> trans_fdef_to_sexp(tsh& sh,const rbuf<tword>& v)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v[i].val!=rskey(c_function))
			{
				continue;
			}
			int left=i+1;
			rstr name;
			if(v.get(left).val!=rsoptr(c_sbk_l))
			{
				name=v.get(left).val;
				left++;
			}
			if(v.get(left).val!=rsoptr(c_sbk_l))
			{
				continue;
			}
			int right=ybase::find_symm_sbk(sh,v,left);
			rbuf<tword> result=v.sub(0,i);
			ifn(name.empty())
			{
				result+=tword(rsoptr(c_mbk_l),v[i].pos);
				result+=tword(rsoptr(c_equal),v[i].pos);
				result+=tword(name,v[i].pos);
			}
			result+=tword(rsoptr(c_mbk_l),v[i].pos);
			result+=tword(rskey(c_function),v[i].pos);
			result+=tword(rsoptr(c_mbk_l),v[i].pos);
			rbuf<rbuf<tword> > vparam=ybase::split_comma<tword>(sh,v.sub(left+1,right));
			rbuf<tword> temp1=ybase::combine_v(vparam);
			temp1=trans_fdef_to_sexp(sh,temp1);
			result+=temp1;
			result+=tword(rsoptr(c_mbk_r),v[i].pos);
			int bbk_right=ybase::find_symm_bbk(sh,v,right+1);
			rbuf<tword> temp2=v.sub(right+2,bbk_right);
			temp2=trans_fdef_to_sexp(sh,temp2);
			result+=temp2;
			result+=tword(rsoptr(c_mbk_r),v.get(bbk_right).pos);
			ifn(name.empty())
			{
				result+=tword(rsoptr(c_mbk_r),v.get(bbk_right).pos);
			}
			result+=trans_fdef_to_sexp(sh,v.sub(bbk_right+1));
			return result;
		}
		return v;
	}

	static rbool replace_json(tsh& sh,rbuf<tword>& v)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v[i].val!=rsoptr(c_bbk_l))
			{
				continue;
			}
			if(v.get(i-1).val==rsoptr(c_sbk_r)||v.get(i-1).val==rskey(c_else))
			{
				continue;
			}
			int right=ybase::find_symm_bbk(sh,v,i);
			if(right>=v.count())
			{
				rserror();
				return false;
			}
			rbuf<tword> vin=v.sub(i+1,right);
			ifn(replace_json(sh,vin))
			{
				return false;
			}
			rbuf<rbuf<tword> > vtemp=ybase::split_comma_b(sh,vin);
			rbuf<tword> result;
			result+=tword(rsoptr(c_mbk_l),v[i].pos);
			result+=tword(rstr("_JSON"),v[i].pos);
			for(int j=0;j<vtemp.count();j++)
			{
				rbuf<tword>& item=vtemp[j];
				if(item.get(1).val==rsoptr(c_colon))
				{
					rstr key=item.get(0).val;
					ifn(tword::is_cstr_in(key))
					{
						key=ybase::add_quote(key);
					}
					result+=tword(key,item.get(0).pos);
					result+=item.sub(2);
				}
				else
				{
					result+=tword(rstr(j),item.get(0).pos);
					result+=item;
				}
			}
			result+=tword(rsoptr(c_mbk_r),v[right].pos);
			rbuf<tword> back=v.sub(right+1);
			ifn(replace_json(sh,back))
			{
				return false;
			}
			v=v.sub(0,i)+result+back;
			return true;
		}
		return true;
	}

	static rbool replace_control(tsh& sh,rbuf<tword>& v)
	{
		v=trans_fdef_to_sexp(sh,v);
		ifn(replace_json(sh,v))
		{
			return false;
		}
		rbuf<tword> result;
		for(int i=0;i<v.count();i++)
		{
			if(v[i].val!=rskey(c_function))
			{
				result+=v[i];
				continue;
			}
			int left=i-1;
			int right=ybase::find_symm_mbk(sh,v,left);
			rbuf<rbuf<tword> > vexp=ybase::split_sexp(sh,v.sub(left+1,right));
			rbuf<tword> temp=ybase::combine_v(vexp.sub(2));
			ifn(replace_control(sh,temp))
			{
				return false;
			}
			result+=vexp.get(0);
			result+=vexp.get(1);
			result+=temp;
			i=right-1;
		}
		v=result;
		int id=0;
		ifn(ycontrol::replace_switch(sh,v))
		{
			return false;
		}
		ifn(ycontrol::replace_ifn(sh,v))
		{
			return false;
		}
		ifn(ycontrol::add_semi(sh,v))
		{
			return false;
		}
		ifn(ycontrol::replace_for(sh,v,id))
		{
			return false;
		}
		ifn(ycontrol::add_else(sh,v))
		{
			return false;
		}
		ifn(ycontrol::replace_elif(sh,v))
		{
			return false;
		}
		ifn(ycontrol::replace_if(sh,v,id))
		{
			return false;
		}
		ifn(ycontrol::del_bbk(sh,v))
		{
			return false;
		}
		ifn(replace_return(sh,v))
		{
			return false;
		}
		rbuf<rbuf<tword> > vsent=r_split_a<tword>(v,tword(rsoptr(c_semi)));
		for(int i=0;i<vsent.count();i++)
		{
			ifn(replace_exp(sh,vsent[i]))
			{
				return false;
			}
		}
		v=ybase::combine_v(vsent);
		return true;
	}

	static rbool replace_return(tsh& sh,rbuf<tword>& v)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v[i].val!=rskey(c_return))
			{
				continue;
			}
			int pos=v.find(rsoptr(c_semi),i);
			if(pos>=v.count())
			{
				rserror();
				return false;
			}
			rbuf<tword> result=v.sub(0,i);
			result+=v.sub(i+1,pos+1);
			result+=rsoptr(c_mbk_l);
			result+=rstr("jmp");
			result+=rstr("_func_end");
			result+=rsoptr(c_mbk_r);
			result+=rsoptr(c_semi);
			rbuf<tword> back=v.sub(pos+1);
			ifn(replace_return(sh,back))
			{
				return false;
			}
			result+=back;
			v=result;
			return true;
		}
		return true;
	}

	static rbool replace_exp(tsh& sh,rbuf<tword>& v)
	{
		yrep::replace_neg_v(sh,v);
		if(v.count()==2&&v[1].val==rsoptr(c_colon))
		{
			v=ybase::trans_vstr_to_vword(rf::vstr("[",":",v[0].val,"]"));
		}
		elif(v.count()==2&&ybase::is_jmp_ins(sh.key.get_key_index(v[0].val)))
		{
			v=ybase::trans_vstr_to_vword(rf::vstr("[",v[0].val,v[1].val,"]"));
		}
		else
		{
			ifn(proc_exp(sh,v))
			{
				return false;
			}
			if(v.get(1).val==rsoptr(c_plusplus))
			{
				v=replace_multi(sh,v,"+");
			}
			elif(v.get(1).val==rsoptr(c_minusminus))
			{
				v=replace_multi(sh,v,"-");
			}
			elif(v.get(1).val=="+=")
			{
				v=replace_multi(sh,v,"+");
			}
			elif(v.get(1).val=="-=")
			{
				v=replace_multi(sh,v,"-");
			}
			elif(v.get(1).val=="*=")
			{
				v=replace_multi(sh,v,"*");
			}
			elif(v.get(1).val=="/=")
			{
				v=replace_multi(sh,v,"/");
			}
			elif(v.get(1).val=="%=")
			{
				v=replace_multi(sh,v,"%");
			}
		}
		return true;
	}

	static rbuf<tword> replace_multi(tsh& sh,const rbuf<tword>& v,const rstr& op)
	{
		rbuf<rbuf<tword> > vexp=ybase::split_sexp(sh,v.sub_trim_s(1));
		rbuf<tword> temp;
		temp+=rstr("[");
		temp+=rstr("=");
		temp+=vexp.get(1);
		temp+=rstr("[");
		temp+=op;
		temp+=vexp.get(1);
		if(vexp.count()==3)
		{
			temp+=vexp.get(2);
		}
		else
		{
			temp+=rstr(1);
		}
		temp+=rstr("]");
		temp+=rstr("]");
		return temp;
	}

	static rbool is_continue_prev(const rstr& s)
	{
		return s=="]"||s==")"||tword::is_name_in(s);
	}

	static rbool proc_exp(tsh& sh,rbuf<tword>& v)
	{
		if(v.get_left().val==rsoptr(c_mbk_l))
		{
			return true;
		}
		rbuf<rstr> soptr;
		rbuf<rbuf<tword> > sopnd;
		soptr.push(rsoptr(c_sharp_sharp));
		v.push(tword(rsoptr(c_sharp_sharp)));
		for(int i=0;i<v.count();++i)
		{
			if(v[i].val==rsoptr(c_sharp_sharp)&&
				soptr.get_top()==rsoptr(c_sharp_sharp))
			{
				break;
			}
			if(v[i].is_const())
			{
				sopnd+=v.sub(i,i+1);
			}
			elif(v[i].val==rsoptr(c_sbk_l))
			{
				int right=ybase::find_symm_sbk(sh,v,i);
				if(right>=v.count())
				{
					rserror("miss )");
					return false;
				}
				if(!sopnd.empty()&&is_continue_prev(v.get(i-1).val))
				{
					rbuf<rbuf<tword> > vparam=ybase::split_comma(sh,v.sub(i+1,right));
					for(int j=0;j<vparam.count();j++)
					{
						ifn(proc_exp(sh,vparam[j]))
						{
							return false;
						}
					}
					rbuf<tword> outopnd;
					outopnd+=rsoptr(c_mbk_l);
					outopnd+=sopnd.pop();
					outopnd+=ybase::combine_v(vparam);
					outopnd+=rsoptr(c_mbk_r);
					sopnd+=outopnd;
					i=right;
					continue;
				}
				rbuf<tword> outopnd=v.sub(i+1,right);
				ifn(proc_exp(sh,outopnd))
				{
					return false;
				}
				sopnd+=r_move(outopnd);
				i=right;
			}
			elif(v[i].val==rsoptr(c_mbk_l))
			{
				int right=ybase::find_symm_mbk(sh,v,i);
				if(right>=v.count())
				{
					rserror("miss ]");
					return false;
				}
				if(!sopnd.empty()&&is_continue_prev(v.get(i-1).val))
				{
					rbuf<tword> temp=v.sub(i+1,right);
					ifn(proc_exp(sh,temp))
					{
						return false;
					}
					rbuf<tword> outopnd;
					outopnd+=rsoptr(c_mbk_l);
					outopnd+=rsoptr(c_dot);
					outopnd+=sopnd.pop();
					outopnd+=temp;
					outopnd+=rsoptr(c_mbk_r);
					sopnd+=outopnd;
					i=right;
					continue;
				}
				rbuf<tword> outopnd=v.sub(i,right+1);
				sopnd+=r_move(outopnd);
				i=right;
			}
			elif(v[i].val==rsoptr(c_dot))
			{
				if(sopnd.empty())
				{
					rserror();
					return false;
				}
				rbuf<tword> outopnd;
				outopnd+=rsoptr(c_mbk_l);
				outopnd+=rsoptr(c_dot);
				outopnd+=sopnd.pop();
				outopnd+="\""+v.get(i+1).val+"\"";
				outopnd+=rsoptr(c_mbk_r);
				sopnd+=r_move(outopnd);
				i++;
			}
			elif(sh.optr.is_optr(v[i].val))
			{
				if(soptr.empty())
				{
					rserror("exp miss optr");
					return false;
				}
				rstr cur=v[i].val;
				ifn(sh.optr.is_precede(soptr.top(),cur))
				{
					soptr.push(cur);
					continue;
				}
				rstr theta=soptr.pop();
				if(sopnd.empty())
				{
					rserror("exp miss opnd");
					return false;
				}
				rbuf<tword> first=sopnd.pop();
				rbuf<tword> outopnd;
				if(sh.optr.get_num(theta)==1)
				{
					outopnd+=rstr("[");
					outopnd+=theta;
					outopnd+=first;
					outopnd+=rstr("]");
					sopnd+=outopnd;
					i--;
					continue;
				}
				if(sopnd.empty())
				{
					ybase::print_vword(v);
					rserror("exp miss opnd");
					return false;
				}
				outopnd+=rstr("[");
				outopnd+=theta;
				outopnd+=sopnd.pop();
				outopnd+=first;
				outopnd+=rstr("]");
				sopnd+=outopnd;
				i--;
				continue;
			}
			else
			{
				sopnd+=v.sub(i,i+1);
			}
		}
		if(sopnd.count()!=1)
		{
			rserror("expression error");
			return false;
		}
		v=r_move(sopnd[0]);
		if(v.empty())
		{
			rserror("expression error");
			return false;
		}
		return true;
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
};
