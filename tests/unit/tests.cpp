# include <cmath>
# include <cstdio>
# include <cstring>
# include <memory>
# include <string>

# include "Author.h"
# include "Book.h"
# include "Customer.h"
# include "Invoice.h"
# include "payment.h"

# define CHECK(condition) do { if (!(condition)) { printf("  failed: %s (line %d)\n", #condition, __LINE__); return false; } } while (0)

/* Compara dois valores monetários com tolerância de meio centavo. */
static bool sameValue(double a, double b)
{
    return std::fabs(a - b) < 0.005;
}

/* Cria o autor usado nos testes. */
static Author makeAuthor()
{
    return Author(1, "Machado de Assis", 'M');
}

/* Cria um livro de teste com o identificador e o preço informados. */
static Book makeBook(int id, double price)
{
    std::string name = "Book " + std::to_string(id);

    return Book(id, name.c_str(), makeAuthor(), price);
}

/* Confere o formato da representação textual do autor. */
static bool testAuthorText()
{
    CHECK(makeAuthor().toString() == "Author[name = Machado de Assis, gender = M]");
    return true;
}

/* Confere o formato da representação textual do cliente. */
static bool testCustomerText()
{
    Customer customer(7, "Ana Souza", 10);

    CHECK(customer.toString() == "Customer[id = 7, name = Ana Souza, discount = 10]");
    return true;
}

/* Confere o formato da representação textual do livro. */
static bool testBookText()
{
    Book book(1, "Dom Casmurro", makeAuthor(), 39.9);

    CHECK(book.toString() == "Book[name = Dom Casmurro, Author[name = Machado de Assis, gender = M], price = 39.90]");
    return true;
}

/* Confere que um título maior que o espaço reservado é guardado truncado, sem ultrapassar o vetor. */
static bool testLongTitle()
{
    std::string title(200, 'A');
    Book book(2, title.c_str(), makeAuthor(), 10.0);

    CHECK(strlen(book.getName()) == BOOK_NAME_SIZE - 1);
    return true;
}

/* Confere o total de um pedido com dois livros. */
static bool testAddBooks()
{
    Invoice invoice(1, Customer(7, "Ana Souza", 0));

    CHECK(invoice.addBook(makeBook(1, 39.9), 2));
    CHECK(invoice.addBook(makeBook(2, 25.0), 1));
    CHECK(sameValue(invoice.getTotal(), 104.8));
    return true;
}

/* Confere que o mesmo livro não entra duas vezes no pedido. */
static bool testRepeatedBook()
{
    Invoice invoice(1, Customer(7, "Ana Souza", 0));

    CHECK(invoice.addBook(makeBook(1, 39.9), 1));
    CHECK(!invoice.addBook(makeBook(1, 39.9), 3));
    CHECK(sameValue(invoice.getTotal(), 39.9));
    return true;
}

/* Confere a remoção de um livro do meio do pedido. */
static bool testRemoveBook()
{
    Invoice invoice(1, Customer(7, "Ana Souza", 0));

    CHECK(invoice.addBook(makeBook(1, 10.0), 1));
    CHECK(invoice.addBook(makeBook(2, 20.0), 1));
    CHECK(invoice.addBook(makeBook(3, 30.0), 1));
    CHECK(invoice.removeBook(makeBook(2, 20.0)));
    CHECK(sameValue(invoice.getTotal(), 40.0));
    CHECK(!invoice.removeBook(makeBook(2, 20.0)));
    return true;
}

/* Confere que remover um livro que não está no pedido devolve false. */
static bool testRemoveMissing()
{
    Invoice invoice(1, Customer(7, "Ana Souza", 0));

    CHECK(invoice.addBook(makeBook(1, 10.0), 1));
    CHECK(!invoice.removeBook(makeBook(9, 10.0)));
    return true;
}

