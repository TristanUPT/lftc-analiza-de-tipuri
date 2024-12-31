#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "lexer.h"
#include "ad.h"
#include "at.h"

int iTk;		 // the iterator in tokens
Token *consumed; // the last consumed token

// same as err, but also prints the line of the current token
_Noreturn void tkerr(const char *fmt, ...)
{
	fprintf(stderr, "error in line %d: ", tokens[iTk].line);
	va_list va;
	va_start(va, fmt);
	vfprintf(stderr, fmt, va);
	va_end(va);
	fprintf(stderr, "\n");
	exit(EXIT_FAILURE);
}

bool program();
bool defVar();
bool defFunc();
bool block();
bool funcParams();
bool funcParam();
bool instr();
bool expr();
bool exprLogic();
bool exprAssign();
bool exprComp();
bool exprAdd();
bool exprMul();
bool exprPrefix();
bool factor();
bool baseType();

// const char *tkCodeName(int code) - o funcție care primește ca parametru un cod de atom și îi returnează numele
const char *tkCodeName(int code)
{
	switch (code)
	{
	case VAR:
		return "VAR";
	case FUNCTION:
		return "FUNCTION";
	case ID:
		return "ID";
	case TYPE_INT:
		return "TYPE_INT";
	case TYPE_REAL:
		return "TYPE_REAL";
	case TYPE_STR:
		return "TYPE_STR";
	case IF:
		return "IF";
	case ELSE:
		return "ELSE";
	case WHILE:
		return "WHILE";
	case END:
		return "END";
	case RETURN:
		return "RETURN";
	case INT:
		return "INT";
	case REAL:
		return "REAL";
	case STR:
		return "STR";
	case COMMA:
		return "COMMA";
	case COLON:
		return "COLON";
	case SEMICOLON:
		return "SEMICOLON";
	case LPAR:
		return "LPAR";
	case RPAR:
		return "RPAR";
	case ADD:
		return "ADD";
	case SUB:
		return "SUB";
	case MUL:
		return "MUL";
	case DIV:
		return "DIV";
	case AND:
		return "AND";
	case OR:
		return "OR";
	case NOT:
		return "NOT";
	case ASSIGN:
		return "ASSIGN";
	case EQUAL:
		return "EQUAL";
	case NOTEQ:
		return "NOTEQ";
	case LESS:
		return "LESS";
	case GREATER:
		return "GREATER";
	case GREATEREQ:
		return "GREATEREQ";
	case FINISH:
		return "FINISH";
	default:
		return "UNKNOWN";
	}
}
// functia consume din lab
bool consume(int code)
{
	 //printf("consume(%s)", tkCodeName(code));
	if (tokens[iTk].code == code)
	{
		consumed = &tokens[iTk++];
		 //printf(" => consumed\n");
		return true;
	}
	 //printf(" => found %s\n", tkCodeName(tokens[iTk].code));
	return false;
}

// program ::= ( defVar | defFunc | block )* FINISH

bool program()
{
	 //puts("# program");
	addDomain();
	
addPredefinedFns(); // it will be inserted after the code for domain analysis


	int start = iTk;
	for (;;)
	{
		if (defVar())
		{
		}
		else if (defFunc())
		{
		}
		else if (block())
		{
		}
		else
			break;
	}
	if (consume(FINISH))
	{
		delDomain();
		return true;
	}
	else
	{
		int start = iTk;
		tkerr("syntax error");
	}
	return false;
}

// defVar ::= VAR ID COLON baseType SEMICOLON
bool defVar()
{
	 //puts("# defVar");
	int start = iTk;
	if (consume(VAR))
	{
		if (consume(ID))
		{
			const char *name = consumed->text;
			Symbol *s = searchInCurrentDomain(name);
			if (s)
				tkerr("symbol redefinition: %s", name);
			s = addSymbol(name, KIND_VAR);
			s->local = crtFn != NULL;

			if (consume(COLON))
			{
				if (baseType())
				{
					s->type = ret.type;

					if (consume(SEMICOLON))
					{
						return true;
					}
					else
						tkerr("Expected ';' at the end of variable definition");
				}
				else
					tkerr("Expected a type (e.g., TYPE_INT, TYPE_REAL, TYPE_STR)");
			}
			else
				tkerr("Expected ':' after the variable identifier");
		}
		else
			tkerr("Expected an identifier during variable definition");
	}
	iTk = start;
	return false;
}

