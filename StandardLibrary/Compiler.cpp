// === ROPLang C++ IDE Backend ===
#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <filesystem>
#include <chrono>
#include <atomic>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Verifier.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>

using namespace llvm;
using namespace std;

LLVMContext TheContext;
unique_ptr<Module> TheModule;
IRBuilder<> Builder(TheContext);
ExecutionEngine *TheExecutionEngine = nullptr;

// === Initialize LLVM Target ===
void initializeLLVM() {
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();
    InitializeNativeTargetAsmParser();
}

// === Build Sample Function ===
Function* buildSampleFunction() {
    FunctionType *funcType = FunctionType::get(Type::getInt32Ty(TheContext), false);
    Function *func = Function::Create(funcType, Function::ExternalLinkage, "sample", TheModule.get());
    BasicBlock *BB = BasicBlock::Create(TheContext, "entry", func);
    Builder.SetInsertPoint(BB);
    Builder.CreateRet(ConstantInt::get(Type::getInt32Ty(TheContext), 99));
    return func;
}

// === Inline Build Script ===
void inlineBuild() {
    string irString;
    raw_string_ostream irStream(irString);
    TheModule->print(irStream, nullptr);
    cout << "[BUILD] LLVM IR Generated:\n" << irStream.str() << endl;

    string error;
    EngineBuilder builder(move(TheModule));
    builder.setErrorStr(&error);
    builder.setEngineKind(EngineKind::JIT);
    TheExecutionEngine = builder.create();

    if (!TheExecutionEngine) {
        cerr << "[ERROR] Failed to create ExecutionEngine: " << error << endl;
        exit(1);
    }

    vector<GenericValue> noargs;
    GenericValue result = TheExecutionEngine->runFunction(TheExecutionEngine->FindFunctionNamed("sample"), noargs);
    cout << "[EXEC] Result from compiled function: " << result.IntVal << endl;
}

// === Inline Compile Script ===
void inlineCompile() {
    ofstream irFile("rop_module.ll");
    raw_os_ostream rawIRFile(irFile);
    TheModule->print(rawIRFile, nullptr);
    irFile.close();
    cout << "[COMPILE] Saved LLVM IR to rop_module.ll" << endl;
}

// === Concurrency Engine ===
void concurrentChainExec(vector<void(*)()> funcs) {
    vector<thread> threads;
    for (auto fn : funcs) {
        threads.emplace_back([=]() {
            cout << "[CHAIN] Executing..." << endl;
            fn();
            cout << "[CHAIN] Done." << endl;
        });
    }
    for (auto &t : threads) t.join();
}

// === Splicing Tunnel ===
class SplicingTunnel {
    queue<string> buffer;
    mutex mtx;
    condition_variable cv;
public:
    void transmit(const string &msg) {
        unique_lock<mutex> lock(mtx);
        cout << "[TUNNEL] Transmitting: " << msg << endl;
        buffer.push(msg);
        cv.notify_one();
    }

    string receive() {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [&](){ return !buffer.empty(); });
        string msg = buffer.front(); buffer.pop();
        cout << "[TUNNEL] Received: " << msg << endl;
        return msg;
    }
};

// === Chain Debugger ===
void traceChain(const string &name, const vector<string> &steps) {
    cout << "[TRACE] Chain: " << name << endl;
    for (size_t i = 0; i < steps.size(); ++i) {
        cout << "  Step " << i + 1 << ": " << steps[i] << endl;
    }
}

// === File Watcher ===
void watchFile(const string& filename, function<void()> onChange) {
    using namespace std::chrono_literals;
    atomic<filesystem::file_time_type> last_write_time = filesystem::last_write_time(filename);
    thread watcher([=, &last_write_time]() {
        while (true) {
            this_thread::sleep_for(1s);
            auto current_write_time = filesystem::last_write_time(filename);
            if (current_write_time != last_write_time) {
                last_write_time = current_write_time;
                cout << "[WATCH] Detected change in: " << filename << endl;
                onChange();
            }
        }
    });
    watcher.detach();
}

