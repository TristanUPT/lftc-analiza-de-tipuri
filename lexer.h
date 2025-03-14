#pragma once 

 
enum{ 
 ID 
 // keywords 
 ,TYPE_INT, VAR, FUNCTION, IF, ELSE, WHILE, END, RETURN, TYPE_REAL, TYPE_STR 
 // delimiters 
 ,COMMA, FINISH, COLON, SEMICOLON, LPAR, RPAR 
 // operators 
 ,ASSIGN, EQUAL, NOTEQ, ADD, SUB, MUL, DIV, AND, OR, NOT, NOREQ, LESS, GREATER, GREATEREQ 
 // spaces 
 ,SPACE, COMMENT 
 // constants 
 ,INT, REAL, STR 
 }; 
 
#define MAX_STR  127 
 
typedef struct{ 
 int code;  // ID, TYPE_INT, ... 
 int line;  // the line from the input file 
 union{ 
  char text[MAX_STR+1];  // the chars for ID, STR 
  int i;  // the value for INT 
  double r;  // the value for REAL 
  }; 
 }Token; 
 
#define MAX_TOKENS  4096 
extern Token tokens[]; 
extern int nTokens; 
 
void tokenize(const char *pch); 
void showTokens();