/* Confere que o pedido recusa um livro quando está cheio. */
static bool testFullInvoice()
{
    Invoice invoice(1, Customer(7, "Ana Souza", 0));

    for (int i = 1; i <= MAX_BOOKS; i++)
    {
        CHECK(invoice.addBook(makeBook(i, 10.0), 1));
    }

    CHECK(!invoice.addBook(makeBook(MAX_BOOKS + 1, 10.0), 1));
    return true;
}

/* Confere a remoção do primeiro livro de um pedido cheio. */
static bool testRemoveFromFullInvoice()
{
    std::unique_ptr<Invoice> invoice(new Invoice(1, Customer(7, "Ana Souza", 0)));

    for (int i = 1; i <= MAX_BOOKS; i++)
    {
        CHECK(invoice->addBook(makeBook(i, 10.0), 1));
    }

    CHECK(invoice->removeBook(makeBook(1, 10.0)));
    CHECK(sameValue(invoice->getTotal(), 10.0 * (MAX_BOOKS - 1)));
    CHECK(!invoice->removeBook(makeBook(1, 10.0)));
    return true;
}

/* Confere o total com o desconto do cliente. */
static bool testDiscount()
{
    Invoice invoice(1, Customer(7, "Ana Souza", 10));

    CHECK(invoice.addBook(makeBook(1, 100.0), 1));
    CHECK(sameValue(invoice.getTotalAfterDiscount(), 90.0));
    return true;
}

/* Confere o formato da representação textual do pedido, com o total já com desconto. */
static bool testInvoiceText()
{
    Invoice invoice(1, Customer(7, "Ana Souza", 10));

    CHECK(invoice.addBook(makeBook(1, 100.0), 1));
    CHECK(invoice.toString() == "Invoice[id = 1, Customer[id = 7, name = Ana Souza, discount = 10], total = 90.00]");
    return true;
}

/* Confere que o pagamento de um valor positivo é autorizado e que um valor nulo é recusado. */
static bool testPayment()
{
    CHECK(processPayment(50.0));
    CHECK(!processPayment(0.0));
    return true;
}

/* Associa o nome de cada teste à função que o executa. */
struct Test
{
    const char *name;
    bool (*run)();
};

static const Test TESTS[] =
{
    {"author_text", testAuthorText},
    {"customer_text", testCustomerText},
    {"book_text", testBookText},
    {"book_long_title", testLongTitle},
    {"invoice_add_books", testAddBooks},
    {"invoice_repeated_book", testRepeatedBook},
    {"invoice_remove_book", testRemoveBook},
    {"invoice_remove_missing", testRemoveMissing},
    {"invoice_full", testFullInvoice},
    {"invoice_remove_from_full", testRemoveFromFullInvoice},
    {"invoice_discount", testDiscount},
    {"invoice_text", testInvoiceText},
    {"payment", testPayment},
};

static const int N_TESTS = sizeof TESTS / sizeof TESTS[0];

/* Executa um teste e imprime o resultado. */
static bool runTest(const Test &test)
{
    bool passed = test.run();

    printf("[%s] %s\n", passed ? "ok" : "FAILED", test.name);
    return passed;
}

/* Lista os testes com --list, executa um teste pelo nome ou, sem argumentos, executa todos. */
int main(int argc, char *argv[])
{
    if (argc == 2 && strcmp(argv[1], "--list") == 0)
    {
        for (int i = 0; i < N_TESTS; i++)
        {
            printf("%s\n", TESTS[i].name);
        }

        return 0;
    }

    if (argc == 2)
    {
        for (int i = 0; i < N_TESTS; i++)
        {
            if (strcmp(argv[1], TESTS[i].name) == 0)
            {
                return runTest(TESTS[i]) ? 0 : 1;
            }
        }

        printf("Unknown test: %s\n", argv[1]);
        return 2;
    }

    int failures = 0;

    for (int i = 0; i < N_TESTS; i++)
    {
        if (!runTest(TESTS[i]))
        {
            failures++;
        }
    }

    printf("%d of %d tests failed\n", failures, N_TESTS);
    return failures == 0 ? 0 : 1;
}
