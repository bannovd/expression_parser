#pragma once

#include <iostream>
#include <cstdlib>
#include <cctype>
#include <cstring>

using namespace std;

enum types { DELIMITER = 1, VARIABLE, NUMBER };

const int NUMVARS = 26;

template <class PType> class parser
{
    char* exp_ptr;  // points to the expression
    char token[80]; // holds current token
    char tok_type;  // holds token's type
    PType vars[NUMVARS]; // holds variable's values

    void eval_exp1(PType& result);
    void eval_exp2(PType& result);
    void eval_exp3(PType& result);
    void eval_exp4(PType& result);
    void eval_exp5(PType& result);
    void eval_exp6(PType& result);
    void atom(PType& result);
    void get_token(), putback();
    void serror(int error);
    PType find_var(char* s);
    int isdelim(char c);
public:
    parser();
    PType eval_exp(char* exp);
};

template <class PType> parser<PType>::parser()
{
    int i;
    exp_ptr = NULL;
    for (i = 0; i < NUMVARS; i++)
    {
        vars[i] = (PType)0;
    }
}

// Parser entry point
template <class PType> PType parser<PType>::eval_exp(char* exp)
{
    PType result;
    exp_ptr = exp;
    get_token();
    if (!*token)
    {
        serror(2); // no expression present
        return (PType)0;
    }
    eval_exp1(result);
    if (*token) 
    {
        serror(0); // last token must be null
    }
    return result;
}

// Process an assignment
template <class PType> void parser<PType>::eval_exp1(PType& result)
{
    int slot;
    char ttok_type;
    char temp_token[80];

    if (tok_type == VARIABLE)
    {
        // save old token
        strcpy(temp_token, token);
        ttok_type = tok_type;

        // compute the index of the variable
        slot = toupper(*token) - 'A';

        get_token();
        if (*token != '=')
        {
            putback(); // return current token
            // restore old token - not assignment
            strcpy(token, temp_token);
            tok_type = ttok_type;
        }
        else
        {
            get_token(); // get next part of exp
            eval_exp2(result);
            vars[slot] = result;
            return;
        }
    }

    eval_exp2(result);
}

// Add or subtract two terms
template <class PType> void parser<PType>::eval_exp2(PType& result)
{
    register char op;
    PType temp;

    eval_exp3(result);
    while ((op = *token) == '+' || op == '-')
    {
        get_token();
        eval_exp3(temp);
        switch (op)
        {
        case '-':
            result = result - temp;
            break;
        case '+':
            result = result + temp;
            break;
        }
    }
}

// Multiply or divide two factors
template <class PType> void parser<PType>::eval_exp3(PType& result)
{
    register char op;
    PType temp;

    eval_exp4(result);
    while ((op = *token) == '*' || op == '/' || op == '%') {
        get_token();
        eval_exp4(temp);
        switch (op)
        {
        case '*':
            result = result * temp;
            break;
        case '/':
            result = result / temp;
            break;
        case '%':
            result = (int)result % (int)temp;
            break;
        }
    }
}

// Process an exponent
template <class PType> void parser<PType>::eval_exp4(PType& result)
{
    PType temp, ex;
    register int t;

    eval_exp5(result);
    if (*token == '^')
    {
        get_token();
        eval_exp4(temp);
        ex = result;
        if (temp == 0.0)
        {
            result = (PType)1;
            return;
        }
        for (t = (int)temp - 1; t > 0; --t)
        {
            result = result * ex;
        }
    }
}

// Evaluate a unary + or -
template <class PType> void parser<PType>::eval_exp5(PType& result)
{
    register char  op;

    op = 0;
    if ((tok_type == DELIMITER) && *token == '+' || *token == '-')
    {
        op = *token;
        get_token();
    }
    eval_exp6(result);
    if (op == '-')
    {
        result = -result;
    }
}

// Process a parenthesized expression
template <class PType> void parser<PType>::eval_exp6(PType& result)
{
    if ((*token == '('))
    {
        get_token();
        eval_exp2(result);
        if (*token != ')')
        {
            serror(1);
        }
        get_token();
    }
    else atom(result);
}

// Get the value of a number or a variable
template <class PType> void parser<PType>::atom(PType& result)
{
    switch (tok_type)
    {
    case VARIABLE:
        result = find_var(token);
        get_token();
        return;
    case NUMBER:
        result = (PType)atof(token);
        get_token();
        return;
    default:
        serror(0);
    }
}

// Return a token to the input stream
template <class PType> void parser<PType>::putback()
{
    char* t;
    t = token;
    for (; *t; t++)
    {
        exp_ptr--;
    }
}

// Display a syntax error
template <class PType> void parser<PType>::serror(int error)
{
    static char* e[] =
    {
        "Syntax error",
        "Unbalanced parentheses",
        "No expression present"
    };
    cout << e[error] << endl;
}

// Obtain the next token
template <class PType> void parser<PType>::get_token()
{
    register char* temp;

    tok_type = 0;
    temp = token;
    *temp = '\0';

    if (!*exp_ptr)
    {
        return; // at end of expression
    }

    while (isspace(*exp_ptr))
    {
        ++exp_ptr; // skip over white space
    }

    if (strchr("+-*/%^=()", *exp_ptr))
    {
        tok_type = DELIMITER;
        // advance to next char
        *temp++ = *exp_ptr++;
    }
    else if (isalpha(*exp_ptr))
    {
        while (!isdelim(*exp_ptr))
        {
            *temp++ = *exp_ptr++;
        }
        tok_type = VARIABLE;
    }
    else if (isdigit(*exp_ptr))
    {
        while (!isdelim(*exp_ptr))
        {
            *temp++ = *exp_ptr++;
        }
        tok_type = NUMBER;
    }

    *temp = '\0';
}

// Return true if c is a delimiter
template <class PType> int parser<PType>::isdelim(char c)
{
    if (strchr(" +-/*%^=()", c) || c == 9 || c == '\r' || c == 0)
    {
        return 1;
    }
    return 0;
}

// Return the value of a variable
template <class PType> PType parser<PType>::find_var(char* s)
{
    if (!isalpha(*s))
    {
        serror(1);
        return (PType)0;
    }
    return vars[toupper(*token) - 'A'];
}
