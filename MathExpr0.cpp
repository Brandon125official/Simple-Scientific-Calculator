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
void Push(char o, const char V[], double v) {
	baru = new Token;
	baru->oprt = o;
	strcpy(baru->variable, V);
	baru->value = v;
	baru->Next = top;
	top = baru;
}

bool Pop(char &o, char V[], double &v) {
	if (top == NULL) {
		fprintf(stderr, "Error: pop from empty stack (unbalanced expression)\n");
		return false;
	}
	o = top->oprt;
	strcpy(V, top->variable);
	v = top->value;
	hapus = top;
	top = top->Next;
	delete hapus;
	return true;
}

void insertInfix(char o, const char V[], double v) {
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

void insertPostfix(char o, const char V[], double v) {
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
	for (bantu = head; bantu; bantu = bantu->Next) {
		switch (bantu->oprt) {
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

// Frees an entire linked list, not just the second node.
void freeNotation(struct Token *head) {
	while (head) {
		hapus = head;
		head = head->Next;
		delete hapus;
	}
}

unsigned char priority(char o) {
	switch (o) {
		case '^'            : return 3;
		case '*': case '/'  : return 2;
		case '+': case '-'  : return 1;
		default             : return 0;
	}
}

// '^' is right-associative; '*','/','+','-' are left-associative.
bool isRightAssociative(char o) {
	return o == '^';
}

int main() {
	char mathExpr[256] = "15.5*(a+b)^2/4", sOprnd[33], me;
	int i, j, l;
	bool canLoop;
	double oprnd1, oprnd2, hasil;
	hInfix = tInfix = hPostfix = tPostfix = NULL;
	top = NULL;
	l = strlen(mathExpr);

	/*printf("Masukkan notasi ekspresi matematikanya: ");
	scanf("%255[^\n]", mathExpr);*/

	j = 0;
	for (i = 0; i < l; i++) {
		me = mathExpr[i];
		if (isNumeric(me) || isLower(me) || isUpper(me) || (me == '_')) {
			if (j < (int)sizeof(sOprnd) - 1) {
				sOprnd[j] = me;
				j++;
			} else {
				fprintf(stderr, "Error: operand/variable name too long (max 32 chars)\n");
				return 1;
			}
		} else if (isOprt(me)) {
			if (j > 0) {
				sOprnd[j] = 0;
				if (sOprnd[0] >= '0' && sOprnd[0] <= '9') {
					insertInfix('v', "", atof(sOprnd));
				} else {
					insertInfix('V', sOprnd, 0);
				}
				j = 0;
			}
			insertInfix(me, "", 0);
		} else if (me == ' ' || me == '\t') {
			// ignore whitespace
		} else {
			fprintf(stderr, "Error: unexpected character '%c' in expression\n", me);
			return 1;
		}
	}

	if (j > 0) {
		sOprnd[j] = 0;
		if (sOprnd[0] >= '0' && sOprnd[0] <= '9') {
			insertInfix('v', "", atof(sOprnd));
		} else {
			insertInfix('V', sOprnd, 0);
		}
		j = 0;
	}

	printf("Infix: ");
	printNotation(hInfix);

	for (bantu = hInfix; bantu; bantu = bantu->Next) {
		if (bantu->oprt == 'V') {
			printf("Masukkan nilai %s: ", bantu->variable);
			fflush(stdout);
			if (scanf("%lf", &bantu->value) != 1) {
				fprintf(stderr, "Error: invalid numeric input for %s\n", bantu->variable);
				return 1;
			}
			insertPostfix(bantu->oprt, bantu->variable, bantu->value);
		} else if (bantu->oprt == 'v') {
			insertPostfix(bantu->oprt, bantu->variable, bantu->value);
		} else if (bantu->oprt == '(') {.
			Push(bantu->oprt, bantu->variable, bantu->value);
		} else if (bantu->oprt == ')') {
			while (top != NULL && top->oprt != '(') {
				if (!Pop(me, sOprnd, hasil)) return 1;
				insertPostfix(me, sOprnd, hasil);
			}
			if (top == NULL) {
				fprintf(stderr, "Error: unmatched ')' in expression\n");
				return 1;
			}
			if (!Pop(me, sOprnd, hasil)) return 1; // discard the '('
		} else if (top == NULL) {
			Push(bantu->oprt, bantu->variable, bantu->value);
		} else if (top->oprt == '(') {
			Push(bantu->oprt, bantu->variable, bantu->value);
		} else {
			// Decide whether to push or pop-then-push based on priority/associativity
			if (isRightAssociative(bantu->oprt)) {
				canLoop = (top != NULL) && (priority(bantu->oprt) < priority(top->oprt));
			} else {
				canLoop = (top != NULL) && (priority(bantu->oprt) <= priority(top->oprt));
			}
			while (canLoop) {
				if (!Pop(me, sOprnd, hasil)) return 1;
				insertPostfix(me, sOprnd, hasil);
				if (isRightAssociative(bantu->oprt)) {
					canLoop = (top != NULL) && (priority(bantu->oprt) < priority(top->oprt));
				} else {
					canLoop = (top != NULL) && (priority(bantu->oprt) <= priority(top->oprt));
				}
			}
			Push(bantu->oprt, bantu->variable, bantu->value);
		}
	}

	while (top) {
		if (!Pop(me, sOprnd, hasil)) return 1;
		if (me == '(') {
			fprintf(stderr, "Error: unmatched '(' in expression\n");
			return 1;
		}
		insertPostfix(me, sOprnd, hasil);
	}

	for (bantu = hPostfix; bantu; bantu = bantu->Next) {
		if (bantu->oprt == 'v' || bantu->oprt == 'V') {
			Push(bantu->oprt, bantu->variable, bantu->value);
		} else {
			if (!Pop(me, sOprnd, oprnd2)) return 1;
			if (!Pop(me, sOprnd, oprnd1)) return 1;

			switch (bantu->oprt) {
				case '^': hasil = pow(oprnd1, oprnd2); break;
				case '*': hasil = oprnd1 * oprnd2; break;
				case '/':
					if (oprnd2 == 0.0) {
						fprintf(stderr, "Error: division by zero\n");
						return 1;
					}
					hasil = oprnd1 / oprnd2;
					break;
				case '+': hasil = oprnd1 + oprnd2; break;
				case '-': hasil = oprnd1 - oprnd2; break;
				default:
					fprintf(stderr, "Error: unknown operator '%c'\n", bantu->oprt);
					return 1;
			}
			Push('v', "", hasil);
		}
	}

	printf("Postfix: ");
	printNotation(hPostfix);

	if (!Pop(me, sOprnd, hasil)) return 1;
	printf("Hasil: %0.3f\n", hasil);

	// Clean up all allocated memory
	freeNotation(hInfix);
	freeNotation(hPostfix);
	while (top) {
		double dummy;
		Pop(me, sOprnd, dummy);
	}

	return 0;
}
