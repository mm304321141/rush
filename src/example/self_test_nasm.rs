import rfile.h

void main()
{
	start=xf.tick
	rf.cmd("rnasm_build ..\\src\\rush.cxx")
	print "rnasm_build ..\\src\\rush.cxx "
	printl xf.tick-start
	
	rf.cmd("copy /B /Y ..\\src\\rush.exe rush_nasm.exe")
	
	start=xf.tick
	rf.cmd("rush -nasm ../src/rush.cxx")
	print "rush -nasm "
	printl xf.tick-start

	rf.cmd("copy /B /Y ..\\src\\rush.asm rush_nasm.asm")
	
	start=xf.tick
	rf.cmd("rush_nasm -nasm ../src/rush.cxx")
	print "rush_nasm -nasm "
	printl xf.tick-start
	
	start=xf.tick
	if rfile.read_all_n("rush_nasm.asm")==rfile.read_all_n("..\\src\\rush.asm")
		printl "ok"
	else
		printl "error"
}
