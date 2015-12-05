### Rush编程语言

##### 1. 1+2=3

第一个Rush程序：

```cpp
void main():
    a=1
	b=2
	print(a+b)
```

当然也可以使用int main()。

##### 2. C

Rush支持C风格，上一个程序可以写成这样：

```cpp
void main()
{
	int a=1;
	int b=2;
	print(a+b);
}
```

对比上一节的程序可以看到Rush的一些特点：
1. 更彻底地类型推断，C++11需要使用关键字auto，Rush可以省略auto
2. 函数定义和函数调用均可省略后面的小括号
3. 语句后面的分号也可以省略

##### 3. 面向对象

Rush支持C#（Java）风格，上一个程序可以写成这样：

```cpp
public class main
{
	public static void main()
	{
		int a=1;
		int b=2;
		print(a+b);
	}
}
```

##### 4. Lisp

Rush内部使用Lisp作为中间层，因此上一个程序可以写成这样：

```cpp
void main():
	int a
	int b
	[int,=,[a,1]]
	[int,=,[b,2]]
	[rf,print,[[int,+,[a,b]]]]
```

其中
```cpp
[rf,print,[[int,+,[a,b]]]]
```
等价于
```cpp
rf.print(a+b)
```

另一个S表达式的例子:
<br/>
src\example\test\4_2.rs

##### 5. 省略括号

控制结构（if/for/while）后面的小括号是可以省略的，比如有一个递归求和的函数：

```cpp
int sum(int a)
{
	if a<=1
		return 1
	return sum(a-1)+a
}
```

那么
```cpp
if a<=1
```
等价于
```cpp
if(a<=1)
```

Rush支持各种花括号风格，也支持类似python的控制结构省略花括号，这时编译器将使用缩进区分语句块，缩进可以用tab或者空格表示，1个tab等于4个空格，比如：
```cpp
if 1
	print 2
	print 3
```
等价于
```cpp
if 1 {
	print 2
	print 3
}
```
等价于
```cpp
if(1) 
{
	print(2);
	print(3);
}
```

如果函数调用的右边没有小括号，则本行后面所有单词均作为函数的参数，比如：
```cpp
print(sum(9))
```
等价于
```cpp
print sum(9)
```
等价于
```cpp
print sum 9
```

表达式过长折行的时候不能省略括号（左括号不可折行）：
```cpp
if(1||
	2)
	print "true"

func(1,
	2)
```

##### 6. 常量对象

暂时移除

##### 7. 运算符DIY

Rush支持运算符重载，相同优先级的运算符都是从左往右运算的，C语言可以这样：
```cpp
**p=2
```
但Rush只能这样：
```cpp
*(*p)=2
```

注意Rush的乘法运算符和指针运算符都是*，优先级相同，均为2。而标准C乘法运算符优先级是2，指针运算符优先级是1，两者明显有区别。
<br/>

Rush的系统库也不支持连续赋值，C语言里可以这样：
```cpp
a=b=2
```
Rush只能这样：
```cpp
b=2
a=b
```

如果一定要连续赋值，可以修改src\xlib\rs\int.rs这个文件，将operator=这个函数改成这样：
```cpp
int& operator=(int a)
{
	mov esi,this
	mov [esi],a
	return this
}
```
然后就可以连续赋值（注意括号是必须的）：
```cpp
a=(b=2)
```
可以看到这样的话多返回了一个引用，影响效率。
<br/>
<br/>
src\xlib\rs\int.rs里面还添加一个反向赋值的运算符函数：

```cpp
friend int& operator=>(int a,int& this)
{
	mov esi,this
	mov [esi],a
	return this
}
```

那么使用这个运算符就可以连续反向赋值：
```cpp
2=>a=>b
```
这样a和b都会赋值为2，其实从左往右赋值有时看起来更直观一些。
<br/>	
打开src\xlib\rs\xbase.rs可以找到：
```cpp
define <- =
define <> !=
```
可见赋值运算符还有另一种写法：
```cpp
a <- 2
```
其等价于
```cpp
a = 2
```
而不等于运算符也有另一种写法：
```cpp
a != 2
```
等价于
```cpp
a <> 2
```
BASIC语言的<>好像看起来更直观一些。
<br/>
<br/>
标准C中减法运算符和负号运算符都是 - ，而Rush中减法运算符是 - ，负号运算符是 neg ，下面3句是等价的：
```cpp
a=-2
a=(-2)
a=neg 2
```
对于加了括号的常量表达式，会在编译阶段进行求值。

