#include "stdafx.h"

#include "old.h"


void Old()
{
    int   gameNum   =   1; //   �ƾֺ�   
    int   cards[168],   order[53];   
    int   i,j,k;   

    for   (i=0;   i<168;   i++)   
        cards[i]   =   -1;   
    for   (i=0;   i<=52;   i++)   
        order[i]   =   i;   

    srand(gameNum);   
    for   (i=0;   i<52;   i++)   
    {   
        j   =   rand()   %   order[52];   
        k   =   i/8   +   (i   &   7)   *   21;   
        cards[k]   =   order[j];   
        order[j]   =   order[52-i-1];   
        --order[52];   
    }   

    for   (i=0;i<168;i++)   
    {   
        if   (cards[i]   ==   -1)   
            printf("%-10d",   -1);   
        else   
        {   
            k   =   ((cards[i]+1   %   13)+3)   /   4;   
            k   =   k?k:13; //   ���������С   
            printf("%-2d(%-2X)         ",   k,   cards[i]);   
        }   
    }   
}

//  1��ѹ���˷�������   
//2��ȥ����ȡ�ƶ������һ��λ��   
//    3����ȷ�˷����Ǹ�8������   
//    4����ʾͨ�õ�����   
//    5���㷨������ȷ��   

void   Old1()   
{   

    int   gameNum   =   1;//   �ƾֺ�   
    int   cards[64];//   �����ƾ�   
    int   order[53];//   ȡ�ƶ���   

    int   to;//   ����λ��   
    int   from;//   ȡ��λ��   

    int   i;   

    char   hand[5]="CDHS";//��ɫ��÷������Ƭ�����ģ�����   
    char   num[14]="A23456789TJQK";//     

    for   (i=0;   i<52;   i++)order[i]   =   i;//   ˳��ں�ȡ�ƶ���   
    for   (i=0;   i<8*8;   i++)cards[i]   =   -1;//   �����������   

    srand(gameNum);//ѡ�ֵĺ��룬rand   ������   

    for   (i=0;   i<52;   i++){   
        //   �ڵ�ǰ��52-i��ȡ�ƶ�����ѡһ��λ��   from   ȡ��   
        //   ������������󣬱�֤   from   ���ڶ�����ʵ�����Ƶ�λ��   
        from=rand()   %   (52-i);   

        //   ���ڲ��ú�����ƣ�ʵ����ÿ��8��λ�÷�һ���ƣ�0,8,16,24...56,1,9...   
        //   �����ƾ���λ��to��ֵ��i�ĵ�3λ�͸�3λ����   
        to   =   (i   &   070)>>3     |   (i   &   07)   <<3;     

        //   ȡ�ƶ���ȡ��from��λ�õ���   order[from]�������to��λ��   cards[to]   
        cards[to]   =   order[from];   

        //   ��from��λ�õ���   order[from]�������һ����order[52-i-1]���ϣ�   
        //   ��֤ȡ�ƶ��в����ֿ�λ   
        order[from]   =   order[52-i-1];   

        //   ÿ��ʾ8���ƣ�����һ��   
        if((i&7)==0)printf("\n");   
        //   ��ʾ��������ʲô   
        printf("%c%c   ",   hand[cards[to]%4],   num[cards[to]/4]);   
        //   ��ʾȡ��λ�úͷ���λ�ã������ã�   
        //printf("take   %d   to   %d\n",from,to);   
    }   

    printf("\n");   
    getchar();    
}   
//////////////////////////////////////////////////////////////////////////




MATRIX matrix_alloc(int n)
{
    MATRIX x = (MATRIX)malloc(sizeof(char *)*n+sizeof(char)*n*n);
    int i;
    x[0]=(char *)(x+n);
    for(i=1;i<n;i++)x[i]=x[i-1]+n;
    return x;
}

void matrix_free(MATRIX x){
    free(x);
}

VECTOR vector_alloc(int n){
    VECTOR x = (VECTOR)malloc(sizeof(char)*n);
    return x;
}

void vector_free(VECTOR x){
    free(x);
}

void matrix_sum(MATRIX A, CONST_MATRIX B, int n){
    int i,j;
    for(i=0;i<n;i++){
        for(j=0;j<n;j++){
            A[i][j] ^= B[i][j];
        }
    }
}

void vector_sum(VECTOR a, CONST_VECTOR b, int n){
    int i;
    for(i=0;i<n;i++)a[i]^=b[i];
}

void matrix_mul(MATRIX out, CONST_MATRIX in1, CONST_MATRIX in2, int n){
    int i,j,k;
    for(i=0;i<n;i++)for(j=0;j<n;j++){
        int sum=0;
        for(k=0;k<n;k++){
            if(in1[i][k])
                sum^=in2[k][j];
        }
        out[i][j]=sum;
    }
}

