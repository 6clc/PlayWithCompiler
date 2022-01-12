#include <cstdio>
#include <map>
#include <iostream>
#include <vector>

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"

#include <llvm/ADT/APFloat.h>
#include <llvm/IR/Constants.h>

#include "llvm/Support/raw_ostream.h"

using namespace std;
using namespace llvm;
int get_token();

static LLVMContext* the_context = new LLVMContext();
static Module* the_module = new Module("tmp", *the_context);
static IRBuilder<>* the_builder = new IRBuilder<>(*the_context);
static map<string, Value*> named_values;
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
  CurToken=get_token();
  cout << "token" << CurToken << endl;
  return CurToken;
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

ExprAST* parseExpressionAST();

class NumberExprAST:public ExprAST{
public:
  double val;
  NumberExprAST(double val):val(val){}
  Value* codegen(){
    //cout << "num ast"; 
    return ConstantFP::get(*the_context, APFloat(val));
  }
};

class VariableExprAST:public ExprAST{
public:
  string name;
  VariableExprAST(string name):name(name){}
  Value* codegen(){
    cout << "[var ast " << name << " ]";
    if(named_values.count(name)==0) return nullptr;
    return named_values[name];
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
        return the_builder -> CreateFAdd(ll, rr, "addtmp");
        default:
        return nullptr;
    }
  }
};

class CallableAST: public ExprAST{
public:
  string name;
  vector<ExprAST*> paras;
  CallableAST(string name, vector<ExprAST*> paras):name(name), paras(paras){}
  // void codegen(){
  //   cout << "[调用函数 "<< name <<"]";
  // }
  
};

class ProtoTypeAST: public ExprAST
{
        public:
                string name;
                vector<string> args;
                ProtoTypeAST(string name, vector<string> args)
                        :name(name), args(args){}
                Value* codegen(){
                        fprintf(stderr, "codegem prototype");
                        return nullptr;
                }

};

class FuncAST: public ExprAST{
public:
  ExprAST* proto_type;
  ExprAST* body;
  FuncAST(ExprAST* proto_type, ExprAST* body)
    : proto_type(proto_type), body(body){}
  Value* codegen(){
  //   cout << "[定义函数 " << name << " ]";
         return nullptr;
  }
};

ExprAST* parseNumAST(){
 //  cout << "parseNumAST()" <<endl;
  ExprAST* ret =  new  NumberExprAST(Num);
  get_next_token();
  return ret;
}

ExprAST* parserIndentifierAST(){
 string name = IntentifierStr;
 get_next_token();
 if('(' != CurToken) return new VariableExprAST(name);
 get_next_token();

 vector<ExprAST*>args;
 if(')' != CurToken)
 while(true){
    ExprAST* arg = parseExpressionAST();
    args.push_back(arg);
    if (')' == CurToken) break;
    if (',' != CurToken){
            fprintf(stderr, "call arg must split by ,");
    }
    get_next_token();
 }
 return new CallableAST(name, args);
}

ExprAST* parsePrimaryAST(){
  switch(CurToken){
    case INDENTFIER_TOKEN:
      return parserIndentifierAST();
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
    cout << "predence" << pre_precedence << " " << cur_precedence << endl;
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

ExprAST* parseProtoTypeAST(){
  string name = IntentifierStr;
  cout << "func name " << name << endl;
  get_next_token();
  get_next_token(); // eat (
  
  vector<string> args;
  cout << "liuchao" << CurToken << endl;
  while(CurToken == INDENTFIER_TOKEN){
    args.push_back(IntentifierStr);
    cout << "args name " << IntentifierStr << endl;
    get_next_token();
  }
  // eat )
  get_next_token();
  cout << "parse prototype" << endl;

  return new ProtoTypeAST(name, args);
}

ExprAST* parseFuncAST(){
  fprintf(stderr, "eat def");
  get_next_token(); // eat DEF
  ExprAST* prototype_ast = parseProtoTypeAST();
  // return prototype_ast;
  ExprAST* expression_ast = parseExpressionAST();
  cout << "parse func ast" << endl;
  return new FuncAST(prototype_ast, expression_ast);
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
  // auto* irr = topAST -> codegen();
   // irr->print(errs());
   fprintf(stderr, "\n");
}

void handleDef(){
  ExprAST* funcast = parseFuncAST();
  //auto* irr = funcast -> codegen();
  // irr->print(errs());
  fprintf(stderr, "handle def\n");
}

void main_loop(){
  while(1){
    fprintf(stderr,"ready>>");
    switch(CurToken){
      case ';':
        get_next_token();
        break;
      case DEF_TOKEN:
        handleDef();
        break;
      case EXTERN_TOKEN:
        break;
      default:
	     // handleTopLevel();
       get_next_token();
        break;
    }
  }
}

int main() {
  pred_mp['+'] = 10;
  pred_mp['-'] = 10;
  fprintf(stderr, "ready>>");
  get_next_token();
  main_loop();
  return 0;
}
