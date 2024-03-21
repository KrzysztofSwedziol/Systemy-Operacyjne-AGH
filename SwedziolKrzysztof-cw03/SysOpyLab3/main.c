#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#define BUFFER_SIZE 1024

void reverseFile(FILE *inputFilename, FILE *outputFilename){
    if(inputFilename == NULL){
        perror("Error opening input file");
        exit(EXIT_FAILURE);
    }
    if(outputFilename == NULL){
        perror("Error opening output file");
        fclose(inputFilename);
        exit(EXIT_FAILURE);
    }
    fseek(inputFilename, 0, SEEK_END);
    long fileSize = ftell(inputFilename);

    for(int i = fileSize - 1; i >= 0; i--){
        fseek(inputFilename, i, SEEK_SET);
        int c = fgetc(inputFilename);
        fputc(c, outputFilename);
    }
}

void reverseFileBlock(FILE *inputFilename, FILE *outputFilename){
    if(inputFilename == NULL){
        perror("Error opening input file");
        exit(EXIT_FAILURE);
    }
    if(outputFilename == NULL){
        perror("Error opening output file");
        fclose(inputFilename);
        exit(EXIT_FAILURE);
    }
    fseek(inputFilename, 0, SEEK_END);
    long size = ftell(inputFilename);
    char currArray[BUFFER_SIZE];
    long readSize;
    for(int i = size; i>0; i-=BUFFER_SIZE){
        readSize = (i >= BUFFER_SIZE) ? BUFFER_SIZE : i;
        fseek(inputFilename, -readSize, SEEK_CUR);
        fread(currArray, 1, readSize, inputFilename);
        fseek(inputFilename, -readSize, SEEK_CUR);
        for(long j = readSize-1; j>=0; j--){
            fputc(currArray[j], outputFilename);
        }
    }
}
int main() {


    clock_t start, end;
    double time_used_single, time_used_block;
    FILE *pierwszyPlik;
    FILE *drugiPlik;
    FILE *drugiPlikBlok;
    pierwszyPlik = fopen("/home/krzysztof/CLionProjects/SysOpyLab3/plikDoOdczytu.txt", "r");
    drugiPlik = fopen("/home/krzysztof/CLionProjects/SysOpyLab3/plikDoZapisu.txt", "w");

    start = clock();
    reverseFile(pierwszyPlik, drugiPlik);
    end = clock();
    time_used_single = ((double) (end - start)) / CLOCKS_PER_SEC;

    fclose(pierwszyPlik);
    fclose(drugiPlik);

    pierwszyPlik = fopen("/home/krzysztof/CLionProjects/SysOpyLab3/plikDoOdczytu.txt", "r");
    drugiPlikBlok = fopen("/home/krzysztof/CLionProjects/SysOpyLab3/plikDoZapisuBlokowego.txt", "w");
    start = clock();
    reverseFileBlock(pierwszyPlik, drugiPlikBlok);
    end = clock();
    time_used_block = ((double) (end - start)) / CLOCKS_PER_SEC;

    fclose(pierwszyPlik);
    fclose(drugiPlikBlok);

    FILE *timeFile = fopen("/home/krzysztof/CLionProjects/SysOpyLab3/pomiar_zad_2.txt", "w");
    fprintf(timeFile, "Time used for single character: %f seconds\n", time_used_single);
    fprintf(timeFile, "Time used for block: %f seconds\n", time_used_block);
    fclose(timeFile);


    return 0;
}
