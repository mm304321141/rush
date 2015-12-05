#pragma once

#include "znasm.h"

struct zgpp
{
	static rbool proc(tsh& sh)
	{
		tfunc* ptfi=yfind::find_func(*sh.pmain,"main");
		if(ptfi==null)
		{
			rserror("main not find");
			return false;
		}
		rstr result;
		rbuf<tfunc*> vdec;
		ifn(proc_func(sh,*ptfi,result,vdec))
		{
			rserror("gpp error");
			return false;
		}
		rstr result_s;
		build_struct_dec(sh,result_s);
		rstr head;
		head+=("#include \""+rcode::trans_utf8_to_gbk(
			ybase::get_rs_dir())+"ext/mingw/gpp.h\"\n");

		head+="\n";

		head+=result_s;
		head+="\n";
		head+="_MAIN _PMAIN;\n";
		head+="\n";
		for(int i=0;i<vdec.count();i++)
		{
			head+=get_func_dec(*vdec[i])+";\n";
		}
		head+="\n";
		head+="int main(){main2Emain2829();return 0;}\n";
		head+="\n";
		head+=result;
		rstr name=ybase::get_main_name(sh)+".cpp";
		rfile file;
		ifn(file.open_n(name,"rw"))
		{
			rserror("file open error");
			return false;
		}
		ifn(file.write(head.size(),head.begin()))
		{
			rserror("file write error");
			return false;
		}
		return true;
	}

	static void build_struct_dec(tsh& sh,rstr& result)
	{
		rset<tgpp> s_list;//tgpp按照指针大小排序，故每次生成的结构体顺序不一定相同
		tclass* p;
		for_set(p,sh.s_class)
		{
			tgpp item;
			item.p=p;
			item.visit=false;
			s_list.insert(item);
		}
		tgpp* q;
		for_set(q,s_list)
		{
			ifn(q->visit)
			{
				q->visit=true;
				build_struct_dec_one(sh,q,s_list,result);
			}
		}
	}

	static void build_struct_dec_one(tsh& sh,tgpp* q,rset<tgpp>& s_list,rstr& result)
	{
		if(is_internal_type(sh,q->p->name))
		{
			return;
		}
		for(int i=0;i<q->p->vdata.count();i++)
		{
			if(ybase::is_quote(q->p->vdata[i].type))
			{
				continue;
			}
			tclass* pclass=yfind::find_class(sh,q->p->vdata[i].type);
			if(pclass==null)
			{
				continue;
			}
			tgpp* pitem=s_list.find(tgpp(pclass));
			if(pitem==null)
			{
				continue;
			}
			if(pitem->visit)
			{
				continue;
			}
			pitem->visit=true;
			build_struct_dec_one(sh,pitem,s_list,result);
		}
		result+="struct ";
		if(q->p->name=="main")
		{
			result+="_MAIN";
		}
		else
		{
			result+=trans_type(q->p->name);
		}
		result+="\n{\n";
		for(int i=0;i<q->p->vdata.count();i++)
		{
			result+="	"+trans_type(q->p->vdata[i].type)+" "+q->p->vdata[i].name+";\n";
			if(q->p->vdata[i].is_array())
			{
				result+="	"+trans_type(q->p->vdata[i].type)+" "+q->p->vdata[i].name+"_";
				result+=rstr("[")+(q->p->vdata[i].count-1)+"];\n";//todo 0长数组
			}
		}
		result+="};\n";
	}

	static rbool is_internal_type(tsh& sh,const rstr& s)
	{
		return ybase::is_basic_type(sh,s)||yfind::is_empty_struct_type(sh,s)||s=="ushort"||s=="int64";
	}

	static rstr trans_type(const rstr& s)
	{
		if(s.sub(0,3)=="rp<")
		{
			if(s.get_right()==r_char('&'))
			{
				return "void*&";
			}
			else
			{
				return "void*";
			}
		}
		if(s.get_right()==r_char('&'))
		{
			return znasm::trans_symbol(s.sub(0,s.count()-1))+"&";
		}
		return znasm::trans_symbol(s);
	}

	static rstr get_func_dec(tfunc& tfi)
	{
		return (trans_type(tfi.retval.type)+" "+znasm::get_nasm_symbol(tfi)+
			"("+get_func_param_declare(tfi)+")");
	}

