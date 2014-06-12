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

    for(auto i = config->redis_retargeting_.begin(); i != config->redis_retargeting_.end(); ++i)
    {
        RedisClient *rc = new RedisClient((*i).host,(*i).port,(*i).ttl*24*3600);
        rc->connect();
        rcRetargeting.push_back(rc);
    }

    for(auto i = config->redis_short_term_.begin(); i != config->redis_short_term_.end(); ++i)
    {
        RedisClient *rc = new RedisClient((*i).host,(*i).port,(*i).ttl*24*3600);
        rc->connect();
        rcShortTerm.push_back(rc);
    }

    std::clog<<"["<<tid<<"]start"<<std::endl;
}

Core::~Core()
{
    delete []cmd;
}

/** Обработка запроса на показ рекламы с параметрами ``params``.
	Изменён RealInvest Soft */
void Core::Process(Params *prms)
{
    boost::posix_time::ptime startTime;
    std::vector<long> result;

    startTime = boost::posix_time::microsec_clock::local_time();

    params = prms;

    std::string key = params->getUserKey();

    if(params->accountId().empty())
    {
        std::clog<<"["<<tid<<"]"<<__func__
        <<" wrong input params: retargeting id from: "<<params->getIP()
        <<std::endl;
        return;
    }

    if(params->retargetingId().empty())
    {
        std::clog<<"["<<tid<<"]"<<__func__
        <<" wrong input params: retargeting id from: "<<params->getIP()
        <<std::endl;
    }
    else
    {
        getOffer(result);

        for(auto i = rcRetargeting.begin(); i != rcRetargeting.end(); ++i)
        {
            for(auto j = result.begin(); j!=result.end(); ++j)
            {
                (*i)->zadd(key,0,*j);
            }
        }
    }

    for(auto i = rcShortTerm.begin(); i != rcShortTerm.end(); ++i)
    {
        (*i)->set(key, params->getSearch()+" "+params->getContext());
    }

    std::clog<<"["<<tid<<"]"
    <<"core time:"<<boost::posix_time::to_simple_string(boost::posix_time::microsec_clock::local_time() - startTime)
    <<" account id:"<<params->accountId()
    <<" offer id:"<<params->retargetingId()
    <<" key:"<<key
    <<" found:"<<result.size();
    if(result.size())
    {
        std::clog<<"offer ids:";
        for(auto o = result.begin(); o != result.end(); ++o)
        {
            std::clog<<(*o);
            if(o != result.end())
            {
                std::clog<<",";
            }
        }
    }
    std::clog<<" search:"<<params->getSearch()
    <<" context:"<<params->getContext()
    <<std::endl;

    result.clear();

    request_processed_++;

    return;
}

void Core::PostProcess()
{

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
        std::clog<<"["<<tid<<"]"<<__func__
        <<" error: "<<ex.GetString()
        <<std::endl;

        delete pStmt;
        return false;
    }

    delete pStmt;

    return true;
}
