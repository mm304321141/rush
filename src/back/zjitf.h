#pragma once

#include "../struct/tbase.h"

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
#endif
	}
};
