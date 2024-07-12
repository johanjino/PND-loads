void test(long * restrict a, long * restrict b, int n){
    for (int p=0; p < n/2; p++){
        for (int i=1; i < n; i++){
            a[i] += b[i-1];
        }
    }
}
