#include <stdio.h>
#include <ctype.h>
#include <string.h>

int charClass;
char lexeme[100];
char nextChar;
int lexLen;
int token;
int nextToken;
FILE *in_fp, *fopen();

void addChar();
void getChar();
void getNonBlank();
int lex();

#define LETTER 0
#define DIGIT 1
#define UNKNOWN 99

#define INT_LIT 10
#define IDENT 11
#define ASSIGN_OP 20
#define ADD_OP 21
#define SUB_OP 22
#define MULT_OP 23
#define DIV_OP 24
#define LEFT_PAREN 25
#define RIGHT_PAREN 26
#define FOR_CODE 30
#define IF_CODE 31
#define ELSE_CODE 32
#define WHILE_CODE 33
#define DO_CODE 34
#define INT_CODE 35
#define FLOAT_CODE 36
#define SWITCH_CODE 37

int main()
{
	if((in_fp = fopen("front.in", "r")) == NULL) //front.in파일 open
		printf("ERROR - cannot open front.in\n");
	else {
		getChar();//해당 파일에서 한글자 읽어옴
		do {
			lex();//lex함수 실행
		}	while (nextToken != EOF);//nextToken이 -1이 아닐때까지
	}
}

int lookup(char ch) { //lookup table
	switch (ch) {
		case '(': //읽어온 문자가 '('인 경우
			addChar();//해당 문자 lexeme에 추가
			nextToken = LEFT_PAREN;//nextToken값을 LEFT_PAREN으로 지정
			break;

		case ')': //읽어온 문자가 ')'인 경우
			addChar();
			nextToken = RIGHT_PAREN;
			break;

		case '+'://읽어온 문자가 '+'인 경우
			addChar();
			nextToken = ADD_OP;
			break;

		case '-'://읽어온 문자가 '-'인 경우
			addChar();
			nextToken = SUB_OP;
			break;

		case '*'://읽어온 문자가 '*'인 경우
			addChar();
			nextToken = MULT_OP;
			break;

		case '/'://읽어온 문자가 '/'인 경우
			addChar();
			nextToken = DIV_OP;
			break;

		default: //그 외의 경우
			addChar();
			nextToken = EOF;
			break;
	}
	return nextToken;
}

void addChar() { //lexeme 생성 함수
	if(lexLen <= 98) {
		lexeme[lexLen++] = nextChar;
		lexeme[lexLen] = 0;
	}
	else
		printf("Error - lexeme is too long\n"); //해당 lexeme의 길이가 98보다 클경우 error문구 출력 
}

void getChar() {
	if((nextChar = getc(in_fp)) != EOF) { //파일을 읽는 동안
		if(isalpha(nextChar)) //읽어온 문자가 알파벳인 경우
			charClass = LETTER;
		else if(isdigit(nextChar)) //읽어온 문자가 숫자인 경우
			charClass = DIGIT;
		else charClass = UNKNOWN; //그 외의 경우엔 charClass를 UNKNOWN으로 지정해줌
	}
	else //파일의 끝을 읽었을시에
		charClass = EOF; 
}

void getNonBlank() { //읽어온 문자가 공백일 경우에는 skip해준다
	while(isspace(nextChar))
		getChar();
}

int lex() {
	lexLen = 0;
	getNonBlank();

	switch(charClass) { //charClass변수는 읽은 문자에 대해서 무슨 범주인지 지정해준 변수이다
		case LETTER: //문자일 경우
			addChar();
			getChar();
			while(charClass == LETTER || charClass == DIGIT) { //charClass가 LETTER이거나 DIGIT인 경우에 해당 문자 lexeme배열에 추가
				addChar();
				getChar();
			}
			if(!strcmp("for", lexeme)) //lexeme배열과 for비교시 맞으면 nextToken변수값 설정
				nextToken = FOR_CODE;
			else if(!strcmp("if", lexeme)) //lexeme배열과 if비교시 맞으면 nextToken변수값 설정
				nextToken = IF_CODE;
			else if(!strcmp("else", lexeme)) //lexeme배열과 else비교시 맞으면 nextToken변수값 설정
				nextToken = ELSE_CODE;
			else if(!strcmp("while", lexeme)) //lexeme배열과 while비교시 맞으면 nextToken변수값 설정
				nextToken = WHILE_CODE;
			else if(!strcmp("do", lexeme)) //lexeme배열과 do비교시 맞으면 nextToken변수값 설정
				nextToken = DO_CODE;
			else if(!strcmp("int", lexeme)) //lexeme배열과 int비교시 맞으면 nextToken변수값 설정
				nextToken = INT_CODE;
			else if(!strcmp("float", lexeme)) //lexeme배열과 float비교시 맞으면 nextToken변수값 설정
				nextToken = FLOAT_CODE;
			else if(!strcmp("switch", lexeme)) //lexeme배열과 switch비교시 맞으면 nextToken변수값 설정
				nextToken = SWITCH_CODE;
			else //그 외의 경우에는 변수 lexeme이라고 지정
				nextToken = IDENT;
			break;

		case DIGIT: //해당 문자가 숫자인경우
			addChar();
			getChar();
			while(charClass == DIGIT) {
				addChar();
				getChar();
			}
			nextToken = INT_LIT;
			break;

		case UNKNOWN: //'('나 ')'같은 경우에는 lookup함수 호출
			lookup(nextChar);
			getChar();
			break;

		case EOF: //EOF인 경우에
			nextToken = EOF;
			lexeme[0] = 'E';
			lexeme[1] = 'O';
			lexeme[2] = 'F';
			lexeme[3] = 0;
			break;
	}
	printf("Next token is : %d, Next lexeme is %s\n", nextToken, lexeme); //해당 token과 lexeme을 출력해주는 문구
	return nextToken;
}
