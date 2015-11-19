CC=clang
CFLAGS=-Weverything -Wno-missing-prototypes
LDFLAGS=-lSDL2
PROJ=$(notdir $(CURDIR))
OBJDIR=obj
BINDIR=bin

SOURCES=$(wildcard *.c)
DEPENDS=$(patsubst %.c,$(OBJDIR)/%.d,$(SOURCES))
OBJECTS=$(patsubst %.c,$(OBJDIR)/%.o,$(SOURCES))

.PHONY: $(PROJ)
$(PROJ): $(BINDIR)/$(PROJ)
.PHONY: clean
clean:
	! test -d $(OBJDIR) || rm -r $(OBJDIR)
	! test -d $(BINDIR) || rm -r $(BINDIR)

$(BINDIR):
	mkdir -p $@

$(BINDIR)/$(PROJ): $(BINDIR) $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

$(OBJDIR):
	mkdir -p $@

$(OBJDIR)/%.o: %.c $(OBJDIR) Makefile
	$(CC) -c $(CFLAGS) -MMD $< -o $@

-include $(DEPENDS)
