# include "Book.h"

# include <cstdio>
# include <cstring>

/* Cria um livro vazio, usado apenas para reservar espaço nos vetores do pedido. */
Book::Book()
{
    id = 0;
    name[0] = '\0';
    price = 0.0;
}

/* Cria um livro sem preço definido, que fica igual a zero. */
Book::Book(int id, const char *name, const Author &author) : Book(id, name, author, 0.0)
{
}

/* Cria um livro com o identificador, o título, o autor e o preço informados. */
Book::Book(int id, const char *name, const Author &author, double price)
{
    this->id = id;
    strcpy(this->name, name);
    this->author = author;
    this->price = price;
}

/* Devolve o título do livro. */
const char *Book::getName() const
{
    return name;
}

/* Devolve o autor do livro. */
Author Book::getAuthor() const
{
    return author;
}

/* Devolve o preço do livro. */
double Book::getPrice() const
{
    return price;
}

/* Altera o preço do livro. */
void Book::setPrice(double price)
{
    this->price = price;
}

/* Compara dois livros pelo identificador. */
bool Book::equals(const Book &another) const
{
    return id == another.id;
}

/* Monta a representação textual do livro, no formato Book[name = ?, Author[name = ?, gender = ?], price = ?]. */
std::string Book::toString() const
{
    char price_text[32];

    snprintf(price_text, sizeof price_text, "%.2f", price);
    return "Book[name = " + std::string(name) + ", " + author.toString() + ", price = " + price_text + "]";
}
