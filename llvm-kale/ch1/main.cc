#include <cstdio>
#include <iostream>
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


void main_loop(){
  while(1){
    switch(CurToken){
      case INDENTFIER_TOKEN:
        std::cout << "INDENTFIER: " << IntentifierStr << endl;
        break;
      case DEF_TOKEN:
        std::cout << "DEF: " << endl;
        break;
      case EXTERN_TOKEN:
        std::cout <<  "EXTERN: " <<endl;
        break;
      case NUM_TOKEN:
        std::cout << Num <<endl;
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
