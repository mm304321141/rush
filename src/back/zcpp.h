#pragma once

#include "../rlib/rhash.h"
#include "znasm.h"

struct zcpp
{
	static rbool proc(tsh& sh)
	{
		tfunc* ptfi=yfind::find_func(*sh.pmain,"main");
		if(ptfi==null)
		{
			rf::printl("main not find");
			return false;
		}
		rstr result;
		rbuf<tfunc*> vfunc;
		ifn(proc_func(sh,*ptfi,result,vfunc))
		{
			return false;
		}
		int mem_size=100*1024*1024;
		int stack_size=2*1024*1024;
		rstr head;

		head+="#include <stdio.h>\n";
		head+="#include <stdlib.h>\n";
		head+="#include <string.h>\n";
		head+="\n";

		head+="int _PMAIN_A=0;\n";
		head+="\n";
		head+="#define stack_size "+rstr(stack_size)+"\n";
		head+="\n";

		head+="unsigned char stack[stack_size];\n";
		head+="#define mem32(a) (*(int*)(a))\n";
		head+="#define mem8(a) (*(unsigned char*)(a))\n";
		head+="#define mem32u(a) (*(unsigned int*)(a))\n";
		head+="#define CHAR char\n";
		head+="#define VOID void\n";
		head+="\n";

		head+="int esp=(int)stack+stack_size;\n";
		head+="int ebp=0;\n";
		head+="int esi=0;\n";
		head+="int edi=0;\n";
		head+="int eax=0;\n";
		head+="int ebx=0;\n";
		head+="int ecx=0;\n";
		head+="int edx=0;\n";
		head+="\n";

		for(int i=0;i<vfunc.count();i++)
		{
			head+="void "+znasm::get_nasm_symbol(*vfunc[i])+"();\n";
		}
		head+="\n";

		head+="int main(){main2Emain2829();return 0;}\n";

		head+=result;

		rstr name=ybase::get_main_name(sh)+".cpp";
		rfile file;
		ifn(file.open_n(name,"rw"))
		{
			rf::printl("file open error");
			return false;
		}
		ifn(file.write(head.size(),head.begin()))
		{
			rf::printl("file write error");
			return false;
		}
		return true;
	}

	static rbool proc_func(tsh& sh,tfunc& tfi,rstr& result,rbuf<tfunc*>& vfunc)
	{
		if(tfi.count==1)
		{
			return true;
		}
		tfi.count=1;
		vfunc.push(&tfi);
		rstr symbol=znasm::get_nasm_symbol(tfi);
		if(tfi.is_final)
		{
			result+="\nvoid "+symbol+"(";
			result+="){\n";
			result+=ybase::trans_vword_to_s(tfi.vword);
			result+="\n}\n";
			return true;
		}
		if(tfi.vasm.empty())
		{
			ifn(zbin::compile_vword_to_vasm(sh,tfi,tenv()))
			{
				return false;
			}
		}
		result+="\nvoid "+symbol+"(){\n";
		for(int i=0;i<tfi.vasm.count();i++)
		{
			if(ybase::is_tag<rstr>(tfi.vasm[i].vstr))
			{
				result+=symbol+"_"+tfi.vasm[i].vstr[0]+":\n";
				continue;
			}
			rstr s;
			ifn(proc_asm(sh,tfi,tfi.vasm[i],s))
			{
				rserror(tfi.vasm[i]);
				rserror(tfi);
				return false;
			}
			result+=s;
		}
		result+="}\n";
		for(int i=0;i<tfi.vasm.count();i++)
		{
			tfunc* ptfi=znasm::find_call(sh,tfi.vasm[i]);
			if(ptfi==null)
			{
				continue;
			}
			ifn(proc_func(sh,*ptfi,result,vfunc))
			{
				return false;
			}
		}
		return true;
	}

	static rstr opnd(tsh& sh,const rbuf<rstr>& v)
	{
		if(v.count()==1)
		{
			if(v[0].get_left()==r_char('\"'))
			{
				return "((int)"+v[0]+")";
			}
			return v[0];
		}
		if(v.count()==3)
		{
			if(sh.key.is_asm_reg(v[1]))
			{
				return "mem32("+v[1]+")";
			}
			return v[0]+v[1]+v[2];
		}
		if(v.count()==5)
		{
			if(sh.key.is_asm_reg(v[1])&&v[3].is_number())
			{
				return "mem32("+v[1]+v[2]+v[3]+")";
			}
		}
		rstr::join<rstr>(v,"").printl();
		return rstr();
	}

