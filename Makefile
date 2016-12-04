SOURCEDIR=source
SOURCE=$(wildcard $(SOURCEDIR)/*.c)
HEADERS=$(wildcard $(SOURCEDIR)/*.h)

BUILDDIR=build
OUT=$(BUILDDIR)/as4

CC=gcc
CFLAGS=-Wall -Wextra -Og -g -I/usr/local/include/glad-3.3-compat
LDFLAGS=-Og -g -lglfw -lglad-3.3-compat -ldl


all: $(OUT)

clean:
	rm -rf $(BUILDDIR)

$(OUT): $(SOURCE) $(HEADERS)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(SOURCE) $(LDFLAGS) -o $@
