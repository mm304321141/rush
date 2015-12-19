/*
该程序只能以32位JIT模式运行
*/

void main()
{
	test1()
	test2()
	test3()
	test4()
	test5()
	test6()
	test7()
}

function test1(){
	sum=0
	for(i=0;i<=10;i++){
		for(j=0;j<=10;j++){
			sum=sum+i
		}
	}
	[putsl sum]
	putsl(sum)
	f=function(a){
		function(b){
			a+b
		}
	}
	[putsl [[f 3] 4]]
	putsl((f(3))(4))
	putsl(f(3)(4))
	
	function pro(n){
		if(n<=0){
			0;
		}
		else{
			n+pro(n-1);
		}
	}
	putsl(5*3)
	putsl(pro(100))
	putsl("111a")
	a=[_JSON 0 99 "1a" 989 "abc" 123]
	putsl([. a "0"])
	putsl([. a "1a"])
	putsl([. a 0])
	putsl(a.abc)
	putsl("end----")
}

function test2()
{
	a=[_JSON 0 99 "1a" 989 "abc" [_JSON 
		"tt" "tttt" "f" function(a){
			if(a<=3){
				a=a*a
				a+a
			}
			else{
				a+a
			}
		}
	]]
	putsl(a.abc.tt)
	putsl(a.abc.f(3))
	putsl(a["abc"].tt)
	putsl(a["abc"]["tt"])
	putsl(a["abc"].f(3))
	putsl(a["abc"]["f"](4))
	putsl("end----")
}

function test3()
{
	[= f [function [a] 
			[+ a 2]
		]
	]
	[= a 99]
	[putsl [+ 1 2]]
	[putsl a]
	[putsl [f 4]]
	putsl("end----")
}

function test4()
{
	a={0:99,"1a":989,aaa:123,"abc":{ 
		"tt":"tttt","f":function(a){
			if(a<=3){
				return a*a;
				a+a;
			}
			else{
				a+a;
			}
		}
	}};
	putsl(a.abc.tt);
	putsl(a.abc.f(3));
	putsl(a["abc"].tt);
	putsl(a["abc"]["tt"]);
	putsl(a["abc"].f(3));
	putsl(a["abc"]["f"](4));
	putsl(a.aaa);
	putsl("end----");
}

function test5(){
	a={1,2,"abc",4};
	putsl(a[0]);
	putsl(a[1]);
	putsl(a[2]);
	putsl(a[3]);
	putsl(stringify(a))
	a[4]=9
	putsl(stringify(a))
	putsl(a[4])
	
	//putsl(stringify(code_to_data([putsl [+ 1 2]])))
	//putsl(stringify(code_to_data(putsl(1+2))))
	
	eval("putsl(a[4])")
	
	eval("a[4]=10")
	putsl(a[4])
	eval("putsl(a[4])")
	
	putsl("end----");
}

function test6(){
	obj={
		a:2,
		b:1.5,
		
		get:function(){
			return this.a
		},
		
		set:function(a){
			this.a=a
		}
	}
	
	temp=stringify(obj)
	putsl(temp)
	
	putsl(obj.a)
	putsl(obj.get())
	obj.set(3)
	putsl(obj.a)
	putsl(obj.get())
	obj.a=4
	putsl(obj.a)
	putsl(obj.get())
	
	function f(b){
		b.a=5
	}
	
	f(obj)
	putsl(obj.a)
	putsl(obj.get())
	
	putsl(eval(temp).a)
	
	putsl(0.5)
	c=0.3
	putsl(c+0.9)
	putsl(obj.b)
	
	c.f=function(){
		putsl(999)
	}
	c.f()
	obj.b.a=1
	putsl(obj.b.a)
	
	putsl(string(2))
	putsl(typeof(string(2)))
	
	putsl(typeof(long("123")))
	putsl(long("123"))
	putsl(long(1.6))
	putsl(double(2))
	
	putsl(length(obj))
	putsl(length("12345"))
	putsl(string_sub("12345",2,5))
	
	child={a:100}
	child.prototype=obj
	putsl(obj.a)
	putsl(child.a)
	putsl(child.b)
	
	putsl("end----");
}

#define lambda function

function test7(){
	[= f [lambda [n]
		[cond [== n 0]
			0
			[+ n [f [- n 1]]]]
	]]
	[putsl [f 10]]
}
