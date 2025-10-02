#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"
#include <bits/stdc++.h>

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
