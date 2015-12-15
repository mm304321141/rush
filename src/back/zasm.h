#pragma once

#include "../front/yexp.h"
#include "zopt.h"

//生成汇编代码
//tfunc.vsent->vasm
//返回指针或引用或int的表达式需要放入ebx中
//指针或int的引用是对本身的值进行判断
//其它类型的引用是对引用进行判断
struct zasm
{
	static rbool proc(tsh& sh,tfunc& tfi)
	{
		for(int i=0;i<tfi.vsent.count();i++)
		{
			rbuf<tasm> vasm;
			ifn(proc_ret(sh,tfi.vsent[i],vasm,tfi))
			{
				rserror(tfi.vsent[i],"build asm error");
				return false;
			}
#ifndef _WIN64
			if(tconf::c_op_match)
			{
				yopt::optimize_match(sh,vasm);
			}
#endif
			if(tconf::c_op_merge)
			{
				yopt::optimize_add_sub(sh,vasm);
			}
			for(int j=0;j<vasm.count();j++)
			{
				vasm[j].pos=tfi.vsent[i].pos;
			}
			tfi.vasm+=vasm;
		}
		add_asm_bp(sh,tfi);
		tfi.vsent.free();
		tfi.vword.free();
		return true;
	}

	static void push_double(const tsh& sh,rbuf<tasm>& vasm,double dval)
	{
#ifdef _WIN64
		rsa2(c_mov64,rskey(c_rcx),*((int64*)(&dval)));
		rsa1(c_push,rskey(c_rcx));
#else
		rsa1(c_push,*((uint*)(&dval)+1));
		rsa1(c_push,*((uint*)(&dval)));
#endif
	}

	static rbool add_asm_bp(const tsh& sh,tfunc& tfi)
	{
		rbuf<tasm> vasm;
		int size=yfind::get_func_local_size(tfi);
		ifn(tconf::c_op_zero&&size==0)
		{
			rsa2(c_sub,rs_sp,size);
		}
		rbool b_use_bp=use_bp(sh,tfi.vasm);
		rbool protect_bx=(tfi.name.get_left()==r_char('~')&&
			!yfind::is_empty_struct_type(sh,tfi.ptci->name)&&
			!tfi.vsent.empty()||tfi.is_cfunc);
		if(b_use_bp)
		{
			rsa1(c_push,rs_bp);
			rsa2(rs_mov,rs_bp,rs_sp);
		}
		if(protect_bx)
		{
			rsa1(c_push,rs_bx);
		}
		if(tfi.is_cfunc)
		{
			//todo 如果ebx、esi、edi未修改则不用保护
			rsa1(c_push,rs_si);
			rsa1(c_push,rs_di);
		}
		vasm+=tfi.vasm;
		if(tfi.is_cfunc)
		{
			//todo: change to one line
			rsa1(c_pop,rs_di);
			rsa1(c_pop,rs_si);
		}
		if(protect_bx)
		{
			rsa1(c_pop,rs_bx);
		}
		if(b_use_bp)
		{
			rsa1(c_pop,rs_bp);
		}
		ifn(tconf::c_op_zero&&size==0)
		{
			rsa2(c_add,rs_sp,size);
		}
		size=yfind::get_func_param_size(tfi);
		if(size>0)
		{
			rsa1(c_reti,size);
		}
		else
		{
			rsa0(c_retn);
		}
		tfi.vasm=r_move(vasm);
		return true;
	}

	static rbool use_bp(const tsh& sh,const rbuf<tasm>& vasm)
	{
		ifn(tconf::c_op_bp)
		{
			return true;
		}
		for(int i=0;i<vasm.count();i++)
		{
			for(int j=0;j<vasm[i].vstr.count();j++)
			{
				if(vasm[i].vstr[j]==rs_bp)
				{
					return true;
				}
			}
		}
		return false;
	}

