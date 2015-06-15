#include <signal.h>

#include <boost/foreach.hpp>
#include <boost/date_time.hpp>
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
#include "base64.h"

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
        SimpleRedisClient *rc = new SimpleRedisClient((*i).host,(*i).port,"ret");
        rc->setTimeout((*i).ttl*24*3600);
        rcRetargeting.push_back(rc);
    }

    std::clog<<"["<<tid<<"]start"<<std::endl;
}

Core::~Core()
{
    delete []cmd;
}

std::string Core::Process(Params *prms)
{
    boost::posix_time::ptime startTime;
    std::vector<long> result;
    std::string html;

    startTime = boost::posix_time::microsec_clock::local_time();

    params = prms;

    if(params->account_id_.empty())
    {
        std::clog<<"["<<tid<<"]"<<__func__
                 <<" wrong input params: retargeting id from: "<<params->getIP()
                 <<std::endl;
        return html;
    }

    if(params->retargeting_offer_id_.empty())
    {
        std::clog<<"["<<tid<<"]"<<__func__
                 <<" wrong input params: retargeting id from: "<<params->getIP()
                 <<std::endl;
    }
    else
    {
        if(getOffer(result))
        {
            for(auto i = rcRetargeting.begin(); i != rcRetargeting.end(); ++i)
            {
                for(auto j = result.begin(); j!=result.end(); ++j)
                {
                    (*i)->zadd(params->getUserKey(),::time(NULL),*j);
                    (*i)->expire(params->getUserKey().c_str(), params->getSecondTimeCookie());
                }
            }
        }
    }

    std::clog<<"["<<tid<<"]";

    if(config->logCoretime)
        std::clog<<" core time:"<< boost::posix_time::to_simple_string(boost::posix_time::microsec_clock::local_time() - startTime);

    if(config->logAccountId)
        std::clog<<" account id:"<<params->account_id_;

    if(config->logKey)
        std::clog<<" key:"<<params->getUserKey();

    if(config->logOutPutOfferIds)
    {
        std::clog<<" offer ids:";
        if(result.size())
        {
            for(auto o = result.begin(); o != result.end(); ++o)
            {
                if(o != result.begin())
                {
                    std::clog<<","<<(*o);
                }
                else
                {
                    std::clog<<(*o);
                }
            }
        }
        else
        {
            std::clog<<"0";
        }
    }


    std::clog<<std::endl;

    result.clear();

    request_processed_++;

    html = boost::str(boost::format(config->template_out_)
                       % params->retargeting_offer_id_
                       % params->getSecondTimeCookie()
                       % params->account_id_
                       % params->target_);
    return html;
}


bool Core::getOffer(std::vector<long> &result)
{
    Kompex::SQLiteStatement *pStmt;
    pStmt = new Kompex::SQLiteStatement(config->pDb->pDatabase);

    sqlite3_snprintf(CMD_SIZE, cmd, config->offerSqlAll.c_str(),
                     params->account_id_.c_str(),
                     params->retargeting_offer_id_.c_str());

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

    return result.size() ? true : false;
}

