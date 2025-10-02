#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"
#include <bits/stdc++.h>
#include <cstdint>
#include <optional>

#include "utils.h"

bool utils::blockHasPhiChildren(const llvm::BasicBlock& block) {
	// check if any successor has a phi node
	for(auto const &succ : llvm::successors(&block)) {
		if(!succ->phis().empty()) return true;
	}
	return false;
}

void utils::setAllSuccessors(llvm::BranchInst* branch, llvm::BasicBlock* target_block) {
	// set the successors of the branch instruction
	for(short i=0;i<branch->getNumSuccessors();i++) {
	   	branch->setSuccessor(i, target_block);
	}
}

std::optional<const llvm::BranchInst*> utils::terminatorBR(const llvm::BasicBlock& block) {
	if(auto ret = llvm::dyn_cast<llvm::BranchInst>(block.getTerminator())) return {ret};
	return std::nullopt;
}

utils::SwitchManager::SwitchManager(llvm::SwitchInst& si, llvm::IRBuilder<>& b): switch_inst(si), builder(b) {
	this->block_dispatch_ids = {};
	this->switch_assigned_blocks = {};
}

utils::SwitchManager::~SwitchManager() {
	this->block_dispatch_ids.clear();
	this->switch_assigned_blocks.clear();
}

uint64_t utils::SwitchManager::insert_case_block(llvm::BasicBlock* case_block) {
	uint64_t index;
	if(block_dispatch_ids.find(case_block) != block_dispatch_ids.end()) {
		index = this->block_dispatch_ids[case_block];
		if(this->switch_assigned_blocks.find(index) != this->switch_assigned_blocks.end()) {
			return index;
		}
	} else {
		index = this->assign_index(case_block);
	}

	this->switch_inst.addCase(this->builder.getInt32(index), case_block);
	this->switch_assigned_blocks.insert(index);

	return index;
}

uint64_t utils::SwitchManager::assign_index(llvm::BasicBlock* block) {
	uint64_t index = this->block_dispatch_ids.size()+1;
	this->block_dispatch_ids[block] = index;
	return index;
}

std::optional<uint64_t> utils::SwitchManager::get_index(llvm::BasicBlock* block) {
	if(block_dispatch_ids.find(block) != block_dispatch_ids.end()) {
		return block_dispatch_ids[block];
	}
	return std::nullopt;
}

bool utils::SwitchManager::exists(uint64_t index) {
	return this->switch_assigned_blocks.find(index) != this->switch_assigned_blocks.end();
}
