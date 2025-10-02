test: test-ir test-llvm

test-ir: test.c
	clang -S -emit-llvm test.c -o test.ll

test-llvm: build test.ll
	@opt -load-pass-plugin ./build/src/flattenPass.so -passes=flatten-pass test.ll -o a.out
	opt -stats -time-passes -load-pass-plugin ./build/src/flattenPass.so -passes=flatten-pass,dot-cfg -S test.ll -o test_flat.ll
	@mv .*.dot dots


build: src
	cd build && cmake ..
	cd build && make

clean:
	rm -rf build/*


.PHONY: build clean test test-ir test-llvm
