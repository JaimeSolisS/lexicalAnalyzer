/*Name: Jaime Solis
EID: js96986
*/
/* lex1.c         14 Feb 01; 31 May 12; 11 Jan 18       */

/* This file contains code stubs for the lexical analyzer.
   Rename this file to be lexanc.c and fill in the stubs.    */

/* Copyright (c) 2018 Gordon S. Novak Jr. and
   The University of Texas at Austin. */

/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "token.h"
#include "lexan.h"

/* This file will work as given with an input file consisting only
   of integers separated by blanks:
   make lex1
   lex1
   12345 123    345  357
   */

/* Skip blanks and whitespace.  Expand this function to skip comments too. */
void skipblanks ()
  {
      int c;
      int cNext; 
      while ((c = peekchar()) != EOF
             && (c == ' ' || c == '\n' || c == '\t' //whitespace
             || c == '{' || (c == '(' && (cNext = peek2char()) == '*'))) // comment
      {
        if(c == '{'){
        	while (1){
            	getchar();
            	if (!((c = peekchar()) != EOF && c != '}'))
              		break; 
          	}
        
        }else if(c == '('){
			getchar();
            while (1){
            	getchar();
              	c = cNext; 
              	cNext = peek2char();
              	if (!(cNext != EOF && (c != '*' || cNext != ')')))
                	break;
            }
            getchar();
            c = cNext;
        } 
        if(c != EOF)
        	getchar();
      }      
    }

/* Get identifiers and reserved words */
TOKEN identifier (TOKEN tok)
  {
    int c;
    int hasNumeric = 0;
	int isReserved=0; 
	int lenght = 0;
	char word[16];

	const char* operators[] = {"and", "or", "not", "div", "mod", "in"};
    const char* reserved[] = { "array", "begin", "case", "const", "do",
                                  "downto", "else", "end", "file", "for",
                                  "function", "goto", "if", "label", "nil",
                                  "of", "packed", "procedure", "program", "record",
                                  "repeat", "set", "then", "to", "type",
                                  "until", "var", "while", "with"
							};

	while(1){
    	word[lenght++] = getchar();
		c = peekchar();
		if(CHARCLASS[c] == NUMERIC)
	    	hasNumeric = 1;
		if (!(c != EOF && lenght < 15 && (CHARCLASS[c] == ALPHA || CHARCLASS[c] == NUMERIC)) )
			break; 
    }
		
	while(c != EOF && (CHARCLASS[c] == ALPHA || CHARCLASS[c] == NUMERIC)) {
		getchar();				
		c = peekchar();
	}
	
	word[lenght] = '\0'; //end of word
		
    // Reserved words 
    for(int i = 0; i < 29; i++) {
      if(strcmp(word, reserved[i]) == 0) {
		isReserved=1;
        tok->tokentype = RESERVED;
        tok->whichval = i + 1;
      }
    }

	// Worded operators 
    for(int i = 0; i < 6; i++) {
      if(strcmp(word, operators[i]) == 0) {
		isReserved=1;
        tok->tokentype = OPERATOR;
        tok->whichval = i + (OR - OPERATOR_BIAS) - 1;
      }
    }

	// identifier has a numeric -> No reserved
	if(hasNumeric == 1 || isReserved==0){
		tok->tokentype = IDENTIFIERTOK;
   		strncpy(tok->stringval, word, 16); // should this be 15 or 16?
	}
	return (tok);

  }

TOKEN getstring (TOKEN tok)
  {
    getchar();
	int c = peekchar();
	int cNext = peek2char();
	int lenght = 0; 
	char word[16]; 

	while(c != EOF){
		if(c == '\'') {	
			if(cNext == '\'')
				getchar(); 
			else break;
		}
		
		if(lenght < 15)
			word[lenght++] = getchar();
		else getchar();
		
		c = peekchar();
		cNext = peek2char();
		}

		getchar();
		word[lenght] = '\0';

		tok->tokentype = STRINGTOK;
    	strncpy(tok->stringval, word, 16); 
    	return (tok);
}
    

TOKEN special (TOKEN tok)
  {
    }

/* Get and convert unsigned numbers of all types. */
TOKEN number (TOKEN tok)
  { long num;
    int  c, charval;
    num = 0;
    while ( (c = peekchar()) != EOF
            && CHARCLASS[c] == NUMERIC)
      {   c = getchar();
          charval = (c - '0');
          num = num * 10 + charval;
        }
    tok->tokentype = NUMBERTOK;
    tok->basicdt = INTEGER;
    tok->intval = num;
    return (tok);
  }
