SYSCONF_LINK = g++
CPPFLAGS	 = -Wall -std=c++14
LDFLAGS		 = -O3
LIBS		 = -lm -lglfw -lglew -framework OpenGl
INC			 = -I./include -I./

# Additional folders for file look up
VPATH 	= src:include:common
DESTDIR = ./
TARGET  = TinyGLSL

# Replace all found cpp files to .o for prerequisites
OBJECTS = $(patsubst %.cpp,%.o,$(wildcard src/*.cpp common/*.cpp))

all: $(DESTDIR)$(TARGET)

debug: CPPFLAGS += -g
debug: $(DESTDIR)$(TARGET)

# Rule to create the executable
$(DESTDIR)$(TARGET): $(OBJECTS)
	$(SYSCONF_LINK) -Wall $(LDFLAGS) -o $(DESTDIR)$(TARGET) $(OBJECTS) $(LIBS)

# Rule to create .o files, needs the include path
$(OBJECTS): %.o: %.cpp
	$(SYSCONF_LINK) -Wall $(CPPFLAGS) $(INC) -c $(CFLAGS) $< -o $@

clean:
	-rm -f $(OBJECTS)
	-rm -f $(TARGET)
	-rm -f *.tga