CRT0 := $(LIBBUILDDIR)/crt/crt0.o

# Output object file list
obj-y := $(call mapout, crt0/crt0.o)

CRT0OBJS := $(obj-y)


$(CRT0): $(CRT0OBJS)
	@echo -e "\033[33m  \033[1mLinking CRT0\033[0m"
	@$(LD) -r -o $@ $<

$(LIBBUILDDIR)/%.o: $(LIBDIR)/%.s
	@echo -e "\033[32m    \033[1mAS\033[21m    \033[34m$<\033[0m"
	$(Q) mkdir -p $(dir $@)
	$(Q) $(CC) -c -o $@ $<
