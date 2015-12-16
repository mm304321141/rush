/*
反射
打印出自身的所属的函数和类
*/

import xlib/rs/eval.rs

void main()
{
#ifdef _RJIT
	p=get_cur_func_in((&main).toint)
	p->name.printl
	p->ptci->name.printl
#else
	tasm* p=&main
	p->ptfi->name.printl
	p->ptfi->ptci->name.printl
#endif
}
