CC = g++
INC = -Iinclude
OBJDIR = obj
SRCDIR = src
CC_FLAGS = -w -pedantic $(INC)

EXEC = DistanceVectorSimulator
SOURCES = $(shell find $(SRCDIR) -name '*.cpp')
OBJECTS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))

$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXEC)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | make-folder
	$(CC) $(CC_FLAGS) -c $< -o $@

clean:
	rm -rf $(EXEC) $(OBJDIR)

make-folder:
	mkdir -p $(OBJDIR)