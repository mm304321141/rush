﻿#pragma once

#include "../front/ysent.h"
#include "../front/yword.h"
#include "zasm.h"

//生成二进制代码类
struct zbin
{
	//将一个函数翻译成二进制代码
	static rbool proc(tsh& sh,tfunc& tfi)
	{
		ifn(tfi.vasm.empty())
		{
			return true;
		}
		ifn(compile_vword_to_vasm(sh,tfi,tenv()))
		{
			return false;
		}
		for(int i=0;i<tfi.vasm.count();i++)
		{
			tfi.vasm[i].ptfi=&tfi;
			ifn(proc_asm(sh,tfi.vasm,tfi.vasm[i]))
			{
				rserror(tfi.vasm[i],"asm error");
				return false;
			}
		}
		return true;
	}

	//从函数的词表编译到vasm
	static rbool compile_vword_to_vasm(tsh& sh,tfunc& tfi,tenv env)
	{
		if(tfi.is_asm||tfi.is_extern)
		{
			return true;
		}
		ifn(ysent::proc(sh,tfi,env))
		{
			rserror(tfi,"sent error");
			return false;
		}
		ifn(zasm::proc(sh,tfi))
		{
			rserror(tfi,"asm error");
			return false;
		}
		return true;
	}

	static rbool proc_vasm(tsh& sh,rbuf<tasm>& vasm)
	{
		for(int i=0;i<vasm.count();i++)
		{
			ifn(proc_asm(sh,vasm[i]))
			{
				return false;
			}
		}
		return true;
	}

	static rbool proc_asm(tsh& sh,tasm& oasm)
	{
		rbuf<tasm> vasm;
		return proc_asm(sh,vasm,oasm);
	}
	
	//将汇编代码翻译成二进制代码
	static rbool proc_asm(tsh& sh,rbuf<tasm>& vasm,tasm& oasm)
	{
		if(ybase::is_tag<rstr>(oasm.vstr))
		{
			oasm.ins.type=tins::c_nop_n;
			return true;
		}
		oasm.ins.type=sh.key.get_key_index(oasm.vstr.get_bottom());
		if(oasm.ins.type>tkey::c_rn)
		{
			rserror(oasm);
			return false;
		}
		if(oasm.vstr.count()==2&&
			ybase::is_jmp_ins(oasm.ins.type)&&
			!sh.key.is_asm_reg(oasm.vstr[1]))
		{
			int line=oasm.vstr[1].toint();
			int i;
			for(i=0;i<vasm.count();i++)
			{
				if(ybase::is_tag<rstr>(vasm[i].vstr)&&
					vasm[i].vstr[0]==oasm.vstr[1])
				{
					break;
				}
			}
			if(i>=vasm.count())
			{
				rserror(oasm);
				return false;
			}
			oasm.ins.type*=6;
			oasm.ins.first.val=(uint)(&vasm[i]);
			return true;
		}
		oasm.ins.type*=6;
		ifn(parse_asm(sh,oasm))
		{
			rf::printl(oasm.ptfi->name_dec);
			rserror(oasm);
			return false;
		}
		return true;
	}

	static int find_comma(tsh& sh,rbuf<rstr>& v)
	{
		int count1=0;
		int count2=0;
		int i;
		for(i=1;i<v.count();i++)
		{
			if(rsoptr(c_sbk_l)==v[i])
			{
				count1++;
			}
			elif(rsoptr(c_sbk_r)==v[i])
			{
				count1--;
			}
			elif(rsoptr(c_mbk_l)==v[i])
			{
				count2++;
			}
			elif(rsoptr(c_mbk_r)==v[i])
			{
				count2--;
			}
			elif(count1==0&&count2==0&&v[i]==rsoptr(c_comma))
			{
				break;
			}
		}
		return i;
	}

	static rbool parse_asm(tsh& sh,tasm& item)
	{
		int i=find_comma(sh,item.vstr);
		int first;
		ifn(parse_opnd(sh,item,i-1,item.vstr.sub(1,i),item.ins.first,first))
		{
			rserror();
			return false;
		}
		int second;
		ifn(parse_opnd(sh,item,i+1,item.vstr.sub(i+1),item.ins.second,second))
		{
			rserror();
			return false;
		}
		ifn(obtain_qrun_type(item.ins,first,second))
		{
			rserror();
			return false;
		}
		if(item.ins.type==tins::c_calle_i)
		{
			ifn(sh.func_list.exist((char*)(item.ins.first.val)))
			{
				rserror((char*)(item.ins.first.val));
				return false;
			}
			item.ins.first.val=(int)(sh.func_list[(char*)(item.ins.first.val)]);
		}
		return true;
	}

