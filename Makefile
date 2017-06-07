CC = g++

OBJ_DIR = obj
BIN_DIR = bin

TARGET = send_bmp
OBJS = $(OBJ_DIR)/bmp_header.o $(OBJ_DIR)/fifo_api.o $(OBJ_DIR)/send_bmp.o
LIBS = -lm -pthread
LDFLAGS = 
INCLUDE =
OPTIONS = -Ofast -mcpu=cortex-a9 -mfpu=neon -ftree-vectorize -mvectorize-with-neon-quad -mfloat-abi=hard -ffast-math -Wno-unused-result

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $(BIN_DIR)/$@ $^ $(INCLUDE) $(LIBS) $(LDFLAGS)

$(OBJ_DIR)/%.o: %.cpp
	$(CC) $(OPTIONS) -o $@ -c $< $(INCLUDE) $(LIBS) $(LDFLAGS)

clean:
	$(RM) $(BIN_DIR)/$(TARGET) $(OBJ_DIR)/*.o
