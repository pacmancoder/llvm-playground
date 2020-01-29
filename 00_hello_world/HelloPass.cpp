// Llvm analysis pass example.
//- Says hello to all functions!

#include <llvm/IR/Function.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Passes/PassBuilder.h>

using namespace llvm;

namespace {
    struct HelloPass : public PassInfoMixin<HelloPass> {
        PreservedAnalyses run(
            Function& function,
            FunctionAnalysisManager& passManager
        ) {
            errs() << "Hello, " << function.getName() << "!\n";
            return PreservedAnalyses::all();
        }
    };
}

extern "C" PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK llvmGetPassPluginInfo() {
    return {
        LLVM_PLUGIN_API_VERSION, "HelloPass", "v1.0",
        [](PassBuilder &passBuilder) {
            // Register pass when launching manually from opt tool:
            // opt -load-pass-plugin <plugin_path> -passes=HelloPass ...
            passBuilder.registerPipelineParsingCallback([](
                StringRef name,
                FunctionPassManager& passManager,
                ArrayRef<PassBuilder::PipelineElement>
            ) {
                if (name.equals("HelloPass")) {
                    passManager.addPass(HelloPass());
                }

                return true;
            });
        }
    };
}