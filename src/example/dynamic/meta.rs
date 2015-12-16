/*
元编程
该示例只能以32位JIT模式运行
*/

import xlib/rs/eval.rs

void main()
{
	evalue('printl 123')
	eval('printl 123',get_cur_func,get_cur_ebp)

	a=2
	evalue('printl a')
	evalue('a=3')
	printl a

	printl evalue('return 99')
	printl eval('return eval(\'return 98\',null,0)',get_cur_func,get_cur_ebp)

	C c
	c.test
}

class C
{
	int m_c=5
	
	void test()
	{
		printl(get_cur_func->name)
		printl(get_up_func->name)
		printl(get_cur_func->ptci->name)

		evalue('printl this.m_c')
	}
}
