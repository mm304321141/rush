#pragma once

#include "../rlib/rfile.h"
#include "yword.h"
#include "ysuper.h"

//预处理、获取所有文件
//词表->s_file.vword
struct ypre
{
	static rbool proc(tsh& sh)
	{
		ifn(import_default(sh))
		{
			rserror("");
			return false;
		}
		if(sh.main_cont!=null)
		{
			tfile item;
			item.name=sh.main_file;
			item.cont=sh.main_cont;
			sh.s_file.insert(item);
		}
		else
		{
			ifn(read_file(sh,sh.main_file))
			{
				rserror("can't read main file "+
					sh.main_file.torstr());
				return false;
			}
		}
		ifn(obtain_all_file(sh))
		{
			return false;
		}
		tfile* p;
		for_set(p,sh.s_file)
		{
			ifn(obtain_def(sh,sh.s_define,p->vword))
			{
				return false;
			}
		}
		for_set(p,sh.s_file)
		{
			ifn(replace_ifdef(sh,sh.s_define,p->vword))
			{
				return false;
			}
		}
		for_set(p,sh.s_file)
		{
			ifn(replace_def(sh,sh.s_define,p->vword))
			{
				return false;
			}
		}
		return true;
	}

	static rbool import_default(tsh& sh)
	{
		ifn(tconf::c_auto_import)
		{
			return true;
		}
		return read_file(sh,ybase::get_rs_dir()+"src/xlib/xf.h");
	}

	static rbool read_file(tsh& sh,const rstr& name)
	{
		if(name.empty())
		{
			return false;
		}
		ifn(rfile::exist(name))
		{
			return false;
		}
		tfile item;
		item.name=name;
		if(sh.s_file.exist(item))
		{
			return true;
		}
		item.cont=rfile::read_all_n(name);
		if(item.cont.empty())
		{
			return false;
		}
		item.cont=rcode::to_utf8_txt(item.cont).sub(3);
		sh.s_file.insert(item);
		return true;
	}

	static rbool parse_str(const tsh& sh,rstr& src,rbuf<tword>& dst,const tfile* pfile)
	{
		ifn(yword::parse(sh,src,dst,pfile))
		{
			return false;
		}
		replace_char(dst);
		replace_const(dst);
		//combine_double(dst);
		//combine_float(dst);
		replace_key(sh,dst);
		if(pfile!=null&&rdir::get_suffix_w(pfile->name)!=rstr("rs"))
		{
			replace_extern(sh,dst);
			replace_this(sh,dst);
		}
		return true;
	}

	static void replace_char(rbuf<tword>& v)
	{
		for(int i=0;i<v.count();i++)
		{
			//r_char ( rstr ( "" ) )
			if(v[i].val=="r_char"&&i+5<v.count()&&v[i+2].val=="rstr")
			{
				v[i+2].clear();
				v[i+3].clear();
				v[i+5].clear();
			}
		}
		ybase::arrange(v);
	}

	static rbool obtain_all_file(tsh& sh)
	{
		rbuf<tfile*>  v;
		tfile* p;
		for_set(p,sh.s_file)
		{
			v.push(p);
		}
		for(int i=0;i<v.count();i++)
		{
			ifn(proc_file(sh,v[i]))
			{
				return false;
			}
		}
		return true;
	}

	static rbool proc_file(tsh& sh,tfile* p)
	{
		rbuf<rstr> vname;
		count_tab(*p);
		ifn(parse_str(sh,p->cont,p->vword,p))
		{
			return false;
		}
		//仅编译器内部使用
		ifn(replace_ifdef(sh,sh.s_define,p->vword))
		{
			return false;
		}
		ifn(obtain_name(sh,vname,p->vword,*p))
		{
			rserror(rstr("obtain error ")+
				ybase::get_file_name(p));
			return false;
		}
		for(int i=0;i<vname.count();i++)
		{
			ifn(read_file(sh,vname[i]))
			{
				rserror("can't read file "+vname[i].torstr());
				return false;
			}
			tfile* pfile=sh.s_file.find(tfile(vname[i]));
			if(pfile==null)
			{
				return false;
			}
			ifn(proc_file(sh,pfile))
			{
				return false;
			}
		}
		return true;
	}

