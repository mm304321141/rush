/*
该程序只能以32位JIT模式运行
*/

void main(){
	mov eax,1
	add eax,2
	test1(eax)
	test2()
} 
	
function test1(b){
	printl(b)
	
	function sum(a){
		cond(==(a,0),
			0,
			+(a,sum(-(a,1))))
	}
	printl(sum(100))
	
	=(f,function(a){
		function(b){
			+(a,b)
		}
	})
	printl(f(5)(6))
	
	=(c,{10,20,30})
	printl(c[0])
	printl(c[1])
	printl(eval("c[2]"))
	printl(stringify(c))
	
	=(obj,{
		a:2,
		
		get:function(){
			this.a	
		},
		
		set:function(a){
			=(this.a,a)
		}
	})
	
	printl(obj.get())
	obj.set(5)
	printl(obj.get())
}

function test2(){
	call_cps(
		=(sum,function(a){
			cond(==(a,0),
				0,
				+(a,sum(-(a,1))))
		})
		putsl(sum(10))
	)
}
