#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"

void flatten(llvm::Function& func) {
	// no point of flattening a function that is a declaration or has less than 2 basic blocks
    if (func.isDeclaration() || func.size() <= 2) return;

    // get the entry block of the function
    llvm::BasicBlock& entry_block = func.getEntryBlock();
    llvm::LLVMContext &ctx = func.getContext();

    // allocate space on the stack for dispatch_var control variable at the start of function
    llvm::IRBuilder<> entry_builder(&entry_block, entry_block.begin());
    llvm::AllocaInst* dispatch_var_alloca = entry_builder.CreateAlloca(llvm::Type::getInt32Ty(ctx), nullptr, "dispatch_var");
    // zero out the dispatch_var on the stack
    entry_builder.CreateStore(entry_builder.getInt32(0), dispatch_var_alloca);

    // split the entry block into two blocks; new_entry is the actual new entry block now
    llvm::BasicBlock* new_entry = entry_block.splitBasicBlockBefore(entry_block.getTerminator(), "");

    // create dispatcher block that will have the switch case for jumping
    llvm::BasicBlock* dispatcher_block = llvm::BasicBlock::Create(ctx, "dispatcher", &func);
    llvm::IRBuilder<> builder(dispatcher_block);

    // fetch the value of dispatch_var from the stack
    llvm::Value* dispatch_var = builder.CreateLoad(llvm::Type::getInt32Ty(ctx), dispatch_var_alloca, "dispatch_var");

    // make a switch instruction in dispatch block with default as cut off entry block
    llvm::SwitchInst* switchInst = builder.CreateSwitch(dispatch_var, &entry_block);

    // if dispatch_var==0 then go to the cut off block of the entry block
    switchInst->addCase(builder.getInt32(0), &entry_block);

    // rewire end of new entry block to the dispatch block
    llvm::BranchInst* new_entry_block_term_br = llvm::dyn_cast<llvm::BranchInst>(new_entry->getTerminator());
    new_entry_block_term_br->setSuccessor(0, dispatcher_block);
}
