%include { 

#include "grammar.h"
#include <string.h>
#include <stdlib.h>
#include <string>
#include <cassert>
#include <iostream>
#include "cJSON.h"


using namespace std;
int get_token_id (char*);
const char *getValue (cJSON *token);
const char *getLine (cJSON *token);
cJSON *unary (char *fname, cJSON *a); 
cJSON *binary (char *fname, cJSON *a, cJSON *b); 
cJSON *ternary (char *fname, cJSON *a, cJSON *b, cJSON *c); 
char *linenumber;
char *curtoken;
char *curtype;
}  
   
   
%code {

using namespace std;
typedef struct {char *value; int line;} token;

token* create_token (char *value, int line) {
	token *t = (token*) malloc (sizeof (token));
	t->value = strdup (value);
	t->line = line;
	return t;
}

const char * getValue (cJSON* token) {
	return cJSON_GetObjectItem (token, "value")->valuestring;
}


const char * getLine (cJSON* token) {
	return cJSON_GetObjectItem (token, "line")->valuestring;
}


int main(int argc, char* argv[]) {
	char *result;
	std::string line;
	std::string input = "";
	while (std::getline(std::cin, line)) {
      input += line + "\n";
    }
	if (input == "") {
		cout << "Empty input";
		exit(0);
	}
	
	cJSON *root = cJSON_Parse(input.c_str());
	
	if (!root) {
		cout << "JSON invalid\n";
		exit(0);
	}
	
	void* pParser = ParseAlloc (malloc);
	int num = cJSON_GetArraySize (root);
	
	for (int i = 0; i < num; i++ ) {
	
		// Knoten im Token-Stream auslesen
		cJSON *node = cJSON_GetArrayItem(root,i);
		
		char *line = cJSON_GetArrayItem(node,0)->valuestring;
		char *type = cJSON_GetArrayItem(node,1)->valuestring;
		char *value = cJSON_GetArrayItem(node,2)->valuestring;
		
		cJSON *tok = cJSON_CreateObject();
		cJSON_AddStringToObject(tok, "value", value);
		cJSON_AddStringToObject(tok, "line", line);

		linenumber = line;
		curtoken = value;
		curtype = type;
		// THE und Kommentare werden ueberlesen
		if (strcmp(type, "THE") == 0) continue;
		if (strcmp(type, "COMMENT") == 0) continue;
		if (strcmp(type, "MCOMMENT") == 0) continue;
		
		int tokenid = get_token_id (type);
		Parse (pParser, tokenid, tok);
		
	}
	Parse (pParser, 0, 0);
    ParseFree(pParser, free );
}




/////////////////////// 
/////////////////////// 
// TOKENS
///////////////////////
/////////////////////// 

int get_token_id (char *token) {
	if (strcmp(token, "AMPERSAND") == 0) return AMPERSAND;
	if (strcmp(token, "ASSIGN") == 0) return ASSIGN;
	if (strcmp(token, "BOOLTOKEN") == 0) return BOOLTOKEN;
	if (strcmp(token, "COMMA") == 0) return COMMA;
	if (strcmp(token, "COS") == 0) return COS;
	if (strcmp(token, "COUNT") == 0) return COUNT;
	if (strcmp(token, "DIVIDE") == 0) return DIVIDE;
	if (strcmp(token, "FIRST") == 0) return FIRST;
	if (strcmp(token, "IDENTIFIER") == 0) return IDENTIFIER;
	if (strcmp(token, "IS") == 0) return IS;
	if (strcmp(token, "LPAR") == 0) return LPAR; 
	if (strcmp(token, "RPAR") == 0) return RPAR;
	if (strcmp(token, "LSPAR") == 0) return LSPAR;
	if (strcmp(token, "RSPAR") == 0) return RSPAR;
	if (strcmp(token, "LIST") == 0) return LIST;
	if (strcmp(token, "MINUS") == 0) return MINUS;
	if (strcmp(token, "NOT") == 0) return NOT;
	if (strcmp(token, "NOW") == 0) return NOW;
	if (strcmp(token, "NUMBER") == 0) return NUMBER;
	if (strcmp(token, "NUMTOKEN") == 0) return NUMTOKEN;
	if (strcmp(token, "NULLTOKEN") == 0) return NULLTOKEN;
	if (strcmp(token, "OF") == 0) return OF;
	if (strcmp(token, "PLUS") == 0) return PLUS;
	if (strcmp(token, "POWER") == 0) return POWER;
	if (strcmp(token, "SEMICOLON") == 0) return SEMICOLON;
	if (strcmp(token, "SIN") == 0) return SIN;
	if (strcmp(token, "STRTOKEN") == 0) return STRTOKEN;
	if (strcmp(token, "TIME") == 0) return TIME;
	if (strcmp(token, "TIMES") == 0) return TIMES;
	if (strcmp(token, "TIMETOKEN") == 0) return TIMETOKEN;
	if (strcmp(token, "TRACE") == 0) return TRACE;
	if (strcmp(token, "WRITE") == 0) return WRITE;
	
	printf ("{\"error\" : true, \"message\": \"UNKNOWN TOKEN TYPE %s\"}\n", token);
	exit(0);
} 
  


cJSON* unary (char* fname, cJSON* a) 
{
	cJSON *res = cJSON_CreateObject(); 
	cJSON *arg = cJSON_CreateArray();
	cJSON_AddItemToArray(arg, a);
	cJSON_AddStringToObject(res, "type", fname);
	cJSON_AddItemToObject(res, "arg", arg);
	return res;
} 

cJSON* binary (char *fname, cJSON *a, cJSON *b) 
{
	cJSON *res = cJSON_CreateObject(); 
	cJSON *arg = cJSON_CreateArray();
	cJSON_AddItemToArray(arg, a); 
	cJSON_AddItemToArray(arg, b);
	cJSON_AddStringToObject(res, "type", fname); 
	cJSON_AddItemToObject(res, "arg", arg);
	return res;
}

cJSON* ternary (char *fname, cJSON *a, cJSON *b, cJSON *c) 
{
	cJSON *res = cJSON_CreateObject(); 
	cJSON *arg = cJSON_CreateArray();
	cJSON_AddItemToArray(arg, a); 
	cJSON_AddItemToArray(arg, b); 
	cJSON_AddItemToArray(arg, c);
	cJSON_AddStringToObject(res, "type", fname); 
	cJSON_AddItemToObject(res, "arg", arg);
	return res;
}

}
   
