### Rush Language = Python's syntax + C++'s fast + Lisp's power

##### Why need it？

Reason 1: Rush support similar Python syntax without braces, and is compatible with 60% of the C++ syntax, Rush itself using standard C++ compiler to compile or self compiled.

Reason 2: Rush can be interpreted or compiled, it dominated statically typed, no GC, and support translate to C++, so can be nearly achieved C++'s efficiency.

Reason 3: It using Lisp as intermediate layer, support mixin, macro, meta-programming and a variety of dynamic characteristics, also support call by name, call by need and call by value.

Reason 4: Rush's design goal is simple, fast, stable, fully open source, it's source code structure is much simpler than lua, but it's ability is not less than lua.

##### Rush overall design: (red part is not belong to Rush)

![github](https://github.com/roundsheep/rush/blob/gh-pages/doc/rush.png "github")

##### Rush coding style 1: (pseudo-code like 'Introduction to Algorithms')

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
```

##### Rush coding style 2: (like Python without braces)

```cpp
bool next_permutation<T>(rbuf<T>& v):
	if v.count<=1  
		return false  
	next=v.count-1  
	for  
		temp=next  
		next--  
		if v[next]<v[temp]  
			mid=v.count-1  
			for !(v[next]<v[mid])  
				mid--  
			swap<T>(v[next],v[mid])  
			reverse<T>(v,temp)  
			return true  
		if next==0  
			reverse<T>(v,0)  
			return false
```

##### Rush coding style 3: (Lisp style, S expressions separated by commas)

```cpp
void main():
	int a
	int b
	[int,=,[a,1]]
	[int,=,[b,2]]
	[rf,print,[[int,+,[a,b]]]]
```
			
##### Rush coding style 4: (this is the standard C++ syntax, this section of code can be compiled with VC++, G++ or Rush)

```cpp
static rbool proc_inherit(tsh& sh,tclass& tci,int level=0)
{
	if(level++>c_rs_deep)
		return false;
	if(tci.vfather.empty())
		return true;
	rbuf<tword> v;
	for(int i=0;i<tci.vfather.count();i++)
	{
		rstr cname=tci.vfather[i].vword.get(0).val;
		tclass* ptci=zfind::class_search(sh,cname);
		if(ptci==null)
		{
			ptci=zfind::classtl_search(sh,cname);
			if(ptci==null)
				return false;
		}
		if(!proc_inherit(sh,*ptci,level))
			return false;
		v+=ptci->vword;
	}
	v+=tci.vword;
	tci.vword=v;
	return true;
}
```

##### Rush coding style 5: (full type infer, combined with efficiency of static type and flexible of dynamic type)

```cpp
main():
	printl func(1,2)
	printl func(3.4,9.9)
	printl func('abc','efg')

func(a,b):
	c=a+b
	return c
```

##### Rush coding style 6: (combined with ASM and JS, compatible with 60% of the JS syntax)

```javascript
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
```

##### Rush coding style 7: (Lisp style, use parentheses instead of brackets)

```javascript
define lambda function

function test(){
	[= f [lambda [n]
		[cond [== n 0]
			0
			[+ n [f [- n 1]]]]
	]]
	[putsl [f 10]]
}
```

<br/>
##### Benchmark:

(Intel i5 3.3GHZ, using command 'xxx -nasm ..\src\rush.cxx' self compiled , scan C++ code with 25000 lines, get a ASM file with 250000 lines, it shows the score 1/3 of Rush PK C++. Because of Rush itself using C++11's move and short string optimization, but Rush itself not support the optimization, so I Can believe the really Rush's benchmark can be achieved C++'s 1/1.5, this is close to C++. It also shows Rush's compile speed is very fast, if don't use nasm, it compiled itself requires only 5 seconds)

|EXE name|EXE size|time consuming|PK C++|backend|
|:-------|:-------|:-------------|:-----|:------|
|rush|673 KB|4813 ms|1 -- 1|vs -O2 (this is C++'s speed)|
|rush_mingw|1049 KB|6203 ms|1 -- 1.3|g++ -O2|
|rush_vs|774 KB|14453 ms|1 -- 3|lisp to c++ with vs -O2|
|rush_nasm|762 KB|53579 ms|1 -- 11.1|lisp to nasm|
|rush_gpp|1679 KB|79000 ms|1 -- 16.4|lisp to c++ with g++ -O0|

<br/>
##### Rush supports multiple operation modes, as follows:

##### JIT:

1. cd to the bin directory
2. Command line typing: rush -jit ..\src\example\test\1.rs

##### JIT (64 bit):

1. cd to the bin directory
2. Command line typing: rush64 -jit ..\src\example\test\1.rs

##### Interpret:

1. cd to the bin directory
2. Command line typing: rush ..\src\example\test\1.rs

##### Compile and run (using NASM as backend):

1. cd to the bin directory
2. Command line typing: rnasm ..\src\example\test\1.rs

##### Compile and run (using NASM as backend, 64 bit):

1. cd to the bin directory
2. Command line typing: rnasm64 ..\src\example\test\1.rs

##### Compile and run (translate to C++ and using G++ as backend):

1. cd to the bin directory
2. Command line typing: gpp ..\src\example\test\1.rs

##### Translate to C++ (64 bit):

1. cd to the bin directory
2. Command line typing: rush64 -gpp ..\src\example\64bit_test.rs
3. Import the resulting file src\example\test\64bit_test.cpp with Visual Studio (or using 64 of G++)
4. Select x64, Press F7

##### Compile and run (translate to ASM, then translate to C++, in alpha):

1. cd to the bin directory
2. Command line typing: rcpp ..\src\example\test\53.rs

##### Interpret (JS or Lisp, in alpha):

1. cd to the bin directory
2. Command line typing: rush -jit ..\src\example\dynamic\js.rs

##### HTML5 run (translate to JS, not emscripten, currently only support chrome, in alpha):

1. cd to the bin directory
2. Command line typing: rush -js ..\src\example\test\53.rs
3. Double-click ..\src\example\test\53.html

##### Mac OS or Ubuntu interpretation runs (in alpha):

1. Make sure the clang (3.5 or higher) or g++ (4.8 or higher)
2. cd to the bin directory
3. Command line typing: g++ ../src/rush.cxx -o rush -w -m32 -std=c++11
4. Command line typing: ./rush ../src/example/test/50.rs

##### IOS:

1. cd to the bin directory
2. Command line typing: rush -gpp ..\src\example\test\1.rs
3. The resulting src\example\test\1.cpp and ext\mingw\gpp.h two files into xcode
4. Modify the main function as required, comment out the header files 'windows.h'

##### Android: (green integrated package, independent of other environments)

1. Ensure that the compiled environment is windows with 64 bit
2. Download a key android toolkit and unzip to a path without space or Chinese (1.1G) http://pan.baidu.com/s/1c0oc3Ws
3. Click create_proj.bat
4. Enter the project name such as 'test', waiting for the end of the command window
5. Click proj\test\build_cpp.bat
6. Wait a few minutes, command window appears "Press any key to continue."
7. Press 'Enter' and wait for the end of the command window
8. Get proj\test\proj.android\bin\xxx.apk after successfully
9. If need, use 'rush -gpp' command to translate Rush to CPP and include it into the proj\test\Classes\HelloWorldScene.cpp

##### Third party IDE edit code:

1. Run ext\ide\SciTE.exe
2. Click File -> Open
3. Select the ..\src\example\test\1.rs, click 'Open'
4. Press F5 to run the program (or F7 generate EXE)

<br/>

##### Rush re-compiled from source code:

1. Be sure to install the VS2012 update4 or VS2013
2. Open src\proj\rush.sln
3. Select 'Release' mode (not support 'Debug' because of JIT)
4. Press F7, will generate bin\rush.exe after successfully

##### Self compiled (using NASM or G++ as backend):

1. Double-click self_test.bat
2. Wait a few minutes will generate bin\rush_nasm.exe and bin\rush_gpp.exe (Rush actually complete self compiled requires only 5 seconds, the bottleneck in NASM, reportedly Chez Scheme self compiled is 5 seconds)
3. Note that only NASM mode and GPP mode are available after self compiled

##### Debug:

1. cd to the bin directory
2. Command line typing: rush -gpp ..\src\example\test\1.rs
3. Using gdb or Visual Studio to debugging the src\example\test\1.cpp

##### Auto test example:

1. Double-click the bin\example_test.bat

##### Platform support:

|-|Win32|Win64|Linux32|Linux64(or Mac OS)|
|:-------|:-------|:-------------|:-----|:------|
|C++ -> Lisp|stable|stable|alpha|alpha|
|JS -> Lisp|alpha|todo|todo|todo|
|Python -> Lisp|only syntax|only syntax|only syntax|todo|
|Lisp -> Interpret|alpha|todo|todo|todo|
|Lisp -> ASM|stable|stable|beta|todo|
|Lisp -> C++|stable|stable|beta|todo|
|ASM -> NASM|stable|relative stable|todo|todo|
|ASM -> JIT|stable|relative stable|todo|todo|
|ASM -> Interpret|stable|todo|alpha|todo|
|ASM -> C++|alpha|todo|todo|todo|
|ASM -> JS|alpha|todo|alpha|todo|

##### Language feature:

|Backend|Lisp Macro|C Macro|Closure|Eval|Reflect|Dynamic Type|GC|Template|Operator Overload|
|:-------|:-------|:-------|:--------|:-----|:------|:-------|:--------|:-----|:------|
|ASM -> NASM|no|yes|only static|no|no|no|no|yes|yes|
|ASM -> JIT|yes|yes|only static|yes|yes|no|no|yes|yes|
|ASM -> Interpret|no|yes|only static|no|yes|no|no|yes|yes|
|Lisp -> Interpret|yes|yes|yes|yes|yes|yes|yes|no|no|
|Lisp -> C++|no|yes|no|no|no|no|no|yes|yes|

<br/>

QQ group: 34269848

E-mail: 287848066@qq.com
