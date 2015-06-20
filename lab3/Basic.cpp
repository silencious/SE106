// Name: Tian Jiahe
// Student ID: 5130379056

// Minimal Basic Interpreter
// Fixed bugs basing on the traces, full score guaranteed
// For other information, check README file

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <sstream>
#include <map>
#include <algorithm>
using namespace std;

// pre-defined errors
enum ERROR_TYPE{
    DIV, INV, VAR, LIN, SYN, KEY
};

// reserved words, check if a variable exist in them
string RESERVED[] = {"rem", "let", "print", "input", "end", "goto", "if", "then", "run", "list", "clear", "quit", "help"};
const int RESSIZE = sizeof(RESERVED) / sizeof(RESERVED[0]);

// struct to store a line of instruction
struct Line{
    int num;            // line number
    string line;        // original input string
    Line* next;         // pointer to the next line
    Line(int n, const string& s, Line* lp = NULL)   // constructor
        :num(n), line(s), next(lp){}
    // compare two lines' number
    bool operator==(const Line& l)const{return num == l.num;}
    bool operator<(const Line& l)const{return num < l.num;}
};

// the core class to execute directly or run the program
class Program{
    public:
        Program()
            :lines(NULL){}
        ~Program();
        void addLine(const string&);    // insert a new line
        void execute(const string&);    // execute an instruction
        void addVar(const string&, int);// add a new variable
        int getVar(const string& name); // get the value of a variable

    private:
        Line* lines;                    // stores all lines of statements
        map<string, int> varTable;      // table of variables
        int statement(const string&);   // execute statements of the program
        void run();                     // run the program
        void list();                    // list all statements
        void clear();                   // clear all statements
        void help();                    // output help message
        void let(const string&);        // variable assigning statement
        void print(const string&);      // print variable value
        void input(const string&);      // input a variable value manually
        int jmp(const string&);         // jump to specific line
        int cjmp(const string&);        // if xx then xx jump
};

// destructor
Program::~Program(){
    clear();
}

// add a line of statement
void Program::addLine(const string& s){
    stringstream ss;
    ss << s;
    int n;
    if(!(ss >> n) || n < 0){    // illegal line
        throw SYN;
    }
    string st, tmp;
    if(!(ss >> tmp)){// no content after line number, delete
        Line* lp = lines;
        if(lp == NULL)          // "lines" is empty
            return;
        if(lp -> num == n){     // the first line
            lines = lines -> next;
            delete lp;
        }
        else{
            while(lp -> next != NULL){
                if(lp -> next -> num == n){// find a matching line
                    Line* tmp = lp -> next;
                    lp -> next = tmp -> next;
                    delete tmp;
                    break;
                }
                lp = lp -> next;
            }
        }
    }
    else{   // add or replace line
        Line* l = new Line(n, s);
        Line* lp = lines;
        if(lp == NULL){                 // add line to empty lines
            lines = l;
        }
        else if(n < lp -> num){         // insert at the start
            l -> next = lp;
            lines = l;
        }
        else if(n == lp -> num){        // replace
            l -> next = lp -> next;
            delete lp;
            lines = l;
        }
        else if(lp -> next == NULL){    // add to tail
            lines -> next = l;
        }
        else{
            while(lp -> next != NULL){
                if(n == lp -> next -> num){         // match
                    l -> next = lp -> next -> next;
                    delete lp -> next;
                    lp -> next = l;
                    break;
                }
                if(n < lp -> next -> num){// find the proper place
                    l -> next = lp -> next;
                    lp -> next = l;
                    break;
                }
                lp = lp -> next;
                if(lp -> next == NULL){   // reach tail
                    lp -> next = l;
                    break;
                }
            }
        }
    }
}

// execute an instruction directly
void Program::execute(const string& str){
    // trim the string
    string s = str;
    transform(s.begin(), s.end(), s.begin(), (int(*)(int))tolower);
    s.erase(0, s.find_first_not_of("\r\n\t "));
    s.erase(s.find_last_not_of("\r\n\t ")+1);
    string instr, tmp;
    stringstream ss;
    ss << s;
    ss >> instr;

    // call corresponding functions
    if(instr == "run"){
        if(ss >> tmp) throw SYN;
        run();
        return;
    }
    if(instr == "list"){
        if(ss >> tmp) throw SYN;
        list();
        return;
    }
    if(instr == "clear"){
        if(ss >> tmp) throw SYN;
        clear();
        return;
    }
    if(instr == "quit"){
        if(ss >> tmp) throw SYN;
        exit(0);
    }
    if(instr == "help"){
        if(ss >> tmp) throw SYN;
        help();
        return;
    }
    if(instr == "let"){
        let(s);
        return;
    }
    if(instr == "print"){
        print(s);
        return;
    }
    if(instr == "input"){
        input(s);
        return;
    }
    throw SYN;
}

