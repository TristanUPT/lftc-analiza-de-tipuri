#include <stdio.h> 
#include <ctype.h> 
#include <string.h> 
 
#include "lexer.h" 
#include "utils.h" 
 
Token tokens[MAX_TOKENS]; 
int nTokens; 
 
int line=1;  // the current line in the input file 
 
// adds a token to the end of the tokens list and returns it 
// sets its code and line 
Token *addTk(int code){ 
 if(nTokens==MAX_TOKENS)err("too many tokens"); 
 Token *tk=&tokens[nTokens]; 
 tk->code=code; 
 tk->line=line; 
 nTokens++; 
 return tk; 
 } 
 
// copy in the dst buffer the string between [begin,end) 
char *copyn(char *dst,const char *begin,const char *end){ 
 char *p=dst; 
 if(end-begin>MAX_STR)err("string too long"); 
 while(begin!=end)*p++=*begin++; 
 *p='\0'; 
 return dst; 
 } 
 
void tokenize(const char *pch) { 
    const char *start; 
    Token *tk; 
    char buf[MAX_STR+1]; 
    for(;;) { 
        switch(*pch) { 
            case ' ': case '\t': case '\r': case '\n': 
                if (*pch == '\n') line++; 
                pch++; 
                break; 
            case '\0': 
                addTk(FINISH); 
                return; 
            case ',': 
                addTk(COMMA); 
                pch++; 
                break; 
            case ':': 
                addTk(COLON); 
                pch++;                 
                break; 
            case ';': 
                addTk(SEMICOLON); 
                pch++; 
                break; 
                case '|':  
                if (pch[1] == '|')  
                {  
                addTk(OR);  
                pch += 2;  
                }  
                else  
                {  
                err("Incorrect logical operator: expected '||'.");  
                }  
                break; 
                case '&':  
                if (pch[1] == '&')  
                {  
                addTk(AND);  
                pch += 2;  
                }  
                else  
                {  
                err("Incorrect logical operator: expected '&&'.");  
                }  
                break; 
            case '(': 
                addTk(LPAR); 
                pch++; 
                break; 
            case ')': 
                addTk(RPAR); 
                pch++; 
                break; 
            case '<': 
                addTk(LESS); 
                pch++; 
                break; 
            case '=': 
                addTk(ASSIGN); 
                pch++; 
                break; 
            case '+': 
                addTk(ADD); 
                pch++; 
                break; 
            case '"': 
                start = ++pch; 
                while (*pch != '"' && *pch != '\0') pch++; 
                if (*pch == '\0') {
                    err("String not ended.");
                }
                    tk = addTk(STR); 
                    strncpy(tk->text, start, pch - start); 
                    tk->text[pch - start] = '\0'; 
                    pch++; 
                break; 
            case '#':  
                while (*pch != '\n' && *pch != '\0')  
                pch++;  
                if (*pch == '\0')  
                {  
                err("Comment ended unexpectedly before end of file.");  
                }  
                break; 
            default: 
                if (isalpha(*pch) || *pch == '_') { 
                    start = pch; 
                    while (isalnum(*pch) || *pch == '_') pch++; 
                    char *text = copyn(buf, start, pch); 
                    if (strcmp(text, "int") == 0) addTk(TYPE_INT); 
                    else if (strcmp(text, "var") == 0) addTk(VAR); 
                    else if (strcmp(text, "function") == 0) addTk(FUNCTION); 
                    else if (strcmp(text, "if") == 0) addTk(IF); 
                    else if (strcmp(text, "else") == 0) addTk(ELSE); 
                    else if (strcmp(text, "while") == 0) addTk(WHILE); 
                    else if (strcmp(text, "end") == 0) addTk(END); 
                    else if (strcmp(text, "return") == 0) addTk(RETURN); 
                    else if (strcmp(text, "real") == 0) addTk(TYPE_REAL);
                    else if (strcmp(text, "str") == 0) addTk(TYPE_STR);
                    else { 
                        tk = addTk(ID); 
                        strcpy(tk->text, text);
						} 
                } else if (isdigit(*pch)) { 
                    start = pch; 
                    while (isdigit(*pch)) pch++; 
                    if (*pch == '.') { 
                        pch++;
                     if(!isdigit(*pch))
                     err("No digit after '.'."); 
                        while (isdigit(*pch)) pch++; 
                        tk = addTk(REAL); 
                    } else { 
                        tk = addTk(INT); 
                    } 
                    copyn(tk->text, start, pch); 
                } else { 
                    err("invalid char: %c (%d)", *pch, *pch); 
                } 
                break; 
        } 
    } 
} 
 
void showTokens() { 
    for (int i = 0; i < nTokens; i++) { 
        Token *tk = &tokens[i]; 
        printf("%d ", tk->line); 
        switch (tk->code) { 
            case ID: 
                printf("ID:%s\n", tk->text); 
                break; 
            case TYPE_INT: 
                printf("TYPE_INT\n"); 
                break; 
            case VAR: 
                printf("VAR\n"); 
                break; 
            case FUNCTION: 
                printf("FUNCTION\n"); 
                break; 
            case IF: 
                printf("IF\n"); 
                break; 
            case ELSE: 
                printf("ELSE\n"); 
                break; 
            case WHILE: 
                printf("WHILE\n"); 
                break; 
            case END: 
                printf("END\n"); 
                break; 
            case RETURN: 
                printf("RETURN\n"); 
                break; 
            case FINISH: 
                printf("FINISH\n"); 
                break; 
            case COMMA: 
                printf("COMMA\n"); 
                break; 
            case COLON: 
                printf("COLON\n"); 
                break; 
            case SEMICOLON: 
                printf("SEMICOLON\n"); 
                break; 
            case LPAR: 
                printf("LPAR\n"); 
                break; 
            case RPAR: 
                printf("RPAR\n"); 
                break; 
            case LESS: 
                printf("LESS\n"); 
                break; 
            case ASSIGN: 
                printf("ASSIGN\n"); 
                break; 
            case ADD: 
                printf("ADD\n"); 
                break; 
            case INT: 
                printf("INT:%s\n", tk->text); 
                break; 
            case REAL: 
                printf("REAL:%s\n", tk->text); 
                break; 
            case STR: 
                printf("STR:%s\n", tk->text); 
                break; 
            default: 
                printf("UNKNOWN\n"); 
                break; 
        } 
    } 
}