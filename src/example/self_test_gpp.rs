import rfile.h

void main()
{
	start=xf.tick
	rf.cmd("gpp_build ..\\src\\rush.cxx")
	print "gpp_build ..\\src\\rush.cxx "
	printl xf.tick-start
	
	rf.cmd("copy /B /Y ..\\src\\rush.exe rush_gpp.exe")
	rf.cmd("mingw_test")
	
	start=xf.tick
	rf.cmd("rush_mingw -nasm ../src/rush.cxx")
	print "rush_mingw -nasm "
	printl xf.tick-start
	
	rf.cmd("copy /B /Y ..\\src\\rush.asm rush_gpp.asm")
	
	start=xf.tick
	rf.cmd("rush_gpp -nasm ../src/rush.cxx")
	print "rush_gpp -nasm "
	printl xf.tick-start
	
	if rfile.read_all_n("rush_gpp.asm")==rfile.read_all_n("..\\src\\rush.asm")
		printl "ok"
	else
		printl "error"
}
