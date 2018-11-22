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

/* Popcount con bucle for */
int popcount1(unsigned array[], int tamanio){
	int i, j;
	int result = 0;
	unsigned x;

	for(i=0; i<tamanio; i++){
		x = array[i];
		for(j=0; j<WSIZE; j++){
			unsigned mask = 1 << j;
			result += (x & mask) != 0;
		}
	}

	return result;
}

/* Popcount con bucle while */
int popcount2(unsigned array[], int tamanio){
	int i, j;
	int result = 0;
	unsigned x;

	for(i=0; i<tamanio; i++){
		x = array[i];
		while(x){
			result += x & 0x1;
			x >>= 1;
		}
	}

	return result;
}

/* Popcount con while en ensamblador */
int popcount3(unsigned array[], int tamanio){
	int i, j;
	int result = 0;
	unsigned x;

	for(i=0; i<tamanio; i++){
		x = array[i];
		asm("\n"
        	"ini3:                  	\n\t"    
        	"     shr %[x]	        	\n\t"	// LSB en CF
       		"     adc $0x0, %[r]      	\n\t"	// result += (0 + CF)
        	"     cmp $0x0, %[x]     	\n\t"   
        	"     jne ini3         		\n\t"	// seguir mientras que x!=0
        	: [r] "+r" (result)       // e/s: añadir a lo acumulado por el momento
        	: [x] "r" (x) 			//entrada: valor elemento 
		);
				
	}

	return result;
}

/* Popcount problema 3.49 */
int popcount4(unsigned array[], int tamanio){
	int i, j, val;
	int result = 0;
	unsigned x;

	for (i = 0; i < tamanio; i++) {
		val = 0;
		x = array[i];
		for (j = 0; j < 8; j++) {
			val += x & 0x0101010101010101;
			x >>= 1;
		}
		//val += (val >> 32);
		val += (val >> 16);
		val += (val >> 8);
		result += (val & 0xff);
	}

	return result;
}

/* Popcount SSE3 */
int popcount5(unsigned array[], int tamanio){
	int i;
	int val, result = 0;
	int SSE_mask[] = {0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f};
	int SSE_LUTb[] = {0x02010100, 0x03020201, 0x03020201, 0x04030302};
	
	if (tamanio & 0x3)
		printf("leyendo 128b pero tamanio no múltiplo de 4?\n");
	for (i = 0; i < tamanio; i += 4) {
		asm("movdqu 	%[x], 	%%xmm0	\n\t"
			"movdqa	 %%xmm0, 	%%xmm1	\n\t"	// dos copias de x
			"movdqu 	%[m],	%%xmm6	\n\t"	// máscara
			"psrlw 		$4, 	%%xmm1	\n\t"	
			"pand 	%%xmm6, 	%%xmm0	\n\t"	//; xmm0 – nibbles inferiores
			"pand 	%%xmm6,		%%xmm1	\n\t"	//; xmm1 – nibbles superiores

			"movdqu 	%[l],	%%xmm2	\n\t"	//; ...como pshufb sobrescribe LUT
			"movdqa %%xmm2, 	%%xmm3	\n\t"	//; ...queremos 2 copias
			"pshufb %%xmm0, 	%%xmm2	\n\t"	//; xmm2 = vector popcount inferiores
			"pshufb %%xmm1, 	%%xmm3	\n\t"	//; xmm3 = vector popcount superiores

			"paddb 	%%xmm2,		%%xmm3 	\n\t"	//; xmm3 - vector popcount bytes
			"pxor 	%%xmm0, 	%%xmm0 	\n\t"	//; xmm0 = 0,0,0,0
			"psadbw %%xmm0, 	%%xmm3 	\n\t"	//; xmm3 = [pcnt bytes0..7|pcnt bytes8..15]
			"movhlps %%xmm3, 	%%xmm0 	\n\t"	//; xmm0 = [ 0 |pcnt bytes0..7 ]
			"paddd 	%%xmm3, 	%%xmm0 	\n\t"	//; xmm0 = [ no usado |pcnt bytes0..15]
			"movd 	%%xmm0, 	%[val] 	\n\t"

			: [val]"=r" (val)
			: [x] "m" (array[i]),
			  [m] "m" (SSE_mask[0]),
			  [l] "m" (SSE_LUTb[0]) 
		);	
	
		result += val;
	}

	return result;
}

/* Popcount SSE4.2 */
int popcount6(unsigned array[], int tamanio){
	int i;
	unsigned x;
	int val, result = 0;

	for(i=0; i<tamanio; i++){
		x = array[i];

		asm("popcnt %[x], %[val]"
			: [val] "=r" (val)
			: [x]	"r"  (x)
		);

		result += val;
	}
	
	return result;
	
}

/* Popcount 64 bits p/mejorar prestaciones */
int popcount7(unsigned array[], int tamanio){
	int i;
	unsigned x1, x2;
	int val, result = 0;

	if(tamanio & 0x1)
		printf("leer 64b y tamanio impar?\n");
	for(i=0; i<tamanio; i+=2){
		x1 = array[i];
		x2 = array[i+1];

		asm("popcnt %[x1], %[val]	\n\t"
			"popcnt %[x2], %%edi	\n\t"
			"add	%%edi, %[val]	\n\t"
			: [val] "=&r" (val)
			: [x1]	"r"  (x1),
			  [x2]	"r"	 (x2)
			: "edi"	
		);

		result += val;
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

	crono(popcount1, "Popcount1 (Lenguaje C - for)");	//Popcount con bucle for
	crono(popcount2, "Popcount2 (Lenguaje C - while)");	//Popcount con bucle while
	crono(popcount3, "Popcount3 (Leng ASM - cuerpo while)");	//Popcount con while en ensamblador
	crono(popcount4, "Popcount4 (I.CS:APP 3.49-group 8b)");	//Popcount problema 3.49
	crono(popcount5, "Popcount5 (asm SSE3 - pshufb 128b)");	//Popcount SSE3
	crono(popcount6, "Popcount6 (asm SSE4 - popcount 32b)");	//Popcount SSE4.2
	crono(popcount7, "Popcount7 (asm SSE4 - popcnt 2x32b)");	//Popcount 64 bits

#if ! COPY_PASTE_CALC
	//printf("calculado = %f\t", RESULT);
#endif

    exit(0);
}
