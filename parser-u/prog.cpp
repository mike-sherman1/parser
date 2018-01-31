#include <iostream>
#include <fstream>
#include <cctype>

#include "token.h"
#include "functions.h"

using namespace std;

ifstream ifs;                   // input file stream used by lexan
SymTab   symtab;                // global symbol table
Token    token;                 // global token
int      lookahead = 0;         // no look ahead token yet

int      dbg = 1;               // debug is ON

int main( )
{
    ifs = get_ifs();           // open an input file stream w/ the program
    init_kws();                // initialize keywords in the symtab
    match(lookahead);         // get the first input token
    prog(); //first progression
    return 0;
}

/*
My project is not complete. The error catching is broken and it does not output in postfix notation.
I think there is something wrong with the structure / the way it parses through.
I honestly got confused with a lot of your code, and I think I would've had a better time just starting from scratch.
I had no idea where to put the emit statements to make it postfix notation. I tried to write a function to convert from infix to postfix,
but it was difficult and you said that it was not needed. 
*/

//my methods
void prog() {
	while (1) { //loop over file until end
		if (token.toktype() == KW) { //if token is keyword begin or end
			if (token.lineno() > 1) //if token is keyword end
				emit('\n'); //output new line before keyword end to match given formatting
			emit(token.toktype()); //output keyword
			emit('\n'); //output newline
			token = lexan(); //go to next token
		}
		else { //if not a keyword, there is an error
			error(token.toktype(), 257, "END");
		}
		if (token.toktype() == DONE) { //if end of file, break from loop
			break;
		}
		stmt_list(); //parse statement list
	}
}

void stmt_list() {
	stmt(); //parse first statement
	if (token.toktype() == 59) { //if semicolon
		emit(token.toktype()); //output semicolon
		token = lexan(); //go to next token
		if (token.toktype() == KW) { //if keyword end
			return; //break out of function
		}
		else {
			stmt_list(); //if not keyword end, parse next statement list
		}
	}
	else if (token.toktype() == KW) { //if token is not semicolon and is keyword end
		return; //break out of function
	}
	else { //if token is not semicolon or keyword end then error
		error(token.toktype(), 59, "END");
	}
}

void stmt() {
	var(); //parse first variable
	token = lexan(); //go to next token
	if (token.toktype() == KW) { //if keyword end
		return; //break out of function
	}
	if (token.toktype() == 61) { //if equals sign
		emit(token.toktype()); //output equals sign
		token = lexan(); //go to next token
		expr(); //parse expression
	}
	else { //if not equals sign, then error
		error(token.toktype(), 61, "END");
	}
}

void var() { 
	emit(token.toktype()); //output variable
}

void expr() {
	var(); //parse first variable
	token = lexan(); //go to next token
	if (token.toktype() == KW || token.toktype() == 59) { //if keyword end or semicolon
		return; //break out of function
	}
	if (token.toktype() == 43 || token.toktype() == 45) { //if plus or minus sign
		emit(token.toktype()); //output plus or minus sign
		token = lexan(); //go to next token
		expr(); //parse expression
	}
	else { //if not plus or minus sign, there is an error
		error(token.toktype(), 43, "END");
	}
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
        cerr << token.tokstr();
    }
    cerr << "' of type " << lookahead;

    switch(expt)
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
            cout << " while looking for '" << char(expt) << "'";
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

