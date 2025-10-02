#pragma once
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"
#include <bits/stdc++.h>
#include <cstddef>
#include <cstdint>

namespace utils {

bool blockHasPhiChildren(const llvm::BasicBlock& block);
void setAllSuccessors(llvm::BranchInst* branch, llvm::BasicBlock* target_block);
std::optional<const llvm::BranchInst*> terminatorBR(const llvm::BasicBlock& block);

class SwitchManager {
	public:
	    SwitchManager(llvm::SwitchInst& switch_inst, llvm::IRBuilder<>& builder);
		~SwitchManager();
	    uint64_t insert_case_block(llvm::BasicBlock* case_block);
	    bool exists(uint64_t index);
	    uint64_t assign_index(llvm::BasicBlock* block);
		std::optional<uint64_t> get_index(llvm::BasicBlock* block);
	private:
	    llvm::SwitchInst& switch_inst;
	    llvm::IRBuilder<>& builder;
	    std::map<llvm::BasicBlock*, uint64_t> block_dispatch_ids;
		std::set<uint64_t> switch_assigned_blocks;
};

} // namespace utils
