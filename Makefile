CC=g++
SRCS=regina.cpp
OBJS=$(SRCS:.cpp=.o)
CFLAGS=-O2 -std=gnu++98 -Wall -fno-ident
LDFLAGS=-s -static -flto -ltre `fltk-config --ldstaticflags` 
ARCH=64
ifeq ($(ARCH), 64)
	WINDRES_ARCH=pe-x86-64
else
	WINDRES_ARCH=pe-i386
endif
NAME=regina
EXE=$(NAME).exe

.PHONY: all clean
all: $(EXE)
$(EXE): $(OBJS) $(NAME).res
	$(CC) -o $(EXE) $(OBJS) $(NAME).res -m$(ARCH) $(LDFLAGS)
.cpp.o:
	$(CC) -o $@ $(CFLAGS) -m$(ARCH) -c $<

$(NAME).res: $(NAME).rc
	windres -O coff -F $(WINDRES_ARCH) $(NAME).rc $(NAME).res

clean:
	rm -f $(OBJS) $(EXE) $(NAME).res
