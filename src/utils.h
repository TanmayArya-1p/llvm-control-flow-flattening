#pragma once
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"
#include <bits/stdc++.h>

namespace utils {

bool blockHasPhiChildren(const llvm::BasicBlock& block);
void setAllSuccessors(llvm::BranchInst* branch, llvm::BasicBlock* target_block);
std::optional<const llvm::BranchInst*> terminatorBR(const llvm::BasicBlock& block);

} // namespace utils
