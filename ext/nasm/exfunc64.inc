bshl:
	mov eax,ecx
	mov ecx,edx
	shl eax,cl
	ret

bshr:
	mov eax,ecx
	mov ecx,edx
	shr eax,cl
	ret
	
bsar:
	mov eax,ecx
	mov ecx,edx
	sar eax,cl
	ret

faddl:
	movsd xmm0,[rcx]
	addsd xmm0,[rdx]
	movsd [rcx],xmm0
	ret
	
fsubl:
	movsd xmm0,[rcx]
	subsd xmm0,[rdx]
	movsd [rcx],xmm0
	ret
	
fmull:
	movsd xmm0,[rcx]
	mulsd xmm0,[rdx]
	movsd [rcx],xmm0
	ret
	
fdivl:
	movsd xmm0,[rcx]
	divsd xmm0,[rdx]
	movsd [rcx],xmm0
	ret
	
fclsbl:
	movsd xmm0,[rcx]
	comisd xmm0,[rdx]
	mov ebx,0
	setb bl
	mov eax,ebx
	ret
	
addl:
	mov rsi,[rcx]
	mov rdi,[rdx]
	add rsi,rdi
	mov [rcx],rsi
	ret
	
subl:
	mov rsi,[rcx]
	mov rdi,[rdx]
	sub rsi,rdi
	mov [rcx],rsi
	ret
	
imull:
	mov rsi,[rcx]
	mov rdi,[rdx]
	imul rsi,rdi
	mov [rcx],rsi
	ret
	
idivl:
	push rcx
	mov rax,[rcx]
	mov rcx,[rdx]
	mov rdx,-1
	cmp rax,0
	jl idivl_end
	mov rdx,0
idivl_end:
	idiv rcx
	pop rcx
	mov [rcx],rax
	ret
	
imodl:
	push rcx
	mov rax,[rcx]
	mov rcx,[rdx]
	mov rdx,-1
	cmp rax,0
	jl imodl_end
	mov rdx,0
imodl_end:
	idiv rcx
	pop rcx
	mov [rcx],rdx
	ret
	
clsbl:
	mov rsi,[rcx]
	mov rdi,[rdx]
	cmp rsi,rdi
	mov rbx,0
	setl bl
	mov rax,rbx
	ret
	