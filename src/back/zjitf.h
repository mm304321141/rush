#pragma once

#include "../struct/tbase.h"
#include "../front/yclasstl.h"
#include "../front/ypre.h"

//jit函数
struct zjitf
{
	static tsh*& get_psh()
	{
#ifndef _RS
		static tsh* g_psh;
		return g_psh;
#endif
	}

	static void addl(int64* a,int64* b)
	{
		*a+=*b;
	}

	static void subl(int64* a,int64* b)
	{
		*a-=*b;
	}

	static void imull(int64* a,int64* b)
	{
		*a*=*b;
	}

	static void idivl(int64* a,int64* b)
	{
		*a/=*b;
	}

	static void imodl(int64* a,int64* b)
	{
		*a%=*b;
	}

	static rbool cgsbl(int64* a,int64* b)
	{
		return *a>*b;
	}

	static rbool clsbl(int64* a,int64* b)
	{
		return *a<*b;
	}

	static void faddl(double* a,double* b)
	{
		*a+=*b;
	}

	static void fsubl(double* a,double* b)
	{
		*a-=*b;
	}

	static void fmull(double* a,double* b)
	{
		*a*=*b;
	}

	static void fdivl(double* a,double* b)
	{
		*a/=*b;
	}

	static rbool fcgsbl(double* a,double* b)
	{
		return *a>*b;
	}

	static rbool fclsbl(double* a,double* b)
	{
		return *a<*b;
	}

	static uint bshl(uint a,uint b)
	{
		return a<<b; 
	}

	static uint bshr(uint a,uint b)
	{
		return a>>b; 
	}

	static int bsar(int a,int b)
	{
		return a>>b; 
	}

	static void f_to_d(float a,double* b)
	{
		*b=a;
	}

	static void d_to_f(double* a,float* b)
	{
		*b=(float)(*a);
	}

	//当返回double时X86的ABI是通过xmm返回
	static void sin(double a,double* b)
	{
#ifndef __MINGW32__
		*b=::sin(a);
#endif
	}

	static void cos(double a,double* b)
	{
#ifndef __MINGW32__
		*b=::cos(a);
#endif
	}

	static void sqrt(double a,double* b)
	{
#ifndef __MINGW32__
		*b=::sqrt(a);
#endif
	}

	static void set_main_ret(int a)
	{
		get_psh()->ret_val=a;
	}

	static tfunc* get_cur_func(uchar* pasm)
	{
		taddr* p=get_psh()->s_point.find(taddr((uint)pasm,(uint)pasm+1,null));
		if(p==null)
		{
			return null;
		}
		return p->ptfi;
	}

	static rset<tclass>* get_vclass()
	{
		return &get_psh()->s_class;
	}

	static void* find_dll_q(const char* name)
	{
		tsh& sh=*get_psh();
		if(sh.dic_dll_func.exist(name))
		{
			return sh.dic_dll_func[name];
		}
		if(sh.dic_bind_func.exist(name))
		{
			return sh.dic_bind_func[name];
		}
		return find_dll_full(name);
	}

	static void* find_dll_full(const char* name)
	{
		return null;
	}

	static void z_function(tfunc* ptfi,int count,...)
	{
		tsh& sh=*get_psh();
		rbuf<var> v_var;
		for(int i=0;i<count;i++)
		{
			v_var.push(var((int64)*(&count+i+1)));
		}
		g_state.env=var(rdic<var>());
		rdic<var> table;
		table["_fenv"]=var();
		int left=ptfi->vword.find(rsoptr(c_bbk_l));
		rbuf<tword> v=ptfi->vword.sub(left+1).sub_trim(1);
		ifn(zlang::replace_control(sh,v))
		{
			rserror();
			return;
		}
		v=zlang::trans_fdef_to_sexp(sh,ptfi->vword.sub(0,left+1)+v+ptfi->vword.sub_tail(1));
		var fval(ybase::trans_v_to_s(v),table);
		zlang::eval_func(sh,g_state,v_var,fval);
		zlang::clear();
	}

