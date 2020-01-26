#pragma once

#include <llvm/IR/PassManager.h>

namespace LlvmPlayground {
    struct BinaryEmbedder : public llvm::PassInfoMixin<BinaryEmbedder> {
        llvm::PreservedAnalyses run(llvm::Module& module, llvm::ModuleAnalysisManager& passManager);
    };
}