#include <iostream>
#include <sstream>
#include <cstdlib>
#include <random>
#include <set>
#include <algorithm>

#include "lang.h"
#include "transform.h"
#include "visitor.h"

// ============================================================================
// CHEAT: Transform code to avoid plagiarism detection
// ============================================================================

class Cheat : public Transform {
 private:
  std::mt19937 rng;
  int counter;
  
 public:
  Cheat() : rng(42), counter(0) {}
  
  Variable *transformVariable(Variable *node) override {
    // Rename all variables with a systematic pattern
    return new Variable("v" + std::to_string(std::hash<std::string>{}(node->name) % 100000) + "_" + node->name);
  }
  
  // Add some variation to integer literals
  Expression *transformIntegerLiteral(IntegerLiteral *node) override {
    // Sometimes transform x into (x+1-1) or similar
    if (node->value != 0 && rng() % 3 == 0) {
      // Transform x into (x + 0)
      return new CallExpression("+", {
        new IntegerLiteral(node->value),
        new IntegerLiteral(0)
      });
    }
    return new IntegerLiteral(node->value);
  }
  
  // Sometimes wrap expressions in redundant operations
  Expression *transformExpression(Expression *node) override {
    Expression *result = Transform::transformExpression(node);
    
    // Occasionally wrap in identity operations
    if (rng() % 5 == 0 && !node->is<CallExpression>()) {
      // Wrap in (+ expr 0)
      return new CallExpression("+", {result, new IntegerLiteral(0)});
    }
    
    return result;
  }
  
  // Add dummy operations in blocks
  Statement *transformBlockStatement(BlockStatement *node) override {
    std::vector<Statement *> body;
    for (auto stmt : node->body) {
      body.push_back(transformStatement(stmt));
      
      // Occasionally add a dummy statement
      if (rng() % 10 == 0) {
        // Add a dummy assignment like (set __dummy 0)
        body.push_back(new SetStatement(
          new Variable("__dummy_" + std::to_string(counter++)),
          new IntegerLiteral(0)
        ));
      }
    }
    return new BlockStatement(body);
  }
};

// ============================================================================
// ANTICHEAT: Detect plagiarism
// ============================================================================

// Count various features of a program
class FeatureCounter : public Visitor<int> {
 public:
  int totalNodes = 0;
  int ifCount = 0;
  int forCount = 0;
  int setCount = 0;
  int returnCount = 0;
  int blockCount = 0;
  int callCount = 0;
  int varCount = 0;
  int literalCount = 0;
  std::set<std::string> variableNames;
  std::set<std::string> functionNames;
  
  int visitProgram(Program *node) override {
    for (auto func : node->body) {
      visitFunctionDeclaration(func);
    }
    return 0;
  }
  
  int visitFunctionDeclaration(FunctionDeclaration *node) override {
    totalNodes++;
    functionNames.insert(node->name);
    for (auto param : node->params) {
      variableNames.insert(param->name);
    }
    visitStatement(node->body);
    return 0;
  }
  
  int visitExpressionStatement(ExpressionStatement *node) override {
    totalNodes++;
    visitExpression(node->expr);
    return 0;
  }
  
  int visitSetStatement(SetStatement *node) override {
    totalNodes++;
    setCount++;
    variableNames.insert(node->name->name);
    visitExpression(node->value);
    return 0;
  }
  
  int visitIfStatement(IfStatement *node) override {
    totalNodes++;
    ifCount++;
    visitExpression(node->condition);
    visitStatement(node->body);
    return 0;
  }
  
  int visitForStatement(ForStatement *node) override {
    totalNodes++;
    forCount++;
    visitStatement(node->init);
    visitExpression(node->test);
    visitStatement(node->update);
    visitStatement(node->body);
    return 0;
  }
  
  int visitBlockStatement(BlockStatement *node) override {
    totalNodes++;
    blockCount++;
    for (auto stmt : node->body) {
      visitStatement(stmt);
    }
    return 0;
  }
  
  int visitReturnStatement(ReturnStatement *node) override {
    totalNodes++;
    returnCount++;
    visitExpression(node->value);
    return 0;
  }
  
  int visitIntegerLiteral(IntegerLiteral *node) override {
    totalNodes++;
    literalCount++;
    return 0;
  }
  
  int visitVariable(Variable *node) override {
    totalNodes++;
    varCount++;
    variableNames.insert(node->name);
    return 0;
  }
  
  int visitCallExpression(CallExpression *node) override {
    totalNodes++;
    callCount++;
    for (auto arg : node->args) {
      visitExpression(arg);
    }
    return 0;
  }
};

