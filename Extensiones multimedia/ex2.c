#include <stdio.h>
#include <msa.h>

__attribute__((aligned(16))) double a[256], b[256], c[256], d[256];
__attribute__((aligned(16))) double three[] = {3.0,3.0};
__attribute__((aligned(16))) double five[] = {5.0,5.0};

int main()
{

int i,j;

// fake test image (red channel)
for (i=0; i<256; i++) {
    b[i] = c[i]= (double)(i)/10;
    d[i] = 0.0;
}

c[5] = 0.0;

//#define SIMD_VERSION

#ifdef MSA_VERSION
__asm volatile(
"		LD.D	$w7,0(%[Rthree])	\n"
"		LD.D	$w8,0(%[Rfive])		\n"
// YOUR CODE
"       MOVE    $5,%[Ra]            \n"
"       MOVE    $6,%[Rb]            \n"
"       MOVE    $7,%[Rc]            \n"
"       MOVE    $8,%[Rd]            \n"
"       LI      $4,0x100            \n"     //Guardo cantidad de iteraciones (en este caso 256)
"		LI	    $9,0x2     		    \n"         //Como son elementos de 64 bits entran sólo 2 en un registro de 128 bits.
// FOR i
"loop:  LD.D    $w1,0($5)           \n"
"       LD.D    $w2,0($6)           \n"
"       LD.D    $w3,0($7)           \n"
"       LD.D    $w4,0($8)           \n"     // Cargamos valores de los arreglos. 
"       FADD.D  $w1,$w2,$w3         \n"    // a[i] = b[i] + c[i]
"       ST.D    $w1,0($5)           \n"      // Guardo valor de a[i]
"       FCEQ.D  $w5,$w1,$w2         \n"     // if (a[i] == b[i]) acá voy a generar la máscara
"       FMUL.D  $w6,$w1,$w7         \n"     // d[i] = a[i]*3, los dejo ahi y luego segun la máscara veré cuales posiciones serán reemplazadas
"       LDI.D $w0, 0x0              \n"
"       BSEL.V  $w5,$w0,$w6         \n"     // En w5 quedarán los valores que deberian sustituirse segun la máscara
"       ST.D    $w5,0($8)           \n"     // d[i] = a[i]*3 para los elementos que les toca ser reemplazados
"       FSUB.D  $w2,$w1,$w8         \n"     // b[i] = a[i]-5
"       ST.D    $w2,0($6)           \n"     // Guardo el valor de b[i]
// Movemos posiciones de memoria para apuntar a los siguientes elementos
"       DADD    $5,$5,16           \n"
"       DADD    $6,$6,16           \n"
"       DADD    $7,$7,16           \n"
"       DADD    $8,$8,16           \n"
// Verificamos que seguimos dentro del loop o no
"       SUB     $4,$4,$9           \n"     // Para terminar el ciclo segun la cantidad de elementos procesados
"       BGTZ    $4,loop             \n"     // Volvemos al loop hasta que se procesen todos los elementos
"       NOP                         \n"   
:
: [Ra] "r" (a),
  [Rb] "r" (b),
  [Rc] "r" (c),
  [Rd] "r" (d),
  [Rthree] "r" (three),
  [Rfive] "r" (five)
: "memory", "$4", "$5", "$6", "$7", "$8", "$9"
);
#else
for (i=0; i<256; i++) {
    a[i]=b[i]+c[i];
    if (a[i]==b[i])
       d[i]=a[i]*3;
    b[i]=a[i]-5;
}
#endif


// check output
for (i=0; i<256;i++) {
    printf(" a:%lf b:%lf c:%lf d:%lf\n",a[i],b[i],c[i],d[i]);
}

return 0;
}