void matrix_mul_vector(VECTOR out, CONST_MATRIX m, CONST_VECTOR in, int n){
    int i,j;
    for(i=0;i<n;i++){
        int sum=0;
        for(j=0;j<n;j++){
            sum^=m[i][j]&in[j];
        }
        out[i]=sum;
    }
}

void vector_init_const(VECTOR v, char c, int n){
    int i;
    for(i=0;i<n;i++)v[i]=c;
}

void matrix_init_O(MATRIX o, int n){
    int i,j;
    for(i=0;i<n;i++)for(j=0;j<n;j++)o[i][j]=0;
}

void matrix_init_const(MATRIX m, char c, int n){
    int i,j;
    for(i=0;i<n;i++)for(j=0;j<n;j++)m[i][j]=c;
}

void matrix_init_E(MATRIX e, int n){
    int j;
    matrix_init_O(e,n);
    for(j=0;j<n;j++)e[j][j]=1;
}

void matrix_init_H(MATRIX h, int n){
    int i;
    matrix_init_O(h,n);
    for(i=0;i<n;i++){
        if(i>=1)h[i][i-1]=1;
        h[i][i]=1;
        if(i<n-1)h[i][i+1]=1;
    }
}

void matrix_copy(MATRIX m, CONST_MATRIX k, int n){
    int i,j;
    for(i=0;i<n;i++)for(j=0;j<n;j++)m[i][j]=k[i][j];
}

void vector_copy(VECTOR v, CONST_VECTOR w, int n){
    int i;
    for(i=0;i<n;i++)v[i]=w[i];
}

void matrix_output(CONST_MATRIX m, int n){
    int i,j;
    for(i=0;i<n;i++){
        for(j=0;j<n;j++){
            printf("%d",m[i][j]);
        }
        printf("\n");
    }
}
void vector_output(VECTOR v, int n){
    int i;
    for(i=0;i<n;i++)printf("%d",v[i]);
    printf("\n");
}

void Usage(const char *program_name){
    printf("Usage:\n");
    printf("\t$%s n\n",program_name);
    printf("\t\twhere n is a positive integer no more than 1000\n");
    printf("Or\n");
    printf("\t$%s [0|1]* [0|1]* ... [0|1]*\n", program_name);
    printf("\t\tIn this format, there're n strings of 0 and 1 and length of each string is n too\n");
    printf("The program assumes there're an n*n array. Each cell in the array has a switch "
        "and a light. On touches of the switch in a cell will changes the state of the "
        "light in the cell as well as the lights in the neighbors of the cell, so that "
        "one switch could change states of 5 lights at most\n");
    printf("The program will try to find a solution to turn all lights off\n");
    printf("The first format of input gives an initial n*n array with all lights on\n");
    printf("The second format requires n strings of 0 and 1, and the length of each string"
        " is n too. Each string is correspondent to state of lights in one line of the "
        "array. A digit 0 means the correspondent light is off and 1 means the light is "
        "on.\n");
    printf("The program will output an n*n matrix of 0 and 1 when there's a solution which "
        "tells whether we need to touch a switch to turn off all the lights\nOr the "
        "program will output \"NO SOLUTION\" when there's no solution\n");
    exit(-1);
}

MATRIX P0,P1,H;
VECTOR ME;
MATRIX init;