##### 8. 无缝内联汇编

在上一节的例子中可以看到，Rush用this引用代替了C++的this指针，实际上Rush内部引用和指针都是4个字节的地址（32位），只是外部访问的时候有些区别。C++是这样：
```cpp
this->print
```
Rush则是：
```cpp
this.print
```

Rush支持无缝内联汇编，C内联汇编需要用关键字asm：
```cpp
asm mov esi,a
```
或者
```cpp
asm
{
	mov esi,a
}
```

Rush可以省略关键字asm：
```cpp
mov esi,a
```
a是一个局部变量，编译器会把它替换为：
```cpp
mov esi,[ebp+n]
```
其中n是局部变量a的偏移。
<br/>
<br/>
Rush的栈空间安排是这样的：（地址从低地址到高地址）

|上层ebp|
|:---------:|
|局部变量1|
|局部变量2|
|……|
|局部变量n|
|返回地址|
|函数参数1|
|函数参数2|
|……|
|函数参数n|
|返回值|

返回值由调用者析构，而函数参数和局部变量由被调用者析构，函数参数从右往左入栈。注意和stdcall以及cdecl都是有区别的，Rush的返回值统一放在堆栈中传递，而不是通过eax。
<br/>
<br/>
举个例子：
```cpp
friend int operator+(int a,int b) 
{
	add a,b
	mov s_ret,a
}
```
编译器生成的汇编代码是这样：
```cpp
push ebp
mov ebp , esp
add [ ebp + 8 ] , [ ebp + 12 ]
mov [ ebp + 16 ] , [ ebp + 8 ]
pop ebp
reti 8
```
其中有四条汇编语句是编译器生成的，另外两条是程序员写的。
<br/>
<br/>
由于Rush所有类和函数都是public，故友元的含义已经变了，friend和static是同义语，它们都表示该函数不会自动生成this引用，也就是该函数无法访问到本类的数据成员。
<br/>
<br/>
另外，operator不是Rush的关键字，也就是说：
```cpp
friend int operator+(int a,int b)
```
等价于
```cpp
friend int +(int a,int b) 
```

再看看如何调用int.+(int,int)这个函数，表达式
```cpp
1+2
```
会首先翻译为
```cpp
int . + ( 1 , 2 )
```
再翻译成汇编代码
```cpp
sub esp , 4
push 2
push 1
call [ & int +(int,int) ]
mov esi , esp
mov ebx , [ esi ]
add esp , 4
```
结合上面的栈空间安排表可以很清楚的看到函数的调用过程。
<br/>
<br/>
所有的寄存器也可以当做一个int类型的变量使用，比如想查看一下esp的值可以直接这样：
```cpp
puts esp
```
想计算ecx与edx的商可以这样：
```cpp
puts ecx/edx
```
判断寄存器是否为0：
```cpp
if(eax)
{
	…
}
```
但是不可以对寄存器赋值，下面这样是错误的：
```cpp
eax=1+2
```
可以改为
```cpp
1+2
mov eax,ebx
```
因为凡是返回int或者bool的表达式，编译器均会把这个返回值保存到ebx中。
<br/>
<br/>
bool和int是一样的都是占用4个字节（C++的bool只占用一个字节）。
<br/>
<br/>
Rush的汇编指令与x86大部分是相同的，只是另外增加了一些虚拟指令（也可以理解为伪指令），但是这些虚拟指令很容易转换成标准的x86指令。完整的指令列表请参考..\..\rpp\tvm.h。

##### 9. 伪代码

