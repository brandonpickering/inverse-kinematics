SOURCEDIR=source
SOURCE=$(wildcard $(SOURCEDIR)/*.cpp)
HEADERS=$(wildcard $(SOURCEDIR)/*.hpp) $(wildcard $(SOURCEDIR)/*.inl)

BUILDDIR=build
OUT=$(BUILDDIR)/as4

CC=g++
CFLAGS=-Wall -Wextra -Og -g -std=c++11 -I/usr/local/include/glad-3.3-compat
LDFLAGS=-Og -g -lglfw -lglad-3.3-compat -ldl


all: $(OUT)

clean:
	rm -rf $(BUILDDIR)

$(OUT): $(SOURCE) $(HEADERS)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(SOURCE) $(LDFLAGS) -o $@
