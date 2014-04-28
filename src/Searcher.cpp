#include <boost/crc.hpp>

#include "Searcher.h"
#include "UrlParser.h"

Searcher::Searcher()
{
}

Searcher::~Searcher()
{
    //dtor
}

void Searcher::postProcessKeywords(sphinx_result * res)
{
    if (!res)
    {
        return;
    }

    for (int tt=0; tt<sphinx_get_num_results(client); tt++)
    {
        for( int i=0; i<res->num_matches; i++ )
        {
            result.push_back(sphinx_get_string( res, i, 0 ));
        }
        res++;
    }
}

std::list<std::string> &Searcher::run(const Params *params)
{
    boost::crc_32_type faccountid;

    faccountid.process_bytes(params->accountId().c_str(), params->accountId().length());
    sphinx_int64_t filter = faccountid.checksum();

    addFilter("faccountid", &filter);

    UrlParser *parsed = new UrlParser(params->getLocation());

    addQueryStrings(parsed->getLocation());
    addQueryStrings(parsed->getHost());
    addQueryStrings(parsed->getPath());
    addQueryStrings(parsed->getSearch());

    processKeywords();

    cleanFilter();

    result.clear();

    return result;
}

