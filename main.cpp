#include <iostream>
#include <string>
#include <cstdio>
#include <ctype.h>
#include <vector>
#include <stack>
#include <algorithm>
#include <cstring>

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
    type_of_lex t_lex;
    int v_lex;
    string s_lex;
public:
    Lex ( type_of_lex t = LEX_NULL, int v = 0, const string &s = "" ) : t_lex ( t ), v_lex ( v ) { s_lex = s; }
    Lex ( int v) : t_lex ( LEX_NUM ), v_lex ( v ), s_lex("") { }

    type_of_lex get_type () const {
        return t_lex;
    }

    void put_type (type_of_lex t) {
        t_lex = t;
    }

    int get_value () const {
        return v_lex;
    }

    string get_value_string () const {
        return s_lex;
    }

    string as_string() const
    {
        char str[20];
        if (t_lex == LEX_STRING) {
            return s_lex;
        }
        if (t_lex == LEX_BOOL) {
            if (v_lex == 0)
                return "false";
            else
                return "true";
        }
        snprintf(str,20,"%d",v_lex);
        return str;
    }



    void put_value ( int v ) {
        v_lex = v;
    }

    friend ostream &operator<< ( ostream &s, Lex l );
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
    Ident () {
        declare = false;
        assign = false;
    }

    bool operator== ( const string &s ) const {
        return name == s;
    }

    Ident ( const string n ) {
        name = n;
        declare = false;
        assign = false;
        id_type = LEX_NULL;
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

    bool get_assign()
    {
        return assign;
    }

    void put_assign () {
        assign = true;
    }

    void put_value ( int v ) {
        value = v;
        put_assign ();
    }

    int get_value() {
        return value;
    }

    void put_value ( const string &v ) {
        value_string = v;
        put_assign ();
    }

    Lex to_lex() {
        if (id_type ==  LEX_STRING)
            return Lex(LEX_STRING, 0, value_string);
        return Lex(value);
    }

    void assign_val (const Lex &curr_lex ) {
        if ( id_type == LEX_LABEL )
            throw "invalid label usage";
        type_of_lex vlex = curr_lex.get_type ();
        if ( id_type == LEX_NUM || id_type == LEX_BOOL) {
            if ( vlex == LEX_STRING )
                throw curr_lex;
            put_value ( curr_lex.get_value ());
        } else if ( id_type == LEX_STRING ) {
            if ( vlex == LEX_STRING || vlex == LEX_CONST_STRING) {
                put_value ( curr_lex.get_value_string ());
            } else
                throw curr_lex;
        }
        put_assign ();
    }

    vector<int> label_usage;
};

//////////////////////  TID  ///////////////////////

vector<Ident> TID;

int put ( const string &buf ) {
    vector<Ident>::iterator k;

    if ((k = find ( TID.begin (), TID.end (), buf )) != TID.end ())
        return int ( k - TID.begin ());
    TID.push_back ( Ident ( buf ));
    return int ( TID.size () - 1 );
}

/////////////////////////////////////////////////////////////////

class Scanner {
    FILE *fp;
    char c;
    string buf;

    static int look ( const string &b, const char **list ) {
        int i = 0;
        while ( list[i] ) {
            if ( b == list[i] )
                return i;
            ++i;
        }
        return 0;
    }

    void gc () {
        int cc;
        cc = fgetc ( fp );
        c = cc;
        if ( cc == EOF) {
            c = '@';
        }
    }

    void clear () {
        buf = "";
    }

public:
    static const char *TW[], *TD[];

    Scanner ( const char *program ) {
        fp = fopen ( program, "r" );
        gc ();
    }

    Lex get_lex ();

};

const char *
        Scanner::TW[] = {"", "and", "bool", "do", "else", "if", "false", "int", "not", "or", "program",
                         "read", "then", "true", "var", "while", "write", "goto", "string", NULL};

const char *
        Scanner::TD[] = {"@", ";", ",", ":", "=", "(", ")", "==", "<", ">", "+", "-", "*", "/", "<=", "!=", ">=",
                         "{", "}", NULL};