可以把Rush当做伪代码写，下面就是《算法导论》开篇的插入排序代码（由于Rush数组从下标0开始而《算法导论》从1开始，故稍微修改了下）：
```cpp
define ← =

void insertion_sort(rstr& a):
    for j ← 1; j<a.count; j ← j+1
		key ← a[j]
		i ← j-1
		while i>=0 && a[i]>key
			a[i+1] ← a[i]
			i ← i-1
		a[i+1] ← key

void main():
	rstr a="cab132"
	printl a
	insertion_sort a
	printl a
```

对于方法（函数）参数为空的情况，可以省略后面的括号，即
```cpp
a.count
```
等价于
```cpp
a.count()
```

##### 10. 模板函数

Rush的函数模板可以替换任何单词，跟宏差不多，比如有一个递归求和的函数(参考第5节)和一个递归求阶乘的函数：
```cpp
int sum(int a)
{
	if a<=1
		return 1
	return sum(a-1)+a
}

int pro(int a)
{
	if a<=1
		return 1
	return pro(a-1)*a
}
```
发现这两个函数极其相似，那么可以这样定义一个模板函数：
```cpp
int func<T>(int a)
{
	if a<=1
		return 1
	return func<T>(a-1) T a
}
```
像下面这样调用就可以了：
```cpp
void main()
{
	puts func<+>(10)
	puts func<*>(10)
	
	puts func<+> 10
	puts func<*> 10
}
```

Rush的模板函数十分强大，不仅支持模板函数作为类的成员，还支持模板动态生成：
```cpp
void main()
{
	int* p=1p
	p.to<char*>.printl
	putsl typeof(p.to<char*>)

	A a
	puts a.func<2>
	puts a.func<5>
}

class A
{
	int m_a=2

	int func<T>
	{
		return m_a+T
	}
}
```
可以看到模板函数和普通函数使用方法完全相同，也不需要关键字template，看起来更简洁一些。但是Rush的模板函数暂不支持类型推测，也就是说调用模板函数时后面的尖括号是必须的。

##### 11. 宏

Rush的define是在预处理阶段进行替换，功能比较弱，一般不需要使用。Rush另有一个更好用的宏mac，mac宏属于类的成员，而不是作用于全局，这样可以更好地封装。比如：
```cpp
mac fadd(a,b) a+b
```
等价于C语言的
```cpp
#define fadd(a,b) a+b
```
等价于
```cpp
mac fadd(a,b)
{
	a+b
}
```
这时用
```cpp
c=fadd2(1,2)*fadd2(1,2)
puts c
```
得到的是5，显然不是我们想要的结果，所以建议宏加上一些小括号避免优先级问题。
<br/>
<br/>
一个例子：
```cpp
void main()
{
	pro=1
	for i=2;i<=10;i++
		pro+=i
	puts pro
	
	pro=1
	for i=2;i<=10;i++
		pro*=i
	puts pro
}
```
这是一个循环求和与循环求阶乘的程序，可以看到两段代码明显有相似之处，那么用mac定义一个宏：（宏中的花括号和分号不能省略）
```cpp
mac fpro(T)
{
	pro=1;
	for(i=2;i<=10;i++)
    {
    	pro T i;
    }
	puts(pro);
}
```
最后可以这样调用：
```cpp
void main()
{
	fpro(+=)
	fpro(*=)
}
```
可以看到这与模板函数极其相似，与C语言的宏相比不需要写一堆的折行符号\，实际上Rush也没有折行符号。
<br/>
<br/>
另外，Rush还支持一种拆分宏，比如：
```cpp
#putsl(1,'abc',98)
```
宏展开后是这样：
```cpp
putsl(1)
putsl('abc')
putsl(98)
```
那么连续插入vector或者set就很方便了：
```cpp
vector<int> v
#v.push(1,99,2)
```
最新版Rush开始支持超级宏，变参宏可用超级宏实现，请参考src\example\test\11_x.rs。

##### 12. 函数指针

