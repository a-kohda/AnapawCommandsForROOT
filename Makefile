apmode : apmode.cc
	g++ apmode.cc -o apmode `root-config --cflags --libs`
clean :
	rm -f apmode

upload : 
	doxygen && \
	git add -A && \
	git commit -m "Update" && \
	git push origin master

fetch :
	git fetch && \
	git merge origin/master

