COMP=gcc
FLAGS=-Wall -c -g -ansi -pedantic
SRCFILES=main.c parser.c strlib.c interface.c cmdlib.c keylib.c
SRCDIR=src
OBJDIR=obj
EXEC=qsh
CLEAN=$(EXEC) $(OBJDIR) *~ $(SRCDIR)/*~ $(EXEC)?* $(SRCDIR)/.??*
FIN_CL=*~ $(SRCDIR)/*~
INSTDIR=/usr/local/bin

OBJFILES=$(SRCFILES:.c=.o)
SRC=$(addprefix $(SRCDIR)/,$(SRCFILES))
OBJ=$(addprefix $(OBJDIR)/,$(OBJFILES))

.PHONY: all clean install uninstall

all: $(SRC) $(EXEC)
	rm -rf $(FIN_CL)

$(EXEC): $(OBJ)
	$(COMP) $(OBJ) -o $(EXEC)

$(OBJDIR)/%.o:$(SRCDIR)/%.c
	[ -d $(OBJDIR) ] || mkdir $(OBJDIR)
	$(COMP) $(FLAGS) $< -o $@

clean:
	rm -rf $(CLEAN)

install:
	install $(EXEC) $(INSTDIR)

uninstall:
	rm -rf $(INSTDIR)/$(EXEC)