	static rstr opnd_u(tsh& sh,const rbuf<rstr>& v)
	{
		if(v.count()==1)
		{
			return v[0];
		}
		if(v.count()==3)
		{
			if(sh.key.is_asm_reg(v[1]))
			{
				return "mem32u("+v[1]+")";
			}
			return v[0]+v[1]+v[2];
		}
		if(v.count()==5)
		{
			if(sh.key.is_asm_reg(v[1])&&v[3].is_number())
			{
				return "mem32u("+v[1]+v[2]+v[3]+")";
			}
		}
		rstr::join<rstr>(v,"").printl();
		return rstr();
	}

	static rstr opnd_8(const rbuf<rstr>& v)
	{
		if(v.count()==3)
		{
			return "mem8("+v[1]+")";
		}
		if(v.count()==5)
		{
			return "mem8("+v[1]+v[2]+v[3]+")";
		}
		rstr::join<rstr>(v,"").printl();
		return rstr();
	}

	static rbool proc_asm(tsh& sh,tfunc& tfi,tasm& item,rstr& result)
	{
		rbuf<rstr>& vstr=item.vstr;
		if(vstr.empty())
		{
			return false;
		}
		int type=sh.key.get_key_index(vstr[0]);
		tfunc* ptfi;
		switch(type)
		{
		case tkey::c_calle:
			result+="	invoke("+ybase::del_quote(vstr.get(1))+")\n";
			return true;
		case tkey::c_call:
			ptfi=znasm::find_call(sh,item);
			if(ptfi==null)
			{
				result+=("	call dword "+
					znasm::link_vstr(vstr.sub(1))+"\n");
				return true;
			}
			result+="	esp-=4;\n";
			result+="	"+znasm::get_nasm_symbol(*ptfi)+"();\n";
			return true;
		case tkey::c_retn:
			result+="	esp+=4;\n";
			return true;
		case tkey::c_reti:
			result+="	esp+="+rstr(vstr.get(1).toint()+4)+";\n";
			return true;
		case tkey::c_push:
			ptfi=znasm::find_call(sh,item);
			if(ptfi==null)
			{
				result+="	esp-=4;\n";
				result+="	mem32(esp)="+opnd(sh,vstr.sub(1))+";\n";
			}
			else
			{
				result+="	push "+znasm::get_nasm_symbol(*ptfi)+"\n";
			}
			return true;
		case tkey::c_pop:
			result+="	"+opnd(sh,vstr.sub(1))+"=mem32(esp);\n";
			result+="	esp+=4;\n";
			return true;
		case tkey::c_jmp:
			result+="	goto "+znasm::get_nasm_symbol(tfi)+"_"+vstr.get(1)+";\n";
			return true;
		case tkey::c_jebxz:
			result+="	if(ebx==0){goto "+znasm::get_nasm_symbol(tfi)+"_"+vstr.get(1)+";}\n";
			return true;
		case tkey::c_jebxnz:
			result+="	if(ebx){goto "+znasm::get_nasm_symbol(tfi)+"_"+vstr.get(1)+";}\n";
			return true;
		case tkey::c_nop:
			return true;
		case tkey::c_lea:
			result+=("	"+opnd(sh,znasm::get_opnd1_v(vstr))+"="+
				opnd(sh,znasm::get_opnd2_v(vstr).sub(1,4))+";\n");
			return true;
		case tkey::c_mov:
			if(znasm::count_mbk_l(vstr)==2)
			{
				result+=("	"+opnd(sh,znasm::get_opnd1_v(vstr))+"="+
					opnd(sh,znasm::get_opnd2_v(vstr))+";\n");
				return true;
			}
			ptfi=znasm::find_call(sh,item);
			if(ptfi==null)
			{
				result+=("	"+opnd(sh,znasm::get_opnd1_v(vstr))+"="+
					opnd(sh,znasm::get_opnd2_v(vstr))+";\n");
			}
			else
			{
				result+=("	mov dword "+znasm::get_opnd1(vstr)+" , "+
					znasm::get_nasm_symbol(*ptfi)+"\n");
			}
			return true;
		case tkey::c_mov8:
			result+="	"+opnd_8(znasm::get_opnd1_v(vstr))+"="+opnd_8(znasm::get_opnd2_v(vstr))+";\n";
			return true;
		case tkey::c_mov64:
			return false;
		case tkey::c_add:
			result+=("	"+opnd(sh,znasm::get_opnd1_v(vstr))+"+="+
				opnd(sh,znasm::get_opnd2_v(vstr))+";\n");
			return true;
		case tkey::c_sub:
			result+=("	"+opnd(sh,znasm::get_opnd1_v(vstr))+"-="+
				opnd(sh,znasm::get_opnd2_v(vstr))+";\n");
			return true;
		case tkey::c_imul:
			result+=("	"+opnd(sh,znasm::get_opnd1_v(vstr))+"*="+
				opnd(sh,znasm::get_opnd2_v(vstr))+";\n");
			return true;
		case tkey::c_idiv:
			result+=("	"+opnd(sh,znasm::get_opnd1_v(vstr))+"/="+
				opnd(sh,znasm::get_opnd2_v(vstr))+";\n");
			return true;
		case tkey::c_imod:
			result+=("	"+opnd(sh,znasm::get_opnd1_v(vstr))+"%="+
				opnd(sh,znasm::get_opnd2_v(vstr))+";\n");
			return true;
		case tkey::c_cesb:
			result+=("	ebx="+opnd(sh,znasm::get_opnd1_v(vstr))+"=="+
				opnd(sh,znasm::get_opnd2_v(vstr))+";\n");
			return true;
		case tkey::c_cnesb:
			result+=("	ebx="+opnd(sh,znasm::get_opnd1_v(vstr))+"!="+
				opnd(sh,znasm::get_opnd2_v(vstr))+";\n");
			return true;
		case tkey::c_cgsb:
			result+=("	ebx="+opnd(sh,znasm::get_opnd1_v(vstr))+">"+
				opnd(sh,znasm::get_opnd2_v(vstr))+";\n");
			return true;
		case tkey::c_cgesb:
			result+=("	ebx="+opnd(sh,znasm::get_opnd1_v(vstr))+">="+
				opnd(sh,znasm::get_opnd2_v(vstr))+";\n");
			return true;
		case tkey::c_clsb:
			result+=("	ebx="+opnd(sh,znasm::get_opnd1_v(vstr))+"<"+
				opnd(sh,znasm::get_opnd2_v(vstr))+";\n");
			return true;
		case tkey::c_clesb:
			result+=("	ebx="+opnd(sh,znasm::get_opnd1_v(vstr))+"<="+
				opnd(sh,znasm::get_opnd2_v(vstr))+";\n");
			return true;
		case tkey::c_band:
			result+=("	"+opnd(sh,znasm::get_opnd1_v(vstr))+"&="+
				opnd(sh,znasm::get_opnd2_v(vstr))+";\n");
			return true;
		case tkey::c_bor:
			result+=("	"+opnd(sh,znasm::get_opnd1_v(vstr))+"|="+
				opnd(sh,znasm::get_opnd2_v(vstr))+";\n");
			return true;
		case tkey::c_bnot:
			result+="	not dword "+znasm::link_vstr(vstr.sub(1))+"\n";
			return true;
		case tkey::c_bxor:
			result+=("	"+opnd(sh,znasm::get_opnd1_v(vstr))+"^="+
				opnd(sh,znasm::get_opnd2_v(vstr))+";\n");
			return true;
		case tkey::c_bshl:
			return false;
		case tkey::c_bshr:
			return false;
		case tkey::c_bsar:
			return false;
		case tkey::c_udiv:
			result+=("	"+opnd_u(sh,znasm::get_opnd1_v(vstr))+"/="+
				opnd_u(sh,znasm::get_opnd2_v(vstr))+";\n");
			return true;
		case tkey::c_umod:
			result+=("	"+opnd_u(sh,znasm::get_opnd1_v(vstr))+"%="+
				opnd_u(sh,znasm::get_opnd2_v(vstr))+";\n");
			return true;
		case tkey::c_ucgsb:
			result+=("	ebx="+opnd_u(sh,znasm::get_opnd1_v(vstr))+">"+
				opnd_u(sh,znasm::get_opnd2_v(vstr))+";\n");
			return true;
		case tkey::c_ucgesb:
			result+=("	ebx="+opnd_u(sh,znasm::get_opnd1_v(vstr))+">="+
				opnd_u(sh,znasm::get_opnd2_v(vstr))+";\n");
			return true;
		case tkey::c_uclsb:
			result+=("	ebx="+opnd_u(sh,znasm::get_opnd1_v(vstr))+"<"+
				opnd_u(sh,znasm::get_opnd2_v(vstr))+";\n");
			return true;
		case tkey::c_uclesb:
			result+=("	ebx="+opnd_u(sh,znasm::get_opnd1_v(vstr))+"<="+
				opnd_u(sh,znasm::get_opnd2_v(vstr))+";\n");
			return true;
		case tkey::c_rn:
			if(vstr.count()==3&&
				!sh.key.is_asm_reg(vstr[2])&&
				znasm::is_jmp_ins_nasm(vstr[1]))
			{
				result+=("	"+vstr[1]+" "+znasm::get_nasm_symbol(tfi)+"_"+
					vstr.top()+"\n");
			}
			else
			{
				result+="	"+znasm::link_vstr(vstr.sub(1))+"\n";
			}
			return true;
		}
		return false;
	}
};
