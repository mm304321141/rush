class double
{
	rd64 m_in

	~double()
	{
	}

	double()
	{
	}

	double(double a)
	{
		mov64 rsi,this
		mov64 [rsi],a
	}

	double(double& a)
	{
		mov64 rdi,this
		mov64 rsi,a
		mov64 [rdi],[rsi]
	}
	
	//这个整数转浮点很慢
	double(int a)
	{
		rstr s(a)
		xf.sscanf(s.cstr,"%lf",&this)
	}

	double(uint a)
	{
		rstr s(a)
		xf.sscanf(s.cstr,"%lf",&this)
	}
	
	double(float a)
	{
		push this
		push a
		calle "f_to_d",8
		add esp,8
	}
	
	TYPE to<TYPE>()
	{
		xf.error
	}
	
	template<>
	double to<double>()
	{
		return todouble()
	}
	
	template<>
	float to<float>()
	{
		return tofloat()
	}
	
	template<>
	int to<int>()
	{
		return toint()
	}
	
	double todouble()
	{
		mov64 rsi,this
		mov64 s_ret,[rsi]
	}

	int toint()
	{
		rbuf<char> buf(128)
		xf.sprintf64(buf.begin,"%.0lf",this)
		return rstr(buf.begin).toint
	}

	rstr torstr()
	{
		rstr ret(this)
		return ret
	}
	
	float tofloat()
	{
		lea esi,[ebp+s_off s_ret]
		push esi
		push this
		calle "d_to_f",8
		add esp,8
	}
	
	friend double operator neg(double a)
	{
		return 0.0-a;
	}

	void operator=(double a)
	{
		mov64 rsi,this
		mov64 [rsi],a
	}

	void operator=(double& a)
	{
		mov64 rdi,this
		mov64 rsi,a
		mov64 [rdi],[rsi]
	}

	friend void operator<->(double& a,double& b)
	{
		c=a
		a=b
		b=c
	}

	void operator+=(double a)
	{
		sub rsp,40
		mov64 rcx,this
		lea rdx,a
		calle "faddl"
		add rsp,40
	}

	void operator-=(double a)
	{
		sub rsp,40
		mov64 rcx,this
		lea rdx,a
		calle "fsubl"
		add rsp,40
	}

	void operator*=(double a)
	{
		sub rsp,40
		mov64 rcx,this
		lea rdx,a
		calle "fmull"
		add rsp,40
	}
	
	void operator/=(double a)
	{
		sub rsp,40
		mov64 rcx,this
		lea rdx,a
		calle "fdivl"
		add rsp,40
	}

	friend double operator+(double a,double b) 
	{
		a+=b
		return a
	}

	friend double operator-(double a,double b)
	{
		a-=b
		return a
	}

	friend double operator*(double a,double b)
	{
		a*=b
		return a
	}

	friend double operator/(double a,double b)
	{
		a/=b
		return a
	}

	friend bool operator<(double a,double b)
	{
		sub rsp,40
		lea rcx,a
		lea rdx,b
		calle "fclsbl"
		mov s_ret,eax
		add rsp,40
	}

	friend bool operator<=(double a,double b)
	{
		return a<b||a==b;
	}

	friend bool operator==(double a,double b)
	{
		cesb a,b
		if (ebx)
		{
			cesb [ebp+(s_off a+4)],[ebp+(s_off b+4)]
		}
		mov s_ret,ebx
	}

	friend bool operator!=(double a,double b)
	{
		return !(a==b)
	}

	friend bool operator>(double a,double b)
	{
		return b<a;
	}

	friend bool operator>=(double a,double b)
	{
		return a>b||a==b;
	}
	
	static double infinity()
	{
		xf.memcpy(&s_ret,"\x7F\xF0\x00\x00\x00\x00\x00\x00",8);
	}
	
	double abs()
	{
		if this<0.0
			return 0.0-this
		return this
	}
}
