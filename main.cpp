#include <iostream>
#include <string>
#include <cstdio>
#include <ctype.h>
#include <cstdlib>
#include <vector>
#include <stack>
#include <algorithm>

using namespace std;

enum type_of_lex {
  LEX_NULL, /* 0*/
  LEX_AND,   /* 1*/
  LEX_BOOL, /* 2*/
  LEX_DO,     /* 3*/
  LEX_ELSE,  /* 4*/
  LEX_IF,      /* 5*/
  LEX_FALSE, /* 6*/
  LEX_INT,     /* 7*/
  LEX_NOT, /* 8 */
  LEX_OR, /* 9 */
  LEX_PROGRAM, /* 10 */
  LEX_READ, /* 11 */
  LEX_THEN, /* 12 */
  LEX_TRUE, /* 13 */
  LEX_VAR, /* 14 */
  LEX_WHILE, /* 15 */
  LEX_WRITE, /* 16 */
  LEX_GOTO,  /*17*/
  LEX_STRING, /* 18 */

  LEX_FIN,     /*19*/
  LEX_SEMICOLON, /*20*/
  LEX_COMMA, /*21*/
  LEX_COLON, /*22*/
  LEX_ASSIGN, /*23*/
  LEX_LPAREN, /*24*/
  LEX_RPAREN, /*25*/
  LEX_EQ,    /*26*/
  LEX_LSS,   /*27*/
  LEX_GTR, /*28*/
  LEX_PLUS, /*29*/
  LEX_MINUS, /*30*/
  LEX_TIMES, /*31*/
  LEX_SLASH, /*32*/
  LEX_LEQ, /*33*/
  LEX_NEQ, /*34*/
  LEX_GEQ,  /*35*/
  LEX_BEGIN,/*36*/
  LEX_END,  /*37*/

  LEX_NUM,  /*38*/
  LEX_CONST_STRING,    /*39*/
  LEX_ID,              /*40*/
  LEX_LABEL,           /*41*/
  POLIZ_LABEL,         /*42*/
  POLIZ_ADDRESS,       /*43*/
  POLIZ_GO,            /*44*/
  POLIZ_FGO,           /*45*/
  LEX_MINUS_UNAR,      // 46
  LEX_PLUS_UNAR        // 47
};

/////////////////////////  Класс Lex  //////////////////////////

class Lex {
  type_of_lex   t_lex;
  int           v_lex;
  string        s_lex;
public:
  Lex ( type_of_lex t = LEX_NULL, int v = 0, const string &s = ""): t_lex (t), v_lex (v)  { s_lex = s; }
  type_of_lex  get_type () const {
    return t_lex;
  }
  int get_value () const {
    return v_lex;
  }
  string get_value_string () const {
    return s_lex;
  }
  void put_value (int v) {
      v_lex = v;
  }
  friend ostream & operator<< ( ostream &s, Lex l );
};

/////////////////////  Класс Ident  ////////////////////////////

class Ident {
  string name;
  bool declare;
  type_of_lex id_type;
  bool assign;
  int value;
  string value_string;
public:
  Ident() {
    declare = false;
    assign = false;
  }
  bool operator== ( const string& s ) const {
    return name == s;
  }
  Ident ( const string n ) {
    name = n;
    declare = false;
    assign = false;
  }
  string get_name () const {
    return name;
  }
  bool get_declare () const {
    return declare;
  }
  void put_declare () {
    declare = true;
  }
  type_of_lex get_type () const {
    return id_type;
  }
  void put_type ( type_of_lex t ) {
    id_type = t;
  }
  bool get_assign () const {
    return assign;
  }
  void put_assign () {
    assign = true;
  }
  int  get_value () const {
    return value;
  }
  string  get_value_string () const {
    return value_string;
  }
  void put_value (int v) {
    value = v;
  }
  void put_value (const string & v) {
    value_string = v;
  }
  void asign_val(const Lex &curr_lex)
  {
     if (id_type == LEX_LABEL)
       throw "invalid label usage";
     type_of_lex vlex = curr_lex.get_type();
     if (id_type == LEX_INT) {
        if (vlex == LEX_CONST_STRING)
            throw curr_lex;
        put_value(curr_lex.get_value());
     } else if (id_type == LEX_STRING) {
        if (vlex==LEX_CONST_STRING){
            put_value(curr_lex.get_value_string());
        } else
           throw curr_lex;
     }
     put_assign();
  }

};

