# ifndef BOOK_H
# define BOOK_H

# include <string>

# include "Author.h"

# define BOOK_NAME_SIZE 64

/* Representa um livro do catálogo da livraria, com identificador, título, autor e preço. */
class Book
{
public:
    Book();
    Book(int id, const char *name, const Author &author);
    Book(int id, const char *name, const Author &author, double price);
    const char *getName() const;
    Author getAuthor() const;
    double getPrice() const;
    void setPrice(double price);
    bool equals(const Book &another) const;
    std::string toString() const;

private:
    int id;
    char name[BOOK_NAME_SIZE];
    Author author;
    double price;
};

# endif