	static rbool eval_vstr(rbuf<rstr>& vstr,tenv env)
	{
		rbuf<tword> v;
		for(int i=0;i<vstr.count();i++)
		{
			v+=tword(vstr[i]);
			v[i].pos.line=1;
		}
		return eval_v(v,env);
	}

	static rbool eval_v(rbuf<tword>& v,tenv env)
	{
		extern rbool r_zjit_compile_func_to_x86(tsh& sh,tfunc& tfi,tenv env);
#ifdef _MSC_VER
		tsh& sh=*zjitf::get_psh();
		ypre::replace_def(sh,sh.s_define,v);
		ifn(yclasstl::replace_type(sh,v))
		{
			rserror();
			return false;
		}
		tfunc tfi;
		tfi.ptci=sh.pmain;
		tfi.vword=v;
		tfi.pos.line=1;
		tfi.retval.type=rskey(c_int);
		tfi.retval.name=rskey(c_s_ret);
		tfi.is_cfunc=true;
		ifn(r_zjit_compile_func_to_x86(sh,tfi,env))
		{
			rserror();
			return false;
		}
		uchar* temp=tfi.code;
#ifndef _WIN64
		__asm sub esp,4
		__asm call temp
		__asm mov esi,[esp]
		__asm mov temp,esi
		__asm add esp,4
#endif
		return (int)temp;
#else
		return false;
#endif
	}

	static rbool eval(uchar* s,tenv env)
	{
		tsh& sh=*zjitf::get_psh();
		rbuf<tword> v;
		rstr temp=s;
		ifn(ypre::parse_str(sh,temp,v,null))
		{
			return false;
		}
		return eval_v(v,env);
	}

	static void init_addr_list(tsh& sh)
	{
#ifndef _RS
		rsjf("addl",addl);
		rsjf("subl",subl);
		rsjf("imull",imull);
		rsjf("idivl",idivl);
		rsjf("imodl",imodl);
		rsjf("cgsbl",cgsbl);
		rsjf("clsbl",clsbl);
		rsjf("faddl",faddl);
		rsjf("fsubl",fsubl);
		rsjf("fmull",fmull);
		rsjf("fdivl",fdivl);
		rsjf("fcgsbl",fcgsbl);
		rsjf("fclsbl",fclsbl);

		rsjf("bshl",bshl);
		rsjf("bshr",bshr);
		rsjf("bsar",bsar);

		rsjf("d_to_f",d_to_f);
		rsjf("f_to_d",f_to_d);

		rsjf("print",xf::print);
		rsjf("printf",xf::printf);
		rsjf("_vsnprintf",xf::vsnprintf);
		rsjf("sscanf",xf::sscanf);
		rsjf("sprintf",xf::sprintf);
		rsjf("sprintf64",xf::sprintf64);
		rsjf("int64toa",xf::int64toa);
		rsjf("atoint64",xf::atoint64);
		rsjf("exit",xf::exit);
		rsjf("malloc",xf::malloc);
		rsjf("free",xf::free);
		rsjf("memcpy",xf::memcpy);
		rsjf("strlen",xf::strlen);
		rsjf("system",xf::cmd);

		rsjf("ftell",xf::ftell);
		rsjf("fseek",xf::fseek);
		rsjf("_wfopen",xf::_wfopen);
		rsjf("fopen",xf::fopen);
		rsjf("fclose",xf::fclose);
		rsjf("fread",xf::fread);
		rsjf("fwrite",xf::fwrite);
		rsjf("_wremove",xf::_wremove);

		rsjf("GetCommandLineA",xf::GetCommandLineA);
		rsjf("_FindFirstFileW",xf::FindFirstFileW);
		rsjf("_FindNextFileW",xf::FindNextFileW);
		rsjf("_FindClose",xf::FindClose);
		rsjf("GetTickCount",xf::tick);
		rsjf("_getch",xf::getch);
		rsjf("MultiByteToWideChar",xf::MultiByteToWideChar);
		rsjf("Sleep",xf::sleep);

		rsjf("z_function",z_function);
		rsjf("get_cur_func",get_cur_func);
		rsjf("eval",eval);
		rsjf("eval_vstr",eval_vstr);
#endif
	}
};

tsh*& r_zjitf_get_psh()
{
	return zjitf::get_psh();
}