//First we need to solve equation P*X(n) = ME;
//There could be multiple solutions
//For each solution X(n), try
//X(n-1) = INIT+ H*X(n);
//X(K)   = INIT + H*X(k+1)+X(k+2);//for k<n-1
void Solve(MATRIX P, VECTOR ME, int n)
{
    int *freedom_index;
    int freedom_count=0;
    int i,j,k;
    int failed=0;
    freedom_index = (int *)malloc(sizeof(int)*n);
    //First solve equition P*X(n) = ME
    for(i=0;i<n;i++){
        for(j=i;j<n;j++)if(P[j][i]==1)break;
        if(j==n){
            //find a freedom factor
            if(ME[i]!=0){
                failed=1;//No solution, does not return since I want find out the freedom factor
            }
            freedom_index[freedom_count++]=i;
        }else{
            if(j!=i){
                //Swap line j and i
                int temp;
                for(k=i;k<n;k++){
                    temp=P[i][k];
                    P[i][k]=P[j][k];
                    P[j][k]=temp;
                }
                temp = ME[i];
                ME[i] = ME[j];
                ME[j] = temp;
            }
            for(j=0;j<n;j++){
                if(j!=i&&P[j][i]){
                    //If P[j][i] is 1, add Line i into Line j
                    for(k=i;k<n;k++){
                        P[j][k]^=P[i][k];
                    }
                    ME[j]^=ME[i];
                }
            }
        }
    }
    fprintf(stderr,"Freedom factor = %d\n", freedom_count);
    if(failed){
        printf("NO SOLUTION\n");
        free(freedom_index);
        return;
    }
    //Now ME hold's one solution for X[n],
    //And random reset index inside freedom_index of ME to 0, 1
    //    will result in another solution for X[n];
    //Output one solution
    {
        VECTOR *x;
        x=(VECTOR *)malloc(sizeof(VECTOR)*n);
        for(i=0;i<n;i++)x[i]=vector_alloc(n);
        vector_copy(x[n-1],ME,n);
        matrix_mul_vector(x[n-2], (CONST_MATRIX) H, x[n-1],n);
        vector_sum(x[n-2],init[n-1],n);
        for(k=n-3;k>=0;k--){
            matrix_mul_vector(x[k], (CONST_MATRIX)H,x[k+1],n);
            vector_sum(x[k],init[k+1],n);
            vector_sum(x[k],x[k+2],n);
        }
        matrix_output((CONST_MATRIX)x, n);
        for(i=0;i<n;i++)vector_free(x[i]);
        free(x);
    }
    free(freedom_index);
}
int parse(int argc, char **argv){
    if(argc == 2){
        int n = atoi(argv[1]);
        if(n<0||n>1000){
            Usage(argv[0]);
        }else if(n<=1){
            printf("%d\n",n);
            exit(0);
        }else{
            init=matrix_alloc(n);
            matrix_init_const(init,1,n);
            return n;
        }
    }else{
        int n=argc-1;
        int i,j;
        init=matrix_alloc(n);
        for(i=0;i<n;i++){
            char *s=argv[i+1];
            if(strlen(s)!=n){
                matrix_free(init);
                Usage(argv[0]);
            }
            for(j=0;j<n;j++){
                if(s[j]!='0'&&s[j]!='1'){
                    matrix_free(init);
                    Usage(argv[0]);
                }
                init[i][j]=s[j]-'0';
            }
        }
        return n;
    }
    return 0;
}

int Calc0(int argc, char **argv){
    int n;
    int i;
    MATRIX temp_matrix;
    VECTOR temp_vector;
    if(argc<2){
        Usage(argv[0]);
    }
    n=parse(argc,argv);

#ifdef DEBUG
    printf("Input:\n");
    matrix_output((CONST_MATRIX)init,n);
#endif

    ME = vector_alloc(n);
    temp_vector = vector_alloc(n);

    P0 = matrix_alloc(n);
    P1 = matrix_alloc(n);
    H  = matrix_alloc(n);
    temp_matrix = matrix_alloc(n);
    matrix_init_H(H,n);
    matrix_init_E(P0,n);
    matrix_init_H(P1,n);
#ifdef DEBUG
    printf("P(0):\n");
    matrix_output((CONST_MATRIX)P0, n);
    printf("P(1):\n");
    matrix_output((CONST_MATRIX)P1,n);
#endif

    matrix_mul_vector(ME, (CONST_MATRIX)P0, init[0], n);
    matrix_mul_vector(temp_vector, (CONST_MATRIX)P1, init[1], n);
#ifdef DEBUG
    printf("M(0):");vector_output(ME,n);printf("\n");
    printf("M(1):");vector_output(temp_vector,n);printf("\n");
#endif
    vector_sum(ME,temp_vector,n);
#ifdef DEBUG
    printf("S(1):");vector_output(ME,n);printf("\n");
#endif

    for(i=2;i<=n;i++){
        matrix_mul(temp_matrix, (CONST_MATRIX)H, (CONST_MATRIX)P1,n);
        matrix_sum(temp_matrix, (CONST_MATRIX)P0,n);//P(k)= H*P(k-1) + P(k-2)
        matrix_copy(P0, (CONST_MATRIX)P1,n);
        matrix_copy(P1, (CONST_MATRIX)temp_matrix,n);
#ifdef DEBUG
        printf("P(%d):\n",i);
        matrix_output((CONST_MATRIX)P1,n);
#endif
        if(i<n){
            matrix_mul_vector(temp_vector, (CONST_MATRIX)P1, init[i], n);
            vector_sum(ME,temp_vector,n);
#ifdef DEBUG
            printf("M(%d):",i);vector_output(temp_vector,n);printf("\n");
            printf("S(%d):",i);vector_output(ME,n);printf("\n");
#endif
        }
    }

    Solve(P1, ME, n);

    matrix_free(init);
    vector_free(ME);
    vector_free(temp_vector);
    matrix_free(P0);
    matrix_free(P1);
    matrix_free(H);
    matrix_free(temp_matrix);
    return 0;
}
