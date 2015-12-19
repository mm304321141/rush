#pragma once

#include "../front/yclass.h"
#include "../front/yformat.h"
#include "zlang.h"
#include "zvm.h"
#include "zjit.h"
#include "zgpp.h"
#include "zjs.h"
#include "zcpp.h"

//参数解析
struct zmain
{
	static rbool compile(uchar* cont)
	{
		tsh sh;
#ifndef _RS
		zjitf::get_psh()=&sh;
#endif
		zlang::init_op();
		sh.main_cont=cont;
		rbuf<rstr> vparam=rf::get_param();
		if(vparam.count()<2)
		{
			return false;
		}
#ifdef _WIN64
		sh.s_define.insert_c(tmac(rstr("_RS64")));
#endif
		sh.s_define.insert_c(tmac(rstr("_RS")));
		if(vparam[1]=="-jit")
		{
			sh.mode=tsh::c_jit;
			sh.s_define.insert_c(tmac(rstr("_RJIT")));
		}
		elif(vparam[1]=="-gpp")
		{
			sh.mode=tsh::c_gpp;
			sh.s_define.insert_c(tmac(rstr("_RGPP")));
		}
		elif(vparam[1]=="-nasm")
		{
			sh.mode=tsh::c_nasm;
			sh.s_define.insert_c(tmac(rstr("_RNASM")));
		}
		elif(vparam[1]=="-asm")
		{
			sh.mode=tsh::c_asm;
			sh.s_define.insert_c(tmac(rstr("_RVM")));
		}
		elif(vparam[1]=="-js")
		{
			sh.mode=tsh::c_js;
			sh.s_define.insert_c(tmac(rstr("_RJS")));
			sh.s_define.insert_c(tmac(rstr("_RNASM")));
		}
		elif(vparam[1]=="-cpp")
		{
			sh.mode=tsh::c_cpp;
			sh.s_define.insert_c(tmac(rstr("_RCPP")));
			sh.s_define.insert_c(tmac(rstr("_RNASM")));
		}
		else
		{
			sh.s_define.insert_c(tmac(rstr("_RVM")));
		}
		rstr name;
		for(int i=1;i<vparam.count();i++)
		{
			if(vparam[i].get_left()!=r_char('-'))
			{
				name=vparam[i];
				break;
			}
		}
		if(name.get_left()==r_char('\"'))
		{
			name=ybase::del_quote(name);
		}
		name=rcode::trans_gbk_to_utf8(name);
		name=rdir::std_dir(name);
		name=ypre::get_abs_path(name);
		sh.main_file=name;
		ybase::init_path(sh.vpath);
		ybase::init_match(sh.vmatch);
		ifn(ypre::proc(sh))
		{
			rserror("pre process error");
			return false;
		}
		ifn(yformat::proc(sh))
		{
			rserror("format error");
			return false;
		}
		ifn(yclass::proc(sh))
		{
			rserror("extract class error");
			return false;
		}
		ifn(yclasstl::proc(sh))
		{
			rserror("class template replace error");
			return false;
		}
		ifn(ymemb::proc(sh))
		{
			rserror("ymemb error");
			return false;
		}
		if(sh.mode==tsh::c_vm)
		{
			tfunc* ptfi=yfind::find_func(*sh.pmain,"main");
			if(ptfi==null)
			{
				rf::printl("main not find");
				return false;
			}
			zjitf::init_addr_list(sh);
			ifn(zbin::proc(sh,*ptfi))
			{
				return false;
			}
			ifn(zvm::run(sh))
			{
				return false;
			}
		}
		elif(sh.mode==tsh::c_jit)
		{
			zjitf::init_addr_list(sh);
			ifn(zjit::run(sh))
			{
				return false;
			}
		}
		elif(sh.mode==tsh::c_nasm)
		{
			ifn(znasm::proc(sh))
			{
				return false;
			}
		}
		elif(sh.mode==tsh::c_asm)
		{
			ifn(build_asm(sh))
			{
				return false;
			}
		}
		elif(sh.mode==tsh::c_gpp)
		{
			ifn(zgpp::proc(sh))
			{
				return false;
			}
		}
		elif(sh.mode==tsh::c_js)
		{
			ifn(zjs::proc(sh))
			{
				return false;
			}
		}
		elif(sh.mode==tsh::c_cpp)
		{
			ifn(zcpp::proc(sh))
			{
				return false;
			}
		}
		return true;
	}

	static rbool build_asm(tsh& sh)
	{
		rstr result;
		tclass* p;
		for_set(p,sh.s_class)
		{
			tfunc* q;
			for_set(q,p->s_func)
			{
				if(q->vasm.empty())
				{
					ifn(zbin::compile_vword_to_vasm(sh,*q,tenv()))
					{
						return false;
					}
					result+="asm void "+q->name+"(){\n";
					for(int i=0;i<q->vasm.count();i++)
					{
						result+="	"+rstr::join<rstr>(q->vasm[i].vstr," ")+"\n";
					}
					result+="}\n\n";
				}
			}
		}
		rstr name=ybase::get_main_name(sh)+".asm";
		return rfile::write_all_n(name,result);
	}
};

//C++不支持mixin，只能这么写
rbool r_zjit_compile_func_to_x86(tsh& sh,tfunc& tfi,tenv env)
{
	return zjit::compile_func_to_x86(sh,tfi,env);
}
