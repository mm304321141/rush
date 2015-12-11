class rp<T>
{
	rdp m_in

	~rp<T>()
	{
	}

	rp<T>()
	{
	}

	rp<T>(rp<T> a)
	{
		mov64 rsi,a
		mov64 rdi,this
		mov64 [rdi],rsi
	}

	rp<T>(rp<T>& a)
	{
		mov64 rsi,a
		mov64 rsi,[rsi]
		mov64 rdi,this
		mov64 [rdi],rsi
	}

	TYPE to<TYPE>()
	{
		mov64 rsi,this
		mov64 s_ret,[rsi]
	}
	
	template<>
	char to<char>()
	{
		xf.error
	}
	
	template<>
	ushort to<ushort>()
	{
		xf.error
	}
	
	int64 toint64()
	{
		return to<int64>
	}

	int toint()
	{
		xf.error
	}

	uint touint()
	{
		xf.error
	}

	void operator=(rp<T> a)
	{
		mov64 rsi,a
		mov64 rdi,this
		mov64 [rdi],rsi
	}

	void operator=(rp<T>& a)
	{
		mov64 rsi,a
		mov64 rsi,[rsi]
		mov64 rdi,this
		mov64 [rdi],rsi
	}
	
	void operator=(int a)
	{
		xf.error
	}

	T& operator*()
	{
		mov64 rsi,this
		mov64 s_ret,[rsi]
	}
	
	//指针再取地址只能返回空指针，否则会产生递归定义
	rp<void> operator&()
	{
		mov64 s_ret,this
	}

	void operator++()
	{
		this=(to<int64>+sizeof(T)).to<rp<void>>
	}

	void operator--()
	{
		this=(this.to<int64>-sizeof(T)).to<rp<void>>
	}

	void operator+=(int a)
	{
		this=(this.to<int64>+a*sizeof(T)).to<rp<void>>
	}

	void operator+=(uint a)
	{
		this=(this.to<int64>+a*sizeof(T)).to<rp<void>>
	}

	void operator-=(int a)
	{
		this=(this.to<int64>-a*sizeof(T)).to<rp<void>>
	}

	void operator-=(uint a)
	{
		this=(this.to<int64>-a*sizeof(T)).to<rp<void>>
	}

	T& operator[](int a)
	{
		a*=sizeof(T)
		b=a+to<int64>
		mov64 s_ret,b
	}

	T& operator[](uint a)
	{
		a*=sizeof(T)
		b=a+to<int64>
		mov64 s_ret,b
	}

	friend rp<T> operator+(rp<T> a,int b)
	{
		a+=b
		return a
	}

	friend rp<T> operator+(rp<T> a,uint b)
	{
		a+=b
		return a
	}

	friend rp<T> operator-(rp<T> a,int b)
	{
		a-=b
		return a
	}

	friend rp<T> operator-(rp<T> a,uint b)
	{
		a-=b
		return a
	}

	friend int64 operator-(rp<T> a,rp<T> b)
	{
		return (a.to<int64>-b.to<int64>)/sizeof(T)
	}

	friend bool operator<(rp<T> a,rp<T> b)
	{
		return a.to<int64><b.to<int64>
	}

	friend bool operator<=(rp<T> a,rp<T> b)
	{
		return a.to<int64><=b.to<int64>
	}

	friend bool operator>(rp<T> a,rp<T> b)
	{
		return a.to<int64>>b.to<int64>
	}

	friend bool operator>=(rp<T> a,rp<T> b)
	{
		return a.to<int64> >= b.to<int64>
	}

	friend bool operator==(rp<T> a,rp<T> b)
	{
		return a.to<int64> == b.to<int64>
	}

	friend bool operator!=(rp<T> a,rp<T> b)
	{
		return a.to<int64>!=b.to<int64>
	}
	
	void print()
	{
		rf.print(this.toint64)
	}

	void printl()
	{
		rf.printl(this.toint64)
	}
}