Lex Scanner::get_lex () {
    enum state {
        H, IDENT, NUMB, COM, ALE, NEQ, SLASH1, STRING
    };
    int d = 0, j;
    clear ();
    state CS = H;
    do {
        switch ( CS ) {
            case H:
                if ( c == ' ' || c == '\n' || c == '\r' || c == '\t' )
                    gc ();
                else if ( isalpha ( c )) {
                    buf += c;
                    CS = IDENT;
                    gc ();
                } else if ( isdigit ( c )) {
                    d = c - '0';
                    gc ();
                    CS = NUMB;
                } else if ( c == '/' ) {
                    clear ();
                    buf += c;
                    gc ();
                    CS = SLASH1;
                } else if ( c == ':' || c == '<' || c == '>' || c == '=' || c == '!' ) {
                    buf += c;
                    gc ();
                    CS = ALE;
                } else if ( c == '@' )
                    return Lex ( LEX_FIN );
                    /*
                    else if (c == '!') {
                        buf += c;
                        gc();
                        CS = NEQ;
                    }
                    */
                else if ( c == '"' ) {
                    clear ();
                    gc ();
                    CS = STRING;
                } else {
                    clear ();
                    buf += c;
                    if ((j = look ( buf, TD ))) {
                        gc ();
                        return Lex (( type_of_lex ) (j + ( int ) LEX_FIN), j );
                    } else {
                        throw c;
                    }
                }
                break;
            case IDENT:
                if ( isalpha ( c ) || isdigit ( c )) {
                    buf += c;
                    gc ();
                } else {
                    if ((j = look ( buf, TW ))) {
                        if (( type_of_lex ) j == LEX_TRUE)
                            return Lex (LEX_BOOL, 1 );
                        if (( type_of_lex ) j == LEX_FALSE)
                            return Lex (LEX_BOOL, 0 );
                        return Lex (( type_of_lex ) j, j );
                    } else {
                        j = put ( buf );
                        return Lex ( LEX_ID, j );
                    }
                }
                break;
            case SLASH1:
                if ( c == '*' ) {
                    gc ();
                    CS = COM;
                    clear ();
                } else {
                    j = look ( buf, TD );
                    return Lex ( LEX_SLASH, j );
                }
                break;
            case NUMB:
                if ( isdigit ( c )) {
                    d = d * 10 + (c - '0');
                    gc ();
                } else {
                    return Lex ( LEX_NUM, d );
                }
                break;
            case COM:
                if ( c == '@' ) {
                    throw c;
                } else if ( c == '*' ) {
                    gc ();
                    if ( c == '/' ) {
                        CS = H;
                        gc ();
                    }
                } else {
                    gc ();
                }
                break;
            case STRING:
                if ( c == '\n' ) {
                    throw 'n';
                } else if ( c == '\\' ) {
                    gc ();
                    buf += c;
                    gc ();
                } else if ( c != '"' ) {
                    buf += c;
                    gc ();
                } else {
                    gc ();
                    return Lex ( LEX_CONST_STRING, 0, buf );
                }
                break;

            case ALE:
                if ( c == '=' ) {
                    buf += c;
                    gc ();
                    j = look ( buf, TD );
                    return Lex (( type_of_lex ) (j + ( int ) LEX_FIN), j );
                } else {
                    j = look ( buf, TD );
                    return Lex (( type_of_lex ) (j + ( int ) LEX_FIN), j );
                }
                break;

            case NEQ:
                if ( c == '=' ) {
                    buf += c;
                    j = look ( buf, TD );
                    return Lex ( LEX_NEQ, j );
                } else
                    throw '!';
                break;
        } //end switch
    } while ( true );
}

ostream &operator<< ( ostream &s, Lex l ) {
    string t;
    if ( l.t_lex < LEX_FIN )
        t = Scanner::TW[l.t_lex];
    else if ( l.t_lex >= LEX_FIN && l.t_lex < LEX_NUM )
        t = Scanner::TD[l.t_lex - LEX_FIN];
    else if ( l.t_lex == LEX_NUM )
        t = "NUMB";
    else if ( l.t_lex == LEX_CONST_STRING ) {
        t = "LEX_CONST_STRING \"";
        t += l.get_value_string ();
        t += "\"";
    } else if ( l.t_lex == LEX_ID ) {
        t = "LEX_ID (";
        t += TID[l.v_lex].get_name ();
        t += ")";
    } else if ( l.t_lex == LEX_LABEL ) {
        t = "Label";
    } else if ( l.t_lex == POLIZ_LABEL )
        t = "P.Label";
    else if ( l.t_lex == POLIZ_ADDRESS )
        t = "P.Addr";
    else if ( l.t_lex == POLIZ_GO )
        t = "!";
    else if ( l.t_lex == POLIZ_FGO )
        t = "!F";
    else if ( l.t_lex == LEX_MINUS_UNAR )
        t = "LEX_MINUS_UNAR";
    else if ( l.t_lex == LEX_PLUS_UNAR )
        t = "LEX_PLUS_UNAR";
    else
        throw l;
    s << '(' << t << ',' << l.v_lex << ");";
    return s;
}


