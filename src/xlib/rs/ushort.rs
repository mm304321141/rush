﻿class ushort
{
	rd16 m_in

	~ushort()
	{
	}

	ushort()
	{
	}

	ushort(ushort& a)
	{
		mov esi,this
		mov edi,a
		mov8 [esi],[edi]
		mov8 [esi+1],[edi+1]
	}

	ushort(ushort a)
	{
		mov esi,this
		mov8 [esi],a
		mov8 [esi+1],[ebp+(s_off a+1)]
	}
	
	ushort(char a)
	{
		this=a.toint
	}
	
	/*short toshort()
	{
		mov esi,this
		mov8 s_ret,[esi]
		mov8 [ebp+(s_off s_ret+1)],[esi+1]
	}*/
	
	TYPE to<TYPE>()
	{
		xf.print("error\n")
	}
	
	template<>
	int to<int>()
	{
		return toint
	}
	
	template<>
	uint to<uint>()
	{
		return touint
	}

	int toint()
	{
		mov esi,this
		mov s_ret,0
		mov8 s_ret,[esi]
		mov8 [ebp+(s_off s_ret+1)],[esi+1]
	}

	uint touint()
	{
		mov esi,this
		mov s_ret,0
		mov8 s_ret,[esi]
		mov8 [ebp+(s_off s_ret+1)],[esi+1]
	}

	rstr torstr()
	{
		rstr ret(this.toint)
		return ret
	}

	void operator=(ushort& a)
	{
		mov esi,this
		mov edi,a
		mov8 [esi],[edi]
		mov8 [esi+1],[edi+1]
	}

	void operator=(ushort a)
	{
		mov esi,this
		mov8 [esi],a
		mov8 [esi+1],[ebp+(s_off a+1)]
	}
	
	void operator=(int a)
	{
		mov esi,this
		mov8 [esi],a
		mov8 [esi+1],[ebp+(s_off a+1)]
	}

	void operator=(uint a)
	{
		mov esi,this
		mov8 [esi],a
		mov8 [esi+1],[ebp+(s_off a+1)]
	}
	
	void operator*=(int a)
	{
		this=this*a
	}
	
	void operator+=(int a)
	{
		this=this+a
	}

	void operator++()
	{
		this=toint+1
	}

	void operator--()
	{
		this=toint-1
	}

	friend void operator<->(ushort& a,ushort& b)
	{
		c=a
		a=b
		b=c
	}

	friend bool operator==(ushort a,ushort b)
	{
		return a.toint==b.toint
	}

	friend bool operator!=(ushort a,ushort b)
	{
		return a.toint!=b.toint
	}

	friend bool operator>(ushort a,ushort b)
	{
		return a.toint>b.toint
	}

	friend bool operator>=(ushort a,ushort b)
	{
		return a.toint>=b.toint
	}

	friend bool operator<(ushort a,ushort b)
	{
		return a.toint<b.toint
	}

	friend bool operator<=(ushort a,ushort b)
	{
		return a.toint<=b.toint
	}

	void clear()
	{
		this=0
	}

	bool empty()
	{
		return this.toint==0
	}

	/*print()
	{
		rf.print this.torstr
	}

	printl()
	{
		rf.printl this.torstr
	}*/
}
