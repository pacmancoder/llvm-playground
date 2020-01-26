#pragma once

#include <llvm/IR/PassManager.h>

namespace LlvmPlayground {
    struct Profiler : public llvm::PassInfoMixin<Profiler> {
        llvm::PreservedAnalyses run(
            llvm::Module& module,
            llvm::ModuleAnalysisManager& passManager);
    };
}