//////////////////////  TID  ///////////////////////

vector<Ident> TID;

int put ( const string & buf ) {
  vector<Ident>::iterator k;

  if ( (k = find(TID.begin(), TID.end(), buf)) != TID.end() )
    return int(k - TID.begin());
  TID.push_back ( Ident(buf) );
  return int(TID.size() - 1);
}

/////////////////////////////////////////////////////////////////

class Scanner {
  FILE * fp;
  char   c;
  string  buf;
  int look ( const string &b, const char ** list ) {
    int i = 0;
    while ( list[i] ) {
      if ( b == list[i] )
        return i;
      ++i;
    }
    return 0;
  }
  void gc() {
      int cc = c;
      cc = fgetc (fp);
      c = cc;
      if (cc == EOF) {
        c = '@';
      }
    //cout << c;
  }
      void clear()
    {
        buf = "";
    }
public:
  static const char * TW [], * TD [];
  Scanner ( const char * program ) {
    fp = fopen ( program, "r" );
    gc();
  }
  Lex get_lex ();

};

const char *
Scanner::TW    [] = { "", "and", "bool", "do", "else", "if", "false", "int", "not", "or", "program",
                      "read", "then", "true", "var", "while", "write", "goto", "string", NULL };

const char *
Scanner::TD    [] = { "@", ";", ",", ":", "=", "(", ")", "==", "<", ">", "+", "-", "*", "/", "<=", "!=", ">=",
                      "{", "}", NULL };

Lex Scanner::get_lex () {
    enum    state { H, IDENT, NUMB, COM, ALE, NEQ, SLASH1, STRING };
    int     d = 0, j;
    clear();
    state   CS = H;
    do {
      switch ( CS ) {
        case H:
          if ( c == ' ' || c == '\n' || c== '\r' || c == '\t' )
              gc();
          else if ( isalpha(c) ) {
              buf += c;
              CS = IDENT;
              gc();
          }
          else if ( isdigit(c) ) {
              d = c - '0';
              gc();
              CS = NUMB;
          }
          else if ( c== '/' ) {
                clear();
                buf += c;
                gc();
                CS = SLASH1;
          }
          else if ( c == ':' || c== '<' || c == '>' || c == '='  || c == '!' ) {
              buf += c;
              gc();
              CS = ALE;
          }
          else if (c == '@')
              return Lex(LEX_FIN);
          /*
          else if (c == '!') {
              buf += c;
              gc();
              CS = NEQ;
          }
          */
          else if (c == '"') {
              clear();
              gc();
              CS = STRING;
          }
          else {
            clear();
            buf += c;
            if ( (j = look ( buf, TD)) ){
              gc();
              return Lex ( (type_of_lex)(j+(int)LEX_FIN), j );
            }
            else {
              throw c;
            }
          }
          break;
        case IDENT:
          if ( isalpha(c) || isdigit(c) ) {
            buf += c;
            gc();
          }
          else {
            if ( (j = look (buf, TW)) ) {
              return Lex ((type_of_lex)j, j);
            }
            else {
              j = put ( buf );
              return Lex (LEX_ID, j);
            }
          }
          break;
        case SLASH1:
            if (c == '*'){
                gc();
                CS = COM;
                clear();
            }
            else  {
                j = look(buf,TD);
                return Lex(LEX_SLASH,j);
            }
            break;
        case NUMB:
          if ( isdigit(c) ) {
            d = d * 10 + (c - '0');
              gc();
          }
          else {
            //ungetc ( c, fp );
            return Lex ( LEX_NUM, d );
          }
          break;
        case COM:
            if (c == '@'){
                throw c;
            }
            else if (c == '*'){
                gc();
                if (c == '/'){
                    CS = H;
                    gc();
                }
            }
            else {
                gc();
            }
            break;
        case STRING:
            if (c == '\n'){
                throw 'n';
            }
            else if (c == '\\') {
                gc();
                buf += c;
                gc();
            }
            else if (c != '"') {
                buf += c;
                gc();
            } else {
                gc();
                CS = H;
                return Lex(LEX_CONST_STRING, 0, buf );
            }
            break;

        case ALE:
          if ( c== '=') {
            buf += c;
            gc();
            j = look ( buf, TD );
            return Lex ( (type_of_lex) ( j+(int)LEX_FIN ), j );
          }
          else {
            //ungetc ( c, fp );
            j = look ( buf, TD );
            return Lex ( (type_of_lex) (j+(int)LEX_FIN ), j );
          }
          break;
        case NEQ:
          if (c == '=') {
            buf += c;
            j = look ( buf, TD );
            return Lex ( LEX_NEQ, j );
          }
          else
            throw '!';
          break;
    } //end switch
  } while (true);
}

