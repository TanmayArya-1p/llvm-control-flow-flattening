#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/Casting.h"
#include <cstdint>
#include <bits/stdc++.h>
#include <optional>

#include "utils.h"



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

    // create a set to track which blocks have already been assigned in switch cases
    std::set<uint64_t> switch_assigned_blocks;

    // if dispatch_var==0 then go to the cut off block of the entry block
    switchInst->addCase(builder.getInt32(0), &entry_block);

    // rewire end of new entry block to the dispatch block
    llvm::BranchInst* new_entry_block_term_br = llvm::dyn_cast<llvm::BranchInst>(new_entry->getTerminator());
    new_entry_block_term_br->setSuccessor(0, dispatcher_block);

    // make a vector of all target blocks that have to be chopped off at its terminator and skip:
    // 1- dispatcher block
    // 2- blocks with children with phi instructions
    // 3- new entry block created above
    std::vector<std::pair<llvm::BasicBlock*, llvm::BranchInst*> > target_blocks;
    uint64_t dispatch_id = 1;
    std::map<llvm::BasicBlock*, uint64_t> block_dispatch_ids;

    for(auto &block : func) {
    	std::optional<const llvm::BranchInst*> terminator_br = utils::terminatorBR(block);

     	// skip complicated cases :(((
        if(&block == new_entry || &block == dispatcher_block || utils::blockHasPhiChildren(block) || !terminator_br.has_value()) {
            continue;
        }
        target_blocks.push_back(std::make_pair(&block, llvm::dyn_cast<llvm::BranchInst>(block.getTerminator())));
    }

    for(auto& [block_ptr, term_br] : target_blocks) {
	   	// if block not assigned an id then assign it
	   	if(block_dispatch_ids.find(block_ptr) == block_dispatch_ids.end()) {
	  		block_dispatch_ids[block_ptr] = dispatch_id++;
	   	}


	   	// assign ids to successor blocks
		for(auto const& succ: llvm::successors(block_ptr)) {
			if(block_dispatch_ids.find(succ) == block_dispatch_ids.end()) {
				block_dispatch_ids[succ] = dispatch_id++;
			}
		}

		if(term_br->isConditional()) {
			// get the true and false blocks to branch to
			llvm::BasicBlock* true_block = term_br->getSuccessor(0);
			llvm::BasicBlock* false_block = term_br->getSuccessor(1);

			//get the value of the condition evaluated in the branch instruction
			llvm::Value* cond = term_br->getCondition();

			// strip away last branch instruction
			term_br->eraseFromParent();

			// Create IRBuilder that appends to the end of the block
			llvm::IRBuilder<> block_builder(block_ptr);

			// use a select instruction to set dispatcher var
			llvm::Value* dispatcher_var = block_builder.CreateSelect(cond, block_builder.getInt32(block_dispatch_ids[true_block]), block_builder.getInt32(block_dispatch_ids[false_block]));
			block_builder.CreateStore(dispatcher_var, dispatch_var_alloca);

			// add unconditional branch to jump to dispatcher block
			block_builder.CreateBr(dispatcher_block);

			// add true block to switch instruction
			if(switch_assigned_blocks.find(block_dispatch_ids[true_block]) == switch_assigned_blocks.end()) {
				switchInst->addCase(builder.getInt32(block_dispatch_ids[true_block]), true_block);
				switch_assigned_blocks.insert(block_dispatch_ids[true_block]);
			}

			// add false block to switch instruction
			if(switch_assigned_blocks.find(block_dispatch_ids[false_block]) == switch_assigned_blocks.end()) {
				switchInst->addCase(builder.getInt32(block_dispatch_ids[false_block]), false_block);
				switch_assigned_blocks.insert(block_dispatch_ids[false_block]);
			}


		} else if(term_br->isUnconditional()) {
			llvm::BasicBlock* successor = term_br->getSuccessor(0);

			// add successor block to switch instruction
			if(switch_assigned_blocks.find(block_dispatch_ids[successor]) == switch_assigned_blocks.end()) {
				switchInst->addCase(builder.getInt32(block_dispatch_ids[successor]), successor);
				switch_assigned_blocks.insert(block_dispatch_ids[successor]);
			}

			// delete trailing jump
			term_br->eraseFromParent();

			// create IR builder at end of current block
			llvm::IRBuilder<> block_builder(block_ptr);
			block_builder.CreateStore(block_builder.getInt32(block_dispatch_ids[successor]), dispatch_var_alloca);
			block_builder.CreateBr(dispatcher_block);
		} else {
			assert(false && "wtf????");
		}
    }
}
