

#include<stdio.h> 
#define INT_BITS 32 
  
/*Function to right rotate n by d bits*/
int rightRotate(int n, unsigned int d){ 
   /* In n>>d, first d bits are 0. To put last 3 bits of at  
     first, do bitwise or of n>>d with n <<(INT_BITS - d) */
   return (n >> d)|(n << (INT_BITS - d)); 
} 

int trialDivision(int n){
    int f = 1;
    int ctr = 0;
    while(n >= f){
        if (n % f == 0){
            //printf("Factor: %i %i\n", f, n);
            ctr++;
        } 
        f++;
    }
    //printf("Total factors %i\n", ctr);
    return ctr;
}
  
/* Driver program to test above functions */
int main(){ 
    //Spawn 32 threads
    int nThreads = 32;
    int input = 12345;
    int f, c;
    int count = 0;
    for (int i = 0; i < nThreads; i++){
        f =  rightRotate(input, i);
        c =  trialDivision(f);
        printf("Right Rotation of %d by %d is ", input, i); 
        printf("%d which has %d factors\n", f, c);  
        count += c;      
    }
    printf("count: %d", count);
}  


