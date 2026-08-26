CXX ?= c++
CPPFLAGS ?=
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic
LDFLAGS ?=
LDLIBS ?=

PREFIX ?= /usr/local
BINDIR ?= $(PREFIX)/bin
DOCDIR ?= $(PREFIX)/share/doc/shriek
BUILD_DIR ?= build

SHRIEK := $(BUILD_DIR)/shriek
SHRIEK_OBJECTS := $(BUILD_DIR)/shriek.o

.PHONY: all test install clean

all: $(SHRIEK)

$(SHRIEK): $(SHRIEK_OBJECTS)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(BUILD_DIR)/shriek.o: src/shriek.cpp src/shriek.hpp | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $@

install: $(SHRIEK)
	install -d $(DESTDIR)$(BINDIR) $(DESTDIR)$(DOCDIR)
	install -m 755 $(SHRIEK) $(DESTDIR)$(BINDIR)/shriek
	install -m 644 README LICENSE docs/design-specification.md $(DESTDIR)$(DOCDIR)

clean:
	rm -rf $(BUILD_DIR)
