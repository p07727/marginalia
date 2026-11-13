# ifndef INVOICE_H
# define INVOICE_H

# include <string>

# include "Book.h"
# include "Customer.h"

# define MAX_BOOKS 10

/* Representa o pedido de uma compra, com o cliente, os livros comprados e a quantidade de cada um. */
class Invoice
{
public:
    Invoice(int id, const Customer &customer);
    int getId() const;
    Customer getCustomer() const;
    bool addBook(const Book &book, int quantity);
    bool removeBook(const Book &book);
    double getTotal() const;
    double getTotalAfterDiscount() const;
    bool equals(const Invoice &another) const;
    std::string toString() const;

private:
    int id;
    Customer customer;
    int nBooks;
    int quantities[MAX_BOOKS];
    Book books[MAX_BOOKS];
};

# endif