%syntax_error {
  printf ("{\"error\" : true, \"message\": \"Syntax Error: Compiler reports unexpected token \\\"%s\\\" of type \\\"%s\\\" in line %s\"}\n", curtoken, curtype, linenumber);
  exit(0);
} 
  
%token_type {cJSON *}
%default_type {cJSON *} 

/////////////////////// 
/////////////////////// 
// PRECEDENCE
///////////////////////
/////////////////////// 
// je weiter unten, desto bindet der Operator stärker

%right	   COUNT FIRST .
%left 	   COMMA .
%left 	   AMPERSAND .
%left 	   PLUS MINUS .
%left 	   TIMES DIVIDE .
%right     POWER .
%right	   SIN COS .
%nonassoc  IS .

/////////////////////// 
// CODE
///////////////////////

code ::= statementblock(sb) . 
{
	printf (cJSON_Print(sb)); 
}  

/////////////////////// 
// STATEMENTBLOCK
///////////////////////


statementblock(sb) ::= .
{
	cJSON *res = cJSON_CreateObject();
	cJSON_AddStringToObject(res, "type", "STATEMENTBLOCK");
	cJSON *arg = cJSON_CreateArray();
	cJSON_AddItemToObject(res, "statements", arg); 
	sb = res;
}

statementblock(sb) ::= statementblock(a) statement(b) .
{
	cJSON_AddItemToArray(cJSON_GetObjectItem ( a, "statements"), b);
	sb = a;
}

///////////////////////////
// WRITE
///////////////////////////