Rush使用一个比C语言更简单的函数指针语法，比如有一个这样的函数：
```cpp
int fadd(int a,int b)
{
	return a+b;
}
```
C语言通常是这样：
```cpp
typedef int (*FADD)(int,int);
FADD p=fadd;
p(1,2);
```
或者
```cpp
int (*p)(int,int)=fadd;
p(1,2);
```
可以看到C语言必须要指定函数指针的类型，但Rush可以直接这样调用：
```cpp
int[&fadd,1,2]
```
或者
```cpp
p=&fadd
int[p,1,2]
```
中括号左边表示返回值的类型（即使返回值为空也必须写void），中括号里面的第一个参数是函数的地址，而后面的参数编译器会自动推断出类型。
<br/>
<br/>
&fadd表示获取到函数的地址，这是一个静态地址，注意与标准C的区别，前面的取地址运算符是必须的。
<br/>
<br/>
由于Rush的函数指针只有一种类型，即
```cpp
void*
```
那么
```cpp
&fadd
```
得到一个类型为 void* 的指针常量。
<br/>
<br/>
如果有多个名字都是fadd的函数（重载），那就必须指定参数的类型：
```cpp
&fadd(int,int)
```
如果类A需要获取到类B的某一函数的地址，那就必须指定类名：
```cpp
&B.fadd(int,int)
```
或者
```cpp
&B::fadd(int,int)
```
因为Rush中作用域运算符和成员运算符是等价的。
<br/>
再看一个函数：
```cpp
void func(int& a)
{
	...
}
```
千万不要这样调用：
```cpp
a=2
void[&func,a]
```
因为func的第一个参数a的类型是引用，正确的做法是:
```cpp
a=2
void[&func,&a]
```
如果有另外一个函数：
```cpp
void func2(int* a)
{
	...
}
```
也可以这样调用：
```cpp
a=2
void[&func2,&a]
```

##### 13. 动态调用函数

暂时移除

##### 14. 元编程

暂时移除

##### 15. JS

暂时移除

##### 16. 反射

暂时移除

##### 17. 闭包

Rush支持静态闭包：
```cpp
void main()
{
    int a=2
    void[lambda(){puts a}]
}
```
对于匿名函数访问外层变量，Rush会调用赋值函数进行复制（与C++11的[=]类似）。
<br/>
<br/>
目前Rush的闭包有3个限制：
<br/>
1.不能在匿名函数中访问需要析构的外层变量。
<br/>
2.闭包需要访问的外层变量不能使用类型推断。
<br/>
3.闭包不是线程安全的。
<br/>
<br/>
Rush的匿名函数写法和普通函数差不多：
```cpp
lambda(int a,int b){}
lambda(int,int a,int b){}
```
上面第一个函数的返回值为空，带两个整型参数。第二个函数返回值为int，带两个整型参数。如果第一个参数只有类型，而没有变量名，编译器理解为这是一个返回值类型。

##### 18. 多重继承

Rush支持多重继承，不过Rush的继承方式很特别，简单而有效：
```cpp
class A
{
	int m_a

	void fa()
	{
		m_a=1
	}
}

class B:A
{
	void fb()
	{
		m_a=2
	}
}
```
然后这样使用：
```cpp
B b
b.fa
puts b.m_a
b.fb
puts b.m_a
```
可以看到B类继承了A类的数据成员m_a和函数成员fa。对于B类继承自A类，Rush只是简单地拷贝代码，因此，Rush的继承对程序员是透明的：
```cpp
class B
{
	int m_a

	void fa()
	{
		m_a=1
	}

	void fb()
	{
		m_a=2
	}
}
```
就像上面这样，直接把A类的所有代码拷贝到B类的头部。这也是Rush的哲学：熟练运用“Ctrl+C”和“Ctrl+V”可以省下人生80％的时间。
<br/>
显然，Rush的子类构造时不会自动调用父类的构造函数，且不允许父类和子类存在同名且参数类型和个数都相同的函数。
<br/>
<br/>
Rush目前支持3种继承：
1. 模板继承模板 A<T>:B<T>
2. 模板继承非模板 A<T>:C,D
3. 非模板继承非模板 E:C
<br/>
暂不支持继承模板实例 C:A<int>
<br/>
<br/>
多重继承的例子请参考：
<br/>
src\example\test\18_2.rs

##### 19. 变参函数

Rush使用中括号进行变参函数调用。
<br/>
求两个数的和：
```cpp
sum[1,2]
```
求三个数的和：
```cpp
sum[1,2,3]
```

