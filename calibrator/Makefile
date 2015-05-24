CVFLAGS = `pkg-config --cflags opencv`
CVLIBS = `pkg-config --libs opencv`

all:
	g++ $(CVFLAGS) *.cpp $(CVLIBS) -o calibrator.out
clean:
	rm -f calibrator.out
