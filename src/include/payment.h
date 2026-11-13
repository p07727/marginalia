# ifndef PAYMENT_H
# define PAYMENT_H

/* Autoriza no gateway de pagamentos o valor informado e devolve true quando o pagamento é aceito. */
bool processPayment(double amount);

# endif