ostream & operator<< ( ostream &s, Lex l ) {
  string t;
  if ( l.t_lex < LEX_FIN )
    t = Scanner::TW[l.t_lex];
  else if (l.t_lex >= LEX_FIN && l.t_lex < LEX_NUM)
    t = Scanner::TD[l.t_lex-LEX_FIN];
  else if (l.t_lex == LEX_NUM)
    t = "NUMB";
  else if (l.t_lex == LEX_CONST_STRING) {
    t = "LEX_CONST_STRING \"";
    t += l.get_value_string();
    t += "\"";
  }
  else if (l.t_lex == LEX_ID) {
    t = "LEX_ID (";
    t += TID[l.v_lex].get_name();
    t += ")";
  } else if (l.t_lex == LEX_LABEL) {
    t = "Label";
  }
  else if (l.t_lex == POLIZ_LABEL)
    t = "P.Label";
  else if(l.t_lex == POLIZ_ADDRESS)
    t = "P.Addr";
  else if (l.t_lex == POLIZ_GO)
    t = "!";
  else if (l.t_lex == POLIZ_FGO)
    t = "!F";
  else if (l.t_lex == LEX_MINUS_UNAR)
    t = "LEX_MINUS_UNAR";
  else if (l.t_lex == LEX_PLUS_UNAR)
    t = "LEX_PLUS_UNAR";
  else
    throw l;
  s << '(' << t << ',' << l.v_lex << ");";
  return s;
}


template < class T, int max_size >
class Stack
{
    T            s [max_size];
    int          top;
public:
    Stack () { top = 0; }
    void         reset () { top = 0; }
    void         push ( T i );
    T            pop ();
    bool         is_empty () { return top == 0; }
    bool         is_full  () { return top == max_size; }
};

template < class T, int max_size >
void Stack < T, max_size > :: push (T i)
{
    if ( !is_full() )
        s [top++] = i;
    else
        throw "Stack_is_full";
}

template <class T, int max_size >
T Stack < T, max_size > :: pop ()
{
    if ( !is_empty() )
        return s[--top];
    else
        throw "stack_is_empty";
}

class Parser
{
    Lex          curr_lex; // current lex
    type_of_lex  c_type;   // its type
    int          c_val;    // its value
    Scanner      scan;     // lex analyzer
    bool         skip_semi;// allows to skip ; at the end of expression
    //Stack < int, 100 > st_int;
    Stack < type_of_lex, 100 >  st_lex;
    void PRG();  // Program
    void DCL();  // Variables
    void COP();  // Complex operator { }
    void TRM();  // term
    void SMC();  // ;
    void OPR();  // operator
    void ID();   // Ident
    void EXP();  // expression
    void CMP();  // ratio
    void OR();   // or expression
    void AND();  // and expression
    void PM();   // plus/minus expression
    void MD();   // mul / div  expression

    bool lex_compare(type_of_lex type);
    void         declarevar();  // declare onr variable
    bool         check_id();    // check if Ident declared or label
    void         check_op();    // check for compatibility of operands
    void         check_not();   // check operand for not
    void         check_unar(type_of_lex t); // check operand for unar - or +
    void         eq_type();     // equal types of operands for operation
    void         check_id_in_read(); // check ident in read operator
    void         gl()           // get next lexem
    {
        curr_lex = scan.get_lex();
        c_type = curr_lex.get_type();
        c_val = curr_lex.get_value();
        cout << c_type << ", " << curr_lex << endl;
    }

    type_of_lex var_type;       // type of variables in declaration
public:
    Parser (const char *program ) : scan (program) {
        skip_semi = false;
    }
    void         analyze();
};

void Parser::analyze ()
{
    gl();
    PRG();
    cout << endl << "Success!!!" << endl;
}