	static rbool proc_ret(tsh& sh,const tsent& src,rbuf<tasm>& vasm,
		tfunc& tfi,int level=0)
	{
		rstr name=src.vword.get_bottom().val;
		if(sh.key.is_asm_ins(name)||ybase::is_tag<tword>(src.vword))
		{
			vasm+=ybase::trans_vword_to_vstr(src.vword);
			return true;
		}
		//返回常量
		if(src.vword.count()==1)
		{
			if(src.vword[0].is_cint())
			{
				//经测试如果是小程序rskey(c_mov)比"mov"效率高
				rsa2(c_mov,rskey(c_ebx),src.vword[0].val);
				return true;
			}
			elif(sh.key.is_asm_reg(src.vword[0].val))
			{
#ifdef _WIN64
				ifn(sh.key.is_reg_32(src.vword[0].val))
				{
					rserror();
					return false;
				}
#endif
				rsa2(c_mov,rskey(c_ebx),src.vword[0].val);
				return true;
			}
			elif(src.vword[0].is_cpoint())
			{
				if(src.vword[0].is_caddr())
				{
					rsa2(rs_mov,rs_bx,src.vword[0].val);
				}
				else
				{
					rsa2(rs_mov,rs_bx,src.vword[0].val.sub_trim(1));
				}
				return true;
			}
		}
		if(src.vword.count()==7&&src.vword[1].val==rsoptr(c_addr))
		{
			rsa2(rs_mov,rs_bx,rsmbk5(
				rsoptr(c_addr),rsoptr(c_comma),src.vword[3].val,rsoptr(c_comma),src.vword[5].val));
			return true;
		}
		//返回变量
		tdata* ptdi=yfind::find_local(tfi,
			get_src_in(sh,src).vword.get(0).val);
		if(ptdi!=null)
		{
			if(yfind::is_type_mebx(sh,src.type))
			{
				rsa2(c_lea,rs_si,rsmbk3(rs_bp,rsoptr(c_plus),ptdi->off));
				//这里还可以优化
				ifn(add_si(sh,ptdi->type,vasm,src))
				{
					rserror("");
					return false;
				}
				rsa2(rs_mov,rs_bx,rsmbk1(rs_si));
				if(ybase::is_quote(src.type)&&
					(ybase::get_tname(src.type)==rskey(c_int)||
					ybase::is_point(ybase::get_tname(src.type))))
				{
					rsa2(rs_mov,rs_bx,rsmbk1(rs_bx));
				}
			}
			return true;
		}
		//返回函数
		return proc_ret_func(sh,src,vasm,tfi,level);
	}

	static rbool proc_ret_func(tsh& sh,const tsent& src,rbuf<tasm>& vasm,
		tfunc& tfi,int level)
	{
		tdata retval;
		ifn(parse_exp(sh,get_src_in(sh,src),vasm,retval,tfi,level))
		{
			return false;
		}
		//多数函数返回void，因此加上这句判断可以提高效率
		if(src.type!=rskey(c_void)&&yfind::is_type_mebx(sh,src.type))
		{
			rsa2(rs_mov,rs_si,rs_sp);
			int cur=vasm.count();
			ifn(add_si(sh,retval.type,vasm,src))
			{
				rserror("");
				return false;
			}
			if(cur==vasm.count())
			{
				vasm.pop();
				rsa2(rs_mov,rs_bx,rsmbk1(rs_sp));
			}
			else
			{
				rsa2(rs_mov,rs_bx,rsmbk1(rs_si));
			}
			if(ybase::is_quote(src.type)&&
				(ybase::get_tname(src.type)==rskey(c_int)||
				ybase::is_point(ybase::get_tname(src.type))))
			{
				rsa2(rs_mov,rs_bx,rsmbk1(rs_bx));
			}
		}
		ifn(destruct_ret(sh,retval,vasm))
		{
			rserror("");
			return false;
		}
		rsa2(c_add,rs_sp,yfind::get_ceil_space(retval));
		return true;
	}
	
