import xbase.rs

namespace xf
{
	enum
	{
		X_SEEK_SET=0,
		X_SEEK_END=2,
	};
	
	mac mov64_r8_rax
	{
		rbyte 0x49;
		rbyte 0x89;
		rbyte 0xc0;
	}
	
	mac mov64_r9_rax
	{
		rbyte 0x49;
		rbyte 0x89;
		rbyte 0xc1;
	}
	
	static void print(const char* s)
	{
		sub rsp,40
		mov64 rdx,s
		mov64 rcx,"%s"
		calle "printf"
		add rsp,40
	}
	
	static void print_int(int a)
	{
		//x64必须分配4个参数加返回地址的栈
		sub rsp,40
		mov64 rdx,a
		mov64 rcx,"%d\n"
		calle "printf"
		add rsp,40
	}

	static void vsnprintf(char* dst,int count,const char* f,char* args)
	{
		sub rsp,40
		mov64 rcx,dst
		mov64 rdx,count
		mov64 rax,f
		mov64_r8_rax
		mov64 rax,args
		mov64_r9_rax
		calle "_vsnprintf",16
		add rsp,40
	}

	static void sscanf(const char* src,const char* f,uint* n)
	{
		sub rsp,40
		mov64 rcx,src
		mov64 rdx,f
		mov64 rax,n
		mov64_r8_rax
		calle "sscanf",16
		add rsp,40
	}
	
	void sprintf(char* dst,char* format,int n)
	{
		sub rsp,40
		mov64 rcx,dst
		mov64 rdx,format
		mov64 rax,n
		mov64_r8_rax
		calle "sprintf",16
		add rsp,40
	}

	void sprintf(char* dst,char* format,uint n)
	{
		sub rsp,40
		mov64 rcx,dst
		mov64 rdx,format
		mov64 rax,n
		mov64_r8_rax
		calle "sprintf",16
		add rsp,40
	}
	
	void sprintf64(char* dst,char* format,int64 n)
	{
		sub rsp,40
		mov64 rcx,dst
		mov64 rdx,format
		mov64 rax,n
		mov64_r8_rax
		calle "sprintf",16
		add rsp,40
	}

	void sprintf64(char* dst,char* format,double n)
	{
		sub rsp,40
		mov64 rcx,dst
		mov64 rdx,format
		mov64 rax,n
		mov64_r8_rax
		calle "sprintf",16
		add rsp,40
	}

	static void itoa(void* dst,int n)
	{
		sub rsp,40
		mov64 rcx,dst
		mov64 rdx,"%d"
		mov64 rax,n
		mov64_r8_rax
		calle "sprintf",16
		add rsp,40
	}

	static void utoa(void* dst,uint n)
	{
		sub rsp,40
		mov64 rcx,dst
		mov64 rdx,"%u"
		mov64 rax,n
		mov64_r8_rax
		calle "sprintf",16
		add rsp,40
	}

	static void dtoa(void* dst,double n)
	{
		sprintf64(dst,"%f",n);
	}

	static void int64toa(void* dst,int64 n)
	{
		sprintf64(dst,"%I64d",n);
	}

	static int atoi(char* s)
	{
		int result;
		sscanf(s,"%d",&result);
		return result;
	}

	static uint atou(char* s)
	{
		uint result;
		sscanf(s,"%u",&result);
		return result;
	}

	static double atod(char* s)
	{
		double result;
		sscanf(s,"%lf",&result);
		return result;
	}

	static void atoint64(char* s,int64* result)
	{
		sub rsp,40
		mov64 rcx,s
		mov64 rdx,result
		calle "atoint64",16
		add rsp,40
	}

	static void exit(int a)
	{
		sub rsp,40
		mov64 rcx,a
		calle "exit",5
		add esp,40
	}

	static uchar* malloc(int size)
	{
		sub rsp,40
		mov64 rcx,size
		calle "malloc",5
		mov64 s_ret,rax
		add esp,40
	}

