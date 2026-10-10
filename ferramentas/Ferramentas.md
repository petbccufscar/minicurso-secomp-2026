# Minicurso de Cibersegurança: Ferramentas e Práticas

Este repositório contém os materiais, referências e demonstrações das ferramentas apresentadas durante o minicurso de cibersegurança.

## 👾 Pwn
Para acessar os desafios e materiais de exploração, consulte o diretório local:
- [Acessar repositório pwn](./pwn)

---

## 🛠️ Ferramentas Apresentadas

Abaixo estão as principais ferramentas discutidas, acompanhadas de vídeos de referência para aprofundamento:

### [Wireshark](https://www.wireshark.org/)
Ferramenta essencial para análise de tráfego de rede e inspeção de pacotes.
- 📺 **Referência:** [Assistir ao vídeo de demonstração](https://www.youtube.com/watch?v=2Wi6-cCexXA&list=PLe7uewktfHVs&index=1&pp=iAQBsAgC)

### [ExifTool](https://exiftool.org/)
Utilizado para leitura, escrita e manipulação de metadados em diversos tipos de arquivos (imagens, PDFs, etc.).
- 📺 **Referência:** [Assistir ao vídeo de demonstração](https://www.youtube.com/watch?v=3R-7VoLjZMw&list=PLe7uewktfHVs&index=3&pp=iAQBsAgC)

### [Ghidra](https://ghidra-sre.org/)
Framework de engenharia reversa de software (SRE) desenvolvido pela NSA, focado na descompilação de binários.
- 📺 **Referência:** [Assistir ao vídeo de demonstração](https://www.youtube.com/watch?v=fTGTnrgjuGA&list=PLe7uewktfHVs&index=5&pp=iAQBsAgC)

---

## 🔬 Tutorial Prático: Esteganografia básica com strings e binwalk

Abaixo está um cenário prático demonstrando como informações ou outros arquivos podem ser escondidos dentro de uma imagem comum e como podemos detectá-los.

### 1. Criação do Cenário (Escondendo o dado)

Primeiro, vamos preparar uma imagem e inserir uma mensagem oculta (flag) nela.

```bash
# 1. Crie um arquivo de texto com a mensagem secreta (flag)
echo "bandeira{minicurso_concluido}" > secreto.txt

# 2. Baixe ou separe uma imagem qualquer (exemplo: imagem.jpg)

# 3. Vamos compactar o texto para gerar uma assinatura binária que o binwalk reconheça facilmente
zip secreto.zip secreto.txt

# 4. Concatene a imagem original com o arquivo compactado para criar a imagem alterada
cat imagem.jpg secreto.zip > imagem_conteudo.jpg
```

**Nota:** A imagem `imagem_conteudo.jpg` continuará abrindo normalmente em qualquer visualizador de fotos, pois os leitores de imagem ignoram dados adicionados após o final do arquivo original (EOF).

### 2. Execução da Captura (Investigando o arquivo)

Agora, assumindo o papel de analista, vamos investigar os arquivos.

```bash
# 1. Analise a imagem original (não deve mostrar nada de anormal)
binwalk imagem.jpg

# 2. Analise a imagem alterada
binwalk imagem_conteudo.jpg
```

**Resultado esperado:** O `binwalk` detectará a assinatura de um arquivo "Zip archive data" embutido no meio dos bytes da imagem alterada, mostrando que há "mais coisas" em comparação à imagem original.

```bash
# 3. Extraia os textos legíveis do arquivo alterado
strings imagem_conteudo.jpg
```

**Resultado esperado:** O comando `strings` varrerá o binário e, ao rolar a saída (ou usando o comando `strings imagem_conteudo.jpg | tail`), você conseguirá ver vestígios do arquivo oculto e possivelmente a própria mensagem, descobrindo a flag escondida!