	//解析表达式生成汇编代码
	static rbool parse_exp(tsh& sh,const tsent& src,rbuf<tasm>& vasm,tdata& retval,
		tfunc& tfi,int level=0)
	{
		if(level>c_rs_deep)
		{
			rserror("");
			return false;
		}
		level++;
		if(src.vword.get_bottom().val!=rsoptr(c_mbk_l)||
			src.vword.get_top().val!=rsoptr(c_mbk_r))
		{
			rserror("");
			return false;
		}
		if(src.vword.get(1).val==rsoptr(c_dot))
		{
			rserror("");
			return false;
		}
		rbuf<rbuf<tword> > vlisp;
		yexp::get_vlisp(sh,src.vword,vlisp);
		if(src.vword.get(1).val==rskey(c_pcall))
		{
			if(vlisp.count()!=5)
			{
				rserror("");
				return false;
			}
			retval.type=vlisp[1].get(0).val;
			retval.size=yfind::get_type_size(sh,retval.type);
			int size=yfind::get_ceil_space(retval);
			rsa2(c_sub,rs_sp,size);
			rbuf<rbuf<tword> > vparam;
			yexp::get_vlisp(sh,vlisp[4],vparam);
			rbuf<tsent> vsent;
			yexp::get_vsent(vparam,vsent,src);
			for(int i=vsent.count()-1;i>=0;i--)
			{
				//todo:已有类型数组无需重新获取
				ifn(yexp::proc_exp(sh,vsent[i],tfi,level,tenv()))
				{
					return false;
				}
			}
			for(int i=vsent.count()-1;i>=0;i--)
			{
				tdata tdi;
				tdi.type=ybase::get_tname(vsent[i].type);
				tdi.size=yfind::get_type_size(sh,tdi.type);
				size+=yfind::get_ceil_space(tdi);
				ifn(pass_param(sh,vsent[i],tdi,vasm,tfi,level))
				{
					return false;
				}
			}
			tsent sent=src;
			sent.vword=vlisp[3];
			tfunc* ptfi=find_func(sh,sent.vword);
			ifn(yexp::proc_exp(sh,sent,tfi,level,tenv()))
			{
				return false;
			}
			ifn(proc_ret(sh,sent,vasm,tfi,level))
			{
				return false;
			}
			if(sent.type==rstr("rp<void>"))
			{
				rsa1(c_call,rs_bx);
			}
			else
			{
				rserror("");
				return false;
			}
			if(ptfi!=null&&ptfi->is_function)
			{
				rsa2(c_add,rs_sp,size);
			}
			return true;
		}
		if(vlisp.count()!=3)
		{
			rserror("");
			return false;
		}
		yexp::get_vlisp(sh,vlisp[2],vlisp);
		rbuf<tsent> vsent;
		yexp::get_vsent(vlisp,vsent,src);
		for(int i=vsent.count()-1;i>=0;i--)
		{
			ifn(yexp::proc_exp(sh,vsent[i],tfi,level,tenv()))
			{
				return false;
			}
		}
		rstr cname=src.vword.get(1).val;
		rstr fname=src.vword.get(3).val;//p_exp已经合并了name_dec
		tclass* ptci=yfind::find_class(sh,cname);
		if(null==ptci)
		{
			rserror("");
			return false;
		}
		tfunc* ptfi=yfind::find_func_dec(*ptci,fname);
		if(ptfi==null)
		{
			rserror("");
			return false;
		}
		retval=ptfi->retval;
		if(tconf::c_op_empty_func&&
			yfind::is_empty_struct_type(sh,ptci->name)&&
			(yfind::is_destruct(sh,*ptfi)||
			yfind::is_empty_struct(sh,*ptfi)))
		{
			return true;
		}
		if(ptfi->param.count()!=vsent.count())
		{
			rserror("");
			return false;
		}
		rsa2(c_sub,rs_sp,yfind::get_ceil_space(retval));
		for(int i=vsent.count()-1;i>=0;i--)
		{
			ifn(pass_param(sh,vsent[i],ptfi->param[i],vasm,tfi,level))
			{
				return false;
			}
		}
		vasm+=ybase::get_func_declare_call(sh,*ptci,*ptfi);
		return true;
	}