// === ROPLang Construct Hook ===
void buildROPConstruct() {
    // Placeholder: You can dynamically add specific ROPLang logic here.
    cout << "[ROPLANG] Building ROPLang-specific construct..." << endl;
    // Example: emitting a loop, conditional, or inline chain-return structure.
}

// === Main Entry Point ===
int main() {
    initializeLLVM();
    TheModule = make_unique<Module>("rop_module", TheContext);
    buildSampleFunction();
    buildROPConstruct();

    inlineBuild();
    inlineCompile();

    SplicingTunnel tunnel;
    tunnel.transmit("INIT");
    tunnel.receive();

    concurrentChainExec({ inlineBuild, inlineCompile });
    traceChain("compile-sequence", { "IR Gen", "IR Verify", "Machine Target", "Emit Assembly" });

    watchFile("rop_module.ll", [](){
        cout << "[AUTO-BUILD] Rebuilding due to IR file change..." << endl;
        inlineBuild();
        inlineCompile();
    });

    this_thread::sleep_for(chrono::minutes(10));
    return 0;
}
// ROPLang IDE Backend - C++ Version with LLVM-style Codegen, Concurrency, and Build Tools
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Verifier.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <fstream>
#include <iostream>

using namespace llvm;

LLVMContext context;
std::unique_ptr<Module> module;
std::unique_ptr<IRBuilder<>> builder;
ExecutionEngine* engine = nullptr;

void initLLVM() {
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();
    InitializeNativeTargetAsmParser();
    module = std::make_unique<Module>("rop_module", context);
    builder = std::make_unique<IRBuilder<>>(context);
}

Function* buildSampleFunction() {
    FunctionType* funcType = FunctionType::get(Type::getInt32Ty(context), false);
    Function* func = Function::Create(funcType, Function::ExternalLinkage, "sample", module.get());
    BasicBlock* entry = BasicBlock::Create(context, "entry", func);
    builder->SetInsertPoint(entry);
    builder->CreateRet(ConstantInt::get(Type::getInt32Ty(context), 99));
    return func;
}

void inlineBuild() {
    std::string llvmIR;
    raw_string_ostream os(llvmIR);
    module->print(os, nullptr);
    std::cout << "[BUILD] LLVM IR:\n" << os.str() << std::endl;

    std::string errStr;
    EngineBuilder builder(std::move(module));
    builder.setErrorStr(&errStr);
    engine = builder.create();
    if (!engine) {
        std::cerr << "[ERROR] " << errStr << std::endl;
        return;
    }

    std::vector<GenericValue> noargs;
    GenericValue val = engine->runFunction(engine->FindFunctionNamed("sample"), noargs);
    std::cout << "[EXEC] Sample returned: " << val.IntVal.getZExtValue() << std::endl;
}

void inlineCompile() {
    std::ofstream file("rop_module.ll");
    std::string llvmIR;
    raw_string_ostream os(llvmIR);
    module->print(os, nullptr);
    file << os.str();
    file.close();
    std::cout << "[COMPILE] Saved LLVM IR to rop_module.ll" << std::endl;
}

void concurrentChainExec(std::vector<void(*)()> funcs) {
    std::vector<std::thread> threads;
    for (auto& fn : funcs) {
        threads.emplace_back([=]() {
            std::cout << "[CHAIN] Running function..." << std::endl;
            fn();
            std::cout << "[CHAIN] Function done." << std::endl;
        });
    }
    for (auto& t : threads) t.join();
}

class SplicingTunnel {
private:
    std::queue<std::string> buffer;
    std::mutex mtx;
    std::condition_variable cv;

public:
    void transmit(const std::string& msg) {
        std::lock_guard<std::mutex> lock(mtx);
        buffer.push(msg);
        std::cout << "[TUNNEL] Transmitting: " << msg << std::endl;
        cv.notify_one();
    }

