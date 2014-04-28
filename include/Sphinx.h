#ifndef SPHINX_H
#define SPHINX_H

#include <string>
#include <vector>
#include <map>

#include <boost/regex/icu.hpp>

#include <sphinxclient.h>

#include "Config.h"

class Sphinx
{
public:
    Sphinx();
    ~Sphinx();

    static std::map<std::string,int> map_match;
    static std::map<std::string,int> map_rank;
    static std::map<std::string,int> map_sort;

    /** \brief Метод обработки запроса к индексу.
     */
    void addFilter(const std::string &columnName, const std::vector<sphinx_int64_t> &items);
    bool addQueryStrings(const std::string& query);
    void processKeywords();
    virtual void postProcessKeywords(sphinx_result * res) = 0;
    void cleanFilter();
    void setMode(int i);

protected:
    sphinx_client* client;
    bool addFilter(const std::string &columnName, sphinx_int64_t *item);
private:
    const std::string indexName;
    boost::u32regex replaceSymbol,replaceExtraSpace,replaceNumber;
    std::vector<std::string> vSearchQuery;


    sphinx_int64_t *filter;

    void dumpResult(sphinx_result *res) const;

    void setMode(const shpinx_mode *m);

    std::string stringWrapper(const std::string &str, bool replaceNumbers=false);
    std::string Escape(const std::string &str);
};

#endif
