#include "Profiler.hpp"

#include <llvm/IR/Function.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Verifier.h>

using namespace llvm;
using namespace LlvmPlayground;

static FunctionCallee GetProfilerFunction(Module& module, const char* name) {
    SmallVector<Type*, 2> args;
    // Source file
    args.push_back(PointerType::getInt8PtrTy(module.getContext()));
    // Function name
    args.push_back(PointerType::getInt8PtrTy(module.getContext()));

    auto functionCallee = module.getOrInsertFunction(
        name,
        FunctionType::get(Type::getVoidTy(module.getContext()), args, false)
    );

    dyn_cast<Function>(functionCallee.getCallee())->setLinkage(Function::ExternalLinkage);

    return functionCallee;
}

GlobalVariable* CreateFunctionNameVariable(Module& module, StringRef functionName) {
    auto* functionNameInitializer = ConstantDataArray::getString(
        module.getContext(),
        functionName);

    return new GlobalVariable(
        module,
        ArrayType::get(Type::getInt8Ty(module.getContext()), functionName.size() + 1),
        true,
        GlobalValue::LinkageTypes::InternalLinkage,
        functionNameInitializer);
}

GlobalVariable* CreateSourceFileNameVariable(Module& module) {
    auto sourceFileName = module.getSourceFileName();

    auto* sourceFileNameInitializer = ConstantDataArray::getString(
        module.getContext(),
        sourceFileName);

    return new GlobalVariable(
        module,
        ArrayType::get(Type::getInt8Ty(module.getContext()), sourceFileName.size() + 1),
        true,
        GlobalValue::LinkageTypes::InternalLinkage,
        sourceFileNameInitializer);
}

static void CreatePrologueBasicBlock(
    Module& module,
    Function& function,
    GlobalVariable* sourceFileNameVar,
    GlobalVariable* functionNameVar,
    FunctionCallee& profilerEnterFunction
) {
    auto* firstBasicBlock = &function.front();

    auto* enterBasicBlock = BasicBlock::Create(module.getContext());
    IRBuilder<> builder(module.getContext());
    builder.SetInsertPoint(enterBasicBlock);

    auto* sourceFileNamePtr = builder.CreateBitCast(
        sourceFileNameVar,
        PointerType::getInt8PtrTy(module.getContext())
    );
    auto* functionNamePtr = builder.CreateBitCast(
        functionNameVar,
        PointerType::getInt8PtrTy(module.getContext()));

    SmallVector<Value*, 2> enterFunctionArgs;
    enterFunctionArgs.push_back(sourceFileNamePtr);
    enterFunctionArgs.push_back(functionNamePtr);

    builder.CreateCall(profilerEnterFunction, enterFunctionArgs);
    builder.CreateBr(firstBasicBlock);

    enterBasicBlock->insertInto(&function, firstBasicBlock);
}

static std::vector<ReturnInst*> GetReturnInstructions(Function& function) {
    std::vector<ReturnInst*> returnInstructions;

    for (auto& basicBlock : function) {
        for (auto& instruction : basicBlock) {
            if (auto* returnInstruction = dyn_cast<ReturnInst>(&instruction)) {
                returnInstructions.push_back(returnInstruction);
            }
        }
    }

    return returnInstructions;
}

static void CreateEpilogueBasicBlocks(
    Module& module,
    Function& function,
    GlobalVariable* sourceFileNameVar,
    GlobalVariable* functionNameVar,
    FunctionCallee& profilerExitFunction
) {
    IRBuilder<> builder(module.getContext());

    for (auto returnInstruction : GetReturnInstructions(function)) {
        builder.SetInsertPoint(returnInstruction);

        auto* sourceFileNamePtr = builder.CreateBitCast(
            sourceFileNameVar,
            PointerType::getInt8PtrTy(module.getContext())
        );
        auto* functionNamePtr = builder.CreateBitCast(
            functionNameVar,
            PointerType::getInt8PtrTy(module.getContext()));

        SmallVector<Value*, 1> exitFunctionCall;
        exitFunctionCall.push_back(sourceFileNamePtr);
        exitFunctionCall.push_back(functionNamePtr);
        builder.CreateCall(profilerExitFunction, exitFunctionCall);
    }
}


PreservedAnalyses Profiler::run(Module& module, ModuleAnalysisManager& passManager) {
    auto profilerEnterFunction = GetProfilerFunction(module, "profiler_enter_function");
    auto profilerExitFunction = GetProfilerFunction(module, "profiler_exit_function");

    auto* sourceFileNameVar = CreateSourceFileNameVariable(module);

    for (auto& function : module.functions()) {
        auto linkage = function.getLinkage();
        // Skip external functions
        if (linkage == Function::ExternalLinkage || linkage == Function::ExternalWeakLinkage) {
            continue;
        }

        StringRef functionName = function.getName();

        auto* functionNameVar = CreateFunctionNameVariable(module, functionName);

        CreatePrologueBasicBlock(
            module, function, sourceFileNameVar, functionNameVar, profilerEnterFunction);
        CreateEpilogueBasicBlocks(
            module, function, sourceFileNameVar, functionNameVar, profilerExitFunction);

        verifyFunction(function);
    }

    return PreservedAnalyses::none();
}