	static rstr get_func_param_declare(const tfunc& tfi)
	{
		rstr s;
		for(int j=0;j<tfi.param.count();j++)
		{
			if(j!=0)
			{
				s+=",";
			}
			s+=trans_type(tfi.param[j].type)+" "+tfi.param[j].name;
		}
		return r_move(s);
	}

	static void replace_template_type(tsh& sh,rbuf<tword>& v)
	{
		for(int i=0;i<v.count();i++)
		{
			tclass* ptci=yfind::find_class(sh,v[i].val);
			if(ptci==null)
			{
				continue;
			}
			v[i].val=trans_type(v[i].val);
		}
	}

	static rbool proc_func(tsh& sh,tfunc& tfi,rstr& result,
		rbuf<tfunc*>& vdec,int level=0)
	{
		if(level>300)
		{
			rserror(tfi,"gpp level overflow");
			return false;
		}
		level++;
		if(tfi.count==1)
		{
			return true;
		}
		tfi.count=1;
		if(tfi.is_final)
		{
			replace_template_type(sh,tfi.vword);
			result+=get_func_dec(tfi)+"\n";
			result+="{\n";
			if(tfi.name=="*"&&tfi.param.count()==1&&tfi.ptci->name.sub(0,3)=="rp<")
			{
				//todo:
				result+="return *("+trans_type(ybase::get_tname(tfi.retval.type))+"*)this;";
			}
			else
			{
				rbuf<rstr> vstr=ybase::trans_vword_to_vstr(tfi.vword);
				result+=rstr::join<rstr>(vstr," ");
			}
			result+="\n}\n\n";
			vdec+=&tfi;
			return true;
		}
		if(tfi.vsent.empty())
		{
			ifn(ysent::proc(sh,tfi,tenv()))
			{
				rserror(tfi,"sent error");
				return false;
			}
		}
		int id=0;
		ifn(proc_ret_var(sh,tfi,id))
		{
			rserror("");
			return false;
		}
		ifn(proc_param_var(sh,tfi,id))
		{
			rserror("");
			return false;
		}
		for(int i=0;i<tfi.vsent.count();i++)
		{
			if(sh.key.is_asm_ins(tfi.vsent[i].vword.get_bottom().val))
			{
				continue;
			}
			if(ybase::is_tag<tword>(tfi.vsent[i].vword))
			{
				continue;
			}
			if(tfi.vsent[i].vword.get(1)==rsoptr(c_equal))
			{
				continue;
			}
			ifn(yexp::proc_exp(sh,tfi.vsent[i],tfi,0,tenv()))
			{
				return false;
			}
		}
		rstr symbol=znasm::get_nasm_symbol(tfi);
		result+=get_func_dec(tfi)+"//"+tfi.ptci->name+"."+tfi.name_dec+"\n";
		result+="{\n";
		result+="	int ebx;\n";
		if(tfi.retval.type!=rskey(c_void))
		{
			result+="	";
			if(ybase::is_quote(tfi.retval.type))
			{
				result+=trans_type(tfi.retval.type.sub_trim(1))+"*";
			}
			else
			{
				result+=trans_type(tfi.retval.type);
			}
			result+=" "+tfi.retval.name+";\n";
		}
		for(int i=0;i<tfi.local.count();i++)
		{
			if(!ybase::is_quote(tfi.local[i].type)&&tfi.local[i].name!=rskey(c_pmain))
			{
				rstr temp=trans_type(tfi.local[i].type)+" "+tfi.local[i].name;
				if(tfi.local[i].is_array())
				{
					result+="	"+temp+"_["+tfi.local[i].count+"];\n";
					result+=("	"+trans_type(tfi.local[i].type)+"& "+tfi.local[i].name+
						"=*"+tfi.local[i].name+"_;\n");
				}
				else
				{
					result+="	"+temp+";\n";
				}
			}
		}
		result+="\n";
		rbuf<rbuf<rstr> > vtemp;
		for(int i=0;i<tfi.vsent.count();i++)
		{
			if(ybase::is_tag<tword>(tfi.vsent[i].vword))
			{
				vtemp+=rf::vstr(tfi.vsent[i].vword[0].val,tfi.vsent[i].vword[1].val);
				//todo:_func_end标号有时不是必须的
				continue;
			}
			rbuf<rstr> vstr=ybase::trans_vword_to_vstr(tfi.vsent[i].vword);
			if(is_call(sh,vstr))
			{
				ifn(trans_call(sh,tfi,vstr))
				{
					return false;
				}
			}
			//只有int或者int&可以作为逻辑判断条件
			if(ybase::get_tname(tfi.vsent[i].type)=="int")
			{
				vstr.push_front("=");
				vstr.push_front("ebx");
			}
			vtemp+=vstr;
		}
		ifn(trans_ret_quote(vtemp,tfi))
		{
			rserror("");
			return false;
		}
		trans_control(tfi,vtemp);
		for(int i=0;i<vtemp.count();i++)
		{
			ifn(ybase::is_tag<rstr>(vtemp[i]))
			{
				result+="	";
			}
			result+=rstr::join<rstr>(vtemp[i]," ");
			result+=" ;\n";
		}
		if(tfi.retval.type!=rskey(c_void))
		{
			if(ybase::is_quote(tfi.retval.type))
			{
				result+="	return *"+tfi.retval.name+";\n";
			}
			else
			{
				result+="	return "+tfi.retval.name+";\n";
			}
		}
		result+="}\n\n";
		for(int i=0;i<tfi.vsent.count();i++)
		{
			if(ybase::is_tag<tword>(tfi.vsent[i].vword))
			{
				continue;
			}
			rbuf<rstr> vstr=ybase::trans_vword_to_vstr(tfi.vsent[i].vword);
			ifn(proc_func_v(sh,tfi,result,vdec,level,vstr))
			{
				return false;
			}
		}
		vdec+=&tfi;
		return true;
	}

