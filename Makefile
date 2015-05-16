CVFLAGS = `pkg-config --cflags opencv`
CVLIBS = `pkg-config --libs opencv`

all:
	g++ $(CVFLAGS) *.cpp $(CVLIBS) -o cars.out
clean:
	rm -f cars.out
