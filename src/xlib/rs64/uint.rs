class uint
{
	rd32 m_in

	enum
	{
		c_min=0,
		c_max=0xffffffff,
	}

	~uint()
	{
	}

	uint()
	{
	}

	uint(uint a)
	{
		mov64 rsi,[rsp+8]
		mov [rsi],[rsp+16]
	}

	uint(uint& a)
	{
		mov64 rdi,[rsp+8]
		mov64 rsi,[rsp+16]
		mov [rdi],[rsi]
	}

	uint(int a)
	{
		mov64 rsi,[rsp+8]
		mov [rsi],[rsp+16]
	}

	uint(char a)
	{
		mov edi,this
		mov [edi],0
		mov8 [edi],a
	}

	TYPE to<TYPE>()
	{
		mov64 rsi,this
		mov s_ret,[rsi]
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
	
	template<>
	int64 to<int64>()
	{
		return toint.toint64
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

	rstr torstr()
	{
		rstr ret(this)
		return ret
	}

	void operator=(uint a)
	{
		mov64 rsi,[rsp+8]
		mov [rsi],[rsp+16]
	}

	void operator=(uint& a)
	{
		mov64 rdi,[rsp+8]
		mov64 rsi,[rsp+16]
		mov [rdi],[rsi]
	}

	friend void operator<->(uint& a,uint& b)
	{
		c=a
		a=b
		b=c
	}

	void operator+=(uint a)
	{
		mov64 rsi,[rsp+8]
		mov ecx,[rsp+16]
		add [rsi],ecx
	}

	void operator-=(uint a)
	{
		mov64 rsi,[rsp+8]
		mov ecx,[rsp+16]
		sub [rsi],ecx
	}

	void operator*=(uint a)
	{
		mov64 rsi,[rsp+8]
		imul [rsi],[rsp+16]
	}

	void operator/=(uint a)
	{
		mov64 rsi,[rsp+8]
		_udiv [rsi],[rsp+16]
	}

	void operator%=(uint a)
	{
		mov64 rsi,[rsp+8]
		_umod [rsi],[rsp+16]
	}

	void operator++()
	{
		this+=1
	}

	void operator--()
	{
		this-=1
	}

	friend uint operator+(uint a,uint b) 
	{
		a+=b
		return a
	}

	friend uint operator-(uint a,uint b)
	{
		a-=b
		return a
	}

	friend uint operator*(uint a,uint b)
	{
		a*=b
		return a
	}

	friend uint operator/(uint a,uint b)
	{
		a/=b
		return a
	}

	friend uint operator%(uint a,uint b)
	{
		a%=b
		return a
	}

	friend bool operator<(uint a,uint b)
	{
		_uclsb a,b
		mov s_ret,ebx
	}

	friend bool operator<=(uint a,uint b)
	{
		_uclesb a,b
		mov s_ret,ebx
	}

	friend bool operator==(uint a,uint b)
	{
		cesb a,b
		mov s_ret,ebx
	}

	friend bool operator!=(uint a,uint b)
	{
		cnesb a,b
		mov s_ret,ebx
	}

	friend bool operator>(uint a,uint b)
	{
		return b<a;
	}

	friend bool operator>=(uint a,uint b)
	{
		return b<=a;
	}
}
