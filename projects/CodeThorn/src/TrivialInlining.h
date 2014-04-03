#ifndef TRIVIAL_INLINING_H
#define TRIVIAL_INLINING_H

#include <list>

// TrivialInlining only inlines trivial functions. A function is trivial if it has no return value (void) and no parameters.
// TrivialInlining guarantees that no new code is added to the program (function calls are eliminated and the body of each function replaces
// the function call. Function calls inside expressions are NOT considered.
class TrivialInlining {
 public:
  TrivialInlining();
  // returns true if at least one trivial function was inlined, otherwise false.
  bool inlineFunctions(SgNode* root);
  void setDetailedOutput(bool val);
 private:
  bool detailedOutput;
  SgFunctionCallExp* isTrivialFunctionCall(SgNode* node);
  bool trivialInline(SgFunctionCallExp* funCall);
  std::list<SgFunctionCallExp*> trivialFunctionCalls(SgNode* node);
  size_t inlineFunctionCalls(std::list<SgFunctionCallExp*>& funCallList);
};

#endif
