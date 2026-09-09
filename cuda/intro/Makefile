NVCC   := nvcc
NVFLAGS := -O3
BUILD  := build

SRCS := $(wildcard src/*.cu)
BINS := $(patsubst src/%.cu,$(BUILD)/%,$(SRCS))

.PHONY: all clean
all: $(BINS)

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/%: src/%.cu | $(BUILD)
	$(NVCC) $(NVFLAGS) $< -o $@

clean:
	rm -rf $(BUILD)
