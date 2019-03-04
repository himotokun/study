COMP=g++
FLAGS=-Wall -c -g -ansi -pedantic
SRCFILES=main.cpp lexlib.cpp buffer.cpp syntax.cpp errors.cpp game.cpp parser.cpp
SRCDIR=src
OBJDIR=obj
EXEC=r
CLEAN=$(EXEC) $(OBJDIR)
FIN_CL=*~ $(SRCDIR)/*~
INSTDIR=/usr/local/bin

OBJFILES=$(SRCFILES:.cpp=.o)
SRC=$(addprefix $(SRCDIR)/,$(SRCFILES))
OBJ=$(addprefix $(OBJDIR)/,$(OBJFILES))

.PHONY: all clean install uninstall

all: $(SRC) $(EXEC)
	rm -rf $(FIN_CL)

$(EXEC): $(OBJ)
	$(COMP) $(OBJ) -o $(EXEC)

$(OBJDIR)/%.o:$(SRCDIR)/%.cpp
	[ -d $(OBJDIR) ] || mkdir $(OBJDIR)
	$(COMP) $(FLAGS) $< -o $@

clean:
	rm -rf $(CLEAN)

install:
	install $(EXEC) $(INSTDIR)

uninstall:
	rm -rf $(INSTDIR)/$(EXEC)
