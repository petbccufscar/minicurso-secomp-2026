# minicurso-secomp-2026

Repositório de arquivos utilizados durante o minicurso **"Introdução à Cibersegurança e
resolução de desafios de CTF"**, realizado na **SECOMP 2026** (UFSCar) pelo **PET BCC UFSCar**.

O minicurso é uma introdução prática a *Capture The Flag* (CTF): os participantes resolvem
**dois desafios reais ao vivo**, usando apenas **navegador**, **Burp Suite** e um servidor
já disponibilizado pelos palestrantes. Os desafios foram reaproveitados do CTF da
**SECOMP 2025 (PATOS/POMBOS)**.

> ⚠️ **Aviso:** este repositório contém aplicações **propositalmente vulneráveis** (o desafio
> `wordpress` leva a execução remota de comandos). Use **apenas** em ambiente isolado e
> controlado, com fins **educacionais**. Não exponha na internet sem os cuidados descritos em
> [`PALESTRANTE-SETUP.md`](PALESTRANTE-SETUP.md).

---

## O que tem aqui

| Caminho | Descrição |
|---------|-----------|
| [`MINICURSO.md`](MINICURSO.md) | **Guia do participante** — o passo a passo do exploit dos dois desafios. É o que a turma acompanha durante a demonstração. |
| [`PALESTRANTE-SETUP.md`](PALESTRANTE-SETUP.md) | **Guia do palestrante** — como preparar e publicar o alvo do `wordpress` numa VM local + túnel, antes do evento. |
| [`RiceCat/`](RiceCat/) | Desafio 1 (Criptografia/Esteganografia): o arquivo `RiceCat.gif` e o enunciado. |
| [`wordpress/`](wordpress/) | Desafio 2 (Web): imagem Docker do WordPress vulnerável, plugin wpDiscuz e os `docker-compose` para subir o alvo. |

---

## Os desafios

### 1. RiceCat — Criptografia / Esteganografia 🟢
Uma senha está escondida **depois do fim** do arquivo `RiceCat.gif`. O participante extrai esse
"apêndice" e o decodifica (**ROT13 + Base64**). Resolve-se **100% no navegador** com o
[CyberChef](https://gchq.github.io/CyberChef/), sem instalar nada.

**Conceitos:** dados anexados após o marcador de fim do GIF (`0x3B`); ofuscação em camadas (ROT13/Base64).

### 2. wordpress — Web 🟡
Um site WordPress 5.6.2 roda o plugin **wpDiscuz 7.0.4**, vulnerável a **upload arbitrário de
arquivos (CVE-2020-24186)**. O participante sobe um arquivo `.php` disfarçado de imagem e obtém
**execução remota de comandos (RCE)** para ler a `/flag`.

**Conceitos:** validação de upload por MIME vs. extensão; *webshell*; uso do **Burp Suite** para
manipular requisições HTTP.

---

## Como usar

### Participante
1. Instale o essencial (Windows / PowerShell):
   ```powershell
   winget install Mozilla.Firefox
   winget install PortSwigger.BurpSuite.Community
   ```
2. Pegue com o palestrante o arquivo **`RiceCat.gif`** e a **URL do alvo WordPress**.
3. Siga o [`MINICURSO.md`](MINICURSO.md).

### Palestrante (preparar antes do evento)
1. Suba o alvo numa VM local:
   ```bash
   cd wordpress
   docker compose -f docker-compose.hosted.yml up -d --build
   ```
2. Publique com um túnel (ex.: `cloudflared tunnel --url http://localhost:8080`), configure o
   WordPress e teste o exploit.
3. Passo a passo completo (VM, Docker, túnel, configuração, segurança e reset) em
   [`PALESTRANTE-SETUP.md`](PALESTRANTE-SETUP.md).

> Para um laboratório rápido na **própria máquina** (sem túnel), use
> `wordpress/docker-compose.demo.yml` e acesse `http://localhost:8080`.

---

## Ferramentas usadas
- **Navegador** (Firefox/Chrome) + **CyberChef** — desafio RiceCat.
- **Docker Desktop / Docker Engine** — para subir o alvo WordPress.
- **Burp Suite Community** — para o exploit do WordPress.

---

## Créditos
- **Organização:** PET BCC — UFSCar.
- **Desafios:** originalmente do CTF **SECOMP 2025 (PATOS/POMBOS)**, adaptados para este minicurso.

## Uso responsável
O conteúdo é destinado ao **aprendizado de segurança ofensiva/defensiva** em ambiente autorizado.
Explorar sistemas sem autorização é ilegal. Os mantenedores não se responsabilizam por uso indevido.
