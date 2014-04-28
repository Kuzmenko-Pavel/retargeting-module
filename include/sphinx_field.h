#ifndef SPHINX_FIELD_H
#define SPHINX_FIELD_H

#include <string>

class sphinx_field
{
    public:
        std::string name;
        int weight;

        sphinx_field(const char *n, const char *w);
        virtual ~sphinx_field();
    protected:
    private:
};

#endif // SPHINX_FIELD_H
