#include <signal.h>

#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

#include "KompexSQLiteStatement.h"
#include "KompexSQLiteException.h"

#include "../config.h"

#include <ctime>
#include <cstdlib>

#include "Config.h"
#include "Log.h"
#include "Core.h"

#define CMD_SIZE 8192
Core::Core()
{
    tid = pthread_self();

    sigset_t es;
    sigfillset(&es);
    pthread_sigmask(SIG_BLOCK, &es, NULL);

    cmd = new char[CMD_SIZE];

    rcRetargeting = new RedisClient(config->redis_retargeting_.host,
                                    config->redis_retargeting_.port,
                                    100);
    rcRetargeting->connect();

    rcShortTerm = new RedisClient(config->redis_short_term_.host,
                                    config->redis_short_term_.port,
                                    100);
    rcShortTerm->connect();

    Log::info("%s[%ld] start",__func__,tid);
}

Core::~Core()
{
    delete []cmd;
}

/** Обработка запроса на показ рекламы с параметрами ``params``.
	Изменён RealInvest Soft */
void Core::Process(Params *prms)
{
    std::vector<long> result;
    boost::posix_time::ptime startTime;

    startTime = boost::posix_time::microsec_clock::local_time();

    params = prms;

    std::string key = params->getUserKey();

    if(params->accountId().empty())
    {
        Log::warn("%s wrong input params: account id from: %s",__func__,params->getIP().c_str());
        return;
    }

    if(params->retargetingId().empty())
    {
        Log::warn("%s wrong input params: retargeting id from: %s",__func__,params->getIP().c_str());
    }
    else
    {
        getOffer(result);

            for(auto j = result.begin(); j!=result.end(); ++j)
            {
                rcRetargeting->zadd(key,-1,*j);
                rcRetargeting->expire(key, prms->trackingTime());
            }
    }

    rcShortTerm->set(key, params->getSearch()+" "+params->getContext(),config->redis_short_term_.ttl);
    std::string rr = rcShortTerm->get(key);
    printf("redis: %s\n",rr.c_str());

    Log::info("%s[%ld]core time: %s found size: %d",__func__,
              tid, boost::posix_time::to_simple_string(boost::posix_time::microsec_clock::local_time() - startTime).c_str(),
              result.size());

    result.clear();

    request_processed_++;

    return;
}

bool Core::getOffer(std::vector<long> &result)
{
    Kompex::SQLiteStatement *pStmt;
    pStmt = new Kompex::SQLiteStatement(config->pDb->pDatabase);

    sqlite3_snprintf(CMD_SIZE, cmd, config->offerSqlAll.c_str(),
                        params->accountId().c_str(),
                        params->retargetingId().c_str());

#ifdef DEBUG
    printf("%s\n",cmd);
#endif // DEBUG

    try
    {
        pStmt->Sql(cmd);

        while(pStmt->FetchRow())
        {
            result.push_back(pStmt->GetColumnInt64(0));
        }

        pStmt->FreeQuery();
    }
    catch(Kompex::SQLiteException &ex)
    {
        printf("%s\n", cmd);
        Log::err("%s error: %s",__func__,ex.GetString().c_str());
        delete pStmt;
        return false;
    }

    delete pStmt;

    return true;
}
