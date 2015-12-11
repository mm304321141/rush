class char
{
	rd8 m_in

	~char()
	{
	}

	char()
	{
	}

	char(char& a)
	{
		mov64 rsi,this
		mov64 rdi,a
		mov8 [rsi],[rdi]
	}

	char(char a)
	{
		mov64 rsi,this
		mov8 [rsi],a
	}
	
	TYPE to<TYPE>()
	{
		xf.print("error\n")
	}
	
	template<>
	uint to<uint>()
	{
		return touint
	}
	
	template<>
	int to<int>()
	{
		return toint
	}
	
	template<>
	ushort to<ushort>()
	{
		return toint.toushort
	}
	
	template<>
	int64 to<int64>()
	{
		return toint.toint64
	}

	int toint()
	{
		mov64 rsi,this
		mov s_ret,0
		mov8 s_ret,[rsi]
	}

	uint touint()
	{
		mov64 rsi,this
		mov s_ret,0
		mov8 s_ret,[rsi]
	}

	char tochar()
	{
		mov64 rsi,this
		mov8 s_ret,[rsi]
	}

	void operator=(char& a)
	{
		mov64 rsi,this
		mov64 rdi,a
		mov8 [rsi],[rdi]
	}

	void operator=(char a)
	{
		mov64 rsi,this
		mov8 [rsi],a
	}
	
	void operator=(int a)
	{
		mov64 rsi,this
		mov8 [rsi],a
	}

	void operator=(uint a)
	{
		mov64 rsi,this
		mov8 [rsi],a
	}

	void operator++()
	{
		this=toint+1
	}

	friend void operator<->(char& a,char& b)
	{
		c=a
		a=b
		b=c
	}

	friend bool operator==(char a,char b)
	{
		return a.toint==b.toint
	}

	friend bool operator!=(char a,char b)
	{
		return a.toint!=b.toint
	}

	friend bool operator>(char a,char b)
	{
		return a.toint>b.toint
	}

	friend bool operator>=(char a,char b)
	{
		return a.toint>=b.toint
	}

	friend bool operator<(char a,char b)
	{
		return a.toint<b.toint
	}

	friend bool operator<=(char a,char b)
	{
		return a.toint<=b.toint
	}
	
	bool empty()
	{
		return this.toint==0
	}
}
