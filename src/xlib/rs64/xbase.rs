#include "rp.rs"
#include "char.rs"
#include "int.rs"
#include "uint.rs"
#include "ushort.rs"
#include "int64.rs"
#include "double.rs"
#include "float.rs"

#include "ralgo.h"
#include "rbuf.h"
#include "rstr.h"
#include "rstrw.h"
#include "rcode.h"
#include "rf.h"
#include "rmutex.h"

#define vector rbuf
#define string rstr
#define bool int
#define rbool int
#define long int64
#define uchar char
#define wchar ushort
#define r_size sizeof
#define r_move
#define r_ceil_div(a,b) r_cond(0==(a)%(b),(a)/(b),(a)/(b)+1)

#define r_max(a,b) r_cond(a>b,a,b)
#define r_min(a,b) r_cond(a<b,a,b)

/*#define r_to_int(a) ((int)(a))
#define r_to_uint(a) ((uint)(a))
#define r_to_char(a) ((char)(a))
#define r_to_uchar(a) ((uchar)(a))
#define r_to_int64(a) ((int64)(a))
#define r_to_double(a) ((double)(a))

#define r_to_pint(a) ((int*)(a))
#define r_to_puint(a) ((uint*)(a))
#define r_to_pchar(a) ((char*)(a))
#define r_to_puchar(a) ((uchar*)(a))
#define r_to_pint64(a) ((int64*)(a))
#define r_to_pdouble(a) ((double*)(a))
#define r_to_prstr(a) ((rstr*)(a))*/

#define true 1
#define false 0

#define null 0p
#define NULL 0p

define$ for_set(_mword,_mword)
{
	for(($0)=($1).begin();($0)!=($1).end();($0)=($1).next($0))
}

#define elif else if
#define while for
#define default else
#define namespace friend class
#define goto jmp
#define struct class
#define static friend
#define const
#define inline
#define asm
#define auto
#define then
#define volatile
#define register
#define signed
#define unsigned
#define operator
#define typename

#define <- =
#define :: .

//与do...while相反
mac until(a)
{
	if(a)
	{
		break;
	}
}

mac$ @ _word ( _mword )
{
	stdcall[#$0,$1]
}

mac$ @@ _word ( _mword )
{
	cdecl[#$0,$1]
}

cfunc void main_c()
{
	//rbyte 0xcc
	/*rbyte 0x41
	rbyte 0x50
	rbyte 0x41
	rbyte 0x51
	push rax
	push rcx
	push rdx*/
	sub rsp,8
	call &main
	add rsp,8
	/*pop rdx
	pop rcx
	pop rax
	rbyte 0x41
	rbyte 0x59
	rbyte 0x41
	rbyte 0x58*/
}

int r_cond(bool cond,int a,int b)
{
	if(cond)
	{
		return a
	}
	return b
}

T r_cond<T>(bool cond,T a,T b)
{
	if(cond)
	{
		return a
	}
	return b
}

char r_char(char* s)
{
	return s[0]
}

void print(rstr s)
{
	rf.print(s);
}

void printl(rstr s=rstr())
{
	rf.printl(s);
}

void* findf(rstr s)
{
	return xf.find_func(s.cstr)
}

//推荐使用函数进行位运算，
//如果一定要使用位运算符请自行DIY位运算符和优先级
int NOT(int a)
{
	bnot a
	mov s_ret,a
}

int AND(int a,int b)
{
	band a,b
	mov s_ret,a
}

int OR(int a,int b)
{
	bor a,b
	mov s_ret,a
}

int ORM(int num)
{
	size=r_ceil_div(sizeof(int),8)*8
	char* p=&num
	p+=size
	int ret=*p.to<int*>
	p+=size
	for(i=1;i<num;i++)
		ret=OR(ret,*p.to<int*>)
		p+=size
	*p.to<int*> =ret
	mov64 rcx,0
	mov ecx,size
	imul ecx,num
	add rcx,8
	pop rbp
	add rsp,sizeof(s_local)
	mov64 rax,[rsp]
	push rcx
	push rax
	call &_reti
}

int XOR(int a,int b)
{
	bxor a,b
	mov s_ret,a
}

int SHL(int a,int b)
{
	sub rsp,40
	mov64 rcx,a
	mov64 rdx,b
	calle "bshl",16
	add rsp,40
	mov64 s_ret,rax
}

int SHR(int a,int b)
{
	sub rsp,40
	mov64 rcx,a
	mov64 rdx,b
	calle "bshr",16
	add rsp,40
	mov64 s_ret,rax
}

int SAR(int a,int b)
{
	sub rsp,40
	mov64 rcx,a
	mov64 rdx,b
	calle "bsar",16
	add rsp,40
	mov64 s_ret,rax
}

rbuf<T> array<T>(int num)
{
	size=r_ceil_div(sizeof(T),8)*8
	char* p=&num
	p+=size
	rbuf<T>* pret=p+num*size
	pret->rbuf<T>()
	for(i=0;i<num;i++)
	{
		pret->push(*p)
		T.~T(*p)
		p+=size
	}
	mov64 rcx,0
	mov ecx,size
	imul ecx,num
	add rcx,8
	pop rbp
	add rsp,sizeof(s_local)
	mov64 rax,[rsp]
	push rcx
	push rax
	call &_reti
}

//采用类似的办法可以回溯函数的调用栈，
//即可从被调用函数访问到调用函数的局部变量
void _reti(int64 addr,int64 i)
{
	mov64 rcx,addr
	mov64 rax,i
	pop rbp
	add rsp,32
	add rsp,rax
#ifdef _RNASM
	rn jmp rcx
#endif
#ifdef _RJIT
	jmp rcx
#endif
#ifdef _RVM
	jmp rcx
#endif
}

void __declare()
{
	//todo:
	rbuf<char*> temp
}