// Calculate similarity between two programs
double calculateSimilarity(Program *prog1, Program *prog2) {
  FeatureCounter fc1, fc2;
  fc1.visitProgram(prog1);
  fc2.visitProgram(prog2);
  
  // Calculate various similarity metrics
  double scores[10];
  int idx = 0;
  
  // 1. Total node count similarity
  int maxNodes = std::max(fc1.totalNodes, fc2.totalNodes);
  int minNodes = std::min(fc1.totalNodes, fc2.totalNodes);
  scores[idx++] = maxNodes > 0 ? (double)minNodes / maxNodes : 1.0;
  
  // 2. Structure similarity (if, for, block counts)
  int struct1 = fc1.ifCount + fc1.forCount + fc1.blockCount;
  int struct2 = fc2.ifCount + fc2.forCount + fc2.blockCount;
  int maxStruct = std::max(struct1, struct2);
  int minStruct = std::min(struct1, struct2);
  scores[idx++] = maxStruct > 0 ? (double)minStruct / maxStruct : 1.0;
  
  // 3. Statement count similarity
  int stmt1 = fc1.setCount + fc1.returnCount;
  int stmt2 = fc2.setCount + fc2.returnCount;
  int maxStmt = std::max(stmt1, stmt2);
  int minStmt = std::min(stmt1, stmt2);
  scores[idx++] = maxStmt > 0 ? (double)minStmt / maxStmt : 1.0;
  
  // 4. Variable name similarity (Jaccard similarity)
  std::set<std::string> intersection, unionSet;
  std::set_intersection(fc1.variableNames.begin(), fc1.variableNames.end(),
                       fc2.variableNames.begin(), fc2.variableNames.end(),
                       std::inserter(intersection, intersection.begin()));
  std::set_union(fc1.variableNames.begin(), fc1.variableNames.end(),
                fc2.variableNames.begin(), fc2.variableNames.end(),
                std::inserter(unionSet, unionSet.begin()));
  scores[idx++] = unionSet.size() > 0 ? (double)intersection.size() / unionSet.size() : 1.0;
  
  // 5. Function name similarity
  std::set<std::string> funcIntersection, funcUnion;
  std::set_intersection(fc1.functionNames.begin(), fc1.functionNames.end(),
                       fc2.functionNames.begin(), fc2.functionNames.end(),
                       std::inserter(funcIntersection, funcIntersection.begin()));
  std::set_union(fc1.functionNames.begin(), fc1.functionNames.end(),
                fc2.functionNames.begin(), fc2.functionNames.end(),
                std::inserter(funcUnion, funcUnion.begin()));
  scores[idx++] = funcUnion.size() > 0 ? (double)funcIntersection.size() / funcUnion.size() : 1.0;
  
  // 6. Ratio of if statements
  double ifRatio1 = fc1.totalNodes > 0 ? (double)fc1.ifCount / fc1.totalNodes : 0;
  double ifRatio2 = fc2.totalNodes > 0 ? (double)fc2.ifCount / fc2.totalNodes : 0;
  scores[idx++] = 1.0 - std::abs(ifRatio1 - ifRatio2);
  
  // 7. Ratio of for statements
  double forRatio1 = fc1.totalNodes > 0 ? (double)fc1.forCount / fc1.totalNodes : 0;
  double forRatio2 = fc2.totalNodes > 0 ? (double)fc2.forCount / fc2.totalNodes : 0;
  scores[idx++] = 1.0 - std::abs(forRatio1 - forRatio2);
  
  // 8. Call expression count similarity
  int maxCall = std::max(fc1.callCount, fc2.callCount);
  int minCall = std::min(fc1.callCount, fc2.callCount);
  scores[idx++] = maxCall > 0 ? (double)minCall / maxCall : 1.0;
  
  // 9. Literal count similarity
  int maxLit = std::max(fc1.literalCount, fc2.literalCount);
  int minLit = std::min(fc1.literalCount, fc2.literalCount);
  scores[idx++] = maxLit > 0 ? (double)minLit / maxLit : 1.0;
  
  // 10. Overall size difference penalty
  double sizeDiff = std::abs(fc1.totalNodes - fc2.totalNodes);
  double avgSize = (fc1.totalNodes + fc2.totalNodes) / 2.0;
  scores[idx++] = avgSize > 0 ? std::max(0.0, 1.0 - sizeDiff / avgSize) : 1.0;
  
  // Calculate weighted average
  double totalScore = 0.0;
  double weights[] = {0.15, 0.15, 0.1, 0.2, 0.15, 0.05, 0.05, 0.05, 0.05, 0.05};
  for (int i = 0; i < idx; i++) {
    totalScore += scores[i] * weights[i];
  }
  
  return totalScore;
}

// ============================================================================
// MAIN: Detect mode and run appropriate function
// ============================================================================

int main() {
  try {
    // Read first program
    Program *prog1 = scanProgram(std::cin);
    
    // Check if there's more input (second program for anticheat mode)
    std::string nextToken;
    removeWhitespaces(std::cin);
    
    if (std::cin.peek() == EOF) {
      // CHEAT MODE: Only one program provided
      auto transformed = Cheat().transformProgram(prog1);
      std::cout << transformed->toString();
    } else {
      // ANTICHEAT MODE: Two programs provided
      Program *prog2 = scanProgram(std::cin);
      
      // Read test inputs (optional, can be used for additional verification)
      std::string input;
      int c;
      while ((c = std::cin.get()) != EOF) {
        input += c;
      }
      
      // Calculate similarity
      double similarity = calculateSimilarity(prog1, prog2);
      
      // Clamp to [0, 1]
      similarity = std::max(0.0, std::min(1.0, similarity));
      
      std::cout << similarity << std::endl;
    }
    
    return 0;
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
}
