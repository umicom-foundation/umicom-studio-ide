.PHONY: build run test clean format
build:
	cmake --preset default || (cmake -S . -B build -G Ninja)
	cmake --build --preset default -j || cmake --build build -j
run: build
	./build/ustudio || true
test: build
	ctest --test-dir build || true
format:
	bash scripts/format.sh || true
clean:
	rm -rf build
