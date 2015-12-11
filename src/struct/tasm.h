#pragma once

#include "../rlib/rstr.h"

struct treg
{
	uint eip_e;
	uint eip;
	uint esp_e;
	uint esp;
	uint ebp_e;
	uint ebp;
	uint esi_e;
	uint esi;
	uint edi_e;
	uint edi;
	uint eax_e;
	uint eax;
	uint ebx_e;
	uint ebx;
	uint ecx_e;
	uint ecx;
	uint edx_e;
	uint edx;

	enum 
	{
		c_eip=4,
		c_esp=12,
		c_ebp=20,
		c_esi=28,
		c_edi=36,
		c_eax=44,
		c_ebx=52,
		c_ecx=60,
		c_edx=68,

		c_rip=0,
		c_rsp=8,
		c_rbp=16,
		c_rsi=24,
		c_rdi=32,
		c_rax=40,
		c_rbx=48,
		c_rcx=56,
		c_rdx=64
	};
};

struct topnd
{
	int val;
	int off;

	enum
	{
		c_null,
		c_imme,//4
		c_reg,//ebp
		c_addr,//[ebp+4]
	};

	topnd()
	{
		clear();
	}

	void clear()
	{
		off=0;
		val=0;
	}

	int64& val64() const
	{
		return *(int64*)this;
	}

	rbool is_reg64() const
	{
		return off%8==0;
	}
};

struct tins
{
	int type;
	topnd first;
	topnd second;

	tins()
	{
		clear();
	}

	int get_index() const
	{
		return type/6;
	}

	int get_first_type() const
	{
		//<c_nop_n是单操作数或无操作数，否则是双操作数
		if(type<c_nop_n)
		{
			return type%6+1;
		}
		return type%6%2+2;
	}

	int get_second_type() const
	{
		if(type<c_nop_n)
		{
			return topnd::c_null;
		}
		return type%6/2+1;
	}

	void clear()
	{
		type=c_rjit_n;
		first.clear();
		second.clear();
	}

	rbool empty()
	{
		return type==c_rjit_n;
	}

	enum
	{
		c_rjit_n,
		c_rjit_n1,
		c_rjit_n2,
		c_rjit_n3,
		c_rjit_n4,
		c_rjit_n5,

		c_byte_i,
		c_byte_i1,
		c_byte_i2,
		c_byte_i3,
		c_byte_i4,
		c_byte_i5,

		c_calle_i,
		c_calle_i1,
		c_calle_i2,
		c_calle_i3,
		c_calle_i4,
		c_calle_i5,

		c_call_i,
		c_call_r,
		c_call_a,
		c_call_a1,
		c_call_a2,
		c_call_a3,

		c_ret_n,
		c_ret_n1,
		c_ret_n2,
		c_ret_n3,
		c_ret_n4,
		c_ret_n5,

		c_ret_i,
		c_ret_r,
		c_ret_a,
		c_ret_a1,
		c_ret_a2,
		c_ret_a3,

		c_push_i,
		c_push_r,
		c_push_a,
		c_push_a1,
		c_push_a2,
		c_push_a3,

		c_pop_i,
		c_pop_r,
		c_pop_a,
		c_pop_a1,
		c_pop_a2,
		c_pop_a3,

		c_jmp_i,
		c_jmp_r,
		c_jmp_a,
		c_jmp_a1,
		c_jmp_a2,
		c_jmp_a3,

		c_jebxz_i,
		c_jebxz_r,
		c_jebxz_a,
		c_jebxz_a1,
		c_jebxz_a2,
		c_jebxz_a3,

		c_jebxnz_i,
		c_jebxnz_r,
		c_jebxnz_a,
		c_jebxnz_a1,
		c_jebxnz_a2,
		c_jebxnz_a3,

		c_bnot_i,
		c_bnot_r,
		c_bnot_a,
		c_bnot_a1,
		c_bnot_a2,
		c_bnot_a3,

		c_halt_n,
		c_halt_n1,
		c_halt_n2,
		c_halt_n3,
		c_halt_n4,
		c_halt_n5,

		c_nop_n,
		c_nop_n1,
		c_nop_n2,
		c_nop_n3,
		c_nop_n4,
		c_nop_n5,

		c_lea_ri,
		c_lea_ai,
		c_lea_rr,
		c_lea_ar,
		c_lea_ra,
		c_lea_aa,

		c_mov_ri,
		c_mov_ai,
		c_mov_rr,
		c_mov_ar,
		c_mov_ra,
		c_mov_aa,

		c_mov8_ri,
		c_mov8_ai,
		c_mov8_rr,
		c_mov8_ar,
		c_mov8_ra,
		c_mov8_aa,

		c_mov64_ri,
		c_mov64_ai,
		c_mov64_rr,
		c_mov64_ar,
		c_mov64_ra,
		c_mov64_aa,

		c_add_ri,
		c_add_ai,
		c_add_rr,
		c_add_ar,
		c_add_ra,
		c_add_aa,

		c_sub_ri,
		c_sub_ai,
		c_sub_rr,
		c_sub_ar,
		c_sub_ra,
		c_sub_aa,
		
		c_imul_ri,
		c_imul_ai,
		c_imul_rr,
		c_imul_ar,
		c_imul_ra,
		c_imul_aa,

		c_idiv_ri,
		c_idiv_ai,
		c_idiv_rr,
		c_idiv_ar,
		c_idiv_ra,
		c_idiv_aa,