// baseType ::= TYPE_INT | TYPE_REAL | TYPE_STR
bool baseType()
{
	 //puts("# baseType");
	int start = iTk;
	if (consume(TYPE_INT))
	{
		ret.type = TYPE_INT;
		return true;
	}
	if (consume(TYPE_REAL))
	{
		ret.type = TYPE_REAL;
		return true;
	}
	if (consume(TYPE_STR))
	{
		ret.type = TYPE_STR;
		return true;
	}
	iTk = start;
	return false;
}

// defFunc ::= FUNCTION ID LPAR funcParams? RPAR COLON baseType defVar* block END
bool defFunc()
{
	 //puts("# defFunc");
	int start = iTk;
	if (consume(FUNCTION))
	{
		if (consume(ID))
		{
			const char *name = consumed->text;
			Symbol *s = searchInCurrentDomain(name);
			if (s)
				tkerr("symbol redefinition: %s", name);
			crtFn = addSymbol(name, KIND_FN);
			crtFn->args = NULL;
			addDomain();

			if (consume(LPAR))
			{
				if (funcParams())
				{
				}
				if (consume(RPAR))
				{
					if (consume(COLON))
					{
						if (baseType())
						{
							crtFn->type = ret.type;
							for (;;)
							{
								if (defVar())
								{
								}
								else
									break;
							}
							if (block())
							{
								if (consume(END))
								{
									delDomain();
									crtFn = NULL;
									return true;
								}
								else
									tkerr("Expected 'END' at the end of a function definition");
							}
							else
								tkerr("Expected a block of instructions in function definition");
						}
						else
							tkerr("Expected a type (e.g., TYPE_INT, TYPE_REAL, TYPE_STR) for the function");
					}
					else
						tkerr("Expected ':' in function definition");
				}
				else
					tkerr("Expected ')' to close parameter list");
			}
			else
				tkerr("Expected '(' to start parameter list");
		}
		else
			tkerr("Expected a function name(identifier)");
		iTk = start;
		return false;
	}
}

// block ::= instr+
bool block()
{
	 //puts("# block");
	int start = iTk;
	if (instr())
	{
		for (;;)
		{
			if (instr())
			{
			}
			else
				break;
		}
		return true;
	}
	iTk = start;
	return false;
}

// funcParams ::= funcParam ( COMMA funcParam )*
bool funcParams()
{
	 //puts("# funcParams");
	int start = iTk;
	if (funcParam())
	{
		for (;;)
		{
			if (consume(COMMA))
			{
				if (funcParam())
				{
				}
				else
					tkerr("Expected parameter after ',' in function parameter list");
			}
			else
				break;
		}
		return true;
	}
	iTk = start;
	return false;
}

// funcParam ::= ID COLON baseType
bool funcParam()
{
	 //puts("# funcParam");
	int start = iTk;
	if (consume(ID))
	{
		const char *name = consumed->text;
		Symbol *s = searchInCurrentDomain(name);
		if (s)
			tkerr("symbol redefinition: %s", name);
		s = addSymbol(name, KIND_ARG);
		Symbol *sFnParam = addFnArg(crtFn, name);

		if (consume(COLON))
		{
			if (baseType())
			{
				s->type = ret.type;
				sFnParam->type = ret.type;
				return true;
			}
			else
				tkerr("Expected a base type for parameter");
		}
		else
			tkerr("Expected ':' after parameter name");
	}
	 else
	tkerr("Expected an identifier for parameter");
	iTk = start;
	return false;
}

// instr ::= expr? SEMICOLON | IF LPAR expr RPAR block ( ELSE block )? END | RETURN expr SEMICOLON | WHILE LPAR expr RPAR block END
bool instr()
{
	 //puts("# instr");
	int start = iTk;

	if (expr())
	{
	}

	if (consume(SEMICOLON))
	{
		return true;
	}

	iTk = start;
	if (consume(IF))
	{
		if (consume(LPAR))
		{
			if (expr())
			{
				if (ret.type == TYPE_STR)tkerr("type error: the if condition must have type int or real");
				if (consume(RPAR))
				{
					if (block())
					{
						if (consume(ELSE))
						{
							if (block())
							{
							}
						}
						if (consume(END))
						{
							return true;
						}
						else
							tkerr("Expected 'END' to close 'IF' block");
					}
					else
						tkerr("Expected a block of instructions after 'IF' condition");
				}
				else
					tkerr("Expected ')' to close 'IF' condition");
			}
			else
				tkerr("Expected an expression in 'IF' condition");
		}
		else
			tkerr("Expected '(' to start 'IF' condition");
	}
	iTk = start;
	if (consume(RETURN))
	{
		if (expr())
		{
			if(!crtFn)tkerr("return can be used only in a function");
            if(ret.type!=crtFn->type)tkerr("the return type must be the same as the function return type");

			if (consume(SEMICOLON))
			{
				return true;
			}
			else
				tkerr("Expected ';' after 'RETURN' expression");
		}
		else
			tkerr("Expected an expression after 'RETURN'");
	}
	iTk = start;
	if (consume(WHILE))
	{
		if (consume(LPAR))
		{
			if (expr())
			{
				if (ret.type == TYPE_STR)tkerr("type error: the while condition must have type int or real");
				if (consume(RPAR))
				{
					if (block())
					{
						if (consume(END))
						{
							return true;
						}
						else
							tkerr("Expected 'END' to close 'WHILE' loop");
					}
					else
						tkerr("Expected a block of instructions in 'WHILE' loop");
				}
				else
					tkerr("Expected ')' to close 'WHILE' condition");
			}
			else
				tkerr("Expected an expression in 'WHILE' condition");
		}
		else
			tkerr("Expected '(' to start 'WHILE' condition");
	}
	iTk = start;
	return false;
}

