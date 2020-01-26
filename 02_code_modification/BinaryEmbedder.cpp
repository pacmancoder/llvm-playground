// Code modification pass example
// - Implements pass which embeds the binary content into variables, annotated with
//   "embed_resource=<path>" annotation
// - Uses new pass manager!

#include "BinaryEmbedder.h"

#include <llvm/IR/Module.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Constants.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Path.h>

#include <string>
#include <vector>
#include <fstream>
#include <iterator>

using namespace llvm;
using namespace LlvmPlayground;

namespace {
    enum class BinaryEmbedderError {
        Ok,
        FileDoesNotExist,
        FileReadError,
    };


    const std::string ANNOTATION_PREFIX = "embed_resource=";
}

static bool IsEmbedResourceAnnotation(StringRef annotation) {
    return annotation.startswith(ANNOTATION_PREFIX);
}

static std::string GetEmbeddedResourcePath(StringRef annotation) {
    return annotation.str().substr(ANNOTATION_PREFIX.size());
}

static BinaryEmbedderError LoadVectorFromFile(const std::string& path, std::vector<char>& data) {
    std::ifstream binaryFile(path);

    if (!binaryFile) {
        return BinaryEmbedderError::FileReadError;
    }

    data = std::vector<char>(
        std::istreambuf_iterator<char>(binaryFile),
        std::istreambuf_iterator<char>());

    return BinaryEmbedderError::Ok;
}

static BinaryEmbedderError LoadEmbeddedDataArrayForAnnotation(
    StringRef basePath,
    StringRef annotation,
    std::vector<char>& data)
{
    const size_t MAX_PATH = 4096;

    SmallVector<char, MAX_PATH> pathBuffer;

    auto embeddedResourcePath = GetEmbeddedResourcePath(annotation);

    if (embeddedResourcePath[0] == '/') {
        // Handle absolute path
        pathBuffer.insert(pathBuffer.end(), embeddedResourcePath.begin(), embeddedResourcePath.end());

    } else {
        // Handle relative path
        if (basePath[0] == '/') {
            // base path is absolute; no need for expansion
            pathBuffer.insert(pathBuffer.end(), basePath.begin(), basePath.end());
        } else {
            // base path is relative to current dir, expand it
            sys::fs::current_path(pathBuffer);
            sys::path::append(pathBuffer, basePath);
        }

        sys::path::remove_filename(pathBuffer);
        sys::path::append(pathBuffer, embeddedResourcePath);
    }

    pathBuffer.push_back(0);

    sys::fs::make_absolute(pathBuffer);

    if (!sys::fs::exists(pathBuffer.data())) {
        return BinaryEmbedderError::FileDoesNotExist;
    }

    return LoadVectorFromFile(pathBuffer.data(), data);
}

static GlobalVariable* CreateEmbeddedDataConstant(Module& module, const std::vector<char>& data) {
    auto* varType = ArrayType::get(IntegerType::getInt8Ty(module.getContext()), data.size() + 1);
    auto* varInitializer = ConstantDataArray::getString(module.getContext(), data.data());
    return new GlobalVariable(
        module,
        varType,
        true,
        GlobalVariable::LinkageTypes::PrivateLinkage,
        varInitializer
    );
}

static BinaryEmbedderError SubstitutePointerVariableTarget(
    Module& module,
    GlobalVariable* original,
    GlobalVariable* newVariable,
    bool& changed
) {
    Value* zero = Constant::getNullValue(
        Type::getInt32Ty(module.getContext()));

    SmallVector<Value*, 2> idxList;
    idxList.push_back(zero);
    idxList.push_back(zero);

    auto getElementPtrInstruction = ConstantExpr::getInBoundsGetElementPtr(
        nullptr,
        newVariable,
        idxList);

    auto newConstPtr = new GlobalVariable(
        module,
        PointerType::getInt8PtrTy(module.getContext()),
        true,
        GlobalVariable::InternalLinkage,
        getElementPtrInstruction);

    // For each instruction which uses original variable, replace variable with
    // the new pointer to embedded data
    for (auto user : original->users()) {
        if (auto inst = dyn_cast<Instruction>(user)) {
            inst->replaceUsesOfWith(original, newConstPtr);
            changed = true;
        }
    }

    return BinaryEmbedderError::Ok;
}

// === BinaryEmbedder impleementation ===

PreservedAnalyses BinaryEmbedder::run(Module& module, ModuleAnalysisManager& passManager) {
    bool isCodeModified = false;

    GlobalVariable* annotations = module.getNamedGlobal("llvm.global.annotations");

    if (!annotations) {
        return PreservedAnalyses::all();
    }
    // First operand of annotation is array of annotations metadata
    ConstantArray* annotationsArray = dyn_cast<ConstantArray>(annotations->getOperand(0));

    for (auto& metadata : annotationsArray->operands()) {
        // Metadata consists of 2 elements:
        // - [0] - Variable/Function annotated by
        // - [1] - Annotation global variable itself
        auto* metadataStruct = dyn_cast<ConstantStruct>(metadata.get());

        auto* annotationGlobalVariable = dyn_cast<GlobalVariable>(
            metadataStruct->getOperand(1)->getOperand(0));

        auto annotationValue = dyn_cast<ConstantDataArray>(
            annotationGlobalVariable->getInitializer()
        )->getAsCString();

        if (IsEmbedResourceAnnotation(annotationValue)) {
            std::vector<char> embeddedData;
            auto error = LoadEmbeddedDataArrayForAnnotation(
                module.getSourceFileName(),
                annotationValue,
                embeddedData);
            if (error != BinaryEmbedderError::Ok) {
                module.getContext().emitError("Can't read embedded data");
                return PreservedAnalyses::all();
            }


            auto* newConst = CreateEmbeddedDataConstant(module, embeddedData);

            // Annotated variable is pointer to Int8 array constant
            auto* annotatedVariable = dyn_cast<GlobalVariable>(
                metadataStruct->getOperand(0)->getOperand(0));


            error = SubstitutePointerVariableTarget(
                module,
                annotatedVariable,
                newConst,
                isCodeModified);
            if (error != BinaryEmbedderError::Ok) {
                module.getContext().emitError("Variable substitution was failed");
                return PreservedAnalyses::all();
            }
        }
    }

    return isCodeModified ? PreservedAnalyses::none() : PreservedAnalyses::all();
}
