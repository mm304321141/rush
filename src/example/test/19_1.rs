/*
可变参数
*/

void main()
{
	printl sum[1,2]
	printl sum[1,2,3]
	printl sum[1,99,3]
	printl sum[1,2,3,4]
	printl sum[1,sum[1,2,5],3,4]
	
	printl sum(1,2)
	printl sum(1,2,3)
	printl sum(1,99,3)
}

#ifndef _RGPP
#ifdef _RS64
int sum(int count,...)
{
	size=r_ceil_div(sizeof(int),8)*8
	char* p=&count
	p+=size
	int ret=0
	for i=0;i<count;i++
		ret+=*p.to<int*>
		p+=size
	*p.to<int*> =ret

	mov64 rcx,0
	mov ecx,size
	imul ecx,count
	add rcx,8
	pop rbp
	add rsp,sizeof(s_local)
	mov64 rax,[rsp]
	push rcx
	push rax
	call &_reti
}
#else
int sum(int count,...)
{
	//sub esp,sizeof(s_local)
	//push ebp
	//mov ebp,esp

	int* p=&count+1
	int ret=0
	for i=0;i<count;i++
		ret+=*p
		p++
	*p=ret

	mov ecx,4
	imul ecx,count
	add ecx,4
	pop ebp
	add esp,sizeof(s_local)
	mov eax,[esp]
	_reti(eax,ecx)

	//这里有编译器自动增加的语句
}
#endif
#else
int sum(int count,...)
{
	rstr temp=count//防止G++对整数进行优化（最高-O2），或者可以-O0，VC++没有此问题
	int* p=&count+1
	int ret=0
	for i=0;i<count;i++
		ret+=*p
		p++
	return ret
}
#endif