// expr ::= exprLogic
bool expr()
{
	//puts("# expr");
	int start = iTk;
	if (exprLogic())
	{
		return true;
	}
	iTk = start;
	return false;
}

// exprLogic ::= exprAssign ( ( AND | OR ) exprAssign )*
bool exprLogic()
{
	//puts("# exprLogic");
	int start = iTk;
	if (exprAssign())
	{
		for (;;)
		{
			if (consume(AND))
			{
				if (exprAssign())
				{
					Ret leftType=ret;
                    if(leftType.type==TYPE_STR)tkerr("the left operand of && or || cannot be of type str");
				}
				else
					tkerr("Expected an expression after logical operator 'AND')");
			}
			else if (consume(OR))
			{
				if (exprAssign())
				{
					if(ret.type==TYPE_STR)tkerr("the right operand of && or || cannot be of type str");
                    setRet(TYPE_INT,false);
				}
				else
					tkerr("Expected an expression after logical operator 'OR'");
			}
			else
				break;
		}
		return true;
	}
	iTk = start;
	return false;
}

// exprAssign ::= ( ID ASSIGN )? exprComp
bool exprAssign()
{
	// //puts("# exprAssign");
	int start = iTk;
	if (consume(ID))
	{
		const char* name = consumed->text;
		if (consume(ASSIGN))
		{
			if (exprComp())
			{
				Symbol *s=searchSymbol(name);
                if(!s)tkerr("undefined symbol: %s",name);
                if(s->kind==KIND_FN)tkerr("a function (%s) cannot be used as a destination for assignment ",name);
                if(s->type!=ret.type)tkerr("the source and destination for assignment must have the same type");
                ret.lval=false;

				return true;
			}
			else
				tkerr("Expected an expression after '='");
		}
		else
			iTk = start;
	}
	if (exprComp())
	{
		return true;
	}
	iTk = start;
	return false;
}

// exprComp ::= exprAdd ( ( LESS | EQUAL ) exprAdd )?
bool exprComp()
{
	//puts("# exprComp");
	int start = iTk;
	if (exprAdd())
	{
		Ret leftType = ret;
		if (consume(LESS))
		{
			if (exprAdd())
			{
				 if (leftType.type != ret.type)
                    tkerr("different types for the operands of <");
                setRet(TYPE_INT, false); // the result of comparison is int 0 or 1
			}
			else
				tkerr("Expected an expression after comparison operator <");
		}
		else if (consume(EQUAL))
		{
			if (exprAdd())
			{
				if (leftType.type != ret.type)
                    tkerr("different types for the operands of ==");
                setRet(TYPE_INT, false); // the result of comparison is int 0 or 1
			}
			else
				tkerr("Expected an expression after comparison operator = ");
		}
		return true;
	}
	iTk = start;
	return false;
}

// exprAdd ::= exprMul ( ( ADD | SUB ) exprMul )*
bool exprAdd()
{
	//puts("# exprAdd");
	int start = iTk;
	if (exprMul())
	{
		for (;;)
		{
			if (consume(ADD))
			{
				 Ret leftType = ret; // Save the left operand type
                if (leftType.type == TYPE_STR)
                    tkerr("the operand of + cannot be of type str");
				if (exprMul())
				{
					if (leftType.type != ret.type)
                        tkerr("different types for the operand of +");
                    ret.lval = false;
				}
				else
					tkerr("Expected an expression after arithmetic operator '+'");
			}
			else if (consume(SUB))
			{
				 Ret leftType = ret; // Save the left operand type
                if (leftType.type == TYPE_STR)
                    tkerr("the operand of - cannot be of type str");
				if (exprMul())
				{
					   if (leftType.type != ret.type)
                        tkerr("different types for the operand of -");
                    ret.lval = false;
				}
				else
					tkerr("Expected an expression after arithmetic operator '-'");
			}
			else
				break;
		}
		return true;
	}
	iTk = start;
	return false;
}

