BUILDDIR      := $(abspath build)
INCLUDES      := $(HOME)/local/include
LIBS          := $(HOME)/local/lib
common-cflags := -std=gnu99 -D_GNU_SOURCE \
                 -Wall -Wextra -Wstack-protector \
                 -fstack-protector-all
CFLAGS        := $(common-cflags) -O0 -g
#CFLAGS        := $(common-cflags) -D_FORTIFY_SOURCE=2 -O2 -DNDEBUG

objs := yacal.o ui_console.o ui_sheet.o ui_digest.o ui_status.o ui_todo.o ui.o \
	pager.o scroller.o todo.o vdir.o vector.o string.o utils.o

all: $(addprefix $(BUILDDIR)/,yacal uc_string uc_pager)

$(BUILDDIR)/yacal: $(addprefix $(BUILDDIR)/, $(objs))
	$(CC) -I$(INCLUDES) -L$(LIBS) -MD $(CFLAGS) -o $@ \
		$(filter %.o,$^) -lical -licalss -lncurses -lreadline

$(BUILDDIR)/uc_string: $(addprefix $(BUILDDIR)/, uc_string.o string.o vector.o)
	$(CC) -I$(INCLUDES) -L$(LIBS) -MD $(CFLAGS) -o $@ \
		$(filter %.o,$^) -lcheck -lpthread -lrt -lm

$(BUILDDIR)/uc_pager: $(addprefix $(BUILDDIR)/, uc_pager.o pager.o string.o \
                        vector.o)
	$(CC) -I$(INCLUDES) -L$(LIBS) -MD $(CFLAGS) -o $@ \
		$(filter %.o,$^) -lcheck -lpthread -lrt -lm

$(BUILDDIR)/%.o: %.c Makefile | $(BUILDDIR)
	$(CC) -I$(INCLUDES) -MD $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	$(RM) $(BUILDDIR)/*.[od]

.PHONY: clobber
clobber:
	$(RM) -r $(BUILDDIR)

.PHONY: doc
doc: | $(BUILDDIR)
	doxygen

.PHONY: dev
dev: .vimrc | $(BUILDDIR)
	ctags -f $(BUILDDIR)/tags -R
	cscope -bqR -f$(BUILDDIR)/cscope.out -s$(CURDIR)

.vimrc:
	echo "set tags+=$(BUILDDIR)/tags" > $@
	echo "cscope add $(BUILDDIR)/cscope.out" >> $@

$(BUILDDIR):
	mkdir -p $@

-include $(BUILDDIR)/*.d
