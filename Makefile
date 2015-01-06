build: clean
	mkdir build
	gcc src/med.c -o ./build/med

clean:
	rm -rf build

run:
	./build/med
