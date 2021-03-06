/***************************************************************************
seg-perf.c

Autores: Carlos Li Hu y David López Ramos
2018 EPS-UAM
 ***************************************************************************/

#include "../includes/utils.h"

/* PROGRAMA PRINCIPAL */
int main(int argc, char **argv) {
    char entrada[SIZE], cadena[SIZE];
    int long_index = 0;
    char opt, simbolo_in, simbolo_out;
    FILE *fIn = NULL, *fOut = NULL, *fAux = NULL;
    int equi = -1, k = 0, i = 0, cont = 0, j = 0;
    double p_p[M] = {0}, p_c[M] = {0};
    double p_cond[M][M] = {0};

    if (argc > 1) {
        strncpy(entrada, argv[1], SIZE);
    } else {
        printf("Ejecucion: %s {-P|-I} [-i filein] [-o fileout]\n", argv[0]);
        exit(-1);
    }

    static struct option options[] = {
        {"P", no_argument, 0, 'p'},
        {"I", no_argument, 0, 'n'},
        {"i", required_argument, 0, '1'},
        {"o", required_argument, 0, '2'},
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long_only(argc, argv, "p:n:1:2", options, &long_index)) != -1) {
        switch (opt) {
            case 'p':
                equi = 1;
                break;

            case 'n':
                equi = 0;
                break;

            case '1':
                fIn = fopen(optarg, "r");
                if (!fIn) exit(-1);
                break;

            case '2':
                fOut = fopen(optarg, "w");
                if (!fOut) exit(-1);
                break;

            case '?':
                break;

            default:
                printf("Ejecucion: %s {-P|-I} [-i filein] [-o fileout]\n", argv[0]);
                exit(-1);
                break;
        }
    }

    if (equi == -1) {
        printf("Ejecucion: %s {-P|-I} [-i filein] [-o fileout]\n", argv[0]);
        exit(-1);
    }

    /*crear entrada estandar*/
    if (!fIn) {
        printf("Leyendo entrada estandar \n");
        fgets(cadena, SIZE, stdin);
        fIn = fopen("teclado.txt", "w");
        fwrite(cadena, 1, strlen(cadena), fIn);
        fclose(fIn);
        fIn = fopen("teclado.txt", "r");
    }
    /* Si no se especifica, usamos salida estandar */
    if (!fOut) {
        fOut = stdout;
    }

    /*parsear texto*/
    parsear(fIn, &fAux);
    fclose(fAux);
    fIn = fopen("auxiliar.txt", "r");
    srand(time(NULL));
    /*leer fichero entrada o estandar*/
    if (fIn) {
        while (fscanf(fIn, "%c", &simbolo_in) != EOF) {
            simbolo_in -= K;
            /*frecuencia del caracter en el texto plano*/
            p_p[(int) simbolo_in]++;

            /*generamos la clave aleatoria*/
            if (equi == 1) {/*distribución equiprobable*/
                k = (rand() % M);
            } else {/*distribución normal*/
                k = (int) randn(13, 1) % M;

            }
            /*ciframos el caracter por desplazamiento*/
            simbolo_out = simbolo_in + k;
            simbolo_out %= M;
            /*frecuencia del caracter en el texto cifrado*/
            p_c[(int) simbolo_out]++;
            /*frecuencia conjunta del caracter plano y cifrado*/
            p_cond[(int) simbolo_in][(int) simbolo_out]++;
            cont++;
            simbolo_out += K;
        }
    }

    fprintf(fOut, "\n");
    /*Normalizamos las probabilidades*/
    for (i = 0; i < M; i++) {
        p_p[i] /= cont;
        p_c[i] /= cont;
        fprintf(fOut, "Pp(%c) = %lf\n", i + K, p_p[i]);
    }
    for (i = 0; i < M; i++) {
        for (j = 0; j < M; j++) {
            /*Calculamos la probabilidad condicionada*/
            p_cond[i][j] /= (p_c[j] * cont);
            fprintf(fOut, "Pp(%c|%c) = %lf\t", i + K, j + K, p_cond[i][j]);
        }
        fprintf(fOut, "\n");
    }


    if (fIn) fclose(fIn);
    if (fOut) fclose(fOut);

    return 0;


}
