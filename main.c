#include <stdio.h> 
#include <stdlib.h> 
 
#include "lexer.h" 
#include "utils.h" 

#include "parser.h"
#include "ad.h"
#include "at.h"
 
int main(){ 
 
    char *inbuf=loadFile("C:\\Users\\Tup20\\Downloads\\files\\test\\1.q"); 
    tokenize(inbuf); 
    //showTokens(); 
    free(inbuf); 
    parse(); 
    return 0; 
 
 
}