template<class T, int max_size>
class Stack {
    T s[max_size];
    int top;
public:
    Stack () { top = 0; }

    void reset () { top = 0; }
    void push ( T i );

    T pop ();

    bool is_empty () { return top == 0; }

    bool is_full () { return top == max_size; }
};

template<class T, int max_size>
void Stack<T, max_size>::push ( T i ) {
    if ( !is_full ())
        s[top++] = i;
    else
        throw "Stack_is_full";
}

template<class T, int max_size>
T Stack<T, max_size>::pop () {
    if ( !is_empty ())
        return s[--top];
    else
        throw "stack_is_empty";
}

class Poliz {
    Lex *p;
    int size;
    int free;
public:
    Poliz ( int max_size ) {
        p = new Lex[size = max_size];
        free = 0;
    }

    ~Poliz () { delete[] p; }

    void put_lex ( Lex l ) {
        if (l.get_type() == LEX_CONST_STRING)
            l.put_type(LEX_STRING);
        p[free] = l;
        free++;
    }

    void put_lex ( Lex l, int place ) { p[place] = l; }

    void blank () { free++; }

    int get_free () { return free; }

    Lex &operator[] ( int index ) {
        if ( index > size )
            throw "POLIZ:out of array";
        else if ( index > free )
            throw "POLIZ:indefinite element of array";
        else
            return p[index];
    }
    void update_ident();

    void print ();
};

void Poliz::print () {
    for ( int i = 0; i < free; ++i ) {
        cout << i << " := " << p[i] << endl;
    }
}

void Poliz::update_ident() {
    if (p[free - 1].get_type() != LEX_ID)
        throw "Identifier expected for = operator";
    p[free - 1].put_type(POLIZ_ADDRESS);
}
class Parser {
    Lex curr_lex; // current lex
    type_of_lex c_type;   // its type
    int c_val;    // its value
    Scanner scan;     // lex analyzer
    bool skip_semi;// allows to skip ; at the end of expression
    Stack<type_of_lex, 100> st_lex;

    void PRG ();  // Program
    void DCL ();  // Variables
    void COP ();  // Complex operator { }
    void TRM ();  // term
    void SMC ();  // ;
    void OPR ();  // operator
    void ID ();   // Ident
    void EXP ();  // expression
    void CMP ();  // ratio
    void OR ();   // or expression
    void AND ();  // and expression
    void PM ();   // plus/minus expression
    void MD ();   // mul / div  expression

    static bool lex_compare ( type_of_lex type );

    void declarevar ();  // declare onr variable
    bool check_id ();    // check if Ident declared or label
    void check_op ();    // check for compatibility of operands
    void check_not ();   // check operand for not
    void check_unar ( type_of_lex t ); // check operand for unar - or +
    // equal types of operands for operation
    void check_id_in_read (); // check ident in read operator
    void gl ()           // get next lexem
    {
        curr_lex = scan.get_lex ();
        c_type = curr_lex.get_type ();
        c_val = curr_lex.get_value ();
        //cout << c_type << ", " << curr_lex << endl;
    }

    type_of_lex var_type;       // type of variables in declaration
public:
    Poliz prog;

    Parser ( const char *program ) : scan ( program ), prog ( 1000 ) {
        skip_semi = false;
    }

    void analyze ();
};

void Parser::analyze () {
    gl ();
    PRG ();
    //prog.print ();
    cout << endl << "Parsrer: Yes!!!" << endl;
}

void Parser::PRG () // Main program
{
    if ( c_type != LEX_PROGRAM )
        throw curr_lex;
    gl ();
    if ( c_type != LEX_BEGIN )
        throw curr_lex;
    gl ();
    DCL ();
    COP ();
    if ( c_type != LEX_FIN )
        throw curr_lex;
}

