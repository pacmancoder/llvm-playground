#include "llvm/IR/Function.h"
#include "llvm/Pass.h"

using namespace llvm;

namespace {
    struct HelloPass : public FunctionPass {
        static char ID;

        HelloPass() : FunctionPass(ID) {}

        bool runOnFunction(Function &F) override {
            errs() << "Hello,  " << F.getName() << "!\n";
            return false;
        }
    };
}

char HelloPass::ID = 0;

static RegisterPass<HelloPass> HelloPassInstance("HelloPass", "Hello World Pass");
