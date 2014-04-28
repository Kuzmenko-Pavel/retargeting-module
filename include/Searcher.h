#ifndef SEARCHER_H
#define SEARCHER_H

#include <string>
#include <list>

#include <Sphinx.h>
#include <Params.h>

class Searcher : public Sphinx
{
    public:
        Searcher();
        virtual ~Searcher();
        std::list<std::string> &run(const Params *params);
    protected:
    private:
        std::list<std::string> result;
        virtual void postProcessKeywords(sphinx_result * res);
};

#endif // SEARCHER_H
