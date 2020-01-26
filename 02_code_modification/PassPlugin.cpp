#include <llvm/Passes/PassPlugin.h>
#include <llvm/Passes/PassBuilder.h>

#include "BinaryEmbedder.h"

using namespace llvm;
using namespace LlvmPlayground;

extern "C" PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK llvmGetPassPluginInfo() {
    return {
        LLVM_PLUGIN_API_VERSION, "BinaryEmbedderPlugin", "v1.0",
        [](PassBuilder &passBuilder) {
            // Register pass when launching manually from opt tool:
            // opt -load-pass-plugin <plugin_path> -passes=binary-embedder ...
            passBuilder.registerPipelineParsingCallback([](
                StringRef name,
                ModulePassManager& passManager,
                ArrayRef<PassBuilder::PipelineElement>
            ) {
                if (name.equals("binary-embedder")) {
                    passManager.addPass(BinaryEmbedder());
                }

                return true;
            });
            // Register pass automatically from clang (except -O0 optimization level :( )
            // clang -fexperimental-new-pass-manager -fpass-plugin=<plugin_pass>
            passBuilder.registerPipelineStartEPCallback([](ModulePassManager& passManager) {
                passManager.addPass(BinaryEmbedder());
            });
        }
    };
}