statement(r) ::= WRITE ex(e) SEMICOLON .
{
	cJSON *res = cJSON_CreateObject(); 
	cJSON_AddStringToObject(res, "type", "WRITE"); 
	cJSON_AddItemToObject(res, "arg", e); 
	r = res; 
}

///////////////////////////
// WRITE TIME
///////////////////////////

statement(r) ::= WRITE TIME OF ex(e) SEMICOLON .
{
	cJSON *res = cJSON_CreateObject(); 
	cJSON_AddStringToObject(res, "type", "WRITE_TIME"); 
	cJSON_AddItemToObject(res, "arg", e); 
	r = res; 
}

statement(r) ::= WRITE TIME ex(e) SEMICOLON .
{
	cJSON *res = cJSON_CreateObject(); 
	cJSON_AddStringToObject(res, "type", "WRITE_TIME"); 
	cJSON_AddItemToObject(res, "arg", e); 
	r = res; 
}

///////////////////////////
// TRACE
///////////////////////////

statement(r) ::= TRACE(l) ex(e) SEMICOLON .
{
	cJSON *res = cJSON_CreateObject(); 
	cJSON_AddStringToObject(res, "type", "TRACE"); 
	cJSON_AddStringToObject(res, "line", getLine(l)); 
	cJSON_AddItemToObject(res, "arg", e); 
	r = res; 
}

///////////////////////////
// TRACE TIME
///////////////////////////

statement(r) ::= TRACE(l) TIME OF ex(e) SEMICOLON .
{
	cJSON *res = cJSON_CreateObject(); 
	cJSON_AddStringToObject(res, "type", "TRACE_TIME"); 
	cJSON_AddStringToObject(res, "line", getLine(l));
	cJSON_AddItemToObject(res, "arg", e); 
	r = res; 
}

statement(r) ::= TRACE(l) TIME ex(e) SEMICOLON .
{
	cJSON *res = cJSON_CreateObject(); 
	cJSON_AddStringToObject(res, "type", "TRACE_TIME");
	cJSON_AddStringToObject(res, "line", getLine(l));
	cJSON_AddItemToObject(res, "arg", e); 
	r = res; 
}

///////////////////////////
// ASSIGN
///////////////////////////

statement(r) ::= IDENTIFIER(i) ASSIGN ex(e) SEMICOLON .
{
	cJSON *res = cJSON_CreateObject(); 
	cJSON_AddStringToObject(res, "type", "ASSIGN");
	cJSON_AddStringToObject(res, "varname", getValue(i)); 
	cJSON_AddItemToObject(res, "arg", e); 
	r = res; 
}

///////////////////////////
// ASSIGN TIME
///////////////////////////

statement(r) ::= TIME OF IDENTIFIER(i) ASSIGN ex(t) SEMICOLON .
{
	cJSON *res = cJSON_CreateObject(); 
	cJSON_AddStringToObject(res, "type", "ASSIGN_TIME");
	cJSON_AddStringToObject(res, "varname", getValue(i)); 
	cJSON_AddItemToObject(res, "arg", t); 
	r = res; 
}

statement(r) ::= TIME IDENTIFIER(i) ASSIGN ex(t) SEMICOLON .
{
	cJSON *res = cJSON_CreateObject(); 
	cJSON_AddStringToObject(res, "type", "ASSIGN_TIME");
	cJSON_AddStringToObject(res, "varname", getValue(i)); 
	cJSON_AddItemToObject(res, "arg", t); 
	r = res; 
}

///////////////////////////
// EXPRESSIONS
///////////////////////////

// ()
ex(r) ::= LPAR ex(a) RPAR .    
{ 
	r = a; 
}

// Variable
ex(r) ::= IDENTIFIER(a) .      
{ 
	cJSON *res = cJSON_CreateObject(); 
	cJSON_AddStringToObject(res, "type", "IDENTIFIER"); 
	cJSON_AddStringToObject(res, "name", getValue(a)); 
	cJSON_AddStringToObject(res, "line", getLine(a)); 
	r = res; 
}

/////////////////
// Lists
/////////////////