	static rbool trans_ret_quote(rbuf<rbuf<rstr> >& v,tfunc& tfi)
	{
		ifn(ybase::is_quote(tfi.retval.type))
		{
			return true;
		}
		for(int i=1;i<v.count();i++)
		{
			ifn(v[i].get_left()=="jmp"&&v[i].get(1)=="_func_end")
			{
				continue;
			}
			//上一句不一定是原来的返回语句，中间只可能间隔析构函数，析构函数特征是无返回值
			int j;
			for(j=i-1;j>=0;j--)
			{
				ifn(tfi.vsent[j].type=="void"||tfi.vsent[j].type.empty())
				{
					break;
				}
			}
			if(j<0)
			{
				return false;
			}
			if(v[j].count()>2&&v[j][0]=="ebx"&&v[j][1]=="=")
			{
				v[j]=v[j].sub(2);
			}
			v[j]=rf::vstr(tfi.retval.name,"=","&")+v[j];
		}
		return true;
	}

	static void trans_control(tfunc& tfi,rbuf<rbuf<rstr> >& v)
	{
		rbuf<rbuf<rstr> > result;
		for(int i=0;i<v.count();i++)
		{
			if(v[i].get_left()=="jebxz")
			{
				rbuf<rstr> temp;
				temp+="if";
				temp+="(";
				temp+="!";
				temp+="ebx";
				temp+=")";
				temp+="jmp";
				temp+=v[i].get(1);
				result+=temp;
			}
			elif(v[i].get_left()=="jebxnz")
			{
				rbuf<rstr> temp;
				temp+="if";
				temp+="(";
				temp+="ebx";
				temp+=")";
				temp+="jmp";
				temp+=v[i].get(1);
				result+=temp;
			}
			elif(v[i].get_left()=="cnesb")
			{
				rbuf<rstr> temp;
				temp+="ebx";
				temp+="=";
				temp+=v[i].get(1);
				temp+="!=";
				temp+=v[i].get(3);
				result+=temp;
			}
			elif(v[i].get_left()=="cesb")
			{
				rbuf<rstr> temp;
				temp+="ebx";
				temp+="=";
				temp+=v[i].get(1);
				temp+="==";
				temp+=v[i].get(3);
				result+=temp;
			}
			elif(v[i].get_left()=="mov")
			{
				rbuf<rstr> temp;
				int j;
				for(j=0;j<tfi.local.count();j++)
				{
					if(tfi.local[j].name==v[i].get(1)&&ybase::is_quote(tfi.local[j].type))
					{
						temp+=trans_type(tfi.local[j].type);
						temp+=v[i].get(1);
						temp+="=";
						temp+="*("+trans_type(tfi.local[j].type).sub_trim(1)+"*)";
						temp+=v[i].get(3);
						result+=temp;
						break;
					}
				}
				if(j<tfi.local.count())
				{
					continue;
				}
				temp+=v[i].get(1);
				temp+="=";
				temp+=v[i].get(3);
				result+=temp;
			}
			else
			{
				result+=v[i];
			}
		}
		v=result;
	}

