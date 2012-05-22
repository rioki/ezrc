
UNAME = $(shell uname -o)
ifeq ($(UNAME), Msys)
	EXEEXT = .exe
endif

.PHONY: clean

ezrc: ezrc.cpp

clean:
	rm ezrc$(EXEEXT)