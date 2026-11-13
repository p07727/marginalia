# include <cmath>
# include <cstdio>
# include <iostream>
# include <map>
# include <memory>
# include <sstream>
# include <string>

# include "Author.h"
# include "Book.h"
# include "Customer.h"
# include "Invoice.h"
# include "payment.h"

/* Guarda o estado da sessão: os autores e os livros cadastrados e o pedido do cliente atual. */
struct Store
{
    std::map<int, Author> authors;
    std::map<int, Book> books;
    std::unique_ptr<Invoice> invoice;
};

/* Lê da linha um número inteiro e devolve false quando o texto não é um inteiro válido. */
static bool readInt(std::istringstream &in, int &value)
{
    std::string text;
    size_t used = 0;

    if (!(in >> text))
    {
        return false;
    }

    try
    {
        value = std::stoi(text, &used);
    }
    catch (const std::exception &)
    {
        return false;
    }

    return used == text.size();
}

/* Lê da linha um número real e devolve false quando o texto não é um número finito. */
static bool readDouble(std::istringstream &in, double &value)
{
    std::string text;
    size_t used = 0;

    if (!(in >> text))
    {
        return false;
    }

    try
    {
        value = std::stod(text, &used);
    }
    catch (const std::exception &)
    {
        return false;
    }

    return used == text.size() && std::isfinite(value);
}

/* Lê o restante da linha, sem os espaços iniciais, como um texto livre. */
static std::string readRest(std::istringstream &in)
{
    std::string rest;

    std::getline(in >> std::ws, rest);
    return rest;
}

/* Informa se a linha não tem mais nada além do que já foi lido. */
static bool atEnd(std::istringstream &in)
{
    std::string extra;

    return !(in >> extra);
}

/* Trata o comando "customer <id> <discount> <name>", que abre o pedido do cliente. */
static std::string commandCustomer(Store &store, std::istringstream &in)
{
    int id = 0;
    int discount = 0;

    if (!readInt(in, id) || !readInt(in, discount))
    {
        return "Error: use customer <id> <discount> <name>";
    }

    std::string name = readRest(in);

    if (name.empty())
    {
        return "Error: missing customer name";
    }

    store.invoice.reset(new Invoice(1, Customer(id, name, discount)));
    return "Invoice opened for customer " + std::to_string(id);
}

/* Trata o comando "author <id> <gender> <name>", que cadastra um autor. */
static std::string commandAuthor(Store &store, std::istringstream &in)
{
    int id = 0;
    std::string gender;

    if (!readInt(in, id) || !(in >> gender) || (gender != "M" && gender != "F"))
    {
        return "Error: use author <id> <M or F> <name>";
    }

    std::string name = readRest(in);

    if (name.empty())
    {
        return "Error: missing author name";
    }

    store.authors[id] = Author(id, name.c_str(), gender[0]);
    return "Author " + std::to_string(id) + " registered";
}

/* Trata o comando "book <id> <author_id> <price> <title>", que cadastra um livro no catálogo. */
static std::string commandBook(Store &store, std::istringstream &in)
{
    int id = 0;
    int author_id = 0;
    double price = 0.0;

    if (!readInt(in, id) || !readInt(in, author_id) || !readDouble(in, price) || price < 0.0)
    {
        return "Error: use book <id> <author_id> <price> <title>";
    }

    std::string title = readRest(in);

    if (title.empty())
    {
        return "Error: missing book title";
    }

    std::map<int, Author>::iterator author = store.authors.find(author_id);

    if (author == store.authors.end())
    {
        return "Error: unknown author";
    }

    Book book(id, title.c_str(), author->second, price);
    store.books[id] = book;
    return "Book " + std::to_string(id) + " registered";
}

/* Trata os comandos "add <book_id> <quantity>" e "remove <book_id>", que alteram o pedido. */
static std::string commandItem(Store &store, std::istringstream &in, bool adding)
{
    int book_id = 0;
    int quantity = 0;

    if (!store.invoice)
    {
        return "Error: open the invoice with the customer command";
    }

    if (!readInt(in, book_id) || (adding && !readInt(in, quantity)) || !atEnd(in))
    {
        return adding ? "Error: use add <book_id> <quantity>" : "Error: use remove <book_id>";
    }

    std::map<int, Book>::iterator book = store.books.find(book_id);

    if (book == store.books.end())
    {
        return "Error: unknown book";
    }

    if (adding)
    {
        if (!store.invoice->addBook(book->second, quantity))
        {
            return "Error: book not added to the invoice";
        }

        return "Book " + std::to_string(book_id) + " added to the invoice with quantity " + std::to_string(quantity);
    }

    if (!store.invoice->removeBook(book->second))
    {
        return "Error: book is not in the invoice";
    }

    return "Book " + std::to_string(book_id) + " removed from the invoice";
}

/* Trata o comando "pay", que autoriza o pagamento do total do pedido com desconto. */
static std::string commandPay(Store &store)
{
    char message[96];
    double total;

    if (!store.invoice)
    {
        return "Error: open the invoice with the customer command";
    }

    total = store.invoice->getTotalAfterDiscount();

    if (!processPayment(total))
    {
        return "Error: payment declined";
    }

    snprintf(message, sizeof message, "Payment of %.2f authorized", total);
    return message;
}

/* Interpreta um comando e devolve a resposta que deve ser impressa. */
static std::string answer(Store &store, const std::string &line)
{
    std::istringstream in(line);
    std::string command;

    in >> command;

    if (command == "customer")
    {
        return commandCustomer(store, in);
    }

    if (command == "author")
    {
        return commandAuthor(store, in);
    }

    if (command == "book")
    {
        return commandBook(store, in);
    }

    if (command == "add" || command == "remove")
    {
        return commandItem(store, in, command == "add");
    }

    if (command == "invoice" && atEnd(in))
    {
        return store.invoice ? store.invoice->toString() : "Error: open the invoice with the customer command";
    }

    if (command == "pay" && atEnd(in))
    {
        return commandPay(store);
    }

    return "Error: unknown command";
}

/* Lê os comandos da entrada padrão, um por linha, e responde a cada um com uma confirmação, com o resultado ou com uma mensagem de erro. */
int main(int argc, char *argv[])
{
    Store store;
    std::string line;

    if (argc > 1)
    {
        std::cerr << "Usage: " << argv[0] << " < commands.txt" << std::endl;
        return 1;
    }

    while (std::getline(std::cin, line))
    {
        if (line.empty() || line[0] == '#')
        {
            continue;
        }

        std::cout << answer(store, line) << std::endl;
    }

    return 0;
}
