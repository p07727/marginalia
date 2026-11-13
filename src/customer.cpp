# include "Customer.h"

/* Cria um cliente com o identificador, o nome e o desconto informados. */
Customer::Customer(int id, const std::string &name, int discount)
{
    this->id = id;
    this->name = name;
    this->discount = discount;
}

/* Devolve o identificador do cliente. */
int Customer::getId() const
{
    return id;
}

/* Devolve o nome do cliente. */
std::string Customer::getName() const
{
    return name;
}

/* Devolve o desconto do cliente, em percentual. */
int Customer::getDiscount() const
{
    return discount;
}

/* Altera o desconto do cliente, em percentual. */
void Customer::setDiscount(int discount)
{
    this->discount = discount;
}

/* Compara dois clientes pelo identificador. */
bool Customer::equals(const Customer &another) const
{
    return id == another.id;
}

/* Monta a representação textual do cliente, no formato Customer[id = ?, name = ?, discount = ?]. */
std::string Customer::toString() const
{
    return "Customer[id = " + std::to_string(id) + ", name = " + name + ", discount = " + std::to_string(discount) + "]";
}
