/*
完全类型推导（静态类型的效率，动态类型的灵活）
*/

main():
	printl func(1,2)
	printl func(3.4,9.9)
	printl func('abc','efg')

func(a,b):
	c=a+b
	return c
