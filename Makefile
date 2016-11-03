BUILDDIR      := $(abspath build)
INCLUDES      := $(HOME)/local/include
LIBS          := $(HOME)/local/lib
common-cflags := -D_GNU_SOURCE \
                 -Wall -Wextra -Wstack-protector \
                 -fstack-protector-all -D_FORTIFY_SOURCE=2
#CFLAGS        := $(common-cflags) -O0 -g
CFLAGS        := $(common-cflags) -O2 -DNDEBUG

objs := yacal.o ui_sheet.o ui_digest.o ui_status.o ui_todo.o ui.o todo.o \
        vdir.o vector.o dstr.o utils.o

yacal: $(addprefix $(BUILDDIR)/, $(objs))
	$(CC) -I$(INCLUDES) -L$(LIBS) -MD $(CFLAGS) -g -o $@ \
		$(filter %.o,$^) -lical -licalss -lncurses

$(BUILDDIR)/%.o: %.c Makefile | $(BUILDDIR)
	$(CC) -I$(INCLUDES) -MD $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	$(RM) $(BUILDDIR)/*.[od]

.PHONY: clobber
clobber:
	$(RM) -r yacal $(BUILDDIR)

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
