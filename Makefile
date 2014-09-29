
ifndef SYNC
	SYNC	:= git pull
endif

CC      := $(shell xeno-config --cc) -std=c99 -Wall
CFLAGS  := $(shell xeno-config --skin native --cflags)
LDFLAGS := $(shell xeno-config --skin native --ldflags)

define target_template
all: $(1)
$(1).o: $(1).c
	@echo Compiling $$<
	@$$(CC) -c -o $$@ $$< $$(CFLAGS)
$(1): $(1).o
	@echo Linking $$^ into $$@
	@$$(CC) -o $$@ $$^ $$(LDFLAGS)
clean: clean-$(1)
clean-$(1):
	@rm -f $(1) $(1).o
run: run-$(1)
run-$(1): $(1)
	@echo -e \\nRunning $(1)...\\n
	@sudo ./$(1)
	@echo -e \\nDone running $(1)\\n
.PHONY: run-$(1) clean-$(1)
endef

$(foreach target,$(wildcard lab*.c),$(eval $(call target_template,$(target:.c=))))

sync:
	@$(SYNC)

PHONY: all sync clean run