void Parser::PRG() // Main program
{
    if (c_type != LEX_PROGRAM)
        throw curr_lex;
    gl();
    if (c_type != LEX_BEGIN)
        throw curr_lex;
    gl();
    DCL();
    COP();
    if (c_type != LEX_FIN)
        throw curr_lex;
}

void Parser::SMC()  // LEX_semicolon check
{
    if (skip_semi) {
        skip_semi = false;
        return;
    }
    if (c_type != LEX_SEMICOLON)
        throw curr_lex;
    gl();
}

void Parser::DCL() // Variables
{
    while (c_type == LEX_INT || c_type == LEX_BOOL || c_type == LEX_STRING) {
        var_type = c_type;
        gl();
        declarevar();
        while (c_type == LEX_COMMA) {
            gl();
            declarevar();
        }
        if (c_type != LEX_SEMICOLON)
            throw curr_lex;
        gl();
    }
}

void Parser::COP() // {} Complex operator
{
    while (c_type != LEX_END) {
        OPR();
    }
    gl();
}

void Parser::OPR() // Operator
{
    if (c_type == LEX_IF) {
        gl();
        if (c_type != LEX_LPAREN)
            throw curr_lex;
        gl();
        EXP();
        if (c_type != LEX_RPAREN)
            throw curr_lex;
        gl();
        OPR();
        if (c_type == LEX_ELSE) {
            gl();
            OPR();
        } else {
        }
    }
    else if (c_type == LEX_WHILE) {
        gl();
        if (c_type != LEX_LPAREN)
            throw curr_lex;
        gl();
        EXP();
        if (c_type != LEX_RPAREN)
            throw curr_lex;
        gl();
        OPR();
    }
    else if (c_type == LEX_READ) {
        gl();
        if (c_type != LEX_LPAREN)
            throw curr_lex;
        gl();
        if (c_type != LEX_ID)
            throw curr_lex;
        check_id_in_read();
        gl();
        if ( c_type != LEX_RPAREN )
            throw curr_lex;
        gl();
        SMC();
    }
    else if (c_type == LEX_WRITE) {
        gl();
        if (c_type != LEX_LPAREN)
            throw curr_lex;
        gl();
        EXP();
        while (c_type == LEX_COMMA) {
            gl();
            EXP();
        }
        if (c_type != LEX_RPAREN)
            throw curr_lex;
        gl();
        SMC();
    }
    else if (c_type == LEX_BEGIN) {
        gl();
        COP();
    } else {
        EXP();
        SMC();
    }
}

void Parser::EXP() //Expression
{
    type_of_lex t2;
    OR();
    if ( c_type == LEX_ASSIGN){
        gl();
        EXP();
        t2 = st_lex.pop();
        st_lex.push(LEX_ASSIGN);
        st_lex.push(t2);
        check_op();
    }
}

void Parser::OR() // OR
{
    AND();
    while ( c_type == LEX_OR){
        st_lex.push(c_type);
        gl();
        AND();
        check_op();
    }
}

void Parser::AND() // And
{
    CMP();
    while ( c_type == LEX_AND){
        st_lex.push(c_type);
        gl();
        CMP();
        check_op();
    }
}

bool Parser::lex_compare(type_of_lex type)
{
    return type == LEX_EQ || type == LEX_LSS || type == LEX_GTR ||
           type == LEX_LEQ || type == LEX_GEQ || type == LEX_NEQ;
}

void Parser::CMP() // compare operations
{
    PM();
    if (lex_compare(c_type)) {
        st_lex.push(c_type);
        gl();
        PM();
        check_op();
    }
}

void Parser::PM() // plus or minus operations
{
    MD();
    while ( c_type == LEX_PLUS || c_type == LEX_MINUS){
        st_lex.push(c_type);
        gl();
        MD();
        check_op();
    }
}

void Parser::MD() // mul or div operations
{
    TRM();
    while ( c_type == LEX_TIMES || c_type == LEX_SLASH)  {
        st_lex.push(c_type);
        gl();
        TRM();
        check_op();
    }
}