	static void free(void* p)
	{
		sub rsp,40
		mov64 rcx,p
		calle "free",5
		add esp,40
	}

	static void memcpy(void* dst,const void* src,int size)
	{
		sub rsp,40
		mov64 rcx,dst
		mov64 rdx,src
		mov64 rax,size
		mov64_r8_rax
		calle "memcpy",16
		add rsp,40
	}
	
	static int strlen(const char* s)
	{
		sum=0
		for ;!s->empty;s++
			sum++
		return sum
	}

	static void strcpy(char* dst,char* src)
	{
		for *src!=0
			*dst=*src
			dst++
			src++
		*dst=0
	}

	static int strlenw(const wchar* p)
	{
		int ret=0;
		while(*p!=0)
		{
			ret++;
			p++;
		}
		return ret;
	}

	static int tick()
	{
		sub rsp,40
		calle "GetTickCount",1
		mov s_ret,eax
		add rsp,40
	}

	static void sleep(int milliseconds)
	{
		sub rsp,40
		mov64 rcx,milliseconds
		calle "Sleep",5
		add esp,40
	}

	static int getch()
	{
		calle "_getch",1
		mov s_ret,eax
	}

	static int cmd(const char* p)
	{
		sub rsp,40
		mov64 rcx,p
		calle "system",5
		mov s_ret,eax
		add esp,40
	}
	
	static int ftell(void* fp)
	{
		sub rsp,40
		mov64 rcx,fp
		calle "ftell",5
		mov s_ret,eax
		add esp,40
	}

	static int64 ftell64(void* fp)
	{
		return ftell(fp)
	}
	
	static int fseek(void* fp,int off,int start)
	{
		sub rsp,40
		mov64 rcx,fp
		mov64 rdx,off
		mov64 rax,start
		mov64_r8_rax
		calle "fseek",16
		mov s_ret,eax
		add rsp,40
	}

	static int fseek64(void* fp,int64 off,int start)
	{
		return fseek(fp,off.toint,start)
	}
	
	static void* fopen(char* name,char* mode)
	{
		sub rsp,40
		mov64 rcx,name
		mov64 rdx,mode
		calle "fopen",16
		add rsp,40
		mov64 s_ret,rax
	}

	static void* _wfopen(wchar* name,wchar* mode)
	{
		sub rsp,40
		mov64 rcx,name
		mov64 rdx,mode
		calle "_wfopen",16
		add rsp,40
		mov64 s_ret,rax
	}

	static int fclose(void* fp)
	{
		sub rsp,40
		mov64 rcx,fp
		calle "fclose",5
		mov s_ret,eax
		add esp,40
	}

	static int fread(void* buf,int size,int count,void* fp)
	{
		sub rsp,40
		mov64 rcx,buf
		mov64 rdx,size
		mov64 rax,count
		mov64_r8_rax
		mov64 rax,fp
		mov64_r9_rax
		calle "fread",16
		add rsp,40
		mov s_ret,eax
	}

	static int fwrite(const void* buf,int size,int count,void* fp)
	{
		sub rsp,40
		mov64 rcx,buf
		mov64 rdx,size
		mov64 rax,count
		mov64_r8_rax
		mov64 rax,fp
		mov64_r9_rax
		calle "fwrite",16
		add rsp,40
		mov s_ret,eax
	}

	static int _wremove(wchar* name)
	{
		sub rsp,40
		mov64 rcx,name
		calle "_wremove",5
		mov s_ret,eax
		add esp,40
	}
}

#define FILE_ATTRIBUTE_DIRECTORY 0x00000010

struct R_WIN32_FIND_DATAW
{
	int dwFileAttributes;
	int64 ftCreationTime;
	int64 ftLastAccessTime;
	int64 ftLastWriteTime;
	int nFileSizeHigh;
	int nFileSizeLow;
	int dwReserved0;
	int dwReserved1;
	wchar cFileName[260];
	wchar cAlternateFileName[14];
};

struct R_CRITICAL_SECTION
{
	char buf[24];
};
