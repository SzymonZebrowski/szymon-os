float pow(float a, int b){
    int result=a;
    if (b==0) return 1;
    else{
        int i=1;
        if(b<0) a=1/a;

        for(i; i<b; i++){
            result*=a;
        }
    }
    return a;
}

int factorial(int n){
    int sum=1, i=1;
    for(i; i<=n; i++){
        sum*=i;
    }
    return sum;
};

float log(float n){
    int i=0;
    float sum=0;
    for(i; i<20; i++){
        sum += (1/(2*i+1))*pow((n-1)/(n+1), 2*i+1);
    }
    return 2*sum;
}

float log2(float n){
    return log(n)/log(2);
}

float log10(float n){
        return log(n)/log(10);
}

