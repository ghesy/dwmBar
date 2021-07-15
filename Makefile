DESTDIR = /usr/local
CXXFLAGS = -O2 -march=native -std=c++11 -pthread -lX11
ifdef LAPTOP
CXXFLAGS += -DLAPTOP
endif

dwmbar : dwmbar.cpp modules.cpp

install : $(DBOUT)
	@cp -v $(DBOUT) $(DESTDIR)/bin
	@chmod -v 755 $(DESTDIR)/bin/dwmbar

clean :
	@rm -fv *.o dwmbar

.PHONY : all clean install
