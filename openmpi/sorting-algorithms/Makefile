CC     := gcc
MPICC  := mpicc
CFLAGS := -O3 -march=native -Wall -Wextra
BUILD  := build
ALGS   := bubble selection quick merge bitonic

SERIAL   := $(addprefix $(BUILD)/serial-,$(ALGS))
PARALLEL := $(addprefix $(BUILD)/parallel-,$(ALGS))

.PHONY: all clean
all: $(SERIAL) $(PARALLEL) $(BUILD)/psrs

$(BUILD):
	mkdir -p $(BUILD)

# The algorithm is picked at compile time by a -D flag, so each variant
# gets its own binary instead of overwriting a shared one.
$(BUILD)/serial-%: src/sort.c src/serial.c | $(BUILD)
	$(CC) $(CFLAGS) -D$(shell echo $* | tr a-z A-Z) $^ -o $@

$(BUILD)/parallel-%: src/sort.c src/parallel.c | $(BUILD)
	$(MPICC) $(CFLAGS) -D$(shell echo $* | tr a-z A-Z) $^ -o $@

$(BUILD)/psrs: src/sort.c src/psrs.c | $(BUILD)
	$(MPICC) $(CFLAGS) $^ -o $@

clean:
	rm -rf $(BUILD)