		c_imod_ri,
		c_imod_ai,
		c_imod_rr,
		c_imod_ar,
		c_imod_ra,
		c_imod_aa,

		c_cesb_ri,
		c_cesb_ai,
		c_cesb_rr,
		c_cesb_ar,
		c_cesb_ra,
		c_cesb_aa,

		c_cnesb_ri,
		c_cnesb_ai,
		c_cnesb_rr,
		c_cnesb_ar,
		c_cnesb_ra,
		c_cnesb_aa,

		c_cgsb_ri,
		c_cgsb_ai,
		c_cgsb_rr,
		c_cgsb_ar,
		c_cgsb_ra,
		c_cgsb_aa,

		c_cgesb_ri,
		c_cgesb_ai,
		c_cgesb_rr,
		c_cgesb_ar,
		c_cgesb_ra,
		c_cgesb_aa,

		c_clsb_ri,
		c_clsb_ai,
		c_clsb_rr,
		c_clsb_ar,
		c_clsb_ra,
		c_clsb_aa,

		c_clesb_ri,
		c_clesb_ai,
		c_clesb_rr,
		c_clesb_ar,
		c_clesb_ra,
		c_clesb_aa,

		c_band_ri,
		c_band_ai,
		c_band_rr,
		c_band_ar,
		c_band_ra,
		c_band_aa,

		c_bor_ri,
		c_bor_ai,
		c_bor_rr,
		c_bor_ar,
		c_bor_ra,
		c_bor_aa,

		c_bxor_ri,
		c_bxor_ai,
		c_bxor_rr,
		c_bxor_ar,
		c_bxor_ra,
		c_bxor_aa,

		c_bshl_ri,
		c_bshl_ai,
		c_bshl_rr,
		c_bshl_ar,
		c_bshl_ra,
		c_bshl_aa,

		c_bshr_ri,
		c_bshr_ai,
		c_bshr_rr,
		c_bshr_ar,
		c_bshr_ra,
		c_bshr_aa,

		c_bsar_ri,
		c_bsar_ai,
		c_bsar_rr,
		c_bsar_ar,
		c_bsar_ra,
		c_bsar_aa,

		c_udiv_ri,
		c_udiv_ai,
		c_udiv_rr,
		c_udiv_ar,
		c_udiv_ra,
		c_udiv_aa,

		c_umod_ri,
		c_umod_ai,
		c_umod_rr,
		c_umod_ar,
		c_umod_ra,
		c_umod_aa,
	
		c_ucgsb_ri,
		c_ucgsb_ai,
		c_ucgsb_rr,
		c_ucgsb_ar,
		c_ucgsb_ra,
		c_ucgsb_aa,

		c_ucgesb_ri,
		c_ucgesb_ai,
		c_ucgesb_rr,
		c_ucgesb_ar,
		c_ucgesb_ra,
		c_ucgesb_aa,

		c_uclsb_ri,
		c_uclsb_ai,
		c_uclsb_rr,
		c_uclsb_ar,
		c_uclsb_ra,
		c_uclsb_aa,

		c_uclesb_ri,
		c_uclesb_ai,
		c_uclesb_rr,
		c_uclesb_ar,
		c_uclesb_ra,
		c_uclesb_aa,
	};
};

struct tfile;

struct tpos
{
	tfile* file;
	int line;

	tpos()
	{
		clear();
	}

	void clear()
	{
		file=null;
		line=0;
	}

	rbool empty()
	{
		return file==null;
	}

	friend rbool operator==(const tpos& a,const tpos& b)
	{
		return a.file==b.file&&a.line==b.line;
	}

	friend rbool operator!=(const tpos& a,const tpos& b)
	{
		return a.file!=b.file||a.line!=b.line;
	}
};

struct tfunc;

struct tasm
{
	tins ins;//tins放在最前面便于把指针转换为tasm*
	rbuf<rstr> vstr;
	tpos pos;
	tfunc* ptfi;//反射到tfunc，不用初始化
	uchar* start;//jit地址

	tasm()
	{
		clear();
	}

	tasm(const rbuf<rstr>& vstr)
	{
		clear();
		this->vstr=vstr;
	}

	tasm(const rbuf<rstr>& vstr,tpos pos)
	{
		clear();
		this->vstr=vstr;
		this->pos=pos;
	}

	tasm(const tasm& a)
	{
		ins=a.ins;
		vstr=a.vstr;
		pos=a.pos;
		ptfi=a.ptfi;
		start=a.start;
	}

#ifdef SUPPORT_MOVE
	tasm(tasm&& a)
	{
		ins=a.ins;
		vstr=a.vstr;
		pos=a.pos;
		ptfi=a.ptfi;
		start=a.start;
	}

	void operator=(tasm&& a)
	{
		ins=a.ins;
		vstr=a.vstr;
		pos=a.pos;
		ptfi=a.ptfi;
		start=a.start;
	}
#endif

	void operator=(const tasm& a)
	{
		ins=a.ins;
		vstr=a.vstr;
		pos=a.pos;
		ptfi=a.ptfi;
		start=a.start;
	}

	void clear()
	{
		ins.clear();
		vstr.clear();
		pos.clear();
		ptfi=null;
		start=null;
	}
};

//优化节点
struct top_node
{
	rbuf<tasm> src;
	rbuf<tasm> dst;

	void clear()
	{
		src.clear();
		dst.clear();
	}
};
