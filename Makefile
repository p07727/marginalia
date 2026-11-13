# Build and verification targets of Marginália, the bookstore software. The sast, sanitizers, valgrind
# and dast targets are the checks that the continuous integration pipeline runs.

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Isrc/include
RELEASE = -O2 -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=2 -fstack-protector-strong
SANITIZE = -O0 -g -fno-omit-frame-pointer -fsanitize=address,undefined -fno-sanitize-recover=all

SOURCES = src/author.cpp src/book.cpp src/customer.cpp src/invoice.cpp src/payment.cpp
HEADERS = $(wildcard src/include/*.h)

.PHONY: all sast sanitizers valgrind dast new-key clean

# Builds the software and the unit tests with the regular flags
all: build/marginalia build/tests

build/marginalia: $(SOURCES) src/main.cpp $(HEADERS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@ $(SOURCES) src/main.cpp

build/tests: $(SOURCES) tests/unit/tests.cpp $(HEADERS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(RELEASE) -g -o $@ $(SOURCES) tests/unit/tests.cpp

build/tests-sanitizers: $(SOURCES) tests/unit/tests.cpp $(HEADERS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(SANITIZE) -o $@ $(SOURCES) tests/unit/tests.cpp

# SAST: automated software review with Semgrep and the C++ rules in .semgrep
sast:
	semgrep scan --config .semgrep --error --metrics=off --disable-version-check src

# Software tests: each unit test runs in its own process, built with ASan and UBSan
sanitizers: build/tests-sanitizers
	@failed=0; for test in $$(build/tests-sanitizers --list); do \
		echo "== $$test"; build/tests-sanitizers $$test || failed=$$((failed + 1)); \
	done; echo "failed tests: $$failed"; test $$failed -eq 0

# Software tests: each unit test runs in its own process, under Valgrind
valgrind: build/tests
	@failed=0; for test in $$(build/tests --list); do \
		echo "== $$test"; valgrind --quiet --error-exitcode=1 build/tests $$test || failed=$$((failed + 1)); \
	done; echo "failed tests: $$failed"; test $$failed -eq 0

build/hostile_cases: tests/dast/hostile_cases.cpp
	@mkdir -p build
	$(CXX) -std=c++17 -Wall -Wextra -O2 -o $@ tests/dast/hostile_cases.cpp

build/new_key: tools/new_key.cpp
	@mkdir -p build
	$(CXX) -std=c++17 -Wall -Wextra -O2 -o $@ tools/new_key.cpp

# DAST: software tests with hostile inputs against the regularly built binary, without looking at the source code
dast: build/marginalia build/hostile_cases
	build/hostile_cases build/marginalia

# Prints a new fake payment gateway key, to simulate a key rotation
new-key: build/new_key
	@build/new_key

clean:
	rm -rf build
