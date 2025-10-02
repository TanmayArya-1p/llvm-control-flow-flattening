#include "llvm/Pass.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Analysis/CGSCCPassManager.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/raw_ostream.h"

#include "flatten.h"

using namespace llvm;


namespace {
	struct ControlFlowFlattenPass : PassInfoMixin<ControlFlowFlattenPass> {
		PreservedAnalyses run(Module& m, ModuleAnalysisManager &mam) {
			for (auto& func : m.functions()) {
				flatten(func);
			}
			return PreservedAnalyses::none();
		}

		static bool isRequired()
		{
			return true;
		}
	};
}

PassPluginLibraryInfo getPassPluginInfo() {
	const auto infoCallback = [](PassBuilder& pb){
		pb.registerPipelineParsingCallback([&](StringRef name,ModulePassManager& mpm,ArrayRef<PassBuilder::PipelineElement> pe) -> bool {
			if(name=="flatten-pass") {
				mpm.addPass(ControlFlowFlattenPass());
				return true;
			}
			return false;
		});
	};
	return {LLVM_PLUGIN_API_VERSION, "FunctionListPass", "0.0.1", infoCallback};
}


extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
	return getPassPluginInfo();
}
