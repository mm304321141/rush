/*
类成员函数使用可变参数
*/

void main()
{
	A a
	a.push[1,99,3]
	a.m_arr.count.printl
	a.push[12,98]
	a.m_arr.count.printl
}

class A
{
	rbuf<int> m_arr

#ifndef _RGPP
#ifdef _RS64
	void push(int count)
	{
		size=r_ceil_div(sizeof(int),8)*8
		char* p=&count
		p+=size
		for i=0;i<count;i++
			m_arr.push(*p.to<int*>)
			p+=size
		mov64 rcx,0
		mov ecx,size
		imul ecx,count
		add rcx,16
		pop rbp
		add rsp,sizeof(s_local)
		mov64 rax,[rsp]
		push rcx
		push rax
		call &_reti
	}
#else
	void push(int count)
	{
		//sub esp,sizeof(s_local)
		//push ebp
		//mov ebp,esp
		//变参函数的参数和局部变量需要手动析构，
		//这个例子使用的局部变量都是int，因此不需要析构

		int* p=&count+1
		for(i=0;i<count;i++)
		{
			m_arr.push(*p)
			p++
		}

		mov ecx,4
		imul ecx,count
		add ecx,8//注意成员函数多一个this引用
		pop ebp
		add esp,sizeof(s_local)
		mov eax,[esp]
		_reti(eax,ecx)

		//这里有编译器自动增加的语句
	}
#endif
#else
	void push(int count)
	{
		int* p=&count+1
		for(i=0;i<count;i++)
		{
			m_arr.push(*p)
			p++
		}
	}
#endif
}