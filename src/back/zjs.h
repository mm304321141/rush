#pragma once

#include "../rlib/rhash.h"
#include "znasm.h"

struct zjs
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
		rbuf<rstr> vconst;
		ifn(proc_func(sh,*ptfi,result,vconst))
		{
			return false;
		}
		int mem_size=100*1024*1024;
		int stack_size=2*1024*1024;
		rstr head;

		head+="_PMAIN_A=0\n";
		head+="\n";
		head+="mem_size="+rstr(mem_size)+"\n";
		head+="stack_size="+rstr(stack_size)+"\n";
		head+="\n";

		head+="mem=new ArrayBuffer(mem_size)\n";
		head+="mem32=new Int32Array(mem)\n";
		head+="mem8=new Uint8Array(mem)\n";
		head+="mem32u=new Uint32Array(mem)\n";
		head+="\n";

		head+="esp=mem32.length\n";
		head+="ebp=0\n";
		head+="esi=0\n";
		head+="edi=0\n";
		head+="eax=0\n";
		head+="ebx=0\n";
		head+="ecx=0\n";
		head+="edx=0\n";
		head+="\n";

		head+="function div(a,b){return parseInt(a/b)}\n";
		head+="function set_str(index,n,s){for(var i=0;i<n-1;i++)";
		head+="{mem8[index+i]=s.charCodeAt(i);}mem8[index+i]=0;}\n";
		head+="\n";

		int const_size=0;
		for(int i=0;i<vconst.count();i++)
		{
			const_size+=vconst[i].size()-1;
			int start=mem_size-stack_size-const_size;
			head+="_RC_"+rstr(i)+"="+start+";set_str("+rstr(start);
			head+=","+rstr(vconst[i].size()-1)+","+vconst[i]+")\n";
		}
		head+="\n";
		
		head+="const_size="+rstr(const_size)+"\n";
		head+="\n";

		head+="alloc_count=new Int32Array(new ArrayBuffer(";
		head+="div(div(mem_size-stack_size-const_size,4096),4)*4))\n";
		head+="for(var g_i=0;g_i<alloc_count.length;g_i++){alloc_count[g_i]=0}\n";
		head+="\n";

		head+="main2Emain2829()\n";

		head+=result;

		rstr name=ybase::get_main_name(sh)+".js";
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
		rstr cont="<!doctype html><html><head><title>The Rush Language</title></head><body>\n";
		cont+="<script type=\"text/javascript\" src=\"";
		cont+=rdir::get_name(ybase::get_main_name(sh));
		cont+=".js\">\n</script></body></html>\n";
		rfile::write_all_n(ybase::get_main_name(sh)+".html",cont);
		return true;
	}

	static uint get_hash(const rstr& s)
	{
		return rhash<int>::bkdr_hash32(s.begin(),s.count());
	}

	static rbool proc_func(tsh& sh,tfunc& tfi,rstr& result,rbuf<rstr>& vconst)
	{
		if(tfi.count==1)
		{
			return true;
		}
		tfi.count=1;
		rstr symbol=znasm::get_nasm_symbol(tfi);
		if(tfi.is_final)
		{
			result+="\nfunction "+symbol+"(";
			result+="){\n";
			result+=ybase::trans_vword_to_s(tfi.vword);
			result+="\n}\n";
			return true;
		}
		if(tfi.vasm.empty())
		{
			if(!zbin::compile_vword_to_vasm(sh,tfi,tenv()))
			{
				return false;
			}
		}
		result+="\nfunction "+symbol+"(){\n";
		result+="var tag=0\n";
		result+="while(true){switch(tag){\n";
		result+="case 0:\n";
		for(int i=0;i<tfi.vasm.count();i++)
		{
			if(ybase::is_tag<rstr>(tfi.vasm[i].vstr))
			{
				result+=rstr("case ")+get_hash(symbol+"_"+tfi.vasm[i].vstr[0])+":\n";
				continue;
			}
			rstr s;
			ifn(proc_asm(sh,tfi,tfi.vasm[i],s,vconst))
			{
				rserror(tfi.vasm[i]);
				rserror(tfi);
				return false;
			}
			result+=s;
		}
		result+="}}}\n";
		for(int i=0;i<tfi.vasm.count();i++)
		{
			tfunc* ptfi=znasm::find_call(sh,tfi.vasm[i]);
			if(ptfi==null)
			{
				continue;
			}
			ifn(proc_func(sh,*ptfi,result,vconst))
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
			return v[0];
		}
		if(v.count()==3)
		{
			if(sh.key.is_asm_reg(v[1]))
			{
				return "mem32["+v[1]+"/4]";
			}
			return v[0]+v[1]+v[2];
		}
		if(v.count()==5)
		{
			if(sh.key.is_asm_reg(v[1])&&v[3].is_number())
			{
				return "mem32[("+v[1]+v[2]+v[3]+")/4]";
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
				return "mem32u["+v[1]+"/4]";
			}
			return v[0]+v[1]+v[2];
		}
		if(v.count()==5)
		{
			if(sh.key.is_asm_reg(v[1])&&v[3].is_number())
			{
				return "mem32u[("+v[1]+v[2]+v[3]+")/4]";
			}
		}
		rstr::join<rstr>(v,"").printl();
		return rstr();
	}
	
	static rstr opnd_8(const rbuf<rstr>& v)
	{
		if(v.count()==3)
		{
			return "mem8["+v[1]+"]";
		}
		if(v.count()==5)
		{
			return "mem8["+v[1]+v[2]+v[3]+"]";
		}
		rstr::join<rstr>(v,"").printl();
		return rstr();
	}

	static void proc_const_str(rbuf<rstr>& vstr,rbuf<rstr>& vconst)
	{
		if(vstr.count()>=2&&vstr[0]=="calle")
		{
			return;
		}
		for(int i=0;i<vstr.count();i++)
		{
			if(vstr[i].get_bottom()==r_char('\"')&&vstr[i].count()>=2)
			{
				vconst.push(vstr[i]);
				vstr[i]="_RC_"+rstr(vconst.count()-1);
			}
		}
	}

	static rbool proc_asm(tsh& sh,tfunc& tfi,tasm& item,rstr& result,rbuf<rstr>& vconst)
	{
		rbuf<rstr>& vstr=item.vstr;
		proc_const_str(vstr,vconst);
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
			result+="	esp-=4\n";
			result+="	"+znasm::get_nasm_symbol(*ptfi)+"()\n";
			return true;
		case tkey::c_retn:
			result+="	esp+=4\n";
			result+="	return\n";
			return true;
		case tkey::c_reti:
			result+="	esp+="+rstr(vstr.get(1).toint()+4)+"\n";
			result+="	return\n";
			return true;
		case tkey::c_push:
			ptfi=znasm::find_call(sh,item);
			if(ptfi==null)
			{
				result+="	esp-=4\n";
				result+="	mem32[esp/4]="+opnd(sh,vstr.sub(1))+"\n";
			}
			else
			{
				result+="	push "+znasm::get_nasm_symbol(*ptfi)+"\n";
			}
			return true;
		case tkey::c_pop:
			result+="	"+opnd(sh,vstr.sub(1))+"=mem32[esp/4]\n";
			result+="	esp+=4\n";
			return true;
		case tkey::c_jmp:
			result+=("	tag="+
				rstr(get_hash(znasm::get_nasm_symbol(tfi)+"_"+vstr.get(1)))+";continue\n");
			return true;
		case tkey::c_jebxz:
			result+=("	if(ebx===0){tag="+
				rstr(get_hash(znasm::get_nasm_symbol(tfi)+"_"+vstr.get(1)))+";continue}\n");
			return true;
		case tkey::c_jebxnz:
			result+=("	if(ebx!==0){tag="+
				rstr(get_hash(znasm::get_nasm_symbol(tfi)+"_"+vstr.get(1)))+";continue}\n");
			return true;
		case tkey::c_nop:
			return true;
		case tkey::c_lea:
			result+=("	"+opnd(sh,znasm::get_opnd1_v(vstr))+"="+
				opnd(sh,znasm::get_opnd2_v(vstr).sub(1,4))+"\n");
			return true;
		case tkey::c_mov:
			if(znasm::count_mbk_l(vstr)==2)
			{
				result+=("	"+opnd(sh,znasm::get_opnd1_v(vstr))+"="+
					opnd(sh,znasm::get_opnd2_v(vstr))+"\n");
				return true;
			}
			ptfi=znasm::find_call(sh,item);
			if(ptfi==null)
			{
				result+=("	"+opnd(sh,znasm::get_opnd1_v(vstr))+"="+
					opnd(sh,znasm::get_opnd2_v(vstr))+"\n");
			}
			else
			{
				result+=("	mov dword "+znasm::get_opnd1(vstr)+" , "+
					znasm::get_nasm_symbol(*ptfi)+"\n");
			}
			return true;
		case tkey::c_mov8:
			result+="	"+opnd_8(znasm::get_opnd1_v(vstr))+"="+opnd_8(znasm::get_opnd2_v(vstr))+"\n";
			return true;
		case tkey::c_mov64:
			return false;
		case tkey::c_add:
			result+=("	"+opnd(sh,znasm::get_opnd1_v(vstr))+"+="+
				opnd(sh,znasm::get_opnd2_v(vstr))+"\n");
			return true;
		case tkey::c_sub:
			result+=("	"+opnd(sh,znasm::get_opnd1_v(vstr))+"-="+
				opnd(sh,znasm::get_opnd2_v(vstr))+"\n");
			return true;
		case tkey::c_imul:
			result+=("	"+opnd(sh,znasm::get_opnd1_v(vstr))+"*="+
				opnd(sh,znasm::get_opnd2_v(vstr))+"\n");
			return true;
		case tkey::c_idiv:
			result+=("	"+opnd(sh,znasm::get_opnd1_v(vstr))+"=div("+
				opnd(sh,znasm::get_opnd1_v(vstr))+","+
				opnd(sh,znasm::get_opnd2_v(vstr))+")\n");
			return true;
		case tkey::c_imod:
			result+=("	"+opnd(sh,znasm::get_opnd1_v(vstr))+"%="+
				opnd(sh,znasm::get_opnd2_v(vstr))+"\n");
			return true;
		case tkey::c_cesb:
			result+=("	ebx=Number("+opnd(sh,znasm::get_opnd1_v(vstr))+"==="+
				opnd(sh,znasm::get_opnd2_v(vstr))+")\n");
			return true;
		case tkey::c_cnesb:
			result+=("	ebx=Number("+opnd(sh,znasm::get_opnd1_v(vstr))+"!=="+
				opnd(sh,znasm::get_opnd2_v(vstr))+")\n");
			return true;
		case tkey::c_cgsb:
			result+=("	ebx=Number("+opnd(sh,znasm::get_opnd1_v(vstr))+">"+
				opnd(sh,znasm::get_opnd2_v(vstr))+")\n");
			return true;
		case tkey::c_cgesb:
			result+=("	ebx=Number("+opnd(sh,znasm::get_opnd1_v(vstr))+">="+
				opnd(sh,znasm::get_opnd2_v(vstr))+")\n");
			return true;
		case tkey::c_clsb:
			result+=("	ebx=Number("+opnd(sh,znasm::get_opnd1_v(vstr))+"<"+
				opnd(sh,znasm::get_opnd2_v(vstr))+")\n");
			return true;
		case tkey::c_clesb:
			result+=("	ebx=Number("+opnd(sh,znasm::get_opnd1_v(vstr))+"<="+
				opnd(sh,znasm::get_opnd2_v(vstr))+")\n");
			return true;
		case tkey::c_band:
			if(znasm::count_mbk_l(vstr)==2)
			{
				result+="	mov ecx , "+znasm::get_opnd2(vstr)+"\n";
				result+="	and "+znasm::get_opnd1(vstr)+" , ecx\n";
			}
			else
			{
				result+="	and dword "+znasm::link_vstr(vstr.sub(1))+"\n";
			}
			return true;
		case tkey::c_bor:
			if(znasm::count_mbk_l(vstr)==2)
			{
				result+="	mov ecx , "+znasm::get_opnd2(vstr)+"\n";
				result+="	or "+znasm::get_opnd1(vstr)+" , ecx\n";
			}
			else
			{
				result+="	or dword "+znasm::link_vstr(vstr.sub(1))+"\n";
			}
			return true;
		case tkey::c_bnot:
			result+="	not dword "+znasm::link_vstr(vstr.sub(1))+"\n";
			return true;
		case tkey::c_bxor:
			if(znasm::count_mbk_l(vstr)==2)
			{
				result+="	mov ecx , "+znasm::get_opnd2(vstr)+"\n";
				result+="	xor "+znasm::get_opnd1(vstr)+" , ecx\n";
			}
			else
			{
				result+="	xor dword "+znasm::link_vstr(vstr.sub(1))+"\n";
			}
			return true;
		case tkey::c_bshl:
			return false;
		case tkey::c_bshr:
			return false;
		case tkey::c_bsar:
			return false;
		case tkey::c_udiv:
			result+=("	"+opnd_u(sh,znasm::get_opnd1_v(vstr))+"=div("+
				opnd_u(sh,znasm::get_opnd1_v(vstr))+","+
				opnd_u(sh,znasm::get_opnd2_v(vstr))+")\n");
			return true;
		case tkey::c_umod:
			result+=("	"+opnd_u(sh,znasm::get_opnd1_v(vstr))+"%="+
				opnd_u(sh,znasm::get_opnd2_v(vstr))+"\n");
			return true;
		case tkey::c_ucgsb:
			result+=("	ebx=Number("+opnd_u(sh,znasm::get_opnd1_v(vstr))+">"+
				opnd_u(sh,znasm::get_opnd2_v(vstr))+")\n");
			return true;
		case tkey::c_ucgesb:
			result+=("	ebx=Number("+opnd_u(sh,znasm::get_opnd1_v(vstr))+">="+
				opnd_u(sh,znasm::get_opnd2_v(vstr))+")\n");
			return true;
		case tkey::c_uclsb:
			result+=("	ebx=Number("+opnd_u(sh,znasm::get_opnd1_v(vstr))+"<"+
				opnd_u(sh,znasm::get_opnd2_v(vstr))+")\n");
			return true;
		case tkey::c_uclesb:
			result+=("	ebx=Number("+opnd_u(sh,znasm::get_opnd1_v(vstr))+"<="+
				opnd_u(sh,znasm::get_opnd2_v(vstr))+")\n");
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
