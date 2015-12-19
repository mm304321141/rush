/*
该程序只能以32位JIT模式运行
*/

define var

void main(){
	mov eax,1
	add eax,2
	js_main(eax)
}
	
function js_main(num){
	putsl(num);
	
	var f=function(a){
		return function(b){
			return a+b;
		};
	};
	
	putsl(f(3)(4));
	
	var k=function(n,h){
		if(n<=1){
			return 1;
		}
		return h(n,k(n-1,h));
	};
	
	putsl(k(10,function(a,b){a+b}));
	putsl(k(10,function(a,b){a*b}));
}