void Parser::SMC ()  // LEX_semicolon check
{
    if ( skip_semi ) {
        skip_semi = false;
        return;
    }
    if ( c_type != LEX_SEMICOLON )
        throw curr_lex;
    gl ();
    prog.put_lex ( Lex ( LEX_SEMICOLON ));
}

void Parser::DCL () // Variables
{
    while ( c_type == LEX_INT || c_type == LEX_BOOL || c_type == LEX_STRING ) {
        var_type = c_type;
        if (var_type == LEX_INT) {
            var_type = LEX_NUM;
        }
        gl ();
        declarevar ();
        while ( c_type == LEX_COMMA ) {
            gl ();
            declarevar ();
        }
        if ( c_type != LEX_SEMICOLON )
            throw curr_lex;
        gl ();
    }
}

void Parser::COP () // {} Complex operator
{
    while ( c_type != LEX_END ) {
        OPR ();
    }
    gl ();
}

void Parser::OPR () // Operator
{
    int pl0, pl1, pl2, pl3;
    if ( c_type == LEX_IF ) {
        gl ();
        if ( c_type != LEX_LPAREN )
            throw curr_lex;
        gl ();
        EXP ();
        if ( c_type != LEX_RPAREN )
            throw curr_lex;
        gl ();
        pl2 = prog.get_free ();  // address of else-jump address
        prog.blank ();          // get cell for  address
        prog.put_lex ( Lex ( POLIZ_FGO )); // !F command
        OPR ();
        if ( c_type == LEX_ELSE ) {
            pl3 = prog.get_free (); // address of address to skip ELSE-part
            prog.blank ();          // get cell for  address
            prog.put_lex ( Lex ( POLIZ_GO )); // ! command
            prog.put_lex ( Lex ( POLIZ_LABEL, prog.get_free ()), pl2 ); // put address of else to pl2 cell
            gl ();
            OPR ();
            prog.put_lex ( Lex ( POLIZ_LABEL, prog.get_free ()), pl3 ); // put address to pl3 cell
        } else {
            prog.put_lex ( Lex ( POLIZ_LABEL, prog.get_free ()), pl2 ); // put address to pl2 cell
        }
    } else if ( c_type == LEX_WHILE ) {
        pl0 = prog.get_free ();      // store WHILE start
        gl ();
        if ( c_type != LEX_LPAREN )
            throw curr_lex;
        gl ();
        EXP ();
        if ( c_type != LEX_RPAREN )
            throw curr_lex;
        pl1 = prog.get_free ();           // address of cycle skip
        prog.blank ();                  // get cell for  address
        prog.put_lex ( Lex ( POLIZ_FGO )); // !F command
        gl ();
        OPR ();
        prog.put_lex ( Lex ( POLIZ_LABEL, pl0 )); // while start
        prog.put_lex ( Lex ( POLIZ_GO ));        // !
        prog.put_lex ( Lex ( POLIZ_LABEL, prog.get_free ()), pl1 );  // put address to skip cycle to pl1 cell
    } else if ( c_type == LEX_READ ) {
        gl ();
        if ( c_type != LEX_LPAREN )
            throw curr_lex;
        gl ();
        if ( c_type != LEX_ID )
            throw curr_lex;
        check_id_in_read ();
        prog.put_lex ( Lex ( POLIZ_ADDRESS, c_val ));
        gl ();
        if ( c_type != LEX_RPAREN )
            throw curr_lex;
        gl ();
        prog.put_lex ( Lex ( LEX_READ ));
        SMC ();
    } else if ( c_type == LEX_WRITE ) {
        gl ();
        if ( c_type != LEX_LPAREN )
            throw curr_lex;
        gl ();
        EXP ();
        while ( c_type == LEX_COMMA ) {
            gl ();
            EXP ();
        }
        if ( c_type != LEX_RPAREN )
            throw curr_lex;
        gl ();
        prog.put_lex ( Lex ( LEX_WRITE ));
        SMC ();
    } else if ( c_type == LEX_BEGIN ) {
        gl ();
        COP ();
    } else {
        EXP ();
        SMC ();
    }
}

void Parser::EXP () //Expression
{
    type_of_lex t2;
    if (c_type == LEX_SEMICOLON || c_type == LEX_END)
        return ;
    OR ();
    if ( c_type == LEX_ASSIGN ) {
        prog.update_ident();
        gl ();
        EXP ();
        t2 = st_lex.pop ();
        st_lex.push ( LEX_ASSIGN );
        st_lex.push ( t2 );
        check_op ();
    }
}

