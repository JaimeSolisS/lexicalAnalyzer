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
	char cspecial[3];
    char c;
	char cclass;
	int i;

	const char* Soperators[] = {"+", "-", "*", "/", ":=", "=", "<>", "<", "<=", ">=", ">", "^", "."};
    const char* Sdelimeters[] = { ",", ";", ":", "(", ")", "[", "]", ".."};

    for(i = 0; i < 4; i++) {
      c = peekchar();
      cclass = CHARCLASS[c];
      
        if(cclass == SPECIAL) {
          cspecial[i] = getchar();
          if(cspecial[i] == ':' && peekchar() == '=')
            cspecial[++i] = getchar();
          else if(cspecial[i] == '<' && (peekchar() == '>' || peekchar() == '='))
            cspecial[++i] = getchar();
          else if(cspecial[i] == '>' && peekchar() == '=')
            cspecial[++i] = getchar();
          else if(cspecial[i] == '.' && peekchar() == '.')
            cspecial[++i] = getchar();
          i++;
          break;
        }
        else break;
      
    }
    cspecial[i] = '\0';

	// Delimeters 
    for(int i = 0; i < 8; i++) {
      if(strcmp(cspecial,Sdelimeters[i]) == 0) {
        tok->tokentype = DELIMITER;
        tok->whichval = i + 1;
      }
    }

    // Operators 
    for(int i = 0; i < 13; i++) {
      if(strcmp(cspecial,Soperators[i]) == 0) {
        tok->tokentype = OPERATOR;
        tok->whichval = i + 1;
      }
    }
	return (tok);

  }

  	// Create look-up tables for decimal conversions, populated inside TOKEN number
	int isPDC =0; 
	double posdc[100];
	int isNDC= 0; 
	double negdc[100];

/* Get and convert unsigned numbers of all types. */
TOKEN number (TOKEN tok)
  { 
    int  c, charval;
	long intNum = 0; // int values
    double doubleNum = 0; // double values -> 10 digits
	int expNum = 0; // value of exponent

	int intCounter= 0; //Counter before decimal point
	int decimalCounter = 1; //decimal conversion counter
	int exponentCounter = 0; //exponent counter

	// 1 true, 0 false
	int nonZero = 0;  
	int intError = 0;  
	int isReal = 0; //double 
	int expPos = 1; // positive
	int floatError = 0; 

	// Integer numbers
    while ( (c = peekchar()) != EOF && CHARCLASS[c] == NUMERIC)
      {  
		c = getchar();
        charval = (c - '0');
        intNum = intNum * 10 + charval; //MAGIC Formula
		if(charval != 0){
			nonZero = 1;
			if(intCounter >= 8)  //From Int to Double type
				charval = 0;
			doubleNum = doubleNum * 10 + charval;
			intCounter++;
		}
    }

	exponentCounter = intCounter - 1; //always one minus the int counter to float format

	 // int error 
    if(intCounter > 10 || intNum < 0 || intNum > 2147483647) // Catch Integer number out of range
		intError = 1; 

	//Decimal conversion 
	if (isPDC==0){ //Create positive array
		posdc[0] = 1;
		for(int i=1; i < 100; i++)
			posdc[i] = posdc[i-1] * 10;
		isPDC=1; 
	}

	if (isNDC==0){ //Create negative array
		negdc[0] = 1;
		for(int i=1; i < 100; i++)
			negdc[i] = negdc[i-1] * .1;
		isNDC=1; 
		} 

	// After decimal point
    if((c = peekchar()) == '.' && CHARCLASS[peek2char()] == NUMERIC)
	{
        getchar(); //ignore '.' 
		isReal = 1;
		double dcharval;

		while ( (c = peekchar()) != EOF && CHARCLASS[c] == NUMERIC)
		{  
			c = getchar();
			dcharval = (c - '0');

			if(dcharval != 0){
				nonZero = 1;
			}
			if(nonZero == 0){ //if zero, move decimal point to right for x.e^-y  
				exponentCounter--;
			}
			 /* 
			 for(int i = 0; i <= decimalCounter++; i++)
						dcharval *= .1;
			 Took too long, used look up tables instead  */
			dcharval *= negdc[decimalCounter++]; //turn dcharval to decimal equivalent
			
			if(nonZero == 1 && intCounter < 8){ //Just care for8 significant digits
				intCounter++;
				doubleNum += dcharval;
			}
		}
	}
	
	if(exponentCounter > 0){
		doubleNum *= negdc[exponentCounter]; //transform to e notation
	}
	else if(exponentCounter < 0){
		doubleNum *= posdc[-exponentCounter];
	}
	// read exponent
	if((c = peekchar()) == 'e') 
	{
		getchar(); // ignore  'e'
		isReal = 1;
		nonZero =0;
		intCounter =0;
			
		c = peekchar();
		if(c == '+'){
			getchar(); //ignore '+'
		} else if(c == '-'){
			getchar(); //ignore '-'
			expPos = 0;
		}
			
			
		// exponent value
		while ( (c = peekchar()) != EOF && CHARCLASS[c] == NUMERIC)
        {   
			c = getchar();
            charval = (c - '0');
			if(charval != 0)
			    nonZero = 1;
			if(nonZero == 1)
				expNum = expNum * 10 + charval;
        }
        
        if(expNum < 0)
			floatError = 1;
    	if(expPos == 0)
			expNum = -expNum;
	}

    tok->tokentype = NUMBERTOK;

    if(isReal == 0){
		if(intError == 1){
	        printf("Integer number out of range\n");
	    }
        tok->basicdt = INTEGER;
        tok->intval = intNum;
	}else{
		expNum += exponentCounter; //  exponent value	  
		if(expNum > 38 || expNum < -38 ||  floatError == 1){
	        doubleNum = 0;
	        printf("Floating number out of range\n");
	    }else{
		    if(expNum > 0) //positive
				doubleNum *= posdc[expNum];
			else if(expNum < 0) //negative
				doubleNum *= negdc[-expNum];
	      	}
		tok->basicdt = REAL;
		tok->realval = doubleNum;
	}
    return (tok);
}