实际上Rush会自动把参数的个数作为参数传递过去，即
```cpp
sum[1,2]
```
等价于
```cpp
int[&sum,2,1,2]
```

目前Rush的可变参数用起来虽然简单，写起来却比较痛苦：
```cpp
int sum(int count)
{
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
}
```

类成员函数使用可变参数的例子是：
<br/>
src\example\test\19_2.rs

##### 20. 默认参数

举例说明：
```cpp
int func(int a,int b=a)
{
	return a+b;
}

void main()
{
	printl func(2)
	printl func(1,2)
}
```

有必要解释一下它的工作原理，对于上面的func函数，编译器将自动生成两个函数：
```cpp
int func(int a)
{
	int b=a;
	return a+b;
}

int func(int a,int b)
{
	return a+b;
}
```

显然，Rush的默认参数比C++更灵活，因为后面的参数不仅可以访问到前面的参数，还可以访问到类的成员变量：
```cpp
class A
{
	int m_a
	
	void func(int a,int b=m_a)
	{
		...
	}
}
```

##### 21. 多线程

直接调用系统API

##### 22. 本地调用

暂时移除

##### 23. 类型转换

Rush没有强制转换，可以通过函数转换类型：
```cpp
int(2).touint
```
或者
```cpp
int(2).to<uint>
```
或者
```cpp
uint(2)
```
把有符号整数2转换成无符号整数。
<br/>
<br/>
整数和字符串之间的相互转换是很方便的：
```cpp
int(2).torstr
"123".toint
```

Rush系统库只定义了char和int的相互转换：
```cpp
char ch=`a
printl ch.toint
```
将输出97。
```cpp
char ch=-1
printl ch.toint
```
将输出255。
<br/>
注意和标准C的区别，Rush将char转换为int不会进行符号扩展。如果需要char和uint的相互转换请修改src\xlib\rs\char.rs这个文件。
<br/>
<br/>
Rush支持自动类型转换：
```cpp
void func(uint a)
{
	...
}
```
那么
```cpp
func(2)
```
会自动替换为
```cpp
func(uint(2))
```
所以，请小心定义拷贝构造函数。
<br/>
另外，和Java一样，Rush推荐尽量少使用无符号数。

##### 24. 动态数组

Rush里类名和对象名不可以同名，下面这样是错误的：
```cpp
myclass myclass;
```

汇编的关键字不可以用来命名变量，但可以命名函数，比如vector类中有一个push函数，和汇编关键字push同名，下面这样没有问题：
```cpp
vector<int> a
a.push(2)
```

Rush用rbuf代替STL的vector，用rstr代替string，用法大致相同。push就是vector的push_back，而cstr就是string的c_str。
<br/>
打开src\xlib\rs\xbase.rs可以找到：
```cpp
#define vector rbuf
#define string rstr
```

要注意的是rbuf的size方法和count方法是不同的：
```cpp
vector<int> a
a.push(2)
printl a.size
printl a.count
```
输出是：
```cpp
4
1
```

即size返回所有元素占用的字节数，而count返回元素个数，这和STL是有区别的。

##### 25. 多参数组

Rush可以用模板实现静态数组，不过默认库只提供动态数组。另外新版本不再提供数组定义的语法糖，仅使用多参数组（数组运算符带多个参数）来实现多维数组：
```cpp
import xlib/rs/rbufm.rs

void main()
{
    rbufm<int> arr(5,5)

	for i=0;i<5;i++
		for j=0;j<5;j++
			arr[i,j]=i
			arr[i,j].print
}
```

下面是三维数组的例子：
```cpp
import xlib/rs/rbufm.rs