void Parser::OR () // OR
{
    AND ();
    while ( c_type == LEX_OR ) {
        st_lex.push ( c_type );
        gl ();
        AND ();
        check_op ();
    }
}

void Parser::AND () // And
{
    CMP ();
    while ( c_type == LEX_AND ) {
        st_lex.push ( c_type );
        gl ();
        CMP ();
        check_op ();
    }
}

bool Parser::lex_compare ( type_of_lex type ) {
    return type == LEX_EQ || type == LEX_LSS || type == LEX_GTR ||
           type == LEX_LEQ || type == LEX_GEQ || type == LEX_NEQ;
}

void Parser::CMP () // compare operations
{
    PM ();
    if ( lex_compare ( c_type )) {
        st_lex.push ( c_type );
        gl ();
        PM ();
        check_op ();
    }
}

void Parser::PM () // plus or minus operations
{
    MD ();
    while ( c_type == LEX_PLUS || c_type == LEX_MINUS ) {
        st_lex.push ( c_type );
        gl ();
        MD ();
        check_op ();
    }
}

void Parser::MD () // mul or div operations
{
    TRM ();
    while ( c_type == LEX_TIMES || c_type == LEX_SLASH ) {
        st_lex.push ( c_type );
        gl ();
        TRM ();
        check_op ();
    }
}

void Parser::TRM ()   // Term
{
    int pl;
    if ( c_type == LEX_MINUS ) {
        gl ();
        TRM ();
        check_unar ( LEX_MINUS_UNAR );
    } else if ( c_type == LEX_PLUS ) {
        gl ();
        TRM ();
        check_unar ( LEX_PLUS_UNAR );
    } else if ( c_type == LEX_NOT ) {
        gl ();
        TRM ();
        check_not ();
    } else if ( c_type == LEX_ID ) {
        if ( check_id ()) {
            prog.put_lex ( Lex ( LEX_ID, c_val ));
            gl ();
        } else {
            int id_num = c_val;
            gl ();
            if ( c_type == LEX_COLON ) {
                if (TID[id_num].get_type() != LEX_NULL)
                    throw "label redeclared";
                TID[id_num].put_declare ();
                TID[id_num].put_type(LEX_LABEL);
                TID[id_num].put_value(prog.get_free());
                if (TID[id_num].label_usage.size() > 0) {
                    for (unsigned i = 0; i < TID[id_num].label_usage.size(); i++) {
                        prog.put_lex(Lex(POLIZ_LABEL, prog.get_free()), TID[id_num].label_usage[i]);
                    }
                    TID[id_num].label_usage.clear();
                }

                gl ();
                skip_semi = true;
            } else {
                throw "Ident not declared";
            }
        }
    } else if ( c_type == LEX_GOTO ) {
        gl ();
        if ( c_type != LEX_ID )
            throw "goto needs label";
        type_of_lex t = TID[c_val].get_type ();
        if (t == LEX_LABEL) {
            prog.put_lex(Lex(POLIZ_LABEL, TID[c_val].get_value()));
        } else if (t == LEX_NULL) {
            pl = prog.get_free ();
            TID[c_val].label_usage.push_back(pl);
            prog.blank ();
        } else
            throw "can' use variable as a label";
        prog.put_lex ( Lex ( POLIZ_GO )); // ! command
        gl ();
    } else if ( c_type == LEX_NUM ) {
        st_lex.push ( LEX_NUM );
        prog.put_lex ( curr_lex );
        gl ();
    } else if ( c_type == LEX_TRUE || c_type == LEX_FALSE ) {
        st_lex.push ( LEX_BOOL );
        prog.put_lex ( curr_lex );
        gl ();
    } else if ( c_type == LEX_CONST_STRING ) {
        st_lex.push ( LEX_STRING );
        prog.put_lex ( curr_lex );
        gl ();
    } else if ( c_type == LEX_LPAREN ) {
        gl ();
        EXP ();
        if ( c_type != LEX_RPAREN )
            throw curr_lex;
        gl ();
    } else
        throw curr_lex;
}