    std::string receive() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [&]() { return !buffer.empty(); });
        std::string msg = buffer.front();
        buffer.pop();
        std::cout << "[TUNNEL] Received: " << msg << std::endl;
        return msg;
    }
};

void traceChain(const std::string& name, const std::vector<std::string>& steps) {
    std::cout << "[TRACE] Chain: " << name << std::endl;
    for (size_t i = 0; i < steps.size(); ++i)
        std::cout << "  Step " << (i + 1) << ": " << steps[i] << std::endl;
}

int main() {
    initLLVM();
    buildSampleFunction();

    inlineBuild();
    inlineCompile();

    SplicingTunnel tunnel;
    tunnel.transmit("INIT");
    tunnel.receive();

    concurrentChainExec({inlineBuild, inlineCompile});
    traceChain("compile-sequence", {"IR Gen", "IR Verify", "Machine Target", "Emit Assembly"});

    return 0;
}

// === ROPLang C++ IDE Backend ===
#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <filesystem>
#include <chrono>
#include <atomic>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Verifier.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>

using namespace llvm;
using namespace std;

LLVMContext TheContext;
unique_ptr<Module> TheModule;
IRBuilder<> Builder(TheContext);
ExecutionEngine *TheExecutionEngine = nullptr;

// === Initialize LLVM Target ===
void initializeLLVM() {
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();
    InitializeNativeTargetAsmParser();
}

// === Build Sample Function ===
Function* buildSampleFunction() {
    FunctionType *funcType = FunctionType::get(Type::getInt32Ty(TheContext), false);
    Function *func = Function::Create(funcType, Function::ExternalLinkage, "sample", TheModule.get());
    BasicBlock *BB = BasicBlock::Create(TheContext, "entry", func);
    Builder.SetInsertPoint(BB);
    Builder.CreateRet(ConstantInt::get(Type::getInt32Ty(TheContext), 99));
    return func;
}

// === Inline Build Script ===
void inlineBuild() {
    string irString;
    raw_string_ostream irStream(irString);
    TheModule->print(irStream, nullptr);
    cout << "[BUILD] LLVM IR Generated:\n" << irStream.str() << endl;

    string error;
    EngineBuilder builder(move(TheModule));
    builder.setErrorStr(&error);
    builder.setEngineKind(EngineKind::JIT);
    TheExecutionEngine = builder.create();

    if (!TheExecutionEngine) {
        cerr << "[ERROR] Failed to create ExecutionEngine: " << error << endl;
        exit(1);
    }

    vector<GenericValue> noargs;
    GenericValue result = TheExecutionEngine->runFunction(TheExecutionEngine->FindFunctionNamed("sample"), noargs);
    cout << "[EXEC] Result from compiled function: " << result.IntVal << endl;
}

// === Inline Compile Script ===
void inlineCompile() {
    ofstream irFile("rop_module.ll");
    raw_os_ostream rawIRFile(irFile);
    TheModule->print(rawIRFile, nullptr);
    irFile.close();
    cout << "[COMPILE] Saved LLVM IR to rop_module.ll" << endl;
}

// === Concurrency Engine ===
void concurrentChainExec(vector<void(*)()> funcs) {
    vector<thread> threads;
    for (auto fn : funcs) {
        threads.emplace_back([=]() {
            cout << "[CHAIN] Executing..." << endl;
            fn();
            cout << "[CHAIN] Done." << endl;
        });
    }
    for (auto &t : threads) t.join();
}

// === Splicing Tunnel ===
class SplicingTunnel {
    queue<string> buffer;
    mutex mtx;
    condition_variable cv;
public:
    void transmit(const string &msg) {
        unique_lock<mutex> lock(mtx);
        cout << "[TUNNEL] Transmitting: " << msg << endl;
        buffer.push(msg);
        cv.notify_one();
    }

