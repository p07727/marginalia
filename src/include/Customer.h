# ifndef CUSTOMER_H
# define CUSTOMER_H

# include <string>

/* Representa o cliente da livraria, com identificador, nome e desconto, um percentual de 0 a 100 aplicado ao total do pedido. */
class Customer
{
public:
    Customer(int id, const std::string &name, int discount);
    int getId() const;
    std::string getName() const;
    int getDiscount() const;
    void setDiscount(int discount);
    bool equals(const Customer &another) const;
    std::string toString() const;

private:
    int id;
    std::string name;
    int discount;
};

# endif