// Empty list []
ex(r) ::= LSPAR RSPAR .    
{ 
	cJSON *res = cJSON_CreateObject();
	cJSON_AddStringToObject(res, "type", "EMPTYLIST");
	r = res; 
}

// [1]
ex(r) ::= LSPAR exlist(a) RSPAR .    
{ 
	cJSON *res = cJSON_CreateObject();
	cJSON_AddStringToObject(res, "type", "LIST");
	cJSON_AddItemToObject(res, "elements", a); 
	r = res; 
}

// [1] = 1
exlist(r) ::= ex(a) .
{
	cJSON *arg = cJSON_CreateArray();
	cJSON_AddItemToArray(arg, a);
	r = arg;
}

// [1] = [1], 1
exlist(r) ::= exlist(a) COMMA ex(b) .
{
	cJSON_AddItemToArray(a, b);
	r = a;
}

///////////////////////////
// Terminal nodes
///////////////////////////

ex(r) ::= NUMTOKEN (a).        
{ 
	cJSON *res = cJSON_CreateObject();
	cJSON_AddStringToObject(res, "type", "NUMTOKEN"); 
	cJSON_AddStringToObject(res, "value", getValue(a)); 
	r = res; 
} 

ex(r) ::= TIMETOKEN (a).        
{ 
	cJSON *res = cJSON_CreateObject();
	cJSON_AddStringToObject(res, "type", "TIMETOKEN"); 
	cJSON_AddStringToObject(res, "value", getValue(a)); 
	r = res; 
} 


ex(r) ::= STRTOKEN (a).        
{ 
	cJSON *res = cJSON_CreateObject();
	cJSON_AddStringToObject(res, "type", "STRTOKEN"); 
	cJSON_AddStringToObject(res, "value", getValue(a)); 
	r = res; 
}

ex(r) ::= BOOLTOKEN (a).        
{ 
	cJSON *res = cJSON_CreateObject();
	cJSON_AddStringToObject(res, "type", "BOOLTOKEN"); 
	cJSON_AddStringToObject(res, "value", getValue(a)); 
	r = res; 
}

ex(r) ::= NOW .      
{ 
	cJSON *res = cJSON_CreateObject(); 
	cJSON_AddStringToObject(res, "type", "NOW"); 
	r = res; 
}

ex(r) ::= NULLTOKEN .      
{ 
	cJSON *res = cJSON_CreateObject(); 
	cJSON_AddStringToObject(res, "type", "NULLTOKEN"); 
	r = res; 
}

///////////////////////////
// Operations
///////////////////////////

/////////////////
// Unary
/////////////////

ex(r) ::= ex(a) IS LIST .                               
{r = unary ("IS_LIST", a); }

ex(r) ::= ex(a) IS NOT LIST .                               
{r = unary ("IS_NOT_LIST", a); }

ex(r) ::= ex(a) IS NUMBER .                               
{r = unary ("IS_NUMBER", a); }

ex(r) ::= COUNT ex(a) .                               
{r = unary ("COUNT", a); }

ex(r) ::= FIRST ex(a) .                               
{r = unary ("FIRST", a); }

ex(r) ::= SIN ex(a) .                               
{r = unary ("SIN", a); }

ex(r) ::= COS ex(a) .                               
{r = unary ("COS", a); }

/////////////////
// Binary
/////////////////

ex(r) ::= ex(a) AMPERSAND ex(b) .                                
{r = binary ("AMPERSAND", a, b); }

ex(r) ::= ex(a) PLUS ex(b) .                                
{r = binary ("PLUS", a, b); }

ex(r) ::= ex(a) MINUS ex(b) .                               
{r = binary ("MINUS", a, b); }

ex(r) ::= ex(a) TIMES ex(b) .                               
{r = binary ("TIMES", a, b); }

ex(r) ::= ex(a) DIVIDE ex(b) .                              
{r = binary ("DIVIDE", a, b); }

ex(r) ::= ex(a) POWER ex(b) .                               
{r = binary ("POWER", a, b); }

/////////////////
// Ternary
/////////////////


                                           