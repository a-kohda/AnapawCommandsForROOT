apmode : apmode.cc
	g++ `root-config --cflags --libs` apmode.cc -o apmode
clean : 
	rm -f apmode
