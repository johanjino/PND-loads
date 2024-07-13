void test(int *a, int *b, int i);

int main(int argc, char **argv) {
	int a[100]; 
	int b[100];
	for (int i=0; i < argc; i++) {
		a[i] = i*i;
		b[i] = i;
	}
	if (argv[1]) {
		for (int i=0; i < argc; i++) {
			test(a, b, i);	
			a[i] *= b[i];	
		}
	}

	return a[argc];
}
