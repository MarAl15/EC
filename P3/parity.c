#include <stdio.h> // printf() 
#include <stdlib.h> // exit()
#include <math.h> // pow()
#include <sys/time.h> // gettimeofday(), struct timeval

#define TEST 0 // Cambiar para probar con una lista mas chica
#define COPY_PASTE_CALC 0 // Cambiar para ver el resultado

#if ! TEST
	#define NBITS 20
	#define SIZE (1<<NBITS) // tamaño suficiente para tiempo apreciable
	unsigned lista[SIZE];
	#define RESULT (pow(2,20)) // fórmula
#else
// ----------------------------------------------------------------------
	#define SIZE 4
	unsigned lista[] = {0x80000000, 0x00100000, 0x00000800, 0x00000001};
	//unsigned lista[] = {0x7fffffff, 0xffefffff, 0xfffff7ff, 0xfffffffe, 0x01000024, 0x00356700, 0x8900ac00, 0x00bd00ef};
	//unsigned lista[] = {0x0, 0x10204080, 0x3590ac06, 0x70b0d0e0, 0xffffffff, 0x12345678, 0x9abcdef0, 0xcafebeef};
	#define RESULT 4
// ----------------------------------------------------------------------
#endif


#define WSIZE 8*sizeof(int)

/* Parity con bucle for */
int parity1(unsigned array[], int tamanio) {
    int i, j, val;
    unsigned x;
    int result = 0;

    for (i=0; i<tamanio; i++){
        val = 0;
        x = array[i];

        for (j = 0; j < WSIZE; j++){
            val ^= x & 0x1;
            x >>= 1;
        }

        result += val & 0x1;
    }

    return result;
}

/* Parity con bucle while */
int parity2(unsigned array[], int tamanio) {
    int i, j, val;
    unsigned x;
    int result = 0;

    for (i=0; i<tamanio; i++){
        val = 0;
        x = array[i];

        while(x){
            val ^= x & 0x1;
            x >>= 1;
        }

        result += val & 0x1;
    }

    return result;
}

/* Parity problema 3.22 */
int parity3(unsigned array[], int tamanio) {
    int i, j, val;
    unsigned x;
    int result = 0;

    for (i=0; i<tamanio; i++){
		val = 0; 
        x = array[i];

        while(x){
            val ^= x;
            x >>= 1;
        }

        result += val & 0x1;
    }

    return result;
}

/* Parity con while en ensamblador */
int parity4(unsigned array[], int tamanio) {
    int i, j, val;
    unsigned x;
    int result = 0;

    for (i=0; i<tamanio; i++){
		val = 0; 
        x = array[i];
	
		asm("\n"
			"ini3:					\n\t"	//Seguir mientras que x!=0
			"	xor %[x], %[val]	\n\t"
			"	shr %[x]			\n\t"
			"	test %[x], %[x]		\n\t"
			"	jnz ini3			\n\t"
			: [val] "+r" (val)
			: [x] "r" (x)
			: "edx"
		);

        result += val & 0x1;
    }

    return result;
}

/* Parity con idea de sumar en árbol (Pr 3.49) */
int parity5(unsigned array[], int tamanio) {
    int i, j;
    unsigned x;
    int result = 0;

    for (i=0; i<tamanio; i++){ 
        x = array[i];

		for (j=16; j>=1; j/=2){
            x ^= x>>j;
		}

        result += x & 0x1;
    }

    return result;
}

/* Parity con SETcc y MOVZx */
int parity6(unsigned array[], int tamanio) {
    int i, j;
    unsigned x;
    int result = 0;

    for (i=0; i<tamanio; i++){ 
        x = array[i];

		asm(
                "mov %[x], %%edx	\n\t"
                "shr $16, %%edx		\n\t"
                "xor %[x], %%edx	\n\t"
                "xor %%dh, %%dl		\n\t"
                "setnp  %%dl		\n\t"
                "movzx  %%dl, %[x]	\n\t"
                : [x] "+r" (x)
                :
                : "edx"
        );

        result += x & 0x1;
    }

    return result;
}

void crono(int (*func)(), char* msg){
	struct timeval tv1,tv2;   // gettimeofday() secs-usecs
	long           tv_usecs;  // y sus cuentas

	gettimeofday(&tv1,NULL);
	int result = func(lista, SIZE);
	gettimeofday(&tv2,NULL);

	tv_usecs=(tv2.tv_sec -tv1.tv_sec )*1E6+
			(tv2.tv_usec-tv1.tv_usec);

	printf("%ld \t us:%s\n", tv_usecs, msg);
}

int main(){

#if ! TEST
	int i;                  // inicializar array
	for (i=0; i<SIZE; i++)
		lista[i]=i;
#endif

	crono(parity1, "Parity1 (Lenguaje C - for)"); // Parity con bucle for
	crono(parity2, "Parity2 (Lenguaje C - while)");	// Parity con bucle while
	crono(parity3, "Parity3 (I.CS:APP 3.22 - mask final)"); // Parity problema 3.22
 	crono(parity4, "Parity4 (Leng ASM - cuerpo while)"); // Parity con while en ensamblador
	crono(parity5, "Parity5 (I.CS:APP 3.49 - 32b,16.1b)"); // Parity con idea de sumar en árbol (Pr 3.49)
	crono(parity6, "Parity6 (Leng ASM - cuerpo for-setnp)"); // Parity con SETcc y MOVZx

#if ! COPY_PASTE_CALC
	//printf("calculado = %f\t", RESULT);
#endif

    exit(0);
}