    string receive() {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [&](){ return !buffer.empty(); });
        string msg = buffer.front(); buffer.pop();
        cout << "[TUNNEL] Received: " << msg << endl;
        return msg;
    }
};

// === Chain Debugger ===
void traceChain(const string &name, const vector<string> &steps) {
    cout << "[TRACE] Chain: " << name << endl;
    for (size_t i = 0; i < steps.size(); ++i) {
        cout << "  Step " << i + 1 << ": " << steps[i] << endl;
    }
}

// === File Watcher ===
void watchFile(const string& filename, function<void()> onChange) {
    using namespace std::chrono_literals;
    atomic<filesystem::file_time_type> last_write_time = filesystem::last_write_time(filename);
    thread watcher([=, &last_write_time]() {
        while (true) {
            this_thread::sleep_for(1s);
            auto current_write_time = filesystem::last_write_time(filename);
            if (current_write_time != last_write_time) {
                last_write_time = current_write_time;
                cout << "[WATCH] Detected change in: " << filename << endl;
                onChange();
            }
        }
    });
    watcher.detach();
}

// === ROPLang Construct Hook ===
void buildROPConstruct() {
    // Placeholder: You can dynamically add specific ROPLang logic here.
    cout << "[ROPLANG] Building ROPLang-specific construct..." << endl;
    // Example: emitting a loop, conditional, or inline chain-return structure.
}

// === Main Entry Point ===
int main() {
    initializeLLVM();
    TheModule = make_unique<Module>("rop_module", TheContext);
    buildSampleFunction();
    buildROPConstruct();

    inlineBuild();
    inlineCompile();

    SplicingTunnel tunnel;
    tunnel.transmit("INIT");
    tunnel.receive();

    concurrentChainExec({ inlineBuild, inlineCompile });
    traceChain("compile-sequence", { "IR Gen", "IR Verify", "Machine Target", "Emit Assembly" });

    watchFile("rop_module.ll", [](){
        cout << "[AUTO-BUILD] Rebuilding due to IR file change..." << endl;
        inlineBuild();
        inlineCompile();
    });

    this_thread::sleep_for(chrono::minutes(10));
    return 0;
}

class ExprAST {
public:
    virtual ~ExprAST() = default;
    virtual Value* codegen() = 0;
};

class NumberExprAST : public ExprAST {
    int Val;
public:
    NumberExprAST(int Val) : Val(Val) {}
    Value* codegen() override {
        return ConstantInt::get(Type::getInt32Ty(TheContext), Val);
    }
};

class BinaryExprAST : public ExprAST {
    char Op;
    unique_ptr<ExprAST> LHS, RHS;
public:
    BinaryExprAST(char Op, unique_ptr<ExprAST> LHS, unique_ptr<ExprAST> RHS)
        : Op(Op), LHS(move(LHS)), RHS(move(RHS)) {}

    Value* codegen() override {
        Value *L = LHS->codegen();
        Value *R = RHS->codegen();
        if (!L || !R) return nullptr;

        switch (Op) {
            case '+': return Builder.CreateAdd(L, R, "addtmp");
            case '-': return Builder.CreateSub(L, R, "subtmp");
            case '*': return Builder.CreateMul(L, R, "multmp");
            case '/': return Builder.CreateSDiv(L, R, "divtmp");
            default: return nullptr;
        }
    }
};

enum Token { tok_eof = -1, tok_number = -2, tok_plus = '+', tok_minus = '-', tok_mul = '*', tok_div = '/' };

int CurTok;
int getNextToken() {
    // Basic tokenizer that returns one of the Token enums.
}

unique_ptr<ExprAST> ParseExpression();
unique_ptr<ExprAST> ParsePrimary();
unique_ptr<ExprAST> ParseBinaryRHS(int prec, unique_ptr<ExprAST> LHS);