void main()
{
    rbufm<int> arr(5,3,4)

	for i=0;i<5;i++
		for j=0;j<3;j++
			for k=0;k<4;k++
				arr[i,j,k]=j
				arr[i,j,k].print
}
```

需要注意的是，如果将数组作为函数参数传递，会传递数组的拷贝，也就是说，形参数组和实参数组互不影响。如果形参数组和实参数组需要共享内存，那么可以传递引用：
```cpp
void func(rbufm<int>& arr)
{
	...
}
```

当然也可以使用嵌套的模板来实现多维数组：
```cpp
void main()
{
    rbuf<rbuf<rbuf<int>>> arr

	arr.alloc(5)
	for i=0;i<arr.count;i++
		arr[i].alloc(3)
		for j=0;j<arr[i].count;j++
			arr[i][j].alloc(4)

	for i=0;i<arr.count;i++
		for j=0;j<arr[i].count;j++
			for k=0;k<arr[i][j].count;k++
				arr[i][j][k]=j
				arr[i][j][k].print
}
```
以上代码定义了一个5层3行4列的三维数组。

##### 26. 重载

不同类型指针作为函数参数传递时不需要强制转换，因此仅有指针类型不同的函数不能重载，下面的写法是有歧义的：
```cpp
void func(int* a)
{
	...
}

void func(char* a)
{
	...
}
```

不同类型的引用可以重载：
```cpp
void func(int& a)
{
	...
}

void func(char& a)
{
	...
}
```

相同类型的引用和非引用可以重载：
```cpp
void func(int& a)
{
	...
}

void func(int a)
{
	...
}
```

下面是自定义结构体的例子：
```cpp
class A
{
	int* m_p
	
	A()
	{
		m_p=r_new<int>5	
	}
	
	A(A& a)
	{
		m_p=r_new<int>5
		for i=0 to 4
			m_p[i]=a.m_p[i]
	}
	
	~A()
	{
		if m_p!=null
			r_delete<int>m_p
			m_p=null
	}
	
	operator=(A& a)
	{
		this.~A
		this.A(a)
	}
}
```

自定义类型必须提供两个拷贝构造函数（一个引用一个非引用），如果没有非引用版本编译器会自动生成一个和引用版本函数体完全相同的函数，赋值函数也是一样。（多个参数的构造函数不会自动增加非引用版本）
<br/>
<br/>
另外，默认参数的重载规则和C++一样，不再赘述。

##### 27. 指针和引用

Rush里引用不是“别名”，而是地址，因此千万不要这样写：
```cpp
a=2
int& b=a
```
正确的做法是：
```cpp
a=2
int& b
lea b,[ebp+s_off a]
```
其中s_off表示取得局部变量的偏移，这样的话a与b共享同一段内存。
```cpp
a.print
b.print
```
将输出两个2。
<br/>
<br/>
一般不会使用引用作为局部变量，而是使用引用作为函数参数：
```cpp
func(a)
a.print
```
函数func的定义是：
```cpp
void func(int& n)
{
	n=5
}
```
指针的用法和标准C差不多：
```cpp
a=3
p=&a
a.print
p->print
```
对于二重指针（或以上）则不可以使用类型推断：
```cpp
a=4
p=&a
int** pp=&p
```
对于三重指针（或以上）Rush也没有定义语法糖，因此需要使用嵌套的模板来定义：
```cpp
rp<rp<rp<int>>> ppp=&pp
print *(*(*ppp))
```

由此可以看到Rush的指针实际上是模板类的一个实例，具体情况请参考：
<br/>
src\xlib\rs\rp.rs

##### 28. 全局变量

全局变量定义的时候不能使用类型推断，而且必须以g_开头：
```cpp
int g_a=2
rbuf<char> g_b(3)

void main()
{
	printl g_a
	b=g_b
	b.count.printl
}
```

##### 29. 成员变量

举例说明：
```cpp
class A
{
	int m_a=2
    int m_b(3)

	A()
	{
	}
}
```

即对于类的成员变量（数据成员），可以直接在定义处初始化，上面的例子等价于：
```cpp
class A
{
	int m_a
    int m_b

	A()
	{
		m_a=2
        m_b=3
	}
}
```

即编译器会自动在该类的每一个构造函数的头部加上这么一个初始化语句。

另外，Rush将类的成员变量统一按1字节对齐：
```cpp
class A
{
	char m_a
	int m_b
}
```
这样的话
```cpp
sizeof A
```
将返回5。注意这样写的话在x86上不会有问题，但移植到arm上就有问题了，故不推荐这种写法。

##### 30. 局部变量

和javascript一样，Rush的局部变量在整个函数都是可见的，比如：
```cpp
for(int i=0;i<10;i++)
{
	...
}
if(i>=10)
	...
