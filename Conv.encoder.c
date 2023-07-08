///This work is used to simulate Convolutional code encoder (Hereinafter referred to as conv.code.)
///Input file includes only one parameter: Length
///indicates the length of information sequence
///This work is a (2, 1, 6) convolutional code with generator matrix
///G(D) = (1 + D^2 + D^3 + D^5 + D^6, 1 + D + D^2 + D^3 + D^6)
///The program output will be a txt file containing the sequence of encoded information.
///Length of sequence of encoded information will be 2 * (L + memory_number), memory_number = 6

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>


///This function is used to generate information sequence
//L means length of information sequence
void u_gen (int L, int* u) {
    u[0] = 1;
    u[1] = 0;
    u[2] = 0;
    u[3] = 0;
    u[4] = 0;
    u[5] = 0;

    for(int i = 6; i < L; i++) {
        u[i] = u[i - 6] ^ u[i - 5];
    }
}

///Conv.code codeword generator
void codeword_gen(int* u, int* state, int L, int mem_num, int* codeword) {

    int index = 0;

    for (int i = 0; i < L + mem_num; i++) {
        int x1 = u[i] ^ state[1] ^ state[2] ^ state[4] ^ state[5];
        int x2 = u[i] ^ state[0] ^ state[1] ^ state[2] ^ state[5];

        codeword[index] = x1;
        index++;
        codeword[index] = x2;
        index++;

        for (int j = mem_num - 1; j > 0; j--) {
            state[j] = state[j - 1];
        }
        state[0] = u[i];
/*
        for (int k = 0; k < 6; k++) {
        printf("%d ", state[k]);
        }
        printf("\n");
*/
    }

}

///This function is used to perform BPSK modulation
//total_bits means how many bits need to be modulated
void BPSK_mod(int* codeword, int total_bits, int* x) {

    for (int i = 0; i < total_bits; i++) {
        x[i] = pow(-1, codeword[i]);
    }
}



const char* filename = "Sim.txt";

int main(void) {

    ///Read file & transform the contents from "char" to "int"
    ///--------------------------------------------------------------
    FILE *in_file = fopen(filename, "r");
    if (!in_file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    struct stat sb;
    if (stat(filename, &sb) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    char *file_contents = malloc(sb.st_size);
    int int_data[500];
    int index = 0;

    //Scan all "char" in the file but ignore blank & the "chars" between "%" and newline character
    const char* d = " \n";
    char *p;
    while (fscanf(in_file, "%[^%]%*[^\n] ", file_contents) != EOF) {
        p = strtok(file_contents, d);
        while (p != NULL) {
            int_data[index] = atoi(p);
            //printf("%s ", p);
            //printf("%d\n", int_data[index]);
            index++;
            p = strtok(NULL, d);
        }
    }
    fclose(in_file);
/*
    for(int i = 0; i < 2; i++) {
        printf("%d", int_data[i]);
    }
*/
    ///--------------------------------------------------------------


    ///Define parameters
    ///--------------------------------------------------------------
    int L = int_data[0];        //L : length of information sequence
    int mem_num = 6;            //mem_num : number of memories
    int length = L + mem_num;   //length : length of information sequence plus redundant bits
    //printf("%d\n", L);
    ///--------------------------------------------------------------


    ///Generate information sequence
    ///--------------------------------------------------------------
    int* u = malloc(L * sizeof(int));
    u_gen(L, u);
/*
    for(int i = 0; i < L; i++) {
        printf("%d ", u[i]);
    }
    printf("\n");
*/

    //Add redundant bits
    int* u_with_initial = calloc(length, sizeof(int));
    for (int i = 0; i < L; i++) {
        u_with_initial[i] = u[i];
    }
/*
    for(int i = 0; i < length; i++) {
        printf("%d ", u_with_initial[i]);
    }
    printf("\n");
*/
    ///--------------------------------------------------------------


    ///Generate codeword
    int* codeword = calloc(2 * length , sizeof(int));
    int* state = calloc(mem_num, sizeof(int));
    codeword_gen(u_with_initial, state, L, mem_num, codeword);
/*
    for (int i = 0; i < 2 * length; i++) {
        printf("%d ", codeword[i]);
    }
    printf("\n");
*/

    ///The codeword adopts BPSK modulation
    int* x = malloc(2 * length * sizeof(int));
    BPSK_mod(codeword, 2 * length, x);
/*
    for (int i = 0; i < 2 * length; i++) {
        printf("%d ", x[i]);
    }
    printf("\n");
*/


    ///Write the decoded information sequences into an output txt file
    ///----------------------------------------------------------------------------
    char x_char[30000] = {};
    index = 0;
    for (int i = 0; i < 2 * length; i++) {

        sprintf(&x_char[index], "%d", x[i]);
        if (x[i] == -1) {
            index += 2;
        }
        else if (x[i] == 1) {
            index++;
        }

        x_char[index] = ' ';
        index++;
    }

/*
    for (int i = 0; i < 30000; i++) {
        printf("%c", x_char[i]);
    }
    printf("\n");
*/

    printf("Complete! Please see the x.txt in the folder.\n");

    FILE *out_txt;
    out_txt = fopen("x.txt","w");
    fprintf(out_txt, x_char);
    fclose(out_txt);

    getchar();

    free(file_contents);
    free(u);
    free(u_with_initial);
    free(codeword);
    free(state);
    free(x);

    exit(EXIT_SUCCESS);
}