	static rbool proc_func_v(tsh& sh,tfunc& tfi,rstr& result,
		rbuf<tfunc*>& vdec,int level,const rbuf<rstr>& v)
	{
		if(v.count()==1)
		{
			return true;
		}
		if(v.get(1)==rsoptr(c_equal))
		{
			return proc_func_v(sh,tfi,result,vdec,level,v.sub(2));
		}
		if(v.get_left()==rsoptr(c_mbk_l))
		{
			if(v.get(1)==rsoptr(c_addr)&&v.count()==7)
			{
				tclass* ptci=yfind::find_class(sh,v[3]);
				if(ptci==null)
				{
					rserror("");
					return false;
				}
				tfunc* ptfi=yfind::find_func_dec(*ptci,v[5]);
				if(ptfi==null)
				{
					rserror("");
					return false;
				}
				ifn(proc_func(sh,*ptfi,result,vdec,level))
				{
					return false;
				}
				return true;
			}
			if(v.get(1)==rsoptr(c_dot))
			{
				rbuf<rbuf<rstr> > vparam=ybase::split_comma<rstr>(
					sh,v.sub(1,v.count()-1));
				if(vparam.count()!=3)
				{
					rserror("");
					return false;
				}
				ifn(proc_func_v(sh,tfi,result,vdec,level,vparam[1]))
				{
					return false;
				}
				return true;
			}
			if(v.get(1)==rskey(c_pcall))
			{
				rbuf<rbuf<rstr> > vparam=ybase::split_comma<rstr>(
					sh,v.sub(1,v.count()-1));
				if(vparam.count()!=5)
				{
					rserror("");
					return false;
				}
				ifn(proc_func_v(sh,tfi,result,vdec,level,vparam[3]))
				{
					return false;
				}
				vparam=ybase::split_comma<rstr>(
					sh,vparam[4].sub(1,vparam[4].count()-1));
				for(int i=0;i<vparam.count();i++)
				{
					ifn(proc_func_v(sh,tfi,result,vdec,level,vparam[i]))
					{
						return false;
					}
				}
				return true;
			}
			tfunc* ptfi=find_call(sh,v);
			if(ptfi==null)
			{
				rserror("");
				return false;
			}
			rbuf<rbuf<rstr> > vparam=ybase::split_comma<rstr>(
				sh,v.sub(6,v.count()-2));
			for(int i=0;i<vparam.count();i++)
			{
				ifn(proc_func_v(sh,tfi,result,vdec,level,vparam[i]))
				{
					return false;
				}
			}
			ifn(proc_func(sh,*ptfi,result,vdec,level))
			{
				return false;
			}
			return true;
		}
		return true;
	}

	static rbool is_call(tsh& sh,const rbuf<rstr>& v)
	{
		if(v.get(1)==rsoptr(c_equal))
		{
			return is_call(sh,v.sub(2));
		}
		if(find_call(sh,v)!=null)
		{
			return true;
		}
		return v.get(0)=="["&&(v.get(1)==rskey(c_pcall)||v.get(1)==rsoptr(c_dot));
	}

	static tfunc* find_call(tsh& sh,const rbuf<rstr>& v)
	{
		if(v.get(1)==rsoptr(c_equal))
		{
			return find_call(sh,v.sub(2));
		}
		tclass* ptci=yfind::find_class(sh,v.get(1));
		if(ptci==null)
		{
			return null;
		}
		rstr fname=v.get(3);
		return yfind::find_func_dec(*ptci,fname);
	}

