// Llvm analysis pass example.
//- Says hello to all functions from clang!

#include <llvm/IR/Function.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Passes/PassBuilder.h>

using namespace llvm;

namespace {
    struct HelloClangPass : public PassInfoMixin<HelloClangPass> {
        PreservedAnalyses run(
            Function& function,
            FunctionAnalysisManager& passManager
        ) {
            errs() << "Hello from clang, " << function.getName() << "!\n";
            return PreservedAnalyses::all();
        }
    };
}

extern "C" PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK llvmGetPassPluginInfo() {
    return {
        LLVM_PLUGIN_API_VERSION, "HelloPass", "v1.0",
        [](PassBuilder &passBuilder) {
            // Register pass automatically from clang
            // clang -fexperimental-new-pass-manager -fpass-plugin=<plugin_pass>
            passBuilder.registerOptimizerLastEPCallback([](
                FunctionPassManager& passManager,
                PassBuilder::OptimizationLevel
            ) {
                passManager.addPass(HelloClangPass());
            });
        }
    };
}