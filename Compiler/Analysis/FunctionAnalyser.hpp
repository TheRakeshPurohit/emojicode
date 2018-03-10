//
//  FunctionAnalyser.hpp
//  EmojicodeCompiler
//
//  Created by Theo Weidmann on 28/07/2017.
//  Copyright © 2017 Theo Weidmann. All rights reserved.
//

#ifndef FunctionAnalyser_hpp
#define FunctionAnalyser_hpp

#include "AST/ASTExpr.hpp"
#include "AST/ASTTypeExpr.hpp"
#include "Package/Package.hpp"
#include "PathAnalyser.hpp"
#include "Scoping/SemanticScoper.hpp"
#include "Types/Type.hpp"
#include "Types/TypeContext.hpp"
#include <memory>
#include <utility>

namespace EmojicodeCompiler {

class ASTArguments;
class TypeExpectation;
class SemanticAnalyser;

/// This class is responsible for managing the semantic analysis of a function.
class FunctionAnalyser {
public:
    FunctionAnalyser(Function *function, SemanticAnalyser *analyser) :
            scoper_(std::make_unique<SemanticScoper>(SemanticScoper::scoperForFunction(function))),
            typeContext_(function->typeContext()), function_(function), analyser_(analyser) {}

    FunctionAnalyser(Function *function, std::unique_ptr<SemanticScoper> scoper, SemanticAnalyser *analyser) :
            scoper_(std::move(scoper)), typeContext_(function->typeContext()), function_(function),
            analyser_(analyser) {}
    void analyse();

    PathAnalyser& pathAnalyser() { return pathAnalyser_; }
    SemanticAnalyser* semanticAnalyser() const { return analyser_; }
    SemanticScoper& scoper() { return *scoper_; }
    const TypeContext& typeContext() const { return typeContext_; }
    Function* function() const { return function_; }
    Compiler* compiler() const { return function_->package()->compiler(); }

    Type integer();
    Type boolean();
    Type doubleType();
    Type symbol();

    /// Parses an expression node, verifies it return type and boxes it according to the given expectation.
    /// Calls @c expect internally.
    Type expectType(const Type &type, std::shared_ptr<ASTExpr>*, std::vector<CommonTypeFinder> *ctargs = nullptr);
    /// Parses an expression node and boxes it according to the given expectation. Calls @c box internally.
    Type expect(const TypeExpectation &expectation, std::shared_ptr<ASTExpr>*);
    /// Makes the node comply with the expectation by dereferencing, temporarily storing or boxing it.
    /// @param node A pointer to the node pointer. The pointer to which this pointer points might be changed.
    /// @note Only use this if there is a good reason why expect() cannot be used.
    Type comply(Type exprType, const TypeExpectation &expectation, std::shared_ptr<ASTExpr> *node);

    void validateMetability(const Type &originalType, const SourcePosition &p) const;

    /// Analyses @c node and sets the expression type of the node to the type that will be returned.
    /// @returns The type denoted by the $type-expression$ resolved by Type::resolveOnSuperArgumentsAndConstraints.
    Type analyseTypeExpr(const std::shared_ptr<ASTTypeExpr> &node, const TypeExpectation &exp);

    /// Checks that the function can be accessed or issues an error. Checks that the function is not deprecated
    /// and issues a warning otherwise.
    void checkFunctionUse(Function *function, const SourcePosition &p) const;

    Type analyseFunctionCall(ASTArguments *node, const Type &type, Function *function);
private:
    PathAnalyser pathAnalyser_;
    /// The scoper responsible for scoping the function being compiled.
    std::unique_ptr<SemanticScoper> scoper_;
    TypeContext typeContext_;

    Function *function_;
    SemanticAnalyser *analyser_;

    /// Issues a warning at the given position if the function is deprecated.
    void deprecatedWarning(Function *function, const SourcePosition &p) const;

    void analyseReturn(const std::shared_ptr<ASTBlock> &);
    void analyseInitializationRequirements();

    /// Ensures that node has the required number of generic arguments for a call to function.
    /// If none are provided but function expects generic arguments, this method tries to infer them.
    void ensureGenericArguments(ASTArguments *node, const Type &type, Function *function);

    /// Returns true if exprType and expectation are callables and there is a mismatch between the argument or return
    /// StorageTypes.
    bool callableBoxingRequired(const TypeExpectation &expectation, const Type &exprType);
    /// Boxes or unboxes the value to the StorageType specified by expectation.simplifyType()
    Type box(Type exprType, const TypeExpectation &expectation, std::shared_ptr<ASTExpr> *node);
    /// Callable boxes the value if callableBoxingRequired() returns true.
    Type callableBox(Type exprType, const TypeExpectation &expectation, std::shared_ptr<ASTExpr> *node);

    void boxToBox(Type &exprType, const TypeExpectation &expectation, std::shared_ptr<ASTExpr> *node) const;

    void boxToSimple(Type &exprType, std::shared_ptr<ASTExpr> *node) const;

    void boxToSimpleOptional(Type &exprType, const TypeExpectation &expectation, std::shared_ptr<ASTExpr> *node) const;
};

}  // namespace EmojicodeCompiler

#endif /* FunctionAnalyser_hpp */
