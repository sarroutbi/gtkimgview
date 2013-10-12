CC=gcc
INC_FLAGS=$(shell pkg-config --cflags gtk+-3.0)
LIB_FLAGS=$(shell pkg-config --libs gtk+-3.0)
TARGET=gtkimgview
TARGET_FILES=gtkimgview.c filelist.c

all: $(TARGET)

$(TARGET): $(TARGET_FILES)
								$(CC) -g -Wall $(INC_FLAGS) $(TARGET_FILES) $(LIB_FLAGS) -o $@

clean:
								rm $(TARGET) *~