	static rbool pass_param(tsh& sh,const tsent& src,const tdata& dst,rbuf<tasm>& vasm,
		tfunc& tfi,int level)
	{
		if(src.vword.empty())
		{
			rserror("");
			return false;
		}
		tsent src_in=get_src_in(sh,src);
		if(src_in.vword.get(1).val==rskey(c_pcall)||
			yfind::is_class(sh,src_in.vword.get(1).val))
		{
			//先申请参数空间
			rsa2(c_sub,rs_sp,yfind::get_ceil_space(dst));
			//递归处理子表达式
			tdata retval;
			ifn(parse_exp(sh,src_in,vasm,retval,tfi,level))
			{
				return false;
			}
			//获取传递参数的地址分别放入esi和edi中，
			//包括函数返回后再dot，如int.get().m_in
			ifn(obtain_var_addr_f(sh,retval,src,vasm))
			{
				rserror("");
				return false;
			}
			//传递参数
			ifn(copy_param(sh,src.type,dst.type,vasm))
			{
				rserror("");
				return false;
			}
			//析构返回值
			ifn(destruct_ret(sh,retval,vasm))
			{
				rserror("");
				return false;
			}
			//释放栈
			rsa2(c_add,rs_sp,yfind::get_ceil_space(retval));
		}
		elif(src.vword[0].is_cint())//todo src.vword[0]保存到临时变量中
		{
			rsa1(c_push,src.vword[0].val);
		}
		elif(src.vword[0].is_cuint())
		{
			rsa1(c_push,src.vword[0].val.sub_trim(1));
		}
		elif(src.vword[0].is_cdouble())
		{
			push_double(sh,vasm,src.vword[0].val.todouble());
		}
		elif(src.vword.count()==7&&
			src.vword.get(1).val==rsoptr(c_addr)&&
			src.vword[0].val==rsoptr(c_mbk_l))
		{
#ifdef _WIN64
			rbuf<rstr> vstr=ybase::trans_vword_to_vstr(src.vword);
			vstr.push_front(rsoptr(c_comma));
			vstr.push_front(rs_bx);
			vstr.push_front(rskey(c_mov64));
			vasm+=vstr;
			rsa1(c_push,rs_bx);
#else
			rbuf<rstr> vstr=ybase::trans_vword_to_vstr(src.vword);
			vstr.push_front(rskey(c_push));
			vasm+=vstr;
#endif
		}
		elif(src.vword[0].is_cpoint())
		{
			if(src.vword.get_bottom().is_caddr())
			{
#ifdef _WIN64
				rsa2(rs_mov,rs_bx,src.vword[0].val);
				rsa1(c_push,rs_bx);
#else
				rsa1(c_push,src.vword[0].val);
#endif
			}
			else
			{
				rsa1(c_push,src.vword[0].val.sub_trim(1));
			}
		}
		elif(src.vword[0].is_cstr())
		{
#ifdef _WIN64
			rsa2(rs_mov,rs_bx,src.vword[0].val);
			rsa1(c_push,rs_bx);
#else
			rsa1(c_push,src.vword[0].val);
#endif
		}
		elif(sh.key.is_asm_reg(src.vword[0].val))
		{
			rsa1(c_push,src.vword[0].val);
		}
		else
		{
			rstr name=src_in.vword.get(0).val;
			tdata* ptdi=yfind::find_local(tfi,name);
			if(ptdi==null)
			{
				tfi.name.printl();
				rserror(name);
				return false;
			}
			int cur=vasm.count()+3;
			ifn(obtain_var_addr_var(sh,src,dst,ptdi,vasm))
			{
				rserror("");
				return false;
			}
#ifndef _WIN64
			if(cur==vasm.count()&&tconf::c_op_pass)
			{
				if(yfind::is_op_pass_type(sh,dst.type)&&
					dst.type==src.type)
				{
					vasm.cur_count-=3;
					rsa1(c_push,rsmbk3(rs_bp,rsoptr(c_plus),ptdi->off));
					return true;
				}
				if(ybase::is_quote(dst.type)&&ybase::is_quote(src.type))
				{
					vasm.cur_count-=3;
					rsa1(c_push,rsmbk3(rs_bp,rsoptr(c_plus),ptdi->off));
					return true;
				}
				if(ybase::is_quote(dst.type))
				{
					vasm.cur_count-=3;
					rsa2(c_lea,rs_si,rsmbk3(rs_bp,rsoptr(c_plus),ptdi->off));
					rsa1(c_push,rs_si);
					return true;
				}
			}
#endif
			//调用拷贝构造函数时还可以优化
			return copy_param(sh,src.type,dst.type,vasm);
		}
		return true;
	}

