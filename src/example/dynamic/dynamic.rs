/*
惰性求值的方式实现短路求值
Rush是同时支持call_by_name和call_by_need和call_by_value的语言
该示例只能以32位JIT模式运行
*/

import xlib/rs/eval.rs

void main()
{
	int* p=null

	if and(p!=null,*p==2)
		printl('and')
	
	rbuf<int> v

	if or(v.empty,v[0]!=5)
		printl('or')
	
	//and和or是惰性的，可以带多个参数
	if and(2>1,p==null,v.empty,3!=4)
		printl('true')

	if and(and(2>1),3!=4)
		printl('true')

	a=1
	b=2
	c=3

	printl and(and(a,b),c)
	printl or(or(a,b),c)

	a=0
	b=1
	c=0

	printl and(and(a,b),c)
	printl or(or(a,b),c)
	printl or(and(a,b),c)
	printl and(or(a,b),c)
}