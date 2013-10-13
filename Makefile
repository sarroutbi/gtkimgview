CC=gcc
CCFLAGS=-g -Wall
INC_FLAGS=$(shell pkg-config --cflags gtk+-3.0)
LIB_FLAGS=$(shell pkg-config --libs gtk+-3.0)
TARGET=gtkimgview
SRC_DIR=src
TARGET_FILES=$(wildcard $(SRC_DIR)/*.c)

all: $(TARGET)

$(TARGET): $(TARGET_FILES)
								$(CC) $(CCFLAGS) $(INC_FLAGS) $(TARGET_FILES) $(LIB_FLAGS) -o $@

clean:
								@rm -rfv $(TARGET) *~ $(SRC_DIR)/*~