	static rbool obtain_var_addr_var(const tsh& sh,const tsent& src,const tdata& dst,
		const tdata* ptdi,rbuf<tasm>& vasm)
	{
		rsa2(c_sub,rs_sp,yfind::get_ceil_space(dst));
		rsa2(rs_mov,rs_di,rs_sp);
		rsa2(c_lea,rs_si,rsmbk3(rs_bp,rsoptr(c_plus),ptdi->off));
		return add_si(sh,ptdi->type,vasm,src);
	}

	static rbool obtain_var_addr_f(const tsh& sh,tdata& retval,
		const tsent& src,rbuf<tasm>& vasm)
	{
		rsa2(c_lea,rs_di,rsmbk3(rs_sp,rsoptr(c_plus),yfind::get_ceil_space(retval)));
		rsa2(rs_mov,rs_si,rs_sp);
		return add_si(sh,retval.type,vasm,src);
	}

	static tsent get_src_in(const tsh& sh,const tsent& src)
	{
		if(src.vword.get(1).val!=rsoptr(c_dot))
		{
			return src;
		}
		tsent sent=src;
		int start=0;
		while(start+2<src.vword.count()&&
			  src.vword[start].val==rsoptr(c_mbk_l)&&
			  src.vword[start+1].val==rsoptr(c_dot))
		{
			start+=3;
		}
		if(src.vword[start].val==rsoptr(c_mbk_l))
		{
			int right=ybase::find_symm_mbk(sh,src.vword,start);
			sent.vword=src.vword.sub(start,right+1);
		}
		else
		{
			sent.vword=src.vword.sub(start,start+1);
		}
		return r_move(sent);
	}
	
	// [ . , [ . , [ . , a , b ] , c ] , d ]
	static rbool add_si(const tsh& sh,rstr type,rbuf<tasm>& vasm,const tsent& src)
	{
		int start=0;
		const rbuf<tword>& v=src.vword;
		if(v.get(1).val!=rsoptr(c_dot))
		{
			return true;
		}
		while(start+2<v.count()&&
			v[start].val==rsoptr(c_mbk_l)&&
			v[start+1].val==rsoptr(c_dot))
		{
			start+=3;
		}
		int right=start+2;
		if(v[start].val==rsoptr(c_mbk_l))
		{
			right=ybase::find_symm_mbk(sh,v,start);
			if(right>=v.count())
			{
				return false;
			}
			right+=2;
		}
		for(int i=0;i<start;i+=3)
		{
			tclass* ptci=yfind::find_class_t(sh,type);
			if(null==ptci)
			{
				return false;
			}
			tdata* ptdi=yfind::find_data_member(*ptci,v.get(right).val);
			if(null==ptdi)
			{
				return false;
			}
			if(ybase::is_quote(type))
			{
				rsa2(rs_mov,rs_si,rsmbk1(rs_si));
			}
			if(ptdi->off!=0)
			{
				rsa2(c_add,rs_si,ptdi->off);
			}
			type=ptdi->type;
			right+=3;
		}
		return true;
	}

