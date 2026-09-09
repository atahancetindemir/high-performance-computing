NVCC   := nvcc
NVFLAGS := -O3
BUILD  := build

SRCS := $(wildcard *.cu)
BINS := $(patsubst %.cu,$(BUILD)/%,$(SRCS))

.PHONY: all clean
all: $(BINS)

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/%: %.cu | $(BUILD)
	$(NVCC) $(NVFLAGS) $< -o $@

clean:
	rm -rf $(BUILD)
