
void main()
{
	test1
	test2
	test3
	test4
	test5
	test6
	test7
}

//浮点测试
void test1()
{
	printl 0.2+0.3
	printl 0.2*0.3
	printl 0.2/0.3
	printl 0.2-0.3
	printl 0.2<0.3
	printl 0.2>0.3
	a=99.2*20
	printl a
	printl(-2)
}

//逻辑运算测试
void test2()
{
	printl 0&&0
	printl 1&&0
	printl 0&&1
	printl 1&&1
	printl 0||0
	printl 1||0
	printl 0||1
	printl 1||1
}

//自动类型转换测试
void test3()
{
	uint a=1
	int c=5
	printl a+sizeof(int)!=c
}

//64位运算测试
void test4()
{
	int64 a=9
	int64 b=3
	printl a/b
	printl a*b
	printl a%b
	printl a-b
	printl a<b
	printl a>b
}

void test5()
{
	if (2)==2
		printl('ok',,,)
}

void test6()
{
	printl ORM[1,2]
	printl ORM[1,2,4,8]
	printl ORM[1]
}

void test7():
	rbuf<int> v
	v.push(1)
	v.push(2)
	v.push(3)
	printl v.count
	v.push(4)
	v.push(5)
	printl v.count
	printl A(2,3)
	
#define B(a,b) (a*b)
#define A(a,b) (B(a,b)+b)
	