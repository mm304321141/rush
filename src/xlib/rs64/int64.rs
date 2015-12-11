class int64
{
	rd64 m_in

	~int64()
	{
	}

	int64()
	{
	}

	int64(int64 a)
	{
		mov64 rdi,this
		mov64 rsi,a
		mov64 [rdi],rsi
	}

	int64(int64& a)
	{
		mov64 rdi,this
		mov64 rsi,a
		mov64 rsi,[rsi]
		mov64 [rdi],rsi
	}

	int64(int a)
	{
		mov64 rsi,this
		mov [rsi],a
		mov [rsi+4],0
		if(a<0)
		{
			mov64 rsi,this
			mov [rsi+4],uint.c_max
		}
	}

	int64(uint a)
	{
		mov64 rsi,this
		mov [rsi],a
		mov [rsi+4],0
	}
	
	TYPE to<TYPE>()
	{
		if sizeof(int64)!=sizeof(TYPE)
			xf.print("error")
			xf.exit(0)
		mov64 rsi,this
		mov64 s_ret,[rsi]
	}
	
	template<>
	int to<int>()
	{
		return toint
	}
	
	template<>
	uint to<uint>()
	{
		return toint.touint
	}
	
	template<>
	char to<char>()
	{
		return toint.tochar
	}
	
	template<>
	ushort to<ushort>()
	{
		return toint.toushort
	}

	int toint()
	{
		mov64 rsi,this
		mov s_ret,[rsi]
	}

	rstr torstr()
	{
		rstr ret(this)
		return ret
	}

	void operator=(int64 a)
	{
		mov64 rdi,this
		mov64 rsi,a
		mov64 [rdi],rsi
	}

	void operator=(int64& a)
	{
		mov64 rdi,this
		mov64 rsi,a
		mov64 rsi,[rsi]
		mov64 [rdi],rsi
	}

	friend void operator<->(int64& a,int64& b)
	{
		c=a
		a=b
		b=c
	}

	void operator+=(int64 a)
	{
		sub rsp,40
		mov64 rcx,this
		lea rdx,a
		calle "addl"
		add rsp,40
	}

	void operator-=(int64 a)
	{
		sub rsp,40
		mov64 rcx,this
		lea rdx,a
		calle "subl"
		add rsp,40
	}

	void operator*=(int64 a)
	{
		sub rsp,40
		mov64 rcx,this
		lea rdx,a
		calle "imull"
		add rsp,40
	}
	
	void operator/=(int64 a)
	{
		sub rsp,40
		mov64 rcx,this
		lea rdx,a
		calle "idivl"
		add rsp,40
	}
	
	void operator%=(int64 a)
	{
		this=this%a
	}

	void operator++()
	{
		this+=1
	}

	void operator--()
	{
		this-=1
	}

	friend int64 operator+(int64 a,int64 b) 
	{
		a+=b
		return a
	}

	friend int64 operator-(int64 a,int64 b)
	{
		a-=b
		return a
	}
	
	friend int64 operator*(int64 a,int64 b)
	{
		a*=b
		return a
	}

	friend int64 operator/(int64 a,int64 b)
	{
		a/=b
		return a
	}
	
	//todo
	friend int64 operator%(int64 a,int64 b)
	{
		return a-a/b*b
	}
	
	friend bool operator<(int64 a,int64 b)
	{
		sub rsp,40
		lea rcx,a
		lea rdx,b
		calle "clsbl"
		mov s_ret,eax
		add rsp,40
	}

	friend bool operator<=(int64 a,int64 b)
	{
		return a<b||a==b;
	}

	friend bool operator==(int64 a,int64 b)
	{
		cesb a,b
		if(ebx)
		{
			cesb [rbp+(s_off a+4)],[rbp+(s_off b+4)]
		}
		mov s_ret,ebx
	}

	friend bool operator!=(int64 a,int64 b)
	{
		return !(a==b)
	}

	friend bool operator>(int64 a,int64 b)
	{
		return b<a;
	}

	friend bool operator>=(int64 a,int64 b)
	{
		return b<=a;
	}
	
	friend int64 operator neg(int64 a)
	{
		return 0-a;
	}
}
