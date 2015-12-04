﻿class double
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
		mov esi,this
		mov [esi],[ebp+s_off a]
		mov [esi+4],[ebp+(s_off a+4)]
	}

	double(double& a)
	{
		mov edi,this
		mov esi,a
		mov [edi],[esi]
		mov [edi+4],[esi+4]
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
		mov esi,this
		lea edi,[ebp+s_off s_ret]
		mov [edi],[esi]
		mov [edi+4],[esi+4]
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
		mov esi,this
		mov [esi],[ebp+s_off a]
		mov [esi+4],[ebp+(s_off a+4)]
	}

	void operator=(double& a)
	{
		mov edi,this
		mov esi,a
		mov [edi],[esi]
		mov [edi+4],[esi+4]
	}

	friend operator<->(double& a,double& b)
	{
		c=a
		a=b
		b=c
	}

	void operator+=(double a)
	{
		lea esi,a
		push esi
		push this
		calle "faddl",8
		add esp,8
	}

	void operator-=(double a)
	{
		lea esi,a
		push esi
		push this
		calle "fsubl",8
		add esp,8
	}

	void operator*=(double a)
	{
		lea esi,a
		push esi
		push this
		calle "fmull",8
		add esp,8
	}
	
	void operator/=(double a)
	{
		lea esi,a
		push esi
		push this
		calle "fdivl",8
		add esp,8
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
		lea esi,b
		push esi
		lea esi,a
		push esi
		calle "fclsbl",9
		mov s_ret,eax
		add esp,8
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

	void clear()
	{
		mov esi,this
		mov [esi],0
		mov [esi+4],0
	}

	bool empty()
	{
		return 0==this
	}

	/*print()
	{
		rf.print this
	}

	printl()
	{
		rf.printl this
	}*/
}
