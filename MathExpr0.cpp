#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

struct Token {
	char oprt, variable[33];  //33 karena tidak bisa melewati 32 byte
	double value;
	struct Token *Next;
} *hInfix, *tInfix, *hPostfix, *tPostfix, *baru, *bantu, *hapus, *top;

//queue function
void Push(char o, char V[], double v) {
	baru = new Token;
	baru->oprt = o;
	strcpy(baru->variable, V);
	baru->value = v;
	baru->Next = top;
	top = baru;
}

void Pop(char &o, char V[], double &v) {
	o = top->oprt;
	strcpy(V, top->variable);
	v = top->value;
	hapus = top;
	top = top->Next;
	delete hapus;
}

void insertInfix(char o, char V[], double v) {
	baru = new Token;
	baru->oprt = o;
	strcpy(baru->variable, V);
	baru->value = v;
	baru->Next = NULL;    //setiap ada data baru, belakangnya pasti kosong
	if (hInfix) {
		tInfix->Next = baru;
		tInfix = baru;
	} else hInfix = tInfix = baru; 
}

void insertPostfix(char o, char V[], double v) {
	baru = new Token;
	baru->oprt = o;
	strcpy(baru->variable, V);
	baru->value = v;
	baru->Next = NULL; 
	if (hPostfix) {
		tPostfix->Next = baru;
		tPostfix = baru;
	} else hPostfix = tPostfix = baru; 
}

//dengan kesepakatan sendiri
void printNotation(struct Token *head) {
	for(bantu = head; bantu; bantu = bantu = bantu->Next) { 
		switch(bantu->oprt) {            
			case 'V': printf("%s ", bantu->variable); break;
			case 'v': printf("%0.3f ", bantu->value); break;
			default : printf("%c ", bantu->oprt);
		}
	}
	printf("\n");
}

bool isNumeric(char c) {
	return (c >= '0' && c <= '9') || c == '.';
}

bool isOprt(char c) {
	return c == '+' || c == '-' || 
	       c == '*' || c == '/' || 
		   c == '^' || c == '(' || c == ')';
}

bool isUpper(char c) {
	return (c >= 'A' && c <= 'Z');
}

bool isLower(char c) {
	return (c >= 'a' && c <= 'z');
}

void freeNotation(struct Token *head) {
	hapus = head;
	head = head->Next;
	delete head;
}

unsigned char priority(char o) {
	switch(o) {
		case '^'     : return 3;
		case '*': case '/': return 2;
		case '+': case '-': return 1;
		default      : return 0;
	}
}

int main() {
	char mathExpr[256] = "15.5*(a+b)^2/4", sOprnd[33], me;
	int i, j, l;
	bool canLoop;
	double oprnd1, oprnd2, hasil;
	hInfix = tInfix = hPostfix = tPostfix = NULL;
	l = strlen(mathExpr);
	
	/*printf("Masukkan notasi ekspresi matematikanya: ");
	scanf("%[^\n]", mathExpr);
	fflush(stdin);*/
		j = 0;
	for(i = 0; i < l; i++) {
		me = mathExpr[i];
		if(isNumeric(me) || isLower(me) || isUpper(me) || (me == '.') || (me == '_')) {
			sOprnd[j] = me;
			j++;
		} else if(isOprt(me) || (me == '(') || (me == ')')) {
			if(j > 0) {
				sOprnd[j] = 0;
				//printf("%s\n", sOprnd);
				if(sOprnd[0] >= '0' && sOprnd[0] <= '9') {
					insertInfix('v', "", atof(sOprnd));
				} else {
					insertInfix('V', sOprnd, 0);
				}
				j = 0;
			}
			//printf("%c\n", me);
			insertInfix(me, "", 0);
		} else {
			//error
		}
	}
	
	if(j > 0) {
		sOprnd[j] = 0;
		//printf("%s\n", sOprnd);
		if(sOprnd[0] >= '0' && sOprnd[0] <= '9') {
			insertInfix('v', "", atof(sOprnd));
		} else {
			insertInfix('V', sOprnd, 0);
		}
		j = 0;
	}
			
	printf("Infix: ");
	printNotation(hInfix);
	
	for(bantu = hInfix; bantu; bantu = bantu->Next) {
		if(bantu->oprt == 'V') {
			printf("Masukkan nilai %s: ", bantu->variable);
			scanf("%Lf", &bantu->value);
			insertPostfix(bantu->oprt, bantu->variable, bantu->value);
		} else if(bantu->oprt == 'v') {
			insertPostfix(bantu->oprt, bantu->variable, bantu->value);
		} else if(top == NULL) {
			Push(bantu->oprt, bantu->variable, bantu->value);
		} else if(bantu->oprt == '(') {
			Push(bantu->oprt, bantu->variable, bantu->value);
		} else if(top->oprt == '(') {
			Push(bantu->oprt, bantu->variable, bantu->value);
		} else if(bantu->oprt == ')') {
			while(top->oprt != '(') {
				Pop(me, sOprnd, hasil);
				insertPostfix(me, sOprnd, hasil);
			}
			Pop(me, sOprnd, hasil);
		} else if(priority(bantu->oprt) > priority(top->oprt)) {
			Push(bantu->oprt, bantu->variable, bantu->value);
		} else {
			if(top == NULL) {
				canLoop = false;
			} else {
				canLoop = priority(bantu->oprt) <= priority(top->oprt);
			}
			while (canLoop) {
				Pop(me, sOprnd, hasil);
				insertPostfix(me, sOprnd, hasil);
				//recheck
				if(top == NULL) {
					canLoop = false;
				} else {
					canLoop = priority(bantu->oprt) <= priority(top->oprt);
				}
			}
			Push(bantu->oprt, bantu->variable, bantu->value);
		}
	}
	
	while(top) {
		Pop(me, sOprnd, hasil);
		insertPostfix(me, sOprnd, hasil);
	}
	
	
	
	for(bantu = hPostfix; bantu; bantu = bantu->Next) {
		if(bantu->oprt == 'v' || bantu->oprt == 'V') {
			Push(bantu->oprt, bantu->variable, bantu->value);
		} else {
			Pop(me, sOprnd, oprnd2);
			Pop(me, sOprnd, oprnd1);
			
			switch(bantu->oprt) {
				case '^': hasil = pow(oprnd1, oprnd2); break;
				case '*': hasil = oprnd1 * oprnd2; break;
				case '/': hasil = oprnd1 / oprnd2; break;
				case '+': hasil = oprnd1 + oprnd2; break;
				case '-': hasil = oprnd1 - oprnd2; break;
			}
			Push('v', "", hasil);
		}
	}
	
	printf("Postfix: ");
	printNotation(hPostfix);
	
	Pop(me, sOprnd, hasil);
	printf("Hasil: %0.3f", hasil);
	freeNotation(hPostfix);
			
	return 0;
}