	static rbool obtain_name(const tsh& sh,rbuf<rstr>& vname,rbuf<tword>& v,const tfile& f)
	{
		rstr exe_dir=rdir::get_exe_dir();
		for(int i=0;i<v.count();i++)
		{
			if(v[i].val!=rskey(c_import))
			{
				continue;
			}
			if(v.get(i-1).val==rsoptr(c_sharp))
			{
				v[i-1].clear();
			}
			v[i].val.clear();
			rstr name;
			if(v.get(i+1).is_cstr())
			{
				name=v.get(i+1).val;
				v[i+1].clear();
			}
			else
			{
				//处理不带引号的import
				int j;
				for(j=i+1;j<v.count();j++)
				{
					if(v[j].pos.line!=v[i].pos.line)
					{
						break;
					}
					name+=v[j].val;
					v[j].val.clear();
				}
				name=ybase::add_quote(name.torstr());
			}
			if(name.count()<3)
			{
				return false;
			}
			name.pop();
			name.pop_front();
			name=rdir::std_dir(name);
			rstr temp=get_abs_name(rdir::get_prev_dir(f.name),name);
			ifn(rfile::exist(temp))
			{
				ifn(get_file(sh,vname,name,temp))
				{
					return false;
				}
			}
			if(!vname.exist(temp)&&!sh.s_file.exist(tfile(temp)))
			{
				vname.push(temp);
			}
		}
		ybase::arrange(v);
		return true;
	}

	static rbool get_file(const tsh& sh,const rbuf<rstr>& vname,
		const rstr& name,rstr& abs_name)
	{
		for(int i=0;i<sh.vpath.count();i++)
		{
			//todo: 直接import
			abs_name=get_abs_name(sh.vpath[i],name);
			if(rfile::exist(abs_name))
			{
				return true;
			}
		}
		return false;
	}

	static rstr get_abs_name(rstrw path,const rstrw& name)
	{
		if(path.empty())
		{
			return rstr();
		}
		rbuf<rstrw> temp=r_split<rstrw>(name,rstrw("/"));
		if(temp.empty())
		{
			return rstr();
		}
		for(int i=0;i<temp.count();i++)
		{
			if(temp[i]==rstrw(".."))
			{
				path=rdir::get_prev_dir_w(path);
			}
			elif(temp[i]==rstrw("."))
			{
				;
			}
			else
			{
				path+=temp[i];
				if(i!=temp.count()-1)
				{
					path+=rstrw("/");
				}
			}
		}
		return path.torstr();
	}

	static rstr get_abs_path(const rstrw& s)
	{
		if(s.sub(0,2)==rstrw("//")||s.sub(1,3)==rstrw(":/"))
		{
			return s.torstr();
		}
		return get_abs_name(rdir::get_cur_dir_w(),s);
	}