Function* createExprWrapper(unique_ptr<ExprAST> expr) {
    FunctionType *funcType = FunctionType::get(Type::getInt32Ty(TheContext), false);
    Function *func = Function::Create(funcType, Function::ExternalLinkage, "evalExpr", TheModule.get());
    BasicBlock *BB = BasicBlock::Create(TheContext, "entry", func);
    Builder.SetInsertPoint(BB);
    Value *RetVal = expr->codegen();
    Builder.CreateRet(RetVal);
    return func;
}

Function* F = createExprWrapper(move(parsedAST));
GenericValue result = TheExecutionEngine->runFunction(F, {});
cout << "[EVAL] Result: " << result.IntVal << endl;

map<string, Value*> NamedValues;

var x = 5
x = x + 1

concurrentChainExec({ [](){ parseAndEval("5 + 4 * 3"); }, [](){ parseAndEval("7 + 2"); } });

#include <gtest/gtest.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/IR/IRBuilder.h>

using namespace llvm;

// === Test AST Nodes ===
class ASTTest : public ::testing::Test {
protected:
    LLVMContext context;
    unique_ptr<Module> module;
    IRBuilder<> builder;

    ASTTest() : builder(context) {
        module = make_unique<Module>("ROPTest", context);
    }

    // Helper to test that the module builds successfully
    void runTestFunction(Function* func) {
        EngineBuilder builder(move(module));
        unique_ptr<ExecutionEngine> engine(builder.create());
        ASSERT_TRUE(engine != nullptr);
        
        // Run the function
        vector<GenericValue> noargs;
        GenericValue result = engine->runFunction(func, noargs);
        ASSERT_EQ(result.IntVal.getSExtValue(), 99);
    }

    Function* buildSimpleAddFunction() {
        // Create a simple function that adds two numbers
        FunctionType *funcType = FunctionType::get(Type::getInt32Ty(context), false);
        Function *func = Function::Create(funcType, Function::ExternalLinkage, "addTestFunc", module.get());
        BasicBlock *BB = BasicBlock::Create(context, "entry", func);
        builder.SetInsertPoint(BB);

        // Create the operation
        Value *lhs = ConstantInt::get(Type::getInt32Ty(context), 50);
        Value *rhs = ConstantInt::get(Type::getInt32Ty(context), 49);
        Value *sum = builder.CreateAdd(lhs, rhs, "sum");
        builder.CreateRet(sum);

        return func;
    }
};

// === Unit Test: Simple Addition ===
TEST_F(ASTTest, SimpleAdditionTest) {
    Function* addFunc = buildSimpleAddFunction();
    runTestFunction(addFunc);  // Test if the generated function adds correctly
}

// === Unit Test: AST Parse/Execute Expression ===
TEST_F(ASTTest, ASTParseTest) {
    // Create a simple AST node and test the evaluation
    unique_ptr<ExprAST> expr = make_unique<BinaryExprAST>('+', make_unique<NumberExprAST>(3), make_unique<NumberExprAST>(4));
    Function* func = createExprWrapper(move(expr));
    runTestFunction(func);  // Should return 7 for 3 + 4
}

TEST_F(ASTTest, InvalidExpressionTest) {
    // Example of invalid AST generation (this would be caught in real code, just a placeholder)
    unique_ptr<ExprAST> invalidExpr = nullptr;
    ASSERT_THROW(invalidExpr->codegen(), std::exception);
}

