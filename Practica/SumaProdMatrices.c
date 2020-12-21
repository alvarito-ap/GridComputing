#include <stdio.h>
#include <omp.h>
#include <time.h>
#include <stdlib.h>

#define F 500
#define C 500

void imprimirMatriz(int matriz[F][C]);
void limpiar(int matriz[F][C]);

int main(int argc, char *argv[]){

	int a[F][C] = {0}, b[F][C] = {0}, r1[F][C] = {0}, r2[F][C] = {0}, i, j, k;
	clock_t start,end;
	double timeD = 0.0;
	srand(time(NULL));

	for(i=0;i<F;i++){
		for (j=0;j<C;j++){
			a[i][j] = rand()%5;
			b[i][j] = rand()%5;
		}
	}
	//imprimirMatriz(a);imprimirMatriz(b);
	/*
		CADA HILO RESUELVE CADA OPERACION USANDO LOS DOS METODOS DISPONIBLES
	*/

	#pragma omp parallel sections private(i, j, start, end)
	{
		//HILO SUMA
		#pragma omp section
		{	
			start = clock();
			for(i=0;i<F;i++){
				for (j=0;j<C;j++){
					r1[i][j] = a[i][j]+b[i][j];
				}
			}
			end = clock();
			timeD = ((double) (end - start)) / CLOCKS_PER_SEC;
			printf("Tiempo suma iterativa en seccion paralela = %f\n", timeD);
			

			limpiar(r1);
			start = clock();
			for (i=0;i<F*C;i++){
				r1[i%F][i/F] = a[i%F][i/F] + b[i%F][i/F];
			}
			end = clock();
			timeD = ((double) (end - start)) / CLOCKS_PER_SEC;
			printf("Tiempo suma unico for en seccion paralela = %f\n", timeD);
		}

		//HILO MULTIPLICACION
		#pragma omp section
		{
			start = clock();
			for(i=0;i<F;i++){//filas de a
				for(j=0;j<C;j++){//columnas de b
					for(k=0;k<C;k++){//la K marca la columa en a y la fila para b
						r2[i][j]+=a[i][k]*b[k][j];
					}
				}
			}
			end = clock();
			timeD = ((double) (end - start)) / CLOCKS_PER_SEC;
			printf("Tiempo producto iterativo en seccion paralela = %f\n", timeD);
			limpiar(r2);
			start = clock();
			for(i=0, j=0, k=0;i<F;k++){
				if(k==C) {k=0;j++;}                
		        if(j==C){j=0;i++;}
				r2[i][j] += a[i][k]*b[k][j]; 
			}
			end = clock();
			timeD = ((double) (end - start)) / CLOCKS_PER_SEC;
			printf("Tiempo producto unico for en seccion paralela = %f\n", timeD);
		}
	}
	
	/*
		EJECUCION ITERATIVA EN PARALELO
	*/
	limpiar(r1); limpiar(r2);
	start = clock();
	#pragma omp parallel for private(i, j)
	for(i=0;i<F;i++){
		for (j=0;j<C;j++){
			r1[i][j] = a[i][j]+b[i][j];
		}
	}
	end = clock();
	timeD = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("Tiempo suma iterativa en paralelo= %f\n", timeD);
	
	#pragma omp parallel for private(i,j,k)
	for(i=0;i<F;i++){
		for(j=0;j<C;j++){
			#pragma omp reduction(+:r2[i][j])
			for(k=0;k<C;k++){
				r2[i][j]=r2[i][j]+a[i][k]*b[k][j];
			}
		}
	}
	end = clock();
	timeD = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("Tiempo producto iterativo en paralelo = %f\n", timeD);

	/*
		EJECUCION EN PARALELO CON UNICO FOR
	*/
	limpiar(r1); limpiar(r2);
	start = clock();
	#pragma omp  parallel for private(i)
	for (i=0;i<F*C;i++){
		r1[i%F][i/F] = a[i%F][i/F] + b[i%F][i/F];
	}
	end = clock();
	timeD = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("Tiempo suma en paralelo unico for= %f\n", timeD);

	start = clock();
	#pragma omp parallel for private(i, k)
	for(i=0;i<F*C;i++){
		#pragma omp reduction(+:r2[i][j])
		for(k=0;k<C;k++){
			r2[i%F][i/F]=r2[i%F][i/F]+a[i%F][k]*b[k][i/F];
		}
	}
	end = clock();
	timeD = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("Tiempo producto en paralelo unico for= %f\n", timeD);
	return 0;
}

void imprimirMatriz(int matriz[F][C]){
	int i, j;
	printf("\n");
	for(i=0;i<F;i++){
		for(j=0;j<C;j++){
			printf(" [%d] ", matriz[i][j]);
		}
		printf("\n");
	}
}

void limpiar(int matriz[F][C]){
	int i, j;
	printf("\n");
	for(i=0;i<F;i++){
		for(j=0;j<C;j++){
			matriz[i][j] = 0;
		}
	}
}