// exprMul ::= exprPrefix ( ( MUL | DIV ) exprPrefix )*
bool exprMul()
{
	//puts("# exprMul");
	int start = iTk;
	if (exprPrefix())
	{
		for (;;)
		{
			if (consume(MUL))
			{
				Ret leftType=ret;
if(leftType.type==TYPE_STR)tkerr("the operand of * cannot be of type str");
				if (exprPrefix())
				{
					if(leftType.type!=ret.type)tkerr("different types for the operand of *");
                  ret.lval=false;
				}
				else
					tkerr("Expected an expression after arithmetic operator '*'");
			}
			else if (consume(DIV))
			{
				Ret leftType=ret;
          if(leftType.type==TYPE_STR)tkerr("the operand of / cannot be of type str");
				if (exprPrefix())
				{
					if(leftType.type!=ret.type)tkerr("different types for the operand of /");
                ret.lval=false;
				}
				else
					tkerr("Expected an expression after arithmetic operator '/'");
			}
			else
				break;
		}
		return true;
	}
	iTk = start;
	return false;
}

// exprPrefix ::= ( SUB | NOT )? factor
bool exprPrefix()
{
	//puts("# exprPrefix");
	int start = iTk;
	if (consume(SUB))
	{
		if (factor())
		{
			if(ret.type==TYPE_STR)tkerr("the expression of unary - must be of type int or real");
           ret.lval=false;

			return true;
		}
		else
			tkerr("Expected an expression after prefix operator '-'");
	}
	if (consume(NOT))
	{
		if (factor())
		{
			if(ret.type==TYPE_STR)tkerr("the expression of ! must be of type int or real");
           setRet(TYPE_INT,false);

			return true;
		}
		else
			tkerr("Expected an expression after prefix operator 'NOT'");
	}
	if (factor())
	{
		return true;
	}
	iTk = start;
	return false;
}

// factor ::= INT | REAL | STR | LPAR expr RPAR | ID ( LPAR ( expr ( COMMA expr )* )? RPAR )?
bool factor()
{
	//puts("# factor");
	int start = iTk;
	if (consume(INT))
	{
		setRet(TYPE_INT, false);
		return true;
	}
	if (consume(REAL))
	{
		setRet(TYPE_REAL, false);
		return true;
	}
	if (consume(STR))
	{
		setRet(TYPE_STR, false);
		return true;
	}
	if (consume(LPAR))
	{
		if (expr())
		{
			if (consume(RPAR))
			{
				return true;
			}
			else
				tkerr("Expected ')' to close expression");
		}
		else
			tkerr("Expected an expression inside parentheses");
	}
	if (consume(ID))
	{
		 Symbol *s = searchSymbol(consumed->text);
        if (!s)
            tkerr("undefined symbol: %s", consumed->text);
		if (consume(LPAR))
		{
			 if (s->kind != KIND_FN)
                tkerr("%s cannot be called, because it is not a function", s->name);
            Symbol *argDef = s->args;
			if (expr())
			{
				if (argDef)
                {
                    if (argDef->type != ret.type)
                        tkerr("the argument type at function %s call is different from the one given at its definition", s->name);
                    argDef = argDef->next;
                }
				 else
                    tkerr("the function %s is called with too many arguments", s->name);
				for (;;)
				{
					if (consume(COMMA))
					{
						if (expr())
						{
							if (argDef)
                        {
                            if (argDef->type != ret.type)
                                tkerr("the argument type at function %s call is different from the one given at its definition", s->name);
                            argDef = argDef->next;
                        }
                        else
                            tkerr("the function %s is called with too many arguments", s->name);
						}
						else
							tkerr("Expected expression after ','");
					}
					else
						break;
				}
			}
			if (consume(RPAR))
			{
				if (argDef)
                    tkerr("the function %s is called with too few arguments", s->name);

				setRet(s->type, false);
				return true;
			}
			else
				tkerr("Expected ')' to close expression");
		}
		else
        {
            if (s->kind == KIND_FN)
                tkerr("the function %s can only be called", s->name);
            setRet(s->type, true);
            return true;
        }
	}
	iTk = start;
	return false;
}

void parse()
{
	iTk = 0;
	program();
}