void Parser::declarevar () // variable
{
    int i = c_val;
    if ( TID[i].get_declare ())
        throw "twice";
    else {
        TID[i].put_declare ();
        TID[i].put_type ( var_type );
        gl ();
        int z = 1;
        if ( c_type == LEX_ASSIGN ) {
            gl ();
            if ( var_type == LEX_NUM ) {
                if ( c_type == LEX_MINUS ) {
                    z = -1;
                    gl ();
                } else if ( c_type == LEX_PLUS ) {
                    gl ();
                }
            }
            if ( z == -1 ) {
                curr_lex.put_value ( -curr_lex.get_value ());
            }
            TID[i].assign_val(curr_lex);
            gl ();
        }
    }
}

bool Parser::check_id () // check if ID declared and push its type
{
    if ( TID[c_val].get_declare ()) {
        type_of_lex type = TID[c_val].get_type ();
        if ( type == LEX_LABEL ) {
            throw "invalid usage of label";
        }
        st_lex.push ( type );
        return true;
    }
    return false;
}

void Parser::check_op () {
    type_of_lex t1, t2, op, r = LEX_BOOL;
    t2 = st_lex.pop ();
    op = st_lex.pop ();
    t1 = st_lex.pop ();
    if ( t1 == LEX_STRING || t2 == LEX_STRING ) {
        if ( t1 != t2 ) {
            throw "two strings are needed for operation";
        }
        if ( op == LEX_AND || op == LEX_OR || op == LEX_MINUS || op == LEX_TIMES || op == LEX_SLASH ) {
            throw "% operation is incompatible with strings";
        }
        r = LEX_STRING;
    } else if ( op == LEX_PLUS || op == LEX_MINUS || op == LEX_TIMES || op == LEX_SLASH )
        r = LEX_NUM;
    if ( op == LEX_ASSIGN )
        r = t1;
    else if ( op == LEX_OR || op == LEX_AND || lex_compare ( op )) {
        r = LEX_BOOL;
    }
    st_lex.push ( r );
    prog.put_lex ( Lex ( op ));
}

void Parser::check_not () {
    if ( st_lex.pop () != LEX_BOOL )
        throw "not: wrong type";
    st_lex.push ( LEX_BOOL );
    prog.put_lex ( Lex ( LEX_NOT ));
}

void Parser::check_unar ( type_of_lex t ) {
    if ( st_lex.pop () != LEX_NUM )
        throw "wrong type for unar operation";
    st_lex.push ( LEX_NUM );
    prog.put_lex ( Lex ( t ));
}

void Parser::check_id_in_read () {
    if ( !TID[c_val].get_declare ())
        throw "not declared";
}



class Executer {
    Lex pc_el;

public:
    void execute(Poliz& prog);
};

void write(Stack<Lex, 100> &args)
{
    if (!args.is_empty()) {
        string s = args.pop().as_string();
        write(args);
        cout << s << endl;
    }
}

bool take_strings(Stack<Lex, 100> &args, string &s1, string &s2, int &i1, int &i2)
{
    Lex lx;
    lx = args.pop();
    if (lx.get_type() == LEX_STRING) {
        s2 = lx.as_string();
        s1 = args.pop().as_string();
        return true;
    } else {
        i2 = lx.get_value();
        i1 = args.pop().get_value();
        return false;
    }
}

void take_ints(Stack<Lex, 100> &args, int &i1, int &i2)
{
    i2 = args.pop().get_value();
    i1 = args.pop().get_value();
}

