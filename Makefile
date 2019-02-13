#Compiler
CC = g++
#Compiler flags
CFLAGS = -Wall -std=c++11 -m64
#Libraries
LIBS = -lSDL2
#Path to sources
SRC_PATH = src
#Path to object files
OBJ_PATH = obj
#Path to linker output
OUT_PATH = bin
#Name of the executable
EXE_NAME = VkRaytracer
#Path to the executable
EXE = $(OUT_PATH)/$(EXE_NAME)

#Source files excluding headers
SOURCES = \
	$(SRC_PATH)/main.cpp \
	$(SRC_PATH)/VkManager.cpp \
	$(SRC_PATH)/Window.cpp \
	$(SRC_PATH)/Renderer.cpp 

#List of object files
OBJECTS = $(SOURCES:$(SRC_PATH)/%.cpp=$(OBJ_PATH)/%.o)
#List of dependencies (helps with not having to compile unchanged files)
DEPENDENCIES = $(OBJECTS:.o=.d)

#Targets for when the host OS is based on Windows NT (Windows 10 etc...)
ifeq ($(OS), Windows_NT)

#Add path to libraries because windows
INCLUDES = -IC:/VulkanSDK/1.1.82.1/Include/ -LC:/VulkanSDK/1.1.82.1/Lib

#Default targer - compile link objects
.PHONY: all
#Add window's vulkan dll
all: LIBS += -lvulkan-1
all: $(EXE)

#Link object files into the executable
$(EXE) : $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBS) $(INCLUDES) -o $@

#This only includes changed files
-include $(DEPENDENCIES)

#Compile all sources into object files
$(OBJ_PATH)/%.o : $(SRC_PATH)/%.cpp
	#Compile source file
	$(CC) $(CFLAGS) $(INCLUDES) -MP -MMD -c $< -o $@

#Other OSs (assuming it's using bash)
else

#Default targer - compile link objects
.PHONY: all
#Add unix vulkan library
all: LIBS += -lvulkan
all: $(EXE)

#Link object files into the executable
$(EXE) : $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBS) $(INCLUDES) -o $@ 

#This only includes changed files
-include $(DEPENDENCIES)

#Compile all sources into object files
$(OBJ_PATH)/%.o : $(SRC_PATH)/%.cpp
	#Compile sources using the compiler enabling phony targets
	$(CC) $(CFLAGS)  $(INCLUDES) -MP -MMD -c $< -o $@

endif

#Build for debugging
.PHONY : debug
#Clean old object files
debug: clean
#Add compiler flag to enable debugging
debug: CFLAGS += -g
#Compile as ususal
debug: all

#Cleanup
.PHONY : clean
clean:
	#Recursively remove object file
	@$(RM) -r $(OBJ_PATH)/*
	#Recursively clean binary output folder
	@$(RM) -r $(OUT_PATH)/*