```
重复定义两个名字和类型都相同的变量也是允许的：
```cpp
vector<int> v
v.push(2)
v.count.print

vector<int> v
v.count.print
```
输出是
```cpp
1
0
```

因为Rush中定义变量会先析构后构造，这表示将一个变量重新初始化。故Rush的类必须支持空构造函数（即使没有编译器也会自动生成一个），并且需要在析构的时候判断是否已经析构：
```cpp
class A
{
	int* m_p

	A()
	{
		m_p=r_new<int>5	
	}

	~A()
	{
		if(m_p!=null)
			r_delete<int>m_p
			m_p=null
	}
}
```
另外，可以使用赋值运算符进行类型推断：
```cpp
a=1+2
```
等价于
```cpp
auto a=1+2
```
等价于
```cpp
int a=1+2
```

##### 31. 常量

Rush目前还不具备perl那种强大字符串处理能力，不过也向它学习了一些经验。对于双引号扩起来的字符串，编译器并不会总是提供语法糖（请参考No.6），实际上它还是标准C的以0结尾的ASCII串。所以，为了方便处理，Rush还提供了一个用单引号字符串：
```cpp
'123'
```
等价于
```cpp
rstr("123")
```
因此，单引号字符串完全可以当做内置字符串使用，转换成C字符串可以这样：
```cpp
'123'.cstr
```

表示单个字符的方法是：
```cpp
r_char('a')
```

如果整数比较长可以用下划线分隔：
```cpp
1_0000_0000
```
等价于
```cpp
100000000
```

以0x开头的常量表示16进制整型常量，以0b开头的常量表示二进制整型常量：
```cpp
0xa
```
等价于
```cpp
0b1010
```
等价于
```cpp
10
```

双精度浮点(double)常量目前只有小数写法，比如：
```cpp
0.5
```

##### 32. 控制结构

选择结构和大多数语言一样：
```cpp
if ...
	dosomething
elif ...
	dosomething
else
	dosomething
```

条件结构是这样：
```cpp
switch ...
{
	case 1:
	{
		...
	}
	case 2:
	{
		...
	}
	default:
	{
		...
	}
}
```
可以省略花括号和冒号：
```cpp
switch ...
    case 1
    	...
    case 2
    	...
    default
    	...
```

Rush的条件结构不需要break，default是可选的，但如果有必须排在最后。（实际上目前Rush还没有进行switch优化）
<br/>
<br/>
带增量的循环结构：
```cpp
for i=1;i<10;i++
	...
```
不带增量的循环：
```cpp
for i<10
	...
```
死循环有几种写法：
```cpp
for true
	...
```
或者
```cpp
for 1
	...
```
或者
```cpp
for
	...
```

while和for是同义语，是否赋初值和执行增量仅取决于条件表达式里面有没有分号。
<br/>
另外，Rush有两种continue，一种是不执行增量直接进行条件判断：
```cpp
continued
```
另一种是执行增量后再进行条件判断：
```cpp
continue
```
至于break和大多数语言一样，不再赘述。（是否要增加关键字用于跳出多重循环是一个值得讨论的问题）
<br/>
<br/>
和Go语言一样，Rush也抛弃了do...while，下面是do...while的等价写法：
```cpp
for
{
	...
	ifn(i<10)
		break
}
```

其中ifn是语法糖：
```cpp
ifn(i<10)
```
等价于
```cpp
if(!(i<10))
```

Rush的goto和jmp是同义语，下面是一个死循环：
```cpp
F:
	...
	jmp F
```

进行条件跳转也是很容易的：
```cpp
a<b
jebxz G
	printl "a less than b"
G:
```
等价于
```cpp
if(a<b)
	printl "a less than b"
```

注意只有下面int或者bool类型的表达式可以作为条件表达式：（如果使用其它类型作为条件表达式会在运行时出错）

##### 33. 模板类

模板类与C++的区别是构造函数和析构函数后面必须有尖括号和模板参数：
```cpp
class A<T>
{
	T m_a

	A<T>
	{
	}
}