	static rbool trans_dot(tsh& sh,tfunc& tfi,rbuf<rstr>& v)
	{
		rbuf<rstr> result;
		rbuf<rbuf<rstr> > vparam=ybase::split_comma<rstr>(
			sh,v.sub(1,v.count()-1));
		if(vparam.count()!=3)
		{
			rserror("");
			return false;
		}
		ifn(trans_call(sh,tfi,vparam[1]))
		{
			return false;
		}
		ifn(trans_call(sh,tfi,vparam[2]))
		{
			return false;
		}
		result+=vparam[1];
		result+=".";
		result+=vparam[2];
		v=result;
		return true;
	}

	static rbool is_cpoint(const rstr& val)
	{
		return val.sub(0,val.count()-1).is_number()&&val.get_right()==r_char('p');
	}

	static rbool trans_call(tsh& sh,tfunc& tfi,rbuf<rstr>& v)
	{
		if(v.get(1)==rsoptr(c_equal))
		{
			rbuf<rstr> temp=v.sub(2);
			rbool retval=trans_call(sh,tfi,temp);
			v=v.sub(0,2)+temp;
			return retval;
		}
		if(v.count()==1)
		{
			if(v[0].get_left()==r_char('\"'))
			{
				v[0]="(void*)"+v[0];
			}
			elif(is_cpoint(v[0]))
			{
				v[0]="(void*)"+v[0].sub(0,v[0].count()-1);
			}
			elif(v[0].is_number())
			{
				v[0]+="u";
			}
			return true;
		}
		if(v.get_left()==rsoptr(c_mbk_l))
		{
			if(v.count()==7&&v[1]=="&")
			{
				v=rf::vstr("((void*)&"+znasm::trans_symbol(v[3]+"."+v[5])+")");
				return true;
			}
			if(v.get(1)==".")
			{
				return trans_dot(sh,tfi,v);
			}
			if(v.get(1)==rskey(c_pcall))
			{
				rbuf<rbuf<rstr> > vparam=ybase::split_comma<rstr>(
					sh,v.sub(1,v.count()-1));
				if(vparam.count()!=5)
				{
					rserror("");
					return false;
				}
				for(int i=1;i<vparam[2].count()-1;i+=2)
				{
					vparam[2][i]=trans_type(vparam[2][i]);
				}
				ifn(trans_call(sh,tfi,vparam[3]))
				{
					return false;
				}
				v=rf::vstr("(("+trans_type(vparam[1].get(0))+" (*)("+
					rstr::join<rstr>(vparam[2].sub(1,vparam[2].count()-1),"")+"))"+
					rstr::join<rstr>(vparam[3],"")+")(");
				vparam=ybase::split_comma<rstr>(
					sh,vparam[4].sub(1,vparam[4].count()-1));
				for(int i=0;i<vparam.count();i++)
				{
					if(i!=0)
					{
						v[0]+=",";
					}
					ifn(trans_call(sh,tfi,vparam[i]))
					{
						return false;
					}
					v[0]+=rstr::join<rstr>(vparam[i],"");
				}
				v[0]+=")";
				return true;
			}
			tfunc* ptfi=find_call(sh,v);
			if(ptfi==null)
			{
				rserror("");
				return false;
			}
			rbuf<rstr> result;
			result+=znasm::get_nasm_symbol(*ptfi);
			result+="(";
			rbuf<rbuf<rstr> > vparam=ybase::split_comma<rstr>(
				sh,v.sub(6,v.count()-2));
			for(int i=0;i<vparam.count();i++)
			{
				if(i!=0)
				{
					result+=",";
				}
				ifn(trans_call(sh,tfi,vparam[i]))
				{
					return false;
				}
				result+=vparam[i];
			}
			result+=")";
			v=result;
			return true;
		}
		rserror("");
		return false;
	}

	static rbool proc_ret_var(tsh& sh,tfunc& tfi,int& id)
	{
		for(int j=0;j<c_rs_deep;j++)
		{
			for(int i=0;i<tfi.vsent.count();i++)
			{
				ifn(proc_ret_var_one(sh,tfi.vsent[i].vword,id,tfi))
				{
					return false;
				}
			}
			ifn(ybase::part_vsent(tfi.vsent))
			{
				return true;
			}
		}
		return false;
	}

