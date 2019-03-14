CC     = gcc
CLFAGS = -Wall -g -O0
BUILD_DIR = .\src\obj

src = $(wildcard src/*.c)
obj = $(patsubst src/%.c,src/obj/%.o,$(src))

.PHONY: all exe checkdirs clean

all: checkdirs exe 

exe: $(obj)
	$(CC) -o .\bin\project.exe $^ $(CFLAGS) 

src/obj/%.o: src/%.c
	$(CC) -c $< -o $@ $(CLFAGS)

checkdirs: $(BUILD_DIR)

$(BUILD_DIR):
	@mkdir $@
	@mkdir  .\bin\ 

clean:
	del /q $(BUILD_DIR)
	del .\bin\project.exe
