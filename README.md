## LLVM Control Flow Flattening Pass

Implements a basic [control flow flattening](https://github.com/obfuscator-llvm/obfuscator/wiki/Control-Flow-Flattening) pass for LLVM IR.

### Usage instructions

To build the shared object file for the pass run the following command in the root directory
```bash
make build
```


For testing, you can write your code in [test.c](test.c) and run `make test` which will generate the IR before and after flattening along with the object file.
Apart from this, the [dots](dots) directory contains generated dot files that can be used with [graphviz](https://graphviz.org) to visualize the CFG of the flattened IR.

If you wish to use this pass separately, you can use the following flags with clang or opt:

```bash
-load-pass-plugin ./build/src/flattenPass.so -passes=flatten-pass
```

**Note**:
This pass currently ignores flattening blocks that:
- Terminate with anything other than a condition or an unconditional branch instruction.
- Have successor blocks that contain [phi](https://llvm.org/docs/LangRef.html#phi-instruction) instructions.




### References:
 - [LLVM Obfuscator](https://github.com/obfuscator-llvm/obfuscator/wiki/Control-Flow-Flattening)
- [Control Flow Flattening using LLVM Pass - 21verses](https://21verses.blog/2025/01/10/post/)
- [Control Flow Flattening: How to build your own - Sam Russel](https://www.lodsb.com/control-flow-flattening-how-to-build-your-own)
- [Learning LLVM - sh4dy](https://sh4dy.com/2024/07/06/learning_llvm_02/)
