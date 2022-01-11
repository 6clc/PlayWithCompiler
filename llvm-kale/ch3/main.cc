#include <cstdio>
#include <map>
#include <iostream>
#include <vector>

#include "llvm/IR/Context.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"

#include <llvm/ADT/APFloat.h>
#include <llvm/IR/Constants.h>

using namespace std;
int get_token();

LLVMContext* the_context;
Module* the_module;
IRBuilder<>* the_builder;
enum LEXER_TOKEN{
EOF_TOKEN = -1,
DEF_TOKEN = -2,
EXTERN_TOKEN = -3,
INDENTFIER_TOKEN = -4,
NUM_TOKEN = -5,
};

static string IntentifierStr = "";
static double Num = 0.;

static int CurToken = -1;
static int get_next_token(){
  return CurToken=get_token();
}

static map<char, int> pred_mp;

int get_precedence(){
  if(!isascii(CurToken)) return -1;
  if(pred_mp.count(CurToken)==0) return -1;
  return pred_mp[CurToken];
}
int get_token(){
  static int ch = ' ';
  while(isspace(ch)) ch = getchar();
  
  if(isalpha(ch)){
    IntentifierStr = ch;
    while( isalnum(ch = getchar()) ) IntentifierStr += ch;
    
    if ( "def" == IntentifierStr ) return DEF_TOKEN;
    if ( "extern" == IntentifierStr ) return EXTERN_TOKEN;
    return  INDENTFIER_TOKEN;
  }

  if (isdigit(ch) || ch == '.' ) {
    string NumStr;
    do{
      NumStr += ch;
      ch = getchar();
    }while ( isdigit(ch) || ch == '.' );
    Num = stod(NumStr, nullptr);
    return NUM_TOKEN;
  }

  // 其他的符号
  if (ch == EOF) return EOF_TOKEN;
  int ThisChar = ch;
  ch = getchar();
  return ThisChar;
}

class ExprAST{
public:
virtual ~ExprAST(){
}
virtual Value* codegen(){
cout << " base ast, may error" << endl;
return nullptr;
}

};

class NumberExprAST:public ExprAST{
public:
  double val;
  NumberExprAST(double val):val(val){}
  Value* codegen(){
    return ConstantFP::get(the_context, APFloat(val);
  }
};

class VariableExprAST:public ExprAST{
public:
  string name;
  VariableExprAST(string name):name(name){}
  void codegen(){
    cout << "[var ast " << name << " ]";
  }

};

class BinExprAST: public ExprAST{
public:
  char op;
  ExprAST* lhs;
  ExprAST* rhs;
  BinExprAST(char op, ExprAST* lhs, ExprAST* rhs)
    :op(op), lhs(lhs), rhs(rhs){}
  Value* codegen(){
          Value* ll = lhs->codegen();
          Value* rr = rhs->codegen();
          if(!ll || !rr) return nullptr;

          switch(op)
          {
                  case '+':
                          return the_builder.CreateFAdd(ll, rr, "addtmp");
                 


          }

  }
};

class CallableAST: public ExprAST{
public:
  string name;
  vector<ExprAST> paras;
  CallableAST(string name, vector<ExprAST> paras):name(name), paras(paras){}
  void codegen(){
    cout << "[调用函数 "<< name <<"]";
  }
  
};

class FuncAST: public ExprAST{
public:
  string name;
  vector<string> paras;
  ExprAST body;
  FuncAST(string name, vector<string> paras, ExprAST body)
    : name(name), paras(paras), body(body){}
  void codegen(){
    cout << "[定义函数 " << name << " ]";
  }
};

ExprAST* parseNumAST(){
 //  cout << "parseNumAST()" <<endl;
  ExprAST* ret =  new  NumberExprAST(Num);
  get_next_token();
  return ret;
}

ExprAST* parsePrimaryAST(){
  switch(CurToken){
 //   case INDENTFIER_TOKEN:
 //     return parserIndentifierAST();
    case NUM_TOKEN:
      return parseNumAST();
//  case '(':
//      return parseParenAST();
    default:
      	return nullptr; 
  }
}

ExprAST* parseBinaryAST(int pre_precedence, ExprAST* LHS){
  while (1){
    int cur_precedence = get_precedence();
    if(cur_precedence < pre_precedence) return LHS;

    char binop = CurToken;
    get_next_token();
    ExprAST* RHS = parsePrimaryAST();
    int nxt_precedence = get_precedence();
    if(cur_precedence < nxt_precedence){
      RHS = parseBinaryAST(cur_precedence + 1, RHS);
      if (RHS == nullptr) return nullptr;
    }

    LHS = new BinExprAST(binop, LHS, RHS);
  }
  return LHS;

}

ExprAST* parseExpressionAST(){
  ExprAST* lhs = parsePrimaryAST();
  ExprAST* expression_ast = parseBinaryAST(0, lhs);
  return expression_ast;
}

ExprAST* parseTopLevel(){
  ExprAST* express_ast = parseExpressionAST();
//  FunctionAST func_ast = parseFuncAST("none-name", {}, express_ast);
//  return func_ast;
  return express_ast;
}


void handleTopLevel(){
 // auto topAST = parseTopLevel();
 // cout << typeid(topAST).name() <<endl;
  ExprAST* topAST = parseTopLevel();
//  cout << typeid(*topAST).name();
  topAST -> codegen();
}

void main_loop(){
  while(1){
    fprintf(stderr,"INPUT>>");
    switch(CurToken){
      case ';':
        get_next_token();
      case DEF_TOKEN:
        break;
      case EXTERN_TOKEN:
        break;
      default:
	      handleTopLevel();
        break;
    }
  }
}

int main() {
  pred_mp['+'] = 10;
  pred_mp['-'] = 10;
  fprintf(stderr, "input any start");
  get_next_token();
  main_loop();
  return 0;
}