	static rbool proc_ret_var_one(tsh& sh,rbuf<tword>& v,int& id,tfunc& tfi)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v[i]!=rsoptr(c_mbk_l))
			{
				continue;
			}
			rstr type;
			if(v.get(i+1).val==rskey(c_pcall))
			{
				type=v.get(i+3).val;
			}
			else
			{
				tclass* ptci=yfind::find_class(sh,v.get(i+1).val);
				if(ptci==null)
				{
					continue;
				}
				tfunc* ptfi=yfind::find_func_dec(*ptci,v.get(i+3).val);
				if(ptfi==null)
				{
					continue;
				}
				type=ptfi->retval.type;
			}
			if(ybase::is_quote(type))
			{
				continue;
			}
			if(yfind::is_empty_struct_type(sh,type)||
				type==rskey(c_void))
			{
				continue;
			}
			if(v.get(i-1)==rsoptr(c_equal))
			{
				continue;
			}
			int right=ybase::find_symm_mbk(sh,v,i);
			if(right>=v.count())
			{
				return false;
			}
			rstr name=rstr("_GRET")+id;
			id++;
			rbuf<tword> vword;
			tfi.local.push(tdata(type,name));
			vword+=name;
			vword+=rsoptr(c_equal);
			vword+=v.sub(i,right+1);
			vword+=rsoptr(c_semi);
			if(i!=0)
			{
				vword+=v.sub(0,i);
				vword+=name;
				vword+=v.sub(right+1);
			}
			vword+=rsoptr(c_semi);
			vword+=rsoptr(c_mbk_l);
			vword+=type;
			vword+=rsoptr(c_comma);
			vword+="~"+type+"("+type+"&)";
			vword+=rsoptr(c_comma);
			vword+=rsoptr(c_mbk_l);
			vword+=name;
			vword+=rsoptr(c_mbk_r);
			vword+=rsoptr(c_mbk_r);
			v=r_move(vword);
			return true;
		}
		return true;
	}

	static rbool proc_param_var(tsh& sh,tfunc& tfi,int& id)
	{
		for(int j=0;j<c_rs_deep;j++)
		{
			for(int i=0;i<tfi.vsent.count();i++)
			{
				ifn(proc_param_var_one(sh,tfi.vsent[i].vword,id,tfi))
				{
					return false;
				}
			}
			ifn(ybase::part_vsent(tfi.vsent))
			{
				return true;
			}
		}
		return false;
	}

	static rbool proc_param_var_one(tsh& sh,rbuf<tword>& v,int& id,tfunc& tfi)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v[i]!=rsoptr(c_mbk_l))
			{
				continue;
			}
			tclass* ptci=yfind::find_class(sh,v.get(i+1).val);
			if(ptci==null)
			{
				continue;
			}
			tfunc* ptfi=yfind::find_func_dec(*ptci,v.get(i+3).val);
			if(ptfi==null)
			{
				continue;
			}
			int right=ybase::find_symm_mbk(sh,v,i);
			if(right>=v.count())
			{
				return false;
			}
			rbuf<rbuf<tword> > vparam=ybase::split_comma<tword>(sh,v.sub(i+6,right-1));
			for(int j=0;j<ptfi->param.count();j++)
			{
				rstr type=ptfi->param[j].type;
				if(ybase::is_quote(type))
				{
					continue;
				}
				if(yfind::is_empty_struct_type(sh,type))
				{
					continue;
				}
				if(vparam[j].count()==1&&vparam[j][0].val.sub(0,7)=="_GPARAM")
				{
					continue;
				}
				rstr name=rstr("_GPARAM")+id;
				id++;
				rbuf<tword> vword;
				tfi.local.push(tdata(type,name));
				vword+=rsoptr(c_mbk_l);
				vword+=type;
				vword+=rsoptr(c_comma);
				vword+=type+"("+type+"&,"+type+"&)";
				vword+=rsoptr(c_comma);
				vword+=rsoptr(c_mbk_l);
				vword+=name;
				vword+=rsoptr(c_comma);
				vword+=vparam[j];
				vword+=rsoptr(c_mbk_r);
				vword+=rsoptr(c_mbk_r);
				vword+=rsoptr(c_semi);
				vword+=v.sub(0,i+6);
				vparam[j].clear();
				vparam[j]+=name;
				for(int k=0;k<vparam.count();k++)
				{
					if(k!=0)
					{
						vword+=rsoptr(c_comma);
					}
					vword+=vparam[k];
				}
				vword+=v.sub(right-1);
				v=r_move(vword);
				return true;
			}
		}
		return true;
	}
};
