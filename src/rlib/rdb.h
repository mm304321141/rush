#pragma once

#include "rfile.h"

#define rdbint rdb<int,int>

//TL是数据长度，TA是地址
template<typename TA,typename TL>//uchar,ushort,uint,uint64
struct rdb
{
	rfile file;//use multi thread please define multi object
	rbuf<TA> vindex;

	enum
	{
		c_null,//空数据
		c_real,
		c_cp,//压缩数据
		c_error//异常数据
	};

	rdb()
	{
	}

	rdb(rstr name,rstr mode=rstr("r"))
	{
		open(name,mode);
	}

	rbool close()
	{
		return file.close();
	}

	rbool open_off(rstr name,int off,rstr mode="r")
	{
		ifn(file.open(name,mode))
		{
			return false;
		}
		file.set_off(off);
		TL count;
		ifn(file.read(r_size(TL),&count))
		{
			return false;
		}
		TL cmax;
		ifn(file.read(r_size(TL),&cmax))//todo: r_size(cmax)
		{
			return false;
		}
		vindex.realloc_n_not_change(cmax);
		ifn(file.read(count*r_size(TA),vindex.begin()))
		{
			return false;
		}
		vindex.cur_count=count;
		for(int i=0;i<vindex.count();i++)
		{
			vindex[i]+=off;
		}
		return true;
	}

	rbool open(rstr name,rstr mode="r")
	{
		ifn(rfile::exist(name))
		{
			ifn(file.open_n(name,"rw"))
			{
				return false;
			}
			TL count=0;
			ifn(file.write(r_size(TL),&count))
			{
				return false;
			}
			ifn(file.write(r_size(TL),&count))
			{
				return false;
			}
			ifn(file.close())
			{
				return false;
			}
		}
		ifn(file.open(name,mode))
		{
			return false;
		}
		TL count;
		ifn(file.read(r_size(TL),&count))
		{
			return false;
		}
		TL cmax;
		ifn(file.read(r_size(TL),&cmax))
		{
			return false;
		}
		vindex.realloc_n_not_change(cmax);
		ifn(file.read(count*r_size(TA),vindex.begin()))
		{
			return false;
		}
		vindex.num=count;
		return true;
	}

	int count() const
	{
		return vindex.count();
	}

	rstr operator[](int i)
	{
		rstr ret;
		ifn(read(i,ret))
		{
			null;
		}
		return r_move(ret);
	}
	
	rbool read(int i,rstr& data)
	{
		data.clear();
		if(i>=vindex.count()||i<0)
		{
			return false;
		}
		if(vindex[i]==0)
		{
			return true;
		}
		char flag;
		TA off=vindex[i];
		ifn(file.read(off,r_size(char),&flag))
		{
			return false;
		}
		if(flag==c_null)
		{
			return true;
		}
		if(flag!=c_real)
		{
			return false;
		}
		TL len;
		off+=r_size(char);
		ifn(file.read(off,r_size(TL),&len))
		{
			return false;
		}
		data.buf.realloc_n(len);
		off+=r_size(TL);
		ifn(file.read(off,data.count(),data.begin()))
		{
			return false;
		}
		return true;
	}

	rbool write(int i,const rstr& data)
	{
		if(i>=vindex.count()||i<0)
		{
			return false;
		}
		char flag=c_null;
		TA off=vindex[i];
		ifn(file.write(off,r_size(char),&flag))
		{
			return false;
		}
		flag=c_real;
		off=file.size();
		vindex[i]=off;
		ifn(file.write(off,r_size(char),&flag))
		{
			return false;
		}
		off+=r_size(char);
		TL len=data.count();
		ifn(file.write(off,r_size(TL),&len))
		{
			return false;
		}
		off+=r_size(TL);
		ifn(file.write(off,data.count(),data.begin()))
		{
			return false;
		}
		ifn(file.write(get_index_off(i),r_size(TA),&vindex[i]))
		{
			return false;
		}
		return true;
	}

	rbool write_new(const rstr& data)
	{
		if(vindex.cur_count>=vindex.max_count)
		{
			ifn(extend())
			{
				return false;
			}
		}
		TA off;
		off=file.size();
		vindex.push(off);
		char flag=c_real;
		ifn(file.write(off,r_size(char),&flag))
		{
			return false;
		}
		off+=r_size(char);
		TL len=data.count();
		ifn(file.write(off,r_size(TL),&len))
		{
			return false;
		}
		off+=r_size(TL);
		ifn(file.write(off,data.count(),data.begin()))
		{
			return false;
		}
		int i=vindex.count()-1;
		ifn(file.write(get_index_off(i),r_size(TA),&vindex[i]))
		{
			return false;
		}
		ifn(file.write(0,r_size(TL),&vindex.num))
		{
			return false;
		}
		return true;
	}

	rbool extend()
	{
		if(file.size()>=128*1024*1024)
		{
			return false;
		}
		rbuf<uchar> temp;
		temp.alloc(file.size()-get_data_off());
		ifn(file.read(get_data_off(),temp.size(),temp.begin()))
		{
			return false;
		}
		int cmax=vindex.max_count;
		vindex.realloc_not_change(vindex.get_extend_num(vindex.count()));
		for(int i=0;i<vindex.count();i++)
		{
			vindex[i]+=(vindex.max_count-cmax)*r_size(TA);
		}
		ifn(file.write(r_size(TL),r_size(TL),&vindex.max_count))
		{
			return false;
		}
		//这里必须用max_count,不能用num，因为刚开始只有8个字节
		ifn(file.write(r_size(TL)*2,
			vindex.max_count*r_size(TA),vindex.begin()))
		{
			return false;
		}
		ifn(file.write(get_data_off(),temp.size(),temp.begin()))
		{
			return false;
		}
		return true;
	}

	int find(rstr s) const
	{
		for(int i=0;i<count();i++)
		{
			if(operator[](i)==s)
			{
				return i;
			}
		}
		return count();
	}

	TA get_data_off() const
	{
		return vindex.max_count*r_size(TA)+r_size(TL)*2;
	}

	static int get_index_off(int i)
	{
		return r_size(TL)*2+r_size(TA)*i;
	}
};