	static void replace_this(const tsh& sh,rbuf<tword>& v)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v[i]!=rskey(c_this))
			{
				continue;
			}
			if(v.get(i+1)==rsoptr(c_arrow_r))
			{
				v[i+1].val=rsoptr(c_dot);
				continue;
			}
			if(v.get(i-1).val==rsoptr(c_mul)&&v.get(i-2).val==rsoptr(c_sbk_l)&&
				v.get(i+1).val==rsoptr(c_sbk_r))
			{
				v[i-1].clear();
				continue;
			}
			v[i].val.clear();
			v[i].multi+=rsoptr(c_sbk_l);
			v[i].multi+=rsoptr(c_addr);
			v[i].multi+=rskey(c_this);
			v[i].multi+=rsoptr(c_sbk_r);
		}
		ybase::arrange(v);
	}

	static void replace_extern(const tsh& sh,rbuf<tword>& v)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v[i]!=rskey(c_extern))
			{
				continue;
			}
			int pos=v.find(rsoptr(c_semi),i);
			if(pos<v.count())
			{
				ybase::clear_word_val(v,i,pos+1);
			}
		}
		ybase::arrange(v);
	}

	static void replace_key(const tsh& sh,rbuf<tword>& v)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v[i].val==rskey(c_include))
			{
				v[i].val=rskey(c_import);
			}
			elif(v[i].val==rskey(c_private))//这里不能用超级宏实现，因为有带冒号和不带冒号两种
			{
				v[i].clear();
				if(rsoptr(c_colon)==v.get(i+1).val)
				{
					v[i+1].clear();
				}
			}
			elif(v[i].val==rskey(c_public))
			{
				v[i].clear();
				if(rsoptr(c_colon)==v.get(i+1).val)
				{
					v[i+1].clear();
				}
			}
			elif(v[i].val==rskey(c_protected))
			{
				v[i].clear();
				if(rsoptr(c_colon)==v.get(i+1).val)
				{
					v[i+1].clear();
				}
			}
			elif(v[i].val==rsoptr(c_sharp)&&v.get(i+1).val=="pragma")
			{
				if(i+2<v.count())
				{
					v[i+2].clear();
				}
				v[i].clear();
				v[i+1].clear();
			}
			elif(v[i].val=="__FILE__")
			{
				v[i].val=ybase::add_quote(v[i].pos.file->name);
			}
			elif(v[i].val=="__LINE__")
			{
				v[i].val=v[i].pos.line;
			}
		}
		ybase::arrange(v);
	}

	static rbool replace_def_one(const tsh& sh,rbuf<tword>& v,const rset<tmac>& vmac)
	{
		tmac item;
		for(int i=0;i<v.count();i++)
		{
			item.name=v[i].val;
			tmac* p=vmac.find(item);
			if(p==null)
			{
				continue;
			}
			if(p->is_super)
			{
				ifn(ysuper::replace_item(sh,v,i,*p))
				{
					return false;
				}
			}
			else
			{
				v[i].val.clear();
				v[i].multi=p->vstr;
			}
		}
		return true;
	}

	static rbool replace_def(const tsh& sh,const rset<tmac>& vdefine,rbuf<tword>& v)
	{
		for(int i=0;i<c_rs_deep;i++)
		{
			ifn(replace_def_one(sh,v,vdefine))
			{
				return false;
			}
			ifn(ybase::arrange(v))
			{
				return true;
			}
		}
		return false;
	}

	static rbool obtain_def(const tsh& sh,rset<tmac>& vdefine,rbuf<tword>& v)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v[i].val!=rskey(c_define))
			{
				continue;
			}
			if(v.get(i-1).val==rsoptr(c_sharp))
			{
				v[i-1].clear();
			}
			if(v.get(i+1).is_name()&&v.get(i+2)==rsoptr(c_sbk_l))
			{
				//todo:暂时替换成mac
				v[i].val=rskey(c_mac);
				continue;
			}
			if(v.get(i+1)=="$")
			{
				int right=i;
				ifn(ysuper::add_super_mac(sh,v,right,vdefine))
				{
					return false;
				}
				ybase::clear_word_val(v,i,right+1);
				i=right;
				continue;
			}
			tmac item;
			item.name=v.get(i+1).val;
			if(item.name.empty())
			{
				rserror(v[i]);
				return false;
			}
			for(int k=i+2;k<v.count()&&v.get(k).pos==v[i].pos;k++)
			{
				item.vstr.push(v[k].val);
				v[k].clear();
			}
			if(vdefine.exist(item))
			{
				vdefine.erase(item);
				/*rserror(v.get(i+1),"redefined");
				return false;*/
			}
			vdefine.insert(item);
			v[i+1].clear();
			v[i].clear();
		}
		ybase::arrange(v);
		return true;
	}

	//fixme:
	static rbool replace_ifdef(const tsh& sh,const rset<tmac>& vdefine,rbuf<tword>& v)
	{
		tmac item;
		for(int i=v.count()-1;i>=0;i--)
		{
			if(v[i]!=rsoptr(c_sharp))
			{
				continue;
			}
			rstr key=v.get(i+1).val;
			if(key!=rskey(c_ifdef)&&
				key!=rskey(c_ifndef))
			{
				continue;
			}
			if(i+2>=v.count())
			{
				rserror(v[i],"ifdef");
				return false;
			}
			item.name=v[i+2].val;
			int endpos=r_find_a<tword>(v,tword(rskey(c_endif)),i+3);
			if(endpos>=v.count())
			{
				rserror(v[i],"ifdef");
				return false;
			}
			int elsepos=endpos;
			for(int j=i+3;j<endpos;j++)
			{
				if(v[j]==rsoptr(c_sharp)&&
					v.get(j+1)==rskey(c_else))
				{
					elsepos=j+1;
					break;
				}
			}
			rbool defined=vdefine.exist(item);
			if(key==rskey(c_ifdef))
			{
				if(defined)
				{
					ybase::clear_word_val(v,elsepos,endpos);
				}
				else
				{
					ybase::clear_word_val(v,i,elsepos);
				}
			}
			else
			{
				if(defined)
				{
					ybase::clear_word_val(v,i,elsepos);
				}
				else
				{
					ybase::clear_word_val(v,elsepos,endpos);
				}
			}
			v[i].clear();
			v[i+1].clear();
			v[i+2].clear();
			v[endpos-1].clear();
			v[endpos].clear();
			v[elsepos-1].clear();
			v[elsepos].clear();
		}
		ybase::arrange(v);
		return true;
	}

	/*static void combine_double(rbuf<tword>& v)
	{
		for(int i=1;i<v.count()-1;i++)
		{
			if(v[i]=="."&&
				v[i-1].val.is_number()&&
				v[i+1].val.is_number())
			{
				v[i-1].val+=v[i].val;
				v[i-1].val+=v[i+1].val;
				v[i].clear();
				v[i+1].clear();
				i++;
			}
		}
		ybase::arrange(v);
	}*/

	static void combine_float(rbuf<tword>& v)
	{
		for(int i=0;i<v.count();i++)
		{
			ifn(v[i]=="."&&v.get(i-1).val.is_number())
			{
				continue;
			}
			rstr s=v.get(i+1).val;
			if(s.get_top()!=r_char('f'))
			{
				continue;
			}
			ifn(s.sub(0,s.count()-1).is_number())
			{
				continue;
			}
			v[i].multi+="double";
			v[i].multi+="(";
			v[i].multi+=v[i-1].val+"."+s.sub(0,s.count()-1);
			v[i].multi+=")";
			v[i].multi+=".";
			v[i].multi+="tofloat";
			v[i].val.clear();
			v[i+1].val.clear();
			v[i-1].val.clear();
		}
		ybase::arrange(v);
	}

	static void replace_const(rbuf<tword>& v)
	{
		for(int i=0;i<v.count();i++)
		{
			replace_const_word(v[i]);
		}
	}

	static void replace_const_word(tword& word)
	{
		if(word.val.count()>0&&rstr::is_number(word.val[0]))
		{
			if(r_find_a<uchar>(word.val.buf,r_char('_'))<word.val.count())
			{
				rstr s;
				for(int i=0;i<word.val.count();i++)
				{
					if(word.val[i]!=r_char('_'))
					{
						s+=word.val[i];
					}
				}
				word.val=r_move(s);
			}
		}
		if(word.val.count()>2)
		{
			if(word.val[0]==r_char('0')&&word.val[1]==r_char('x'))
			{
				word.val=rstr::trans_hex_to_dec(word.val.sub(2));
			}
			if(word.val[0]==r_char('0')&&word.val[1]==r_char('b'))
			{
				word.val=rstr::trans_bin_to_dec(word.val.sub(2));
			}
		}
	}

	static int count_tab_line(const rstr& s)
	{
		int i;
		for(i=0;i<s.count();i++)
		{
			if(s[i]!=r_char(' ')&&s[i]!=0x9)
			{
				break;
			}
		}
		int sum=0;
		for(int k=0;k<i;k++)
		{
			if(s[k]==r_char(' '))
			{
				sum++;
			}
			elif(s[k]==0x9)
			{
				sum+=4;
			}
		}
		return sum/4;
	}

	static void count_tab(tfile& file)
	{
		file.line_list=r_split_e<rstr>(file.cont,rstr("\n"));
		file.line_list.push_front(rstr());//行号从1开始
		for(int i=0;i<file.line_list.count();i++)
		{
			file.tab_list.push(count_tab_line(file.line_list[i]));
		}
	}
};
