#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/PassRegistry.h"

using namespace llvm;

namespace {
    struct ClangPlugin : public FunctionPass {
        static char ID;

        ClangPlugin() : FunctionPass(ID) {}

        bool runOnFunction(Function &F) override {
            errs() << "Hello, " << F.getName() << "!\n";
            return false;
        }
    };
}

char ClangPlugin::ID = 0;

static void RegisterClangPlugin(const PassManagerBuilder&, legacy::PassManagerBase & PassManager) {
    PassManager.add(new ClangPlugin());
}

static RegisterStandardPasses PassRegistrationPoint(
    PassManagerBuilder::EP_EarlyAsPossible,
    RegisterClangPlugin
);