TEST_F(ASTTest, PerformanceTest) {
    auto start = std::chrono::high_resolution_clock::now();

    // Run a large batch of expressions
    for (int i = 0; i < 1000; ++i) {
        unique_ptr<ExprAST> expr = make_unique<BinaryExprAST>('*', make_unique<NumberExprAST>(i), make_unique<NumberExprAST>(i+1));
        Function* func = createExprWrapper(move(expr));
        runTestFunction(func);
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Execution time for 1000 expressions: " << elapsed.count() << " seconds." << std::endl;
}

#ifndef EXPR_AST_HPP
#define EXPR_AST_HPP

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/raw_ostream.h>
#include <memory>
#include <string>
#include <map>

using namespace llvm;
using namespace std;

// Base class for all expressions
class ExprAST {
public:
    virtual ~ExprAST() = default;
    virtual Value* codegen() = 0;
};

// Expression for number literals
class NumberExprAST : public ExprAST {
    int Val;
public:
    NumberExprAST(int Val) : Val(Val) {}
    Value* codegen() override;
};

// Expression for binary operations
class BinaryExprAST : public ExprAST {
    char Op;
    unique_ptr<ExprAST> LHS, RHS;
public:
    BinaryExprAST(char Op, unique_ptr<ExprAST> LHS, unique_ptr<ExprAST> RHS)
        : Op(Op), LHS(move(LHS)), RHS(move(RHS)) {}
    Value* codegen() override;
};

// Variable Declaration Expression
class VarDeclExprAST : public ExprAST {
    string Name;
    unique_ptr<ExprAST> InitExpr;
public:
    VarDeclExprAST(const string &Name, unique_ptr<ExprAST> InitExpr)
        : Name(Name), InitExpr(move(InitExpr)) {}
    Value* codegen() override;
};

// Assignment Expression
class AssignmentExprAST : public ExprAST {
    string VarName;
    unique_ptr<ExprAST> Right;
public:
    AssignmentExprAST(const string &VarName, unique_ptr<ExprAST> Right)
        : VarName(VarName), Right(move(Right)) {}
    Value* codegen() override;
};

// Parser/AST for the program’s flow, representing a series of statements.
class ProgramAST {
    vector<unique_ptr<ExprAST>> Statements;
public:
    void addStatement(unique_ptr<ExprAST> stmt) {
        Statements.push_back(move(stmt));
    }

    void codegen();
};

#endif // EXPR_AST_HPP

#include "ExprAST.hpp"

extern LLVMContext TheContext;
extern IRBuilder<> Builder;
extern map<string, Value*> NamedValues;

Value* NumberExprAST::codegen() {
    return ConstantInt::get(Type::getInt32Ty(TheContext), Val);
}

Value* BinaryExprAST::codegen() {
    Value *L = LHS->codegen();
    Value *R = RHS->codegen();
    if (!L || !R) return nullptr;

    switch (Op) {
        case '+': return Builder.CreateAdd(L, R, "addtmp");
        case '-': return Builder.CreateSub(L, R, "subtmp");
        case '*': return Builder.CreateMul(L, R, "multmp");
        case '/': return Builder.CreateSDiv(L, R, "divtmp");
        default: return nullptr;
    }
}

Value* VarDeclExprAST::codegen() {
    Value *InitVal = InitExpr->codegen();
    if (!InitVal) return nullptr;
    
    AllocaInst *Alloca = Builder.CreateAlloca(Type::getInt32Ty(TheContext), nullptr, Name);
    Builder.CreateStore(InitVal, Alloca);
    
    NamedValues[Name] = Alloca;
    return Alloca;
}

Value* AssignmentExprAST::codegen() {
    auto Var = NamedValues[VarName];
    if (!Var) {
        cerr << "[ERROR] Unknown variable " << VarName << endl;
        return nullptr;
    }

    Value *RHSVal = Right->codegen();
    if (!RHSVal) return nullptr;
    
    Builder.CreateStore(RHSVal, Var);
    return RHSVal;
}

void ProgramAST::codegen() {
    for (auto &stmt : Statements) {
        stmt->codegen();
    }
}

#ifndef PARSER_HPP
#define PARSER_HPP

#include "ExprAST.hpp"
#include <string>
#include <memory>

extern int CurTok;
extern int getNextToken();

unique_ptr<ExprAST> ParseExpression();
unique_ptr<ExprAST> ParsePrimary();
unique_ptr<ExprAST> ParseBinaryRHS(int prec, unique_ptr<ExprAST> LHS);
unique_ptr<ExprAST> ParseNumberExpr();
unique_ptr<ExprAST> ParseVarDecl();
unique_ptr<ExprAST> ParseAssignment();
void handleParsingError(const std::string &msg);

#endif // PARSER_HPP

#ifndef PARSER_HPP
#define PARSER_HPP

#include "ExprAST.hpp"
#include <string>
#include <memory>

extern int CurTok;
extern int getNextToken();

unique_ptr<ExprAST> ParseExpression();
unique_ptr<ExprAST> ParsePrimary();
unique_ptr<ExprAST> ParseBinaryRHS(int prec, unique_ptr<ExprAST> LHS);
unique_ptr<ExprAST> ParseNumberExpr();
unique_ptr<ExprAST> ParseVarDecl();
unique_ptr<ExprAST> ParseAssignment();
void handleParsingError(const std::string &msg);

#endif // PARSER_HPP

#include "Parser.hpp"
#include <iostream>

int CurTok = 0; // Initialize current token (use tokenization in real code)

unique_ptr<ExprAST> ParseExpression() {
    unique_ptr<ExprAST> LHS = ParsePrimary();
    if (!LHS) return nullptr;
    return ParseBinaryRHS(0, move(LHS));
}

unique_ptr<ExprAST> ParsePrimary() {
    if (CurTok == tok_number) {
        return ParseNumberExpr();
    }
    handleParsingError("Unknown primary expression");
    return nullptr;
}

unique_ptr<ExprAST> ParseNumberExpr() {
    int Val = 0;
    if (CurTok == tok_number) {
        Val = getNextToken(); // consume number token
    }
    return make_unique<NumberExprAST>(Val);
}

unique_ptr<ExprAST> ParseVarDecl() {
    string VarName = "var"; // this will come from tokenized input
    getNextToken(); // skip `var`
    unique_ptr<ExprAST> InitExpr = ParseExpression();


    return make_unique<VarDeclExprAST>(VarName, move(InitExpr));
}

unique_ptr<ExprAST> ParseAssignment() {
    string VarName = "var"; // example, fetch from tokens
    getNextToken(); // consume token
    unique_ptr<ExprAST> Right = ParseExpression();
    return make_unique<AssignmentExprAST>(VarName, move(Right));
}

void handleParsingError(const std::string &msg) {
    cerr << "[ERROR] " << msg << endl;
    exit(1);
}

#include <iostream>
#include <memory>
#include <vector>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/Support/TargetSelect.h>

using namespace llvm;
using namespace std;

extern unique_ptr<Module> TheModule;
extern LLVMContext TheContext;
extern IRBuilder<> Builder;
extern ExecutionEngine *TheExecutionEngine;

int main() {
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();
    InitializeNativeTargetAsmParser();

    TheModule = make_unique<Module>("ROPModule", TheContext);

    // Parse the program
    ProgramAST program;
    program.addStatement(make_unique<VarDeclExprAST>("x", make_unique<NumberExprAST>(5)));
    program.addStatement(make_unique<BinaryExprAST>('+', make_unique<NumberExprAST>(3), make_unique<NumberExprAST>(4)));
    
    // Generate IR and JIT execute
    program.codegen();
    
    // JIT Execution
    EngineBuilder builder(move(TheModule));
    builder.setEngineKind(EngineKind::JIT);
    TheExecutionEngine = builder.create();
    if (!TheExecutionEngine) {
        cerr << "[ERROR] Failed to create Execution Engine!" << endl;
        return 1;
    }
    
    // Execute JIT-compiled code (based on your AST)
    GenericValue result = TheExecutionEngine->runFunction(TheExecutionEngine->FindFunctionNamed("evalExpr"), {});
    cout << "[EXEC] Result from JIT execution: " << result.IntVal << endl;
    
    return 0;
}
