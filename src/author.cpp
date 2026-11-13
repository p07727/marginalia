# include "Author.h"

# include <cstdio>

/* Cria um autor vazio, usado apenas para reservar espaço nos vetores de livros. */
Author::Author()
{
    id = 0;
    name[0] = '\0';
    gender = 'F';
}

/* Cria um autor com o identificador, o nome e o gênero informados. */
Author::Author(int id, const char *name, char gender)
{
    this->id = id;
    snprintf(this->name, sizeof this->name, "%s", name);
    this->gender = gender;
}

/* Devolve o identificador do autor. */
int Author::getId() const
{
    return id;
}

/* Devolve o nome do autor. */
const char *Author::getName() const
{
    return name;
}

/* Devolve o gênero do autor. */
char Author::getGender() const
{
    return gender;
}

/* Compara dois autores pelo identificador. */
bool Author::equals(const Author &another) const
{
    return id == another.id;
}

/* Monta a representação textual do autor, no formato Author[name = ?, gender = ?]. */
std::string Author::toString() const
{
    return "Author[name = " + std::string(name) + ", gender = " + gender + "]";
}