void Parser::TRM()   // Term
{
    if ( c_type == LEX_MINUS ) {
        gl();
        TRM();
        check_unar(LEX_MINUS_UNAR);
    } else if ( c_type == LEX_PLUS ) {
        gl();
        TRM();
        check_unar(LEX_PLUS_UNAR);
    } else
    if ( c_type == LEX_NOT) {
        gl();
        TRM();
        check_not();
    }
    else if ( c_type == LEX_ID ) {
        if (check_id()) {
             gl();
        } else {
             int id_num = c_val;
             gl();
             if (c_type == LEX_COLON) {
                 TID[id_num].put_type(LEX_LABEL);
                 TID[id_num].put_declare();
                 gl();
                 skip_semi = true;
             } else {
                 throw "Ident not declared";
             }
        }
    }
    else if ( c_type == LEX_GOTO ) {
        gl();
        if ( c_type != LEX_ID )
            throw "goto needs label";
        type_of_lex t = TID[c_val].get_type();
        if (t == LEX_INT || t == LEX_STRING || t == LEX_BOOL)
            throw "can' use variable as a label";
        TID[c_val].put_type(LEX_LABEL);
        gl();
    }
    else if ( c_type == LEX_NUM ) {
        st_lex.push( LEX_INT );
        gl();
    } else if ( c_type == LEX_TRUE || c_type == LEX_FALSE) {
        st_lex.push( LEX_BOOL );
        gl();
    } else if ( c_type == LEX_CONST_STRING ) {
        st_lex.push( LEX_STRING );
        gl();
    } else if ( c_type == LEX_LPAREN ) {
        gl();
        EXP();
        if ( c_type != LEX_RPAREN)
            throw curr_lex;
        gl();
    }
    else
        throw curr_lex;
}

void Parser::declarevar() // variable
{
    int i = c_val;
    if ( TID[i].get_declare() )
            throw "twice";
    else {
        TID[i].put_declare();
        TID[i].put_type(var_type);
        gl();
        int z = 1;
        if (c_type == LEX_ASSIGN) {
            gl();
            if (var_type == LEX_INT) {
                if (c_type == LEX_MINUS) {
                    z =-1;
                    gl();
                }
                else if (c_type == LEX_PLUS) {
                    gl();
                }
            }
            if (z == -1) {
                curr_lex.put_value(-curr_lex.get_value());
            }
            TID[i].asign_val(curr_lex);
            gl();
        }
    }
}

bool Parser::check_id () // check if ID declared and push its type
{
    if ( TID[c_val].get_declare() ) {
        type_of_lex type = TID[c_val].get_type();
        if (type == LEX_LABEL) {
            throw "invalid usage of label";
        }
        st_lex.push (type);
        return true;
    }
    return false;
}

void Parser::check_op ()
{
    type_of_lex t1, t2, op, r = LEX_BOOL;
    t2 = st_lex.pop();
    op = st_lex.pop();
    t1 = st_lex.pop();
    if (t1 == LEX_STRING || t2 == LEX_STRING) {
        if (t1 != t2) {
            throw "two strings are needed for operation";
        }
        if (op == LEX_AND || op == LEX_OR || op == LEX_MINUS || op == LEX_TIMES || op == LEX_SLASH) {
            throw "% operation is incompatible with strings";
        }
        r = LEX_STRING;
    } else
    if (op == LEX_PLUS || op == LEX_MINUS || op == LEX_TIMES || op == LEX_SLASH)
        r = LEX_INT;
    if (op == LEX_ASSIGN)
        r = t1;
    else
    if (op == LEX_OR || op == LEX_AND || lex_compare(op)) {
          r = LEX_BOOL;
    }
    if (t1 != t2)
        throw "wrong types are in operation";
    st_lex.push(r);
}

void Parser::check_not()
{
    if (st_lex.pop() != LEX_BOOL)
        throw "not: wrong type";
    st_lex.push (LEX_BOOL);
}

void Parser::check_unar(type_of_lex t)
{
    if (st_lex.pop() != LEX_INT)
        throw "wrong type for unar operation";
    st_lex.push (LEX_INT);
}

void Parser::eq_type ()
{
    if (st_lex.pop() != st_lex.pop())
        throw "wrong types are in :=";
}

void Parser::check_id_in_read()
{
    if ( !TID [c_val].get_declare() )
        throw "not declared";
}

int main()
{
    Parser pr("prog.txt");
    try {
        pr.analyze();
    }
    catch (char c) {
        cout << "error found, char=" << c << " ; code =" << (int) c << endl;
    }
    catch (const char *s) {
        cout << "error found, s=" << s << endl;
    }
    catch (const Lex &l) {
            cout << "error found, unexpected lexem = " << l << endl;
    }
}

