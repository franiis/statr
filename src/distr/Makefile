SOURCE_FILES = runif.c
OPT = -Wall

define cc-command
$(CC) $(CFLAGS) $< -o $@
endef

distr: $(SOURCE_FILES)
	$(cc-command)