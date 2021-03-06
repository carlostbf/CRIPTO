/***************************************************************************
afin.c

Autores: Carlos Li Hu y David López Ramos
2018 EPS-UAM
 ***************************************************************************/

#include "../includes/utils.h"

/**
 * @brief Calcula el maximo comun divisor usando gmp
 *
 * @param resultado de la operación
 * @param primer operando
 * @param segundo operando
 *
 */
void gmp_mcd(mpz_t rop, mpz_t op1, mpz_t op2) {
    mpz_t a, b, r;
    mpz_inits(a, b, r, NULL);
    mpz_set(a, op1);
    mpz_set(b, op2);

    if (mpz_cmp_d(a, 0) == 0) {
        mpz_set(rop, b);
        mpz_clears(a, b, r, NULL);
        return;
    }
    mpz_mod(r, b, a);
    gmp_mcd(rop, r, a);
    mpz_clears(a, b, r, NULL);
}

/**
 * @brief Realiza la descomposición de Euclides Extendida:
 *        g = s*a1 + t*m1
 *        Calcula el inverso de a1 modulo m1 en s
 *
 * @param g el mcd de a1 y m1
 * @param s el inverso de a1
 * @param t el otro factor de la descomposición
 * @param a1 el elemento al que le calculamos el inverso
 * @param m1 el módulo de la operación
 *
 */
void gmp_mcdext(mpz_t g, mpz_t s, mpz_t t, const mpz_t a1, const mpz_t m1) {
    mpz_t a, m, x, y, x1, y1, aux;
    mpz_inits(a, m, x, y, x1, y1, aux, NULL);
    mpz_set(a, a1);
    mpz_set(m, m1);


    if (mpz_cmp_d(a, 0) == 0) {
        mpz_set_d(s, 0);
        mpz_set_d(t, 1);
        mpz_set(g, m);
        return;
    }
    mpz_mod(aux, m, a);
    gmp_mcdext(g, x1, y1, aux, a);
    mpz_mul(aux, m, x1);
    mpz_tdiv_q(aux, aux, a);

    mpz_sub(s, y1, aux);

    mpz_set(t, x1);
    mpz_clears(a, m, x, y, x1, y1, aux, NULL);
}

/* PROGRAMA PRINCIPAL */
int main(int argc, char **argv) {
    char entrada[SIZE], cadena[SIZE];
    int long_index = 0;
    char opt, simbolo_in, simbolo_out;
    mpz_t a, b, m, inv, mcd, t, aux, aux2;
    FILE *fIn = NULL, *fOut = NULL, *fAux = NULL;
    int cifrar = -1;

    if (argc > 1) {
        strncpy(entrada, argv[1], SIZE);
    } else {
        printf("Ejecucion: %s {-C|-D} {-m |Zm|} {-a N×} {-b N+} [-i filein] [-o fileout]\n", argv[0]);
        printf("Ejemplo: %s -C -m 26 -a 5 -b 4\n", argv[0]);
        exit(-1);
    }

    static struct option options[] = {
        {"C", no_argument, 0, 'c'},
        {"D", no_argument, 0, 'd'},
        {"m", required_argument, 0, '1'},
        {"a", required_argument, 0, '2'},
        {"b", required_argument, 0, '3'},
        {"i", required_argument, 0, '4'},
        {"o", required_argument, 0, '5'},
        {0, 0, 0, 0}
    };

    mpz_inits(a, b, m, inv, t, mcd, aux, aux2, NULL);
    while ((opt = getopt_long_only(argc, argv, "c:d:1:2:3:4:5", options, &long_index)) != -1) {
        switch (opt) {
            case 'c':
                cifrar = 1;
                break;

            case 'd':
                cifrar = 0;
                break;

            case '1':
                mpz_set_str(m, optarg, 10);
                break;

            case '2':
                mpz_set_str(a, optarg, 10);
                break;

            case '3':
                mpz_set_str(b, optarg, 10);
                break;

            case '4':
                fIn = fopen(optarg, "r");
                if (!fIn) exit(-1);
                break;

            case '5':
                fOut = fopen(optarg, "w");
                if (!fOut) exit(-1);
                break;

            case '?':
                break;

            default:
                printf("Ejecucion: %s {-C|-D} {-m |Zm|} {-a N×} {-b N+} [-i filein] [-o fileout]\n", argv[0]);
                printf("Ejemplo: %s -C -m 26 -a 5 -b 4\n", argv[0]);
                exit(-1);
                break;
        }
    }
    //Si no se ha especificado si cifrar o descifrar
    if (cifrar == -1) {
        printf("Ejecucion: %s {-C|-D} {-m |Zm|} {-a N×} {-b N+} [-i filein] [-o fileout]\n", argv[0]);
        printf("Ejemplo: %s -C -m 26 -a 5 -b 4\n", argv[0]);
        exit(-1);
    }
    /* Comprobamos si la clave es válida */
    gmp_mcd(mcd, a, m);
    if (mpz_cmp_d(mcd, 1) != 0) {
        printf("La clave no determina una función afín inyectiva\n");
        exit(-1);
    }
    /* Calculamos mcd e inverso para luego */
    mpz_gcdext(mcd, inv, t, a, m);

    /*crear entrada estandar*/
    if (!fIn) {
        printf("Leyendo entrada estandar \n");
        fgets(cadena, SIZE, stdin);
        /*se guarda la entrada en un fichero para reutilizar codigo*/
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

    /*leer fichero entrada o estandar*/
    if (fIn) {
        while (fscanf(fIn, "%c", &simbolo_in) != EOF) {

            //aux es nuestro simbolo de entrada
            //aux2 es nuestro simbolo de salida
            simbolo_in -= K;
            mpz_set_ui(aux, (int) simbolo_in);

            /*Cifrar*/
            if (cifrar == 1) {
                mpz_mul(aux2, a, aux);
                mpz_mod(aux2, aux2, m);
                mpz_add(aux2, aux2, b);
                mpz_mod(aux2, aux2, m);
            }/*Descifrar*/
            else {
                /*simbolo_out = (inv*(simbolo_in - b)) % m;*/
                mpz_sub(aux2, aux, b);
                mpz_mul(aux2, aux2, inv);
                mpz_mod(aux2, aux2, m);
            }

            /*convertir a double o int y sumar 65, codigo de la primera letra A*/
            simbolo_out = mpz_get_d(aux2);
            simbolo_out += K;

            fwrite(&simbolo_out, 1, 1, fOut);

        }

    }

    printf("\n");
    mpz_clears(a, b, m, inv, t, mcd, aux, aux2, NULL);
    if (fIn) fclose(fIn);
    if (fOut) fclose(fOut);
    return 0;


}
