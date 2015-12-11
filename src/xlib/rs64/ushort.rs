class ushort
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
		mov64 rsi,this
		mov64 rdi,a
		mov8 [rsi],[rdi]
		mov8 [rsi+1],[rdi+1]
	}

	ushort(ushort a)
	{
		mov64 rsi,this
		mov8 [rsi],a
		mov8 [rsi+1],[rbp+(s_off a+1)]
	}
	
	ushort(char a)
	{
		this=a.toint
	}
	
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
		mov64 rsi,this
		mov s_ret,0
		mov8 s_ret,[rsi]
		mov8 [rbp+(s_off s_ret+1)],[rsi+1]
	}

	uint touint()
	{
		mov64 rsi,this
		mov s_ret,0
		mov8 s_ret,[rsi]
		mov8 [rbp+(s_off s_ret+1)],[rsi+1]
	}

	rstr torstr()
	{
		rstr ret(this.toint)
		return ret
	}

	void operator=(ushort& a)
	{
		mov64 rsi,this
		mov64 rdi,a
		mov8 [rsi],[rdi]
		mov8 [rsi+1],[rdi+1]
	}

	void operator=(ushort a)
	{
		mov64 rsi,this
		mov8 [rsi],a
		mov8 [rsi+1],[rbp+(s_off a+1)]
	}
	
	void operator=(int a)
	{
		mov64 rsi,this
		mov8 [rsi],a
		mov8 [rsi+1],[rbp+(s_off a+1)]
	}

	void operator=(uint a)
	{
		mov64 rsi,this
		mov8 [rsi],a
		mov8 [rsi+1],[rbp+(s_off a+1)]
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
}
