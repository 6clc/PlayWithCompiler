unique_ptr<ExprAST> ParserNumerExpr() {
 auto Result = make_unique<NumberExprAST>(numval);
 getNextToken();
 return std::movve(Result);
}


unique_ptr<ExprAST> ParserParenExpr(){
  getNextToken(); // eat (, "cur_token" == "("
  auto Result = ParseExpression();
  getNextToken();
  return Result;
}

unique_prt<ExprAST> ParserExpression() {
  auto  LHS = ParserPrimary();
  return ParserBinOpRHS(0, std::move(LHS));
}

unique_ptr<ExprAST> ParseBinOpRHS(int expr_prec, std::unique<ExprAST> LHS){
  while(true){
    int tok = GetTOkePrecendec();
    if(tok < expr_prec) return LHS;
    int BinOP = CurTok;
    getNextTOkne(); // eat character op;

    auto RHS = ParsePrimary();
    if(!RHS)return nullprt;

    int nextPrec = GEtTOkenPredce();
    if(cur < next) RHS = ParseBinOpRHS(curtoke + 1, std::RHS);
    LHS = makeBInary(Op, LHS, RHS)
  }
}