void Executer::execute(Poliz& prog)
{
    Stack<Lex, 100> args;
    int i1, i2, index = 0, size = prog.get_free();
    string s1, s2;
    Lex lx;
    cout << "Start of executing!!!" << endl;
    while (index < size) {
        pc_el = prog[index];
        //if (index == 64)
        //  cout << index << ": " << pc_el << endl;
        switch (pc_el.get_type()) {
        case LEX_TRUE:
            args.push(1);
            break;
        case LEX_FALSE:
            args.push(0);
            break;
        case LEX_STRING:
        case LEX_NUM:
        case POLIZ_ADDRESS:
        case POLIZ_LABEL:
             //cout << index << ":" << "pushing value:" << pc_el.as_string() << endl;
            args.push(pc_el);
            break;
        case LEX_ID:
            int ind;
            ind = pc_el.get_value();
            if (TID[ind].get_assign()) {
                //cout << index << ":"  << "pushing ID: " << TID[ind].to_lex().as_string() << endl;

                args.push(TID[ind].to_lex());
                break;
            }
            else {
                cout << TID[ind].get_name() << endl;
                throw "POLIZ: indefinite identifier";
            }
        case LEX_MINUS_UNAR:
            args.push(Lex(-args.pop().get_value()));
            break;

        case LEX_NOT:
            args.push(Lex(LEX_NUM,!(args.pop().get_value())));
            break;
        case LEX_OR:
            take_ints(args, i1, i2);
            args.push(Lex(i1 || i2));
            break;
        case LEX_AND:
            take_ints(args, i1, i2);
            args.push(i1 && i2);
            break;
        case POLIZ_GO:
            index = args.pop().get_value() - 1;
            break;
        case POLIZ_FGO:
            take_ints(args, i1, i2);
            if (!i1)
                index = i2 - 1;
            break;
        case LEX_WRITE:
            write(args);
            break;
        case LEX_READ: {
            int k;
            i1 = args.pop().get_value();
            if (TID[i1].get_type() == LEX_NUM) {
                cout << "Input int value for";
                cout << TID[i1].get_name() << endl;
                cin >> k;
            }
            else {
                char j[20];
            rep:
                cout << "Input boolean value";
                cout << "(true or false) for ";
                cout << TID[i1].get_name() << endl;
                cin >> j;
                if (!strcmp(j, "true"))
                    k = 1;
                else if (!strcmp(j, "false"))
                    k = 0;
                else {
                    cout << "Error in input:true/false";
                    cout << endl;
                    goto rep;
                }
            }
            TID[i1].put_value(k);
            break;
        }
        case LEX_PLUS_UNAR:
            break;
        case LEX_PLUS:
            if (take_strings(args, s1, s2, i1, i2)) {
                args.push(Lex(LEX_STRING,0, s1 + s2));
            } else {
                args.push(i1 + i2);
            }
            break;
        case LEX_TIMES:
            take_ints(args, i1, i2);
            args.push(i1 * i2);
            break;
        case LEX_MINUS:
            take_ints(args, i1, i2);
            args.push(i1 - i2);
            break;
        case LEX_SLASH:
            take_ints(args, i1, i2);
            if (i2 == 0)
                throw "POLIZ:divide by zero";
            args.push(i1 / i2);
            break;
        case LEX_EQ:
            if (take_strings(args, s1, s2, i1, i2)) {
                args.push(s1 == s2);
            } else {
                args.push(i1 == i2);
            }
            break;
        case LEX_LSS:
            if (take_strings(args, s1, s2, i1, i2)) {
                args.push(s1 < s2);
            } else {
                args.push(i1 < i2);
            }
            break;
        case LEX_GTR:
            if (take_strings(args, s1, s2, i1, i2)) {
                args.push(s1 > s2);
            } else {
                args.push(i1 > i2);
            }
            break;
        case LEX_LEQ:
            take_ints(args, i1, i2);
            args.push ( i1 <= i2);
            break;
        case LEX_GEQ:
            take_ints(args, i1, i2);
            args.push ( i1 >= i2);
            break;
        case LEX_NEQ:
            if (take_strings(args, s1, s2, i1, i2)) {
                args.push(s1 != s2);
            } else {
                args.push(i1 != i2);
            }
        break;
        case LEX_SEMICOLON:
            args.reset();
            break;
        case LEX_ASSIGN:
            lx = args.pop();
            i2 = args.pop().get_value();
//            cout << index << ":"  << TID[i2].get_name() <<"="<< lx.as_string() << ";" << endl;
            TID[i2].assign_val(lx);
            args.push(TID[i2].to_lex());
            break;
        default:
            cout << pc_el << endl;
            throw "POLIZ: unexpected elem\n";
        }
        // end of switch
        ++index;
    };
    //end of while
    cout << "Finish of executing!!!" << endl;
}
class Interpretator {
    Parser pars;
    Executer E;

public:
    Interpretator(const char* program)
        : pars(program){};
    void interpretation();
};
void Interpretator::interpretation()
{
    pars.analyze();
    E.execute(pars.prog);
}

int main()
{
    try {
        Interpretator I("prog.txt");
        I.interpretation();
    }
    catch (char c) {
        cout << "ERROR (char): '" << c << "'";
        cout << "; char code: " << (unsigned) (unsigned char)c << endl;
    }
    catch (const char *s) {
        cout << "ERROR (char*): " << s << endl;
    }
    catch (const Lex&lx) {
        cout << "*" << "unexpected lexem"<< "*"  << endl;
        cout << lx << endl;
    }
    return 0;
}
