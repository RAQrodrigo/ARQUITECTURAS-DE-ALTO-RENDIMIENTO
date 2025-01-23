#include <stdio.h>
#include <msa.h>

__attribute__((aligned(16))) unsigned char red[16384];

int main()
{

int i,j;

// Initialization of the vector
for (i=0; i<16384; i++) {
   red[i] = i & 0xFF;
}

//#define ADDS_VERSION

#ifdef ADDS_VERSION
__asm volatile(
"				li			$4,0x4000		\n"
"				li			$5,0x10			\n"
"				move		$6,%[Red]		\n"
"				ldi.b		$w0,0x5			\n"
"loop:			ld.b 		$w1,0($6)		\n"
"				adds_u.b 	$w1,$w1,$w0 	\n"
"				st.b   		$w1,0($6)		\n"
"				sub	 		$4,$4,$5		\n"
"				dadd    	$6,$6,$5		\n"
"				bgtz		$4,loop			\n"
"				nop							\n"
:
: [Red] "r" (red)
: "memory","$4","$5","$6"
);
#elif ADDVI_VERSION
__asm volatile(
"				li	 		$4,0x4000	\n"
"				move		$5,%[Red]	\n"
"				li	 		$6,0x10		\n"
"				ldi.b  		$w1,0xfa	\n"
"   			ldi.b		$w2,0xff	\n"
"loop: 			ld.b    	$w3,0($5)	\n"   //Elemento i de Red
"				clt_u.b 	$w4,$w3,$w1	\n"      //Compara w3 y w1 y se genera una mascara que dirá que elementos deben ser modificados.
"				addvi.b 	$w5,$w3,0x5	\n"      //Se le suma a todos los elementos 5.
"   			bsel.v  	$w4,$w2,$w5 \n"      //Dependiendo de lo que hay en w4 se toma el valor de w2 o w5
"				st.b    	$w4,0($5)	\n"      //Se guarda lo de w4 en la red.
"				sub	 		$4,$4,$6	\n"      //Le restamos la cantidad de números procesados
"				dadd    	$5,$5,$6	\n"         //Ahora sumamos las posiciones de mem de los elementos procesados para empezar en el primero que no fue procesado
"				bgtz		$4,loop		\n"      //Volvemos a aplicar hasta que $4 sea 0 o menor a este.
"				nop							\n"
:
: [Red] "r" (red)
: "memory","$4","$5","$6"
);
#else
for (i=0;i<16384;i++) {
   if (red[i]< 250)
      red[i] += 5;
   else
      red[i] = 255;
}
#endif


// check output
for (i=0; i<16384;i++) {
    printf(" %u ",red[i]);
}

return 0;
}
