apmode : apmode.cc
	g++ `root-config --cflags --libs` apmode.cc -o apmode
clean :
	rm -f apmode

upload : 
	git add -A && \
	git commit -m "Update" && \
	git push origin master
	
