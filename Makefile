SHELL := bash
.ONESHELL:
.SHELLFLAGS := -eu -o pipefail -c
.DELETE_ON_ERROR:
MAKEFLAGS += --warn-undefined-variables
MAKEFLAGS += --no-builtin-rules
MAKEFLAGS += --no-print-directory

.DEFAULT_GOAL = help

.PHONY: config clean run help test all

APP := myhttp
BUILD := build
SOURCE := src
MARKER := .initialized

SOURCES := $(wildcard **/*.c *.c **/*.h *.h)
CONFIGS := $(wildcard CMakeLists.txt **/CMakeLists.txt)

MAKESILENT=-s
VERBOSE=0
COLOR=ON
export VERBOSE MAKESILENT COLOR

$(BUILD)/$(MARKER):
$(BUILD)/$(MARKER): $(SOURCES) $(CONFIGS)
	@cmake -B$(BUILD) -S .
	@cmake --build $(BUILD)
	@touch $(BUILD)/$(MARKER)

build: ## Build project
build: $(BUILD)/$(MARKER)

clean: ## Remove build directory
clean:
	@$(RM) -rf $(BUILD)


$(BUILD)/$(APP): build
	@$(MAKE) -C $(BUILD) $(APP)

test: ## Run tests
test: build
	@ctest --test-dir $(BUILD)

run: ## Run example
run: $(BUILD)/$(APP)
run:
	@./$(BUILD)/$(APP)

help: ## Show help message
	@awk 'BEGIN {FS = ":.*##"; printf "\nUsage:\n  make \033[36m\033[0m\n"} /^[$$()% 0-9a-zA-Z_-]+:.*?##/ { printf "  \033[36m%-15s\033[0m %s\n", $$1, $$2 } /^##@/ { printf "\n\033[1m%s\033[0m\n", substr($$0, 5) } ' $(MAKEFILE_LIST)