void main()
{
	A<int> a
	A<A<int>> b

    a.m_a=2
	a.m_a.print
    b.m_a.m_a=3
	b.m_a.m_a.print
}
```

目前Rush的模板有3个限制：
1. 不支持模板元编程。
2. 不支持模板默认参数。
3. 不支持变参模板。
<br/>
<br/>
关于模板类嵌套请参考：
<br/>
src\example\test\33_2.rs

##### 34. 包

Rush中：
```cpp
import
```
等价于
```cpp
#include
```
使用方法：
```cpp
import "A.h"
```
或者
```cpp
import 'A.h'
```
或者
```cpp
import A.h
```

注意不要这样写：
```cpp
import <A.h>
```
因为Rush不支持尖括号文件名，用双引号括起来的文件名Rush会在当前目录和编译器目录搜索。另外，和D语言、objective-c一样，Rush会自动处理重复包含的问题。
<br/>
<br/>
Rush支持mixin和递归引用，类和函数均不需要声明即可使用。（这样就不需要向前声明）
<br/>
<br/>
打开src\xlib\rs\xbase.rs可以找到：
```cpp
#define namespace friend class
```
清楚地看到Rush的命名空间不过是一个友元类，或者可以称为静态类，即所有函数都没有this，下面举例说明：
```cpp
namespace MY
{
	//Rush没有枚举变量，只有枚举常量
	enum
	{
		c_a=2
		c_b=4
	}

	void func()
	{
		print c_a
	}
}

void main()
{
	MY.func
	MY::func
	print MY.c_a
	print MY::c_b
}
```
暂时移除命名空间嵌套和类嵌套。

##### 35. 堆

Rush的new和delete既不是运算符，也不是关键字，而是模板函数：
```cpp
p=r_new<int>(5)
```
表示从堆中分配5个int。
<br/>
释放刚才分配的内存：（不需要delete [] p）
```cpp
r_delete<int>(p)
```
一般来说，不需要使用new和delete，（一个很简单的原因，不用new就几乎不可能出现内存泄露），推荐使用rbuf来进行内存分配：
```cpp
rbuf<int> a(5)
```
因为rbuf会在析构时自动释放内存，当然也可以手动释放：
```cpp
a.free
```

可以使用数组运算符或者
```cpp
a.begin
```
或者
```cpp
a.point
```
访问刚才分配的内存。
<br/>
<br/>
如果只需要一个对象，Java通常是这样：
```cpp
B b=new B();
```
而Rush则这样：
```cpp
B b
```
一个对象通常在栈中分配就行了。

##### 36. 库

Rush内核非常小，没有内置数据结构，甚至可以认为Rush没有内置数据类型，所有类型均在外部定义，这样做的好处是可移植性强（移植到64位改动很小）。下面是以模板类的形式提供的几种数据结构：

|名称|用途|
|:--:|:--:|
|rbuf|动态数组，栈|
|rstr|字符串|
|rstrw|utf16字符串|
|rlist|双向链表，队列，栈|
|rset|红黑树|
|rdic|字典|
|rhash|哈希表|

另外，rsrc\ralgo.h提供了快速排序、二分查找、split。下面还有一些封装好的类（示例代码请参考 src\example\test\36_x.rs）：

|名称|用途|
|:--:|:--:|
|rflie|文件操作|
|rdir|遍历目录|
|rsock|TCP套接字|
|rmutex|互斥体|
|rcode|编码转换|

##### 38. 裸奔

暂时移除

##### 39. 图形界面

暂时移除

##### 41. 语言特性

待补充

##### 42. 函数对象

Rush不支持重载小括号，但是可以重载中括号实现C++的函数对象（仿函数）：
```cpp
void main()
{
	A<int,int> a=lambda(int,int n){return n*n}
	print a[3]
}

class A<T1,T2>
{
	void* m_func
	
	=(void* p)
	{
		m_func=p
	}
	
	T1 [](T2 n)
	{
		return T1[m_func,n]
	}
}
```

Rush除了&&和||所有运算符都是函数，而且Rush会在非引用返回值传递引用的情况下增加临时变量，所以不要依赖函数的传参顺序。

