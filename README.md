<p align="center">
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="assets/logo-dark.svg">
    <img alt="Marginália" src="assets/logo-light.svg" width="390">
  </picture>
</p>

---

O Marginália é usado no balcão da livraria para cadastrar autores e livros, montar o pedido de cada cliente e enviar o pagamento ao gateway. O software lê um comando por linha da entrada padrão e responde a cada um com uma única linha. O código-fonte é escrito em C++17, usa apenas a biblioteca padrão e mantém uma classe por entidade do domínio: `Author`, `Book`, `Customer` e `Invoice`.

## Build

```sh
git clone <endereço do repositório>
cd marginalia
make
```

O comando `make` gera o executável `build/marginalia` e o executável dos testes, `build/tests`, e exige apenas o `g++` com suporte a C++17 e o `make`.

## Uso

```sh
build/marginalia <<'EOF'
customer 7 10 Ana Souza
author 1 F J. K. Rowling
book 1 1 49.90 Harry Potter e a Pedra Filosofal
book 2 1 54.90 Harry Potter e a Câmara Secreta
add 1 2
add 2 1
invoice
pay
EOF
```

```
Invoice opened for customer 7
Author 1 registered
Book 1 registered
Book 2 registered
Book 1 added to the invoice with quantity 2
Book 2 added to the invoice with quantity 1
Invoice[id = 1, Customer[id = 7, name = Ana Souza, discount = 10], total = 139.23]
Payment of 139.23 authorized
```

| Comando | Descrição |
|---|---|
| <code>customer&nbsp;&lt;id&gt;&nbsp;&lt;discount&gt;&nbsp;&lt;name&gt;</code> | Abre o pedido do cliente identificado por `id`, com o percentual de desconto e o nome informados |
| <code>author&nbsp;&lt;id&gt;&nbsp;&lt;M&nbsp;or&nbsp;F&gt;&nbsp;&lt;name&gt;</code> | Cadastra o autor identificado por `id`, com o gênero, M ou F, e o nome informados |
| <code>book&nbsp;&lt;id&gt;&nbsp;&lt;author_id&gt;&nbsp;&lt;price&gt;&nbsp;&lt;title&gt;</code> | Cadastra o livro identificado por `id`, vinculado a um autor, com o preço e o título informados |
| <code>add&nbsp;&lt;book_id&gt;&nbsp;&lt;quantity&gt;</code> | Adiciona ao pedido o livro identificado por `book_id`, com a quantidade informada |
| <code>remove&nbsp;&lt;book_id&gt;</code> | Remove do pedido o livro identificado por `book_id`, com toda a quantidade incluída |
| <code>invoice</code> | Mostra o pedido, com os dados do cliente, o desconto e o total já descontado |
| <code>pay</code> | Envia o total do pedido ao gateway de pagamentos, que autoriza ou recusa o pagamento |

As linhas vazias e as linhas que começam com `#` são ignoradas, e as respostas de erro começam sempre com `Error`.

## Regras de negócio

| Código | Descrição |
|---|---|
| RN-01 | O nome do autor é armazenado com no máximo 47 bytes, e o gênero do autor assume exclusivamente os valores M, para masculino, ou F, para feminino. |
| RN-02 | O título do livro é armazenado com no máximo 63 bytes, e o preço do livro corresponde a um valor monetário não negativo, expresso em reais. |
| RN-03 | O desconto do cliente corresponde a um percentual inteiro, compreendido entre 0 e 100, inclusive, e incide sobre o valor total do pedido. |
| RN-04 | A quantidade de cada livro incluído no pedido corresponde a um número inteiro estritamente positivo, e quantidades nulas ou negativas são rejeitadas. |
| RN-05 | O pedido comporta no máximo 10 livros distintos, e cada livro figura no pedido uma única vez, independentemente da quantidade solicitada. |
| RN-06 | As credenciais de serviços externos obedecem às boas práticas de gestão e de proteção de segredos, durante todo o ciclo de vida do software. |

## Testes

```sh
build/tests            # testes de unidade
build/tests --list     # lista os testes de unidade
make sanitizers        # testes de unidade compilados com ASan e UBSan
make valgrind          # testes de unidade executados sob o Valgrind
make sast              # SAST com o Semgrep, sobre o código-fonte em src/
make dast              # DAST com casos hostis, executados contra build/marginalia
```

O SAST exige o Semgrep 1.182.1, instalado com `pipx install semgrep==1.182.1`, e os testes sob o Valgrind exigem o Valgrind, instalado com `sudo apt-get update && sudo apt-get install -y valgrind`.

## Utilitários

```sh
make new-key           # gera uma chave falsa do gateway de pagamentos
```

## Pipeline

O pipeline de integração contínua, definido em [`.github/workflows/ci.yml`](.github/workflows/ci.yml), é executado no GitHub Actions a cada push na `develop` e nas branches `feature/**`, e a cada pull request para a `develop`.

## GitFlow

O repositório segue o GitFlow: a `master` guarda as versões entregues, a `develop` integra a próxima versão, e cada mudança nasce numa branch `feature/*`, integrada à `develop` por pull request.

## Estrutura

```
marginalia/
├── .github/workflows/ci.yml   # pipeline de integração contínua
├── .semgrep/cpp-rules.yml     # regras do SAST
├── assets/                    # logotipo nas versões clara e escura
├── src/
│   ├── include/               # cabeçalhos das classes e do pagamento
│   ├── author.cpp
│   ├── book.cpp
│   ├── customer.cpp
│   ├── invoice.cpp
│   ├── main.cpp               # interpretação dos comandos
│   └── payment.cpp            # integração com o gateway de pagamentos
├── tests/
│   ├── dast/hostile_cases.cpp # casos hostis do DAST
│   └── unit/tests.cpp         # testes de unidade
├── tools/new_key.cpp          # gerador de chave falsa
└── Makefile
```
