#include <stdio.h>
#include <stdlib.h>

void win() {
    printf("\n[+] SUCESSO! Você sequestrou o fluxo de execução e chamou a função win()!\n");
    exit(0);
}

void funcao_vulneravel() {
    char buffer[64];
    printf("O buffer tem 64 bytes. Digite sua entrada: ");
    gets(buffer); 
    printf("Você digitou: %s\n", buffer);
}

int main() {
    setvbuf(stdout, NULL, _IONBF, 0); 
    setvbuf(stdin, NULL, _IONBF, 0);
    printf("--- Ambiente de Teste CTF ---\n");
    funcao_vulneravel();
    printf("Execução normal finalizada.\n");
    return 0;
}