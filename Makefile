ROOTVERSION := $(shell root-config --version | tr '/' '_')

apmode$(ROOTVERSION) : apmode.cc
	g++ apmode.cc -o apmode$(ROOTVERSION) `root-config --cflags --libs` && \
	ln -sf apmode$(ROOTVERSION) apmode

clean :
	rm -f apmode

upload : 
	doxygen && \
	git add -A && \
	git commit -m "Update" && \
	git push -f origin master

fetch :
	git fetch && \
	git merge origin/master

test :
	@echo aa$(ROOTVERSION)aa