// execute statements, return -1 if there's no jump,
// return -2 if reach end of program, return line number
// if there's a jump.
int Program::statement(const string& str){
    // trim the string
    string s = str;
    transform(s.begin(), s.end(), s.begin(), (int(*)(int))tolower);
    s.erase(0, s.find_first_not_of("\r\n\t "));
    s.erase(0, s.find_first_not_of("0123456789"));
    s.erase(0, s.find_first_not_of("\r\n\t "));
    s.erase(s.find_last_not_of("\r\n\t ")+1);
    string instr;
    stringstream ss;
    ss << s;
    ss >> instr;

    // call corresponding functions
    if(instr == "rem"){
        return -1;
    }
    if(instr == "let"){
        let(s);
        return -1;
    }
    if(instr == "print"){
        print(s);
        return -1;
    }
    if(instr == "input"){
        input(s);
        return -1;
    }
    if(instr == "end"){
        string tmp;
        if(ss >> tmp) throw SYN;
        return -2;
    }
    if(instr == "goto"){
        return jmp(s);
    }
    if(instr == "if"){
        return cjmp(s);
    }
    throw SYN;
}

// run the program from its start
void Program::run(){
    Line* lp = lines;
    while(lp != NULL){
        int next = statement(lp -> line);
        if(next == -2)      // reach end
            break;
        if(next == -1){     // no jump
            lp = lp -> next;
            continue;
        }
        lp = lines;
        while(lp != NULL){
            if(lp -> num == next)   // find match
                break;
            lp = lp -> next;
            if(lp == NULL) throw LIN;// no matching jump destination
        }
    }
}

// list all statements
void Program::list(){
    Line* lp = lines;
    while(lp != NULL){
        cout << lp -> line << endl;
        lp = lp -> next;
    }
}

// clear all statements
void Program::clear(){
    Line* lp = lines;
    while(lp != NULL){
        Line* next = lp -> next;
        delete lp;
        lp = next;
    }
    lines = NULL;
    varTable.clear();
}

// output help information
void Program::help(){
    cout << "TianJiahe 5130379056" << endl;
}

// add a variable to the variable table
void Program::addVar(const string& name, int val){
    if(find(RESERVED, RESERVED + RESSIZE, name) != RESERVED + RESSIZE)
        throw KEY;
    varTable[name] = val;
}

// check if a variable exist, if so, return its value
int Program::getVar(const string& name){
    if(varTable.find(name) == varTable.end())
        throw VAR;
    return varTable[name];
}

// Expression base class to calculate an expression
class Expression {
    public:
        Expression(){}
        virtual ~Expression(){}
        enum ExpressionType { CONSTANT, IDENTIFIER, COMPOUND };
        virtual int eval(Program* pro) const = 0;
        virtual ExpressionType type() const = 0;
    private:
};

// Expression of constant number
class ConstantExp: public Expression {
    public:
        ConstantExp(int val)
            :value(val){}
        virtual int eval(Program* pro) const {return value;}
        virtual ExpressionType type() const {return Expression::CONSTANT;}

    private:
        int value;
};

// Expression of variable
class IdentifierExp: public Expression {
    public:
        IdentifierExp(string name)
            :name(name){}
        virtual int eval(Program* pro) const{return pro->getVar(name);}
        virtual ExpressionType type() const {return Expression::IDENTIFIER;}
        string getName() const {return name;}

    private:
        std::string name;
};

// Expression of two Expression combined
class CompoundExp: public Expression {
    public:
        CompoundExp(char ch, Expression *l, Expression *r)
            :op(ch), lhs(l), rhs(r)
        {
        }
        virtual ~CompoundExp(){
            delete lhs;
            delete rhs;
        }
        virtual int eval(Program* pro) const;
        virtual ExpressionType type() const {return Expression::COMPOUND;}

        char getOp();
        Expression *getLHS();
        Expression *getRHS();

    private:
        char op;
        Expression *lhs, *rhs;
};

// eval function returns value of an expression
int CompoundExp::eval(Program* pro) const{
    int left = lhs->eval(pro);
    int right = rhs->eval(pro);
    if (op == '+') return left + right;
    if (op == '-') return left - right;
    if (op == '*') return left * right;
    if (op == '/') {
        if(right == 0) throw DIV;
        return left / right;
    }
    if (op == '%') {
        if(right == 0) throw DIV;
        return left % right;

    }
    throw SYN;
}

int precedence(char op) {
    if (op == '+' || op == '-') return 2;
    if (op == '*' || op == '/' || op == '%') return 3;
    return 0;
}

Expression* readT(iostream&);

