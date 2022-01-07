#include <cstdio>
#include <iostream>
#include <vector>
using namespace std;
int get_token();

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
virtual void codegen(){
}
};

class NumberExprAST:public ExprAST{
public:
  double val;
  NumberExprAST(double val):val(val){}
  void codegen(){
    printf("[number ast %lf]", val);
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
  ExprAST lhs;
  ExprAST rhs;
  BinExprAST(char op, ExprAST& lhs, ExprAST& rhs)
    :op(op), lhs(lhs), rhs(rhs){}
  void codegen(){
    cout << "[binary ast ";
    lhs.codegen();
    cout << " " << op << " ";
    rhs.codegen();
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


ExprAST parsePrimaryAST(){
  switch(token){
    case INDENTFIER_TOKEN:
      return parserIndentifierAST();
    case NUM_TOKEN:
      return parseNumIndentifierAST();
    case 
  }
}

ExprAST parseExpressionAST(){
  ExprAST lhs = parsePrimaryAST();
  ExprAST expression_ast = parseBinaryAST(0, lhs);
  return expression_ast;
}

ExprAST parseTopLevel(){
  ExprAST express_ast = parseExpressionAST();
  FunctionAST func_ast = parseFuncAST("none-name", {}, express_ast);
  return func_ast;
}

void main_loop(){
  while(1){
    switch(CurToken){
      case INDENTFIER_TOKEN:
        VariableExprAST(IntentifierStr).codegen();
        break;
      case DEF_TOKEN:
        std::cout << "DEF: " << endl;
        break;
      case EXTERN_TOKEN:
        std::cout <<  "EXTERN: " <<endl;
        break;
      case NUM_TOKEN:
        NumberExprAST(Num).codegen();
        break;
      default:
        printf("%c\n", CurToken);
        break;
    }
    fprintf(stderr, "%s\n", "please input code");
    get_next_token();
  }
}

int main() {
  fprintf(stderr, "%s\n", "please input code");
  get_next_token();
  main_loop();
  return 0;
}
