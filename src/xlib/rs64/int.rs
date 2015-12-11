class int
{
	rd32 m_in

	enum
	{
		c_min=4294967295,
		c_max=0x7fffffff
	}

	~int()
	{
	}

	int()
	{
	}

	int(int a)
	{
		mov64 rsi,[rsp+8]
		mov ecx,[rsp+16]
		mov [rsi],ecx
		//mov [rsi],[rsp+16]
	}

	int(int& a)
	{
		mov64 rdi,[rsp+8]
		mov64 rsi,[rsp+16]
		mov ecx,[rsi]
		mov [rdi],ecx
		//mov [rdi],[rsi]
	}

	int(char a)
	{
		mov64 rdi,this
		mov [rdi],0
		mov8 [rdi],a
	}

	int(ushort a)
	{
		mov64 rdi,this
		mov [rdi],0
		mov8 [rdi],a
		mov8 [rdi+1],[rbp+(s_off a+1)]
	}

	TYPE to<TYPE>()
	{
		mov64 rsi,this
		mov s_ret,[rsi]
	}
	
	template<>
	char to<char>()
	{
		return tochar()
	}
	
	template<>
	ushort to<ushort>()
	{
		return toushort
	}
	
	template<>
	int64 to<int64>()
	{
		return toint64
	}

	ushort toushort()
	{
		mov64 rsi,this
		mov8 s_ret,[rsi]
		mov8 [rbp+(s_off s_ret+1)],[rsi+1]
	}

	int toint()
	{
		mov64 rsi,this
		mov s_ret,[rsi]
	}

	uint touint()
	{
		mov64 rsi,this
		mov s_ret,[rsi]
	}
	
	int64 toint64()
	{
		int64 ret(this)
		return ret
	}
	
	double todouble()
	{
		double ret(this)
		return ret
	}

	rstr torstr()
	{
		rstr ret(this)
		return ret
	}
	
	char tochar()
	{
		mov64 rsi,this
		mov8 s_ret,[rsi]
	}

	void operator=(int a)
	{
		mov64 rsi,[rsp+8]
		mov ecx,[rsp+16]
		mov [rsi],ecx
		//mov [rsi],[rsp+16]
	}

	void operator=(int& a)
	{
		mov64 rdi,[rsp+8]
		mov64 rsi,[rsp+16]
		mov ecx,[rsi]
		mov [rdi],ecx
		//mov [rdi],[rsi]
	}

	void operator=(uint a)
	{
		mov64 rsi,this
		mov [rsi],a
	}

	friend void operator<->(int& a,int& b)
	{
		c=a
		a=b
		b=c
	}

	friend int& operator=>(int a,int& this)
	{
		mov64 rsi,this
		mov [rsi],a
		return this
	}

	void operator+=(int a)
	{
		mov64 rsi,[rsp+8]
		mov ecx,[rsp+16]
		add [rsi],ecx
	}

	void operator-=(int a)
	{
		mov64 rsi,[rsp+8]
		mov ecx,[rsp+16]
		sub [rsi],ecx
	}

	void operator*=(int a)
	{
		mov64 rsi,[rsp+8]
		imul [rsi],[rsp+16]
	}
	
	void operator/=(int a)
	{
		mov64 rsi,[rsp+8]
		idiv [rsi],[rsp+16]
	}
	
	void operator%=(int a)
	{
		mov64 rsi,[rsp+8]
		imod [rsi],[rsp+16]
	}

	friend int operator neg(int a)
	{
		return 0-a
	}

	void operator++()
	{
		this+=1
	}

	void operator--()
	{
		this-=1
	}

	friend int operator+(int a,int b) 
	{
		a+=b
		return a
	}

	friend int operator-(int a,int b)
	{
		a-=b
		return a
	}

	friend int operator*(int a,int b)
	{
		a*=b
		return a
	}

	friend int operator/(int a,int b)
	{
		a/=b
		return a
	}

	friend int operator%(int a,int b)
	{
		a%=b
		return a
	}

	friend bool operator<(int a,int b)
	{
		clsb [rsp+8],[rsp+16]
		mov [rsp+24],ebx
	}

	friend bool operator<=(int a,int b)
	{
		clesb [rsp+8],[rsp+16]
		mov [rsp+24],ebx
	}

	friend bool operator==(int a,int b)
	{
		cesb [rsp+8],[rsp+16]
		mov [rsp+24],ebx
	}

	friend bool operator!=(int a,int b)
	{
		cnesb [rsp+8],[rsp+16]
		mov [rsp+24],ebx
	}

	friend bool operator>(int a,int b)
	{
		return b<a
	}

	friend bool operator>=(int a,int b)
	{
		return b<=a
	}

	friend bool operator!(int a)
	{
		cesb [rsp+8],0
		mov [rsp+16],ebx
	}

	friend bool operator&&(int a,int b)
	{
	}

	friend bool operator||(int a,int b)
	{
	}
	
	int abs()
	{
		if this<0
			return -this
		return this
	}

	void set(int a)
	{
		this=a
	}

	void clear()
	{
		this=0
	}

	bool empty()
	{
		return 0==this
	}

	void print()
	{
		rf.print(this)
	}

	void printl()
	{
		rf.printl(this)
	}
}
