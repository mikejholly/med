build: clean
	mkdir build
	gcc src/main.c -o ./build/e -ltermbox

clean:
	rm -rf build
