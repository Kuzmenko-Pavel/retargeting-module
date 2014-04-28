#include <typeinfo>

#include <boost/regex/icu.hpp>
#include <boost/algorithm/string.hpp>

#include "Log.h"
#include "Sphinx.h"

Sphinx::Sphinx() :
    indexName(config->sphinx_index_)
{
    client = sphinx_create ( SPH_TRUE );

    sphinx_set_server ( client, config->sphinx_host_.c_str(), config->sphinx_port_ );

    sphinx_open ( client );

    sphinx_set_limits(client, 0, 3, 3, 3);

    sphinx_set_field_weights( client,
                              Config::Instance()->sphinx_fields_.size(),
                              Config::Instance()->sphinx_field_names_,
                              Config::Instance()->sphinx_field_weights_);

    sphinx_set_select(client,config->sphinx_select_.c_str());

    replaceSymbol = boost::make_u32regex("[^а-яА-Яa-zA-Z0-9-]");
    replaceExtraSpace = boost::make_u32regex("\\s+");
    replaceNumber = boost::make_u32regex("(\\b)\\d+(\\b)");
}
//-----------------------------------------------------------------------------------------------
Sphinx::~Sphinx()
{
    sphinx_close ( client );
    sphinx_destroy ( client );
}
//-----------------------------------------------------------------------------------------------
void Sphinx::setMode(int i)
{
    if(config->shpinx_modes_.size() > (u_int)i)
    {
        setMode(config->shpinx_modes_[i]);
    }
}
//-----------------------------------------------------------------------------------------------
void Sphinx::setMode(const shpinx_mode *m)
{
    sphinx_set_match_mode(client,map_match[m->match_]);
    sphinx_set_ranking_mode(client,map_rank[m->rank_], NULL);
    sphinx_set_sort_mode(client,map_sort[m->sort_], NULL);
}
//-----------------------------------------------------------------------------------------------
//Создаем фильтр
void Sphinx::addFilter(const std::string &columnName, const std::vector<sphinx_int64_t> &items)
{
    filter = (sphinx_int64_t *)new sphinx_int64_t[(int)items.size()];
    int counts = 0;

    for(auto it = items.begin(); it != items.end(); ++it)
    {
        filter[counts++] = *it;
    }

    if(sphinx_add_filter( client, columnName.c_str(), counts, filter, SPH_FALSE)!=SPH_TRUE)
    {
        Log::warn("sphinx filter is not working: %s", sphinx_error(client));
    }
}
//-----------------------------------------------------------------------------------------------
bool Sphinx::addFilter(const std::string &columnName, sphinx_int64_t *item)
{
    if(sphinx_add_filter( client, columnName.c_str(), 1, item, SPH_FALSE) != SPH_TRUE)
    {
        Log::warn("sphinx filter is not working: %s", sphinx_error(client));
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------------------------
void Sphinx::cleanFilter()
{
    sphinx_reset_filters ( client );
    delete [] filter;
}
//-----------------------------------------------------------------------------------------------
bool Sphinx::addQueryStrings(const std::string& query)
{
    try
    {
        std::string q, ret;

        q = Escape(query);
        boost::trim(q);
        if (q.empty())
        {
            return false;
        }
        std::string qs = stringWrapper(q);
        std::string qsn = stringWrapper(q, true);
        std::vector<std::string> strs;
        boost::split(strs,qs,boost::is_any_of("\t "),boost::token_compress_on);
        for(u_int i=0; i<Config::Instance()->sphinx_fields_.size(); i++)
        {
            std::string iret;
            for (std::vector<std::string>::iterator it = strs.begin(); it != strs.end(); ++it)
            {
                    if (it != strs.begin())
                    {
                        iret += " | @"+std::string(Config::Instance()->sphinx_field_names_[i])+" "+*it;
                    }
                    else
                    {
                        iret += "@"+std::string(Config::Instance()->sphinx_field_names_[i])+" "+*it;
                    }
            }
            if(i)
            {
                ret += "|("+iret+")";
            }
            else
            {
                ret += "("+iret+")";
            }
        }
        vSearchQuery.push_back(ret);
        return true;
    }
    catch (std::exception const &ex)
    {
        Log::err("exception %s: %s", typeid(ex).name(), ex.what());
        return false;
    }
}
//-----------------------------------------------------------------------------------------------
//select 1 as doc, count(*) from worker group by doc;
void Sphinx::processKeywords()
{
    try
    {
        sphinx_result * res;
        //Создаем запросы
        for (auto it = vSearchQuery.begin(); it != vSearchQuery.end(); ++it)
        {
            sphinx_add_query( client, (*it).c_str(), indexName.c_str(), NULL );
        }

        for(auto mod = config->shpinx_modes_.begin(); mod != config->shpinx_modes_.end(); ++mod)
        {
            setMode(*mod);

            res = sphinx_run_queries(client);
            if(!res)
            {
                Log::warn("unligal sphinx result: %s", sphinx_error(client));
                return;
            }

            //process sphinx results
            int numRes = sphinx_get_num_results(client);
            for (int tt=0; tt < numRes; tt++, res++)
            {
                if (res->status == SEARCHD_ERROR)
                {
                    Log::warn("sphinx: %s",res->error);
                    continue;
                }

                if(res->status == SEARCHD_WARNING)
                {
                    Log::warn("sphinx: %s",res->warning);
                }

#ifdef DEBUG
                dumpResult(res);
#endif // DEBUG

                if (res->num_matches > 0)
                {
                    Log::info("sphinx num matches: %d", res->num_matches);
                }

                postProcessKeywords(res);
            }
        }
    }
    catch (std::exception const &ex)
    {
        Log::warn("Непонятная sphinx ошибка: %s: %s: %s", typeid(ex).name(), ex.what(), sphinx_error(client));
    }

    return;
}
//-----------------------------------------------------------------------------------------------
void Sphinx::dumpResult(sphinx_result *res) const
{
    int i,j, k, mva_len;;
    unsigned int * mva;

    printf("retrieved %d of %d matches\n", res->total, res->total_found );

    printf ( "Query stats:\n" );
    for (i=0; i<res->num_words; i++ )
        printf ( "\t'%s' found %d times in %d documents\n",
                 res->words[i].word, res->words[i].hits, res->words[i].docs );

    printf ( "\nMatches:\n" );
    for( i=0; i<res->num_matches; i++ )
    {
        printf ( "%d. doc_id=%d, weight=%d", 1+i,
                 (int)sphinx_get_id ( res, i ), sphinx_get_weight ( res, i ) );

        for( j=0; j<res->num_attrs; j++ )
        {
            printf ( ", %s=", res->attr_names[j] );
            switch ( res->attr_types[j] )
            {
            case SPH_ATTR_MULTI64:
            case SPH_ATTR_MULTI:
                mva = sphinx_get_mva ( res, i, j );
                mva_len = *mva++;
                printf ( "(" );
                for ( k=0; k<mva_len; k++ )
                    printf ( k ? ",%u" : "%u", ( res->attr_types[j]==SPH_ATTR_MULTI ? mva[k] : (unsigned int)sphinx_get_mva64_value ( mva, k ) ) );
                printf ( ")" );
                break;

            case SPH_ATTR_FLOAT:
                printf ( "%f", sphinx_get_float ( res, i, j ) );
                break;
            case SPH_ATTR_STRING:
                printf ( "%s", sphinx_get_string ( res, i, j ) );
                break;
            default:
                printf ( "%u", (unsigned int)sphinx_get_int ( res, i, j ) );
                break;
            }
        }
        printf ( "\n" );
    }
    printf ( "\n" );
}
//-----------------------------------------------------------------------------------------------
std::map<std::string,int> Sphinx::map_match =
{
    { "SPH_MATCH_ALL", SPH_MATCH_ALL },
    { "SPH_MATCH_ANY", SPH_MATCH_ANY },
    { "SPH_MATCH_PHRASE", SPH_MATCH_PHRASE },
    { "SPH_MATCH_BOOLEAN", SPH_MATCH_BOOLEAN },
    { "SPH_MATCH_EXTENDED", SPH_MATCH_EXTENDED },
    { "SPH_MATCH_FULLSCAN", SPH_MATCH_FULLSCAN },
    { "SPH_MATCH_EXTENDED2", SPH_MATCH_EXTENDED2 }
};
//-----------------------------------------------------------------------------------------------
std::map<std::string,int> Sphinx::map_rank =
{
    { "SPH_RANK_PROXIMITY_BM25", SPH_RANK_PROXIMITY_BM25 },
    { "SPH_RANK_BM25", SPH_RANK_BM25 },
    { "SPH_RANK_NONE", SPH_RANK_NONE },
    { "SPH_RANK_WORDCOUNT", SPH_RANK_WORDCOUNT },
    { "SPH_RANK_PROXIMITY", SPH_RANK_PROXIMITY },
    { "SPH_RANK_MATCHANY", SPH_RANK_MATCHANY },
    { "SPH_RANK_FIELDMASK", SPH_RANK_FIELDMASK },
    { "SPH_RANK_SPH04", SPH_RANK_SPH04 },
    { "SPH_RANK_EXPR", SPH_RANK_EXPR },
    { "SPH_RANK_TOTAL", SPH_RANK_TOTAL }
};
//-----------------------------------------------------------------------------------------------
std::map<std::string,int> Sphinx::map_sort =
{
    { "SPH_SORT_RELEVANCE", SPH_SORT_RELEVANCE },
    { "SPH_SORT_ATTR_DESC", SPH_SORT_ATTR_DESC },
    { "SPH_SORT_ATTR_ASC", SPH_SORT_ATTR_ASC },
    { "SPH_SORT_TIME_SEGMENTS", SPH_SORT_TIME_SEGMENTS },
    { "SPH_SORT_EXTENDED", SPH_SORT_EXTENDED },
    { "SPH_SORT_EXPR", SPH_SORT_EXPR }
};
//-----------------------------------------------------------------------------------------------
/**
    \brief Нормализирует строку строку.

*/
std::string Sphinx::stringWrapper(const std::string &str, bool replaceNumbers)
{
    try
    {
        std::string t = str;
        //Заменяю все не буквы, не цифры, не минус на пробел
        t = boost::u32regex_replace(t,replaceSymbol," ");
        if (replaceNumbers)
        {
            //Заменяю отдельностояшие цифры на пробел, тоесть "у 32 п" замениться на
            //"у    п", а "АТ-23" останеться как "АТ-23"
            t = boost::u32regex_replace(t,replaceNumber," ");
        }
        //Заменяю дублируюшие пробелы на один пробел
        t = boost::u32regex_replace(t,replaceExtraSpace," ");
        boost::trim(t);
        return t;
    }
    catch (std::exception const &ex)
    {
        Log::err("exception: %s query: error: %s",typeid(ex).name(), str.c_str(), ex.what());
        return std::string();
    }
}
//-----------------------------------------------------------------------------------------------
std::string Sphinx::Escape(const std::string &str)
{
    std::string result;
    for (auto it = str.begin(); it != str.end(); it++)
    {
        switch (*it)
        {
        case '\\':
            result.append(" ");
            break;
        case '(':
            result.append(" ");
            break;
        case '?':
            result.append(" ");
            break;
        case '.':
            result.append(" ");
            break;
        case ':':
            result.append(" ");
            break;
        case ')':
            result.append(" ");
            break;
        case '|':
            result.append(" ");
            break;
        case '-':
            result.append(" ");
            break;
        case '!':
            result.append(" ");
            break;
        case '@':
            result.append(" ");
            break;
        case '~':
            result.append(" ");
            break;
        case '%':
            result.append(" ");
            break;
        case '"':
            result.append(" ");
            break;
        case '&':
            result.append(" ");
            break;
        case '/':
            result.append(" ");
            break;
        case '^':
            result.append(" ");
            break;
        case '$':
            result.append(" ");
            break;
        case '=':
            result.append(" ");
            break;
        case '\'':
            result.append(" ");
            break;
        case '\x00':
            result.append(" ");
            break;
        case '\n':
            result.append(" ");
            break;
        case '\r':
            result.append(" ");
            break;
        case '\x1a':
            result.append(" ");
            break;
        default:
            result.append(it, it + 1);
            break;
        }
    }
    boost::trim(result);
    if (result.empty())
    {
        return std::string();
    }
    return result;
}
