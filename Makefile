
PACKAGE = ezrc
VERSION = 0.3.0

CXX      ?= g++ -std=c++0x
CXXFLAGS += -DVERSION=\"$(VERSION)\"
LDFLAGS  += 
prefix   ?= /usr/local

ifeq ($(MSYSTEM), MINGW32)
  EXEEXT    = .exe  
else
  EXEEXT    =
endif

.PHONY: all clean install uninstall

ezrc$(EXEEXT): ezrc.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

README.c: README.md ezrc$(EXEEXT)
	ezrc README.md

check: ezrc-check$(EXEEXT)
	./ezrc-check
	
ezrc-check$(EXEEXT): test.cpp README.c
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@
	
clean:
	rm ezrc$(EXEEXT) *.o
	
install: ezrc$(EXEEXT)
	mkdir -p $(prefix)/bin
	cp ezrc$(EXEEXT) $(prefix)/bin

uninstall:
	rm $(prefix)/bin/ezrc$(EXEEXT)
