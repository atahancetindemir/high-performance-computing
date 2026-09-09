MPICC  := mpicc
CFLAGS := -O3 -march=native -Wall -Wextra
BUILD  := build

SRCS := $(wildcard src/*.c)
BINS := $(patsubst src/%.c,$(BUILD)/%,$(SRCS))

.PHONY: all clean
all: $(BINS)

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/calculate_pi: CFLAGS += -ffast-math -fopenmp-simd

$(BUILD)/%: src/%.c | $(BUILD)
	$(MPICC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(BUILD)
