#include "sphinx_field.h"

sphinx_field::sphinx_field(const char *n, const char *w)
{
    name = n;
    weight = strtol(w,NULL,10);
}

sphinx_field::~sphinx_field()
{
    //dtor
}