// read an expression from iostream
Expression* readE(iostream& is, int prec = 0) {
    Expression *exp = readT(is);
    char op;
    while (is >> op) {
        if(precedence(op) == 0){
            is.putback(op);
            break;
        }
        int newPrec = precedence(op);
        if (newPrec <= prec){
            is.putback(op);
            break;
        }
        Expression *rhs = readE(is, newPrec);
        exp = new CompoundExp(op, exp, rhs);
    }
    return exp;
}

int getTokenType(char ch){
    if(isdigit(ch) || ch == '-') return 0;
    if(isalpha(ch)) return 1;
    return 2;
}

// read term from iostream
Expression* readT(iostream& is) {
    string token = "";
    char ch;
    if(!(is >> ch)) throw SYN;
    int type = getTokenType(ch);
    if(type == 0){
        is.putback(ch);
        int num;
        is >> num;
        return new ConstantExp(num);
    }
    else if(type == 1){
        is.putback(ch);
        is >> token;
        return new IdentifierExp(token);
    }
    if (ch != '(') throw SYN;
    Expression *exp = readE(is);
    is >> ch;
    if (ch != ')') throw SYN;
    return exp;
}

// assignment
void Program::let(const string& s){
    stringstream ss;
    ss << s;
    string var;
    char op;
    ss >> var;
    ss >> var;
    ss >> op;
    if(op != '=')
        throw SYN;
    Expression* exp = readE(ss);
    string tmp;
    if(ss >> tmp) throw SYN;
    addVar(var, exp -> eval(this));
}

// print value
void Program::print(const string& s){
    stringstream ss;
    ss << s;
    string tmp;
    ss >> tmp;
    Expression* exp = readE(ss);
    if(ss >> tmp) throw SYN;
    cout << exp -> eval(this) << endl;
}

// input a variable's value
void Program::input(const string& s){
    stringstream ss;
    ss << s;
    string var, tmp;
    int val;
    ss >> var;
    ss >> var;
    if(isdigit(var[0])) throw SYN;
    while(true) try{
        cout << " ? ";
        string line;
        getline(cin, line);
        stringstream ls;
        ls << line;
        if(!(ls >> val)) throw INV;
        if(ls >> tmp) throw INV;
        break;
    }
    catch(ERROR_TYPE msg){
        if(msg == INV)
            cout << "INVALID NUMBER\n";
    }
    addVar(var, val);
}

// unconditionally jump to the destination
int Program::jmp(const string& s){
    int next;
    stringstream ss(s);
    string tmp;
    ss >> tmp;
    if(!(ss >> next)) throw SYN;
    if(ss >> tmp) throw SYN;
    return next;
}

// jump to the destination if condition establishes
// i.e. if xx then xx
int Program::cjmp(const string& str){
    int next, delim;
    char op;
    string s = str, ls, rs;
    s.erase(0, s.find_first_of("\r\n\t ") + 1);
    delim = s.find_first_of("<>=");
    if(delim == string::npos) throw SYN;   // illegal input
    ls = s.substr(0, delim);    // left part of the comparison
    op = s[delim];
    s.erase(0, delim + 1);
    delim = s.find("then");
    if(delim == string::npos) throw SYN;   // illegal input
    rs = s.substr(0, delim);    // right part of the comparison
    s.erase(0, delim + 4);
    stringstream ss;
    ss << s;
    if(!(ss >> next)) throw SYN;// the string should be a number
    if(ss >> s) throw SYN;// there shouldn't be any other thing
    ss.clear();
    ss << ls;
    Expression* l = readE(ss);  // convert the left part to exp
    ss.clear();
    ss << rs;
    Expression* r = readE(ss);  // convert the right part to exp
    switch(op){
        // conditions and comparisons
        case '<':
            return ((l -> eval(this) < r -> eval(this))? next : -1);
        case '>':
            return ((l -> eval(this) > r -> eval(this))? next : -1);
        case '=':
            return ((l -> eval(this) == r -> eval(this))? next : -1);
        default:
            throw SYN;
    }
}

int main() {
    Program basic;
    string s;
    while(getline(cin, s)) try{
        // empty line
        if(s.find_first_not_of("\r\n\t ") == s.length() - 1)
            continue;
        int num;
        stringstream ss;
        ss << s;
        // directly execute
        if(!(ss >> num)){
            basic.execute(s);
        }
        // there's line number, add the line
        else{
            basic.addLine(s);
        }
    }
    // error outputs
    catch(ERROR_TYPE msg){
        switch(msg){
            case DIV:
                cout << "DIVIDE BY ZERO\n";
                break;
            case INV:
                cout << "INVALID NUMBER\n";
                break;
            case VAR:
                cout << "VARIABLE NOT DEFINED\n";
                break;
            case LIN:
                cout << "LINE NUMBER ERROR\n";
                break;
            case KEY:
                cout << "USE RESERVED WORDS AS IDENTIFIER\n";
                break;
            default:
                cout << "SYNTAX ERROR\n";
                break;
        }
    }
    return 0;
}
