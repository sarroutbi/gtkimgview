CC=gcc
CCFLAGS=-g -Wall
INC_FLAGS=$(shell pkg-config --cflags gtk+-3.0)
LIB_FLAGS=$(shell pkg-config --libs gtk+-3.0)
TARGET=gtkimgview
TARGET_FILES=filelist.c treeview.c gtkimgview.c imgview.c

all: $(TARGET)

$(TARGET): $(TARGET_FILES)
								$(CC) $(CCFLAGS) $(INC_FLAGS) $(TARGET_FILES) $(LIB_FLAGS) -o $@

clean:
								rm $(TARGET) *~