	static rbool obtain_qrun_type(tins& ins,int first,int second)
	{
		if(second==topnd::c_null)
		{
			if(first==topnd::c_null)
			{
				;
			}
			elif(first==topnd::c_imme)
			{
				;
			}
			elif(first==topnd::c_reg)
			{
				ins.type+=1;
			}
			elif(first==topnd::c_addr)
			{
				ins.type+=2;
			}
			else
			{
				return false;
			}
		}
		else
		{
			if(second==topnd::c_imme)
			{
				if(first==topnd::c_reg)
				{
					;
				}
				elif(first==topnd::c_addr)
				{
					ins.type+=1;
				}
				else
				{
					;
				}
			}
			elif(second==topnd::c_reg)
			{
				if(first==topnd::c_reg)
				{
					ins.type+=2;
				}
				elif(first==topnd::c_addr)
				{
					ins.type+=3;
				}
				else
				{
					return false;
				}
			}
			elif(second==topnd::c_addr)
			{
				if(first==topnd::c_reg)
				{
					ins.type+=4;
				}
				elif(first==topnd::c_addr)
				{
					ins.type+=5;
				}
				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
		return true;
	}

	static rbool parse_opnd(tsh& sh,tasm& item,int index,const rbuf<rstr>& v,topnd& o,int& otype)
	{
		otype=topnd::c_null;
		if(v.empty())
		{
			return true;
		}
		if(v.count()==1)
		{
			if(v.top().is_number())
			{
				//123
				otype=topnd::c_imme;
				o.val=v.top().touint();
			}
			elif(v[0].get_bottom()==r_char('\"'))
			{
				//"abc"
				trans_cstr(v[0]);
				if(index>=item.vstr.count())
				{
					return false;
				}
				item.vstr[index]=v[0];
				otype=topnd::c_imme;
				o.val=(uint)(item.vstr[index].begin());
			}
			else
			{
				//ebp
				otype=topnd::c_reg;
				o.off=get_reg_off(sh,v.top());
			}
		}
		elif(v.count()==3)
		{
			if(v.bottom()==rsoptr(c_mbk_l)&&
				v.top()==rsoptr(c_mbk_r))
			{
				//[ebp]
				otype=topnd::c_addr;
				o.off=get_reg_off(sh,v[1]);
				o.val=0;
			}
			elif(v.bottom()==rsoptr(c_sbk_l)&&
				v.top()==rsoptr(c_sbk_r)&&
				v[1].is_number())//todo:
			{
				otype=topnd::c_imme;
				o.val=v[1].touint();
			}
			else
			{
				return false;
			}
		}
		elif(v.count()==5)
		{
			//[ebp+2]
			otype=topnd::c_addr;
			o.off=get_reg_off(sh,v[1]);
			o.val=v[3].touint();
			if(v[2]==rsoptr(c_minus))
			{
				o.val=-o.val;
			}
		}
		elif(v.count()==7&&v[1]==rsoptr(c_addr))
		{
			tclass* ptci=yfind::find_class(sh,v[3]);
			if(ptci==null)
			{
				return false;
			}
			tfunc* ptfi=yfind::find_func_dec(*ptci,v[5]);
			if(ptfi==null)
			{
				return false;
			}
			ifn(proc(sh,*ptfi))
			{
				return false;
			}
			otype=topnd::c_imme;
			o.val=(int)(ptfi->vasm.begin());
			return true;
		}
		elif(v.count()==0)
		{
			return true;
		}
		else
		{
			return false;
		}
		if(o.off>=r_size(treg))
		{
			return false;
		}
		return true;
	}

	static void trans_cstr(rstr& src)
	{
		if(src.count()<2)
		{
			return;
		}
		rstr dst;
		for(int i=1;i<src.count()-1;i++)
		{
			if(src[i]==r_char('\\'))
			{
				if(src.get(i+1)==r_char('b'))
				{
					dst+=r_char('\b');
				}
				elif(src.get(i+1)==r_char('n'))
				{
					dst+=r_char('\n');
				}
				elif(src.get(i+1)==r_char('r'))
				{
					dst+=r_char('\r');
				}
				elif(src.get(i+1)==r_char('\0'))
				{
					dst+=r_char('\0');
				}
				elif(src.get(i+1)==r_char('x'))
				{
					uchar ch=(uchar)(rstr::trans_hex_to_dec(
						src.sub(i+2,i+4)).touint());
					dst+=ch;
					i=i+3;
					continue;
				}
				else
				{
					dst+=src.get(i+1);
				}
				i++;
			}
			else
			{
				dst+=src[i];
			}
		}
		src=r_move(dst);
		src.buf.push((uchar)0);
	}

	static int get_reg_off(tsh& sh,const rstr& s)
	{
		treg reg;
		int ret=r_size(treg);
		if(rskey(c_eax)==s)
		{
			ret=(int)(&reg.eax)-(int)(&reg);
		}
		elif(rskey(c_ebx)==s)
		{
			ret=(int)(&reg.ebx)-(int)(&reg);
		}
		elif(rskey(c_ecx)==s)
		{
			ret=(int)(&reg.ecx)-(int)(&reg);
		}
		elif(rskey(c_edx)==s)
		{
			ret=(int)(&reg.edx)-(int)(&reg);
		}
		elif(rskey(c_esi)==s)
		{
			ret=(int)(&reg.esi)-(int)(&reg);
		}
		elif(rskey(c_edi)==s)
		{
			ret=(int)(&reg.edi)-(int)(&reg);
		}
		elif(rskey(c_esp)==s)
		{
			ret=(int)(&reg.esp)-(int)(&reg);
		}
		elif(rskey(c_ebp)==s)
		{
			ret=(int)(&reg.ebp)-(int)(&reg);
		}
		elif(rskey(c_eip)==s)
		{
			ret=(int)(&reg.eip)-(int)(&reg);
		}
		return ret;
	}
};
