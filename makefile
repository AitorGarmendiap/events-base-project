# -------------------------------
# CS225 event system assignment makefile
# -------------------------------

FLAGS=-Wall -Wextra -Wpedantic -g -std=c++03

# comment/uncomment the following line to toggle verbosity 
#FLAGS+=-DVERBOSE
# comment/uncomment the following line to toggle output coloring 
#FLAGS+=-DUSE_COLORED_OUTPUT

HEADERS=type_info.hh event.hh event_dispatcher.hh testing.hh
SOURCES=type_info.cc event.cc event_dispatcher.cc
DRIVER=driver.cc

EXE=event-tests.exe
ERASE=rm

all : $(HEADERS) $(SOURCES) $(DRIVER)
	g++ $(FLAGS) $(DRIVER) $(SOURCES) -o $(EXE)

clean :
	$(ERASE) $(EXE) 
