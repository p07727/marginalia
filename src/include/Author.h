# ifndef AUTHOR_H
# define AUTHOR_H

# include <string>

# define AUTHOR_NAME_SIZE 48

/* Representa o autor de um livro, com identificador, nome e gênero, que vale 'M' ou 'F'. */
class Author
{
public:
    Author();
    Author(int id, const char *name, char gender);
    int getId() const;
    const char *getName() const;
    char getGender() const;
    bool equals(const Author &another) const;
    std::string toString() const;

private:
    int id;
    char name[AUTHOR_NAME_SIZE];
    char gender;
};

# endif
