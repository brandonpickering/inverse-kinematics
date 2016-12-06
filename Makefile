SOURCEDIR=source
SOURCE=$(wildcard $(SOURCEDIR)/*.cpp)
HEADERS=$(wildcard $(SOURCEDIR)/*.hpp) $(wildcard $(SOURCEDIR)/*.inl)

BUILDDIR=build
OUT=$(BUILDDIR)/as4

GLFWDIR=$(BUILDDIR)/glfw-3.2.1
GLFWSRC=glfw-3.2.1
GLFWLIB=$(GLFWDIR)/src/libglfw3.a

GLADDIR=$(BUILDDIR)/glad-2.1-compat
GLADSRC=glad-2.1-compat
GLADLIB=$(GLADDIR)/libglad.a

CC=g++
CFLAGS=-Wall -Wextra -Og -g -std=c++11 \
       	-I$(GLFWSRC)/include \
       	-I$(GLADSRC)/include
LDFLAGS=-Og -g \
	-L$(dir $(GLFWLIB)) \
	$(shell pkg-config --static --libs $(GLFWDIR)/src/glfw3.pc) \
	-L$(dir $(GLADLIB)) \
	-lglad


all: $(OUT)

clean:
	rm -rf $(BUILDDIR)

$(OUT): $(GLFWLIB) $(GLADLIB) $(SOURCE) $(HEADERS)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(SOURCE) $(LDFLAGS) -o $@

$(GLFWLIB):
	mkdir -p $(GLFWDIR)
	cmake -B$(GLFWDIR) -H$(GLFWSRC)
	make -C $(GLFWDIR)

$(GLADLIB):
	mkdir -p $(GLADDIR)
	gcc -I$(GLADSRC)/include -c $(GLADSRC)/src/glad.c -o $(GLADDIR)/glad.o
	ar rvs $(GLADLIB) $(GLADDIR)/glad.o
