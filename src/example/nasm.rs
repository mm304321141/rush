﻿/*
编译生成exe并运行
*/

import rfile.h
import rdir.h

void main()
{
	v=rf.get_param
	if v.count!=5
		printl('invaild param')
		return
	src=v[2]
	if src.get_left==r_char('\"')
		src=src.sub(1,src.count-1)
	if rf.cmd(r_cond<rstr>(v[4]!='win64','rush','rush64')+' -nasm '+src)
		printl('compile error')
		return
	src_name=rdir.get_real_name(src)
	if rf.cmd('..\\ext\\nasm\\nasm.exe -f '+v[4]+' '+src_name+'.asm -o '+
		src_name+'.obj')
		clear(src_name)
		printl('asm error')
		return
	if v[4]!='win64'
		cmd='..\\ext\\nasm\\GoLink.exe /console /mix /entry _main \"'
	else
		cmd='..\\ext\\nasm\\GoLink.exe /console /mix /entry main \"'
	cmd+=src_name+'.obj\" msvcrt.dll ws2_32.dll kernel32.dll user32.dll'
	if rf.cmd(cmd)
		clear(src_name)
		printl('link error')
		return
	if v[4]!='win64'
		clear(src_name)
	if v[3]=='-build'
		;
	elif v[3]=='-del'
		rf.cmd(rdir.std_dir_rev(src_name+'.exe'))
		//if v[4]=='win64'
		//	return
		while rfile.remove(rcode.trans_gbk_to_utf8(src_name)+'.exe')
			xf.sleep(100)//很可能是由于golink没有关闭EXE导致无法删除
	elif v[3]=='-check'
		if rfile.remove(rcode.trans_gbk_to_utf8(src_name)+'.exe')
			printl 'delete error'
}

void clear(rstr name)
{
	name=rcode.trans_gbk_to_utf8(name)
	rfile.remove(name+'.asm')
	rfile.remove(name+'.obj')
}