/*

    prog        ->  begin stmt_list end

    stmt_list   ->  stmt ; stmt_list
                 |  stmt
				 |	stmt;

    stmt        ->  var = expr

    var         ->  A | B | C

    expr        ->  var + expr
                 |  var - expr
                 |  var

*/

#include <iostream>
#include <fstream>
#include <cctype>
#include <sstream>
#include <stack>
#include <limits>
#include <string>

#include "token.h"
#include "functions.h"

using namespace std;

ifstream ifs;                   // input file stream used by lexan
SymTab   symtab;                // global symbol table
Token    token;                 // global token
int      lookahead = 0;         // no look ahead token yet
string infix; //input for infix to postfix
stringstream output; //output for infix to postfix

int      dbg = 1;               // debug is ON

int priority(char); //prototype for priority calculation function for infix to postfix
void toPostfix(); //prototype for infix to postfix function

int main( )
{
    ifs = get_ifs();           // open an input file stream w/ the program
    init_kws();                // initialize keywords in the symtab
    match(lookahead);         // get the first input token
    prog();
	toPostfix();
	return 0;
}

/*
prog -> begin stmt_list end

stmt_list -> stmt ; stmt_list
| stmt
| stmt ;

stmt -> var = expr

var -> A | B | C

expr -> var + expr
| var - expr
| var
*/

// your methods...
void prog() {
	while (1) {
		if (token.toktype() == KW) {
			if (token.lineno() > 1)
				emit('\n');
			emit(token.toktype());
			emit('\n');
			token = lexan();
		}
		else {
			//error();
		}
		
		if (token.toktype() == DONE) {
			break;
		}
		
		stmt_list();
	
	} //end while
}

void stmt_list() {
	stmt();
	if (token.toktype() == 59) {
		emit(token.toktype());
		token = lexan();
		if (token.toktype() == KW) {
			return;
		}
		else {
			stmt_list();
		}
	}
}

void stmt() {
	var();
	token = lexan();
	if (token.toktype() == KW) {
		return;
	}
	if (token.toktype() == 61) {
		infix.append("=");
		token = lexan();
		expr();
	}
	else {
		//error();
	}
}

void var() {
	infix.append(token.tokstr());
}

void expr() {
	var();
	token = lexan();
	if (token.toktype() == KW) {
		return;
	}
	if (token.toktype() == 43) {
		infix.append("+");
		token = lexan();
		expr();
	}
	else if (token.toktype() == 45) {
		infix.append("-");
		token = lexan();
		expr();
	}
	else {
		//error();
	}
}

int priority(char c) {
	int result;
	if (c == '^')
		result = 1;
	else  if (c == '*' || c == '/')
		result = 2;
	else  if (c == '+' || c == '-')
		result = 3;
	else if (c == '=')
		result = 4;
	return result;
}

void toPostfix() {
	stack<char> operator_stack;

	for (unsigned i = 0; i < infix.length(); i++) {
		if (infix[i] == '+' || infix[i] == '-' || infix[i] == '*' || infix[i] == '/' || infix[i] == '^' || infix[i] == '=') {
			while (!operator_stack.empty() && priority(operator_stack.top()) <= priority(infix[i])) {
				output << operator_stack.top();
				operator_stack.pop();
			}
			operator_stack.push(infix[i]);
		}
		else if (infix[i] == '(') {
			operator_stack.push(infix[i]);
		}
		else if (infix[i] == ')') {
			while (operator_stack.top() != '(') {
				output << operator_stack.top();
				operator_stack.pop();
			}
			operator_stack.pop();
		}
		else {
			output << infix[i];
		}
	}

	while (!operator_stack.empty()) {
		output << operator_stack.top();
		operator_stack.pop();
	}

	cout << output.str() << endl;

	cin.ignore(numeric_limits<streamsize>::max(), '\n');
	//end infix to postfix
}

// utility methods

void emit(int t)
{
    switch(t)
    {
        case '+': case '-': case '=':
            cout << char(t) << ' ';
            break;

        case ';':
            cout << ";\n";
            break;

        case '\n':
            cout << "\n";
            break;

        case ID:
        case KW:
        case UID:
            cout << symtab.tokstr( token.tokvalue()) << ' ';
            break;

        default:
            cout << "'token " << t << ", tokvalue "
                 << token.tokvalue() << "' ";
            break;
    }
}

void error(int t, int expt, const string &str)
{
    cerr << "\nunexpected token '";
    if(lookahead == DONE)
    {
        cerr << "EOF";
    }
    else
    {
        cerr << token.tokstr( );
    }
    cerr << "' of type " << lookahead;

    switch( expt )
    {
        case 0:         // default value; nothing to do
            break;

        case ID:
            cout << " while looking for an ID";
            break;

        case KW:
            cout << " while looking for KW '" << str << "'";
            break;

        default:
            cout << " while looking for '" << char( expt ) << "'";
            break;
    }

    cerr << "\nsyntax error.\n";

    exit(1);
}


void match(int t)
{
    if(lookahead == t)
    {
        token = lexan();               // get next token
        lookahead = token.toktype();   // lookahed contains the tok type
    }
    else
    {
        error(t);
    }
}