	//src的真实地址已经存储在esi中,dst的地址在edi中
	static rbool copy_param(const tsh& sh,const rstr& src,
		const rstr& dst,rbuf<tasm>& vasm)
	{
		//指针可以随意转换
		if(ybase::is_point(dst)&&ybase::is_point(src))
		{
			rsa2(rs_mov,rsmbk1(rs_di),rsmbk1(rs_si));
			return true;
		}
		//目标是引用
		if(ybase::is_quote(dst))
		{
			if(ybase::is_quote(src))
			{
				//源是引用
				rsa2(rs_mov,rsmbk1(rs_di),rsmbk1(rs_si));
			}				
			else
			{
				//源是对象
				rsa2(rs_mov,rsmbk1(rs_di),rs_si);
			}
			return true;
		}
#ifndef _WIN64
		if(tconf::c_op_pass)
		{
			if(yfind::is_op_pass_type(sh,dst))
			{
				if(ybase::is_quote(src))
				{
					//源是引用
					rsa2(rs_mov,rs_si,rsmbk1(rs_si));
				}
				rsa2(rs_mov,rsmbk1(rs_di),rsmbk1(rs_si));
				return true;
			}
			if(dst==rskey(c_double)||dst==rskey(c_int64))
			{
				if(ybase::is_quote(src))
				{
					rsa2(rs_mov,rs_si,rsmbk1(rs_si));
				}
				rsa2(rs_mov,rsmbk1(rs_di),rsmbk1(rs_si));
				rsa2(rs_mov,rsmbk3(rs_di,rsoptr(c_plus),4),rsmbk3(rs_si,rsoptr(c_plus),4));
				return true;
			}
		}
#endif
		//目标是对象需要调用拷贝构造函数
		if(ybase::is_quote(src))
		{
			//源是引用
			rsa1(c_push,rsmbk1(rs_si));
		}
		else
		{
			//源是对象
			rsa1(c_push,rs_si);
		}
		rsa1(c_push,rs_di);
		tclass* ptci=yfind::find_class_t(sh,src);
		if(ptci==null)
		{
			return false;
		}
		tfunc* pcopystruct=yfind::find_copy_struct(*ptci);
		if(pcopystruct==null)
		{
			return false;
		}
		vasm+=ybase::get_func_declare_call(sh,*ptci,*pcopystruct);
		return true;
	}

	static rbool destruct_ret(const tsh& sh,const tdata& retval,rbuf<tasm>& vasm)
	{
		rstr type=retval.type;
		if(ybase::is_quote(type))
		{
			return true;
		}
		if(type==rskey(c_void))
		{
			return true;
		}
		tclass* ptci=yfind::find_class(sh,type);
		if(ptci==null)
		{
			return false;
		}
		tfunc* pdestruct=yfind::find_destruct(*ptci);
		if(pdestruct==null)
		{
			return false;
		}
		if(tconf::c_op_empty_func&&yfind::is_empty_struct_type(sh,type))
		{
			return true;
		}
		rsa2(rs_mov,rs_si,rs_sp);
		rsa1(c_push,rs_si);
		vasm+=ybase::get_func_declare_call(sh,*ptci,*pdestruct);
		return true;
	}

	static tfunc* find_func(tsh& sh,rbuf<tword>& v)
	{
		if(v.count()!=7)
		{
			return null;
		}
		if(v.get(1).val!=rsoptr(c_addr))
		{
			return null;
		}
		tclass* ptci=yfind::find_class(sh,v.get(3).val);
		if(ptci==null)
		{
			return null;
		}
		rstr fname=v.get(5).val;
		return yfind::find_func_dec(*ptci,fname);
	}
};
