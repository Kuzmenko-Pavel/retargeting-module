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

    searcher = new Searcher();

    for(auto i = config->redis_long_term_.begin(); i!= config->redis_long_term_.end(); ++i)
    {
        RedisClient *r = new RedisClient((*i).host, (*i).port, config->redis_long_term_ttl_);
        r->connect();
        rc.push_back(r);
    }

    for(auto i = config->redis_retargeting_.begin(); i!= config->redis_retargeting_.end(); ++i)
    {
        RedisClient *r = new RedisClient((*i).host, (*i).port, config->redis_retargeting_ttl_);
        r->connect();
        rc.push_back(r);
    }

    Log::info("[%ld]core start",tid);
}

Core::~Core()
{
    delete []cmd;
    delete searcher;
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
        Log::warn("wrong input params: account id from: %s",params->getIP().c_str());
        return;
    }

    if(params->retargetingId().empty())
    {
        Log::warn("wrong input params: retargeting id from: %s",params->getIP().c_str());
    }
    else
    {
        getOffer(result);

        for(auto i = rc.begin(); i!= rc.end(); ++i)
        {
            for(auto j = result.begin(); j!=result.end(); ++j)
            {
                (*i)->zadd(key,-1,*j);
                (*i)->expire(key, prms->trackingTime());
            }
        }
    }

    //result = searcher->run(params);


    Log::info("[%ld]core time: %s found size: %d",
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
        Log::err("Core::getOffer error: %s", ex.GetString().c_str());
        delete pStmt;
        return false;
    }

    delete pStmt;

    return true;
}

std::string Core::Status()
{
    // Обработано запросов на момент прошлого обращения к статистике
    static int last_time_request_processed = 0;

    // Время последнего обращения к статистике
    static boost::posix_time::ptime last_time_accessed;

    boost::posix_time::time_duration d;

    // Вычисляем количество запросов в секунду
    if (last_time_accessed.is_not_a_date_time())
	last_time_accessed = time_service_started_;
    boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
    int millisecs_since_last_access =
	    (now - last_time_accessed).total_milliseconds();
    int millisecs_since_start =
	    (now - time_service_started_).total_milliseconds();
    int requests_per_second_current = 0;
    int requests_per_second_average = 0;
    if (millisecs_since_last_access)
	requests_per_second_current =
		(request_processed_ - last_time_request_processed) * 1000 /
		millisecs_since_last_access;
    if (millisecs_since_start)
	requests_per_second_average = request_processed_ * 1000 /
				      millisecs_since_start;

    last_time_accessed = now;
    last_time_request_processed = request_processed_;

    std::stringstream out;
    out << "<html>\n"
	    "<head><meta http-equiv=\"content-type\" content=\"text/html; "
	    "charset=UTF-8\">\n"
	    "<style>\n"
	    "body { font-family: Arial, Helvetica, sans-serif; }\n"
	    "h1, h2, h3 {font-family: \"georgia\", serif; font-weight: 400;}\n"
	    "table { border-collapse: collapse; border: 1px solid gray; }\n"
	    "td { border: 1px dotted gray; padding: 5px; font-size: 10pt; }\n"
	    "th {border: 1px solid gray; padding: 8px; font-size: 10pt; }\n"
	    "</style>\n"
	    "</head>"
	    "<body>\n<h1>Состояние службы Yottos GetMyAd worker</h1>\n"
	    "<table>"
	    "<tr>"
	    "<td>Обработано запросов:</td> <td><b>" << request_processed_ <<
	    "</b> (" << requests_per_second_current << "/сек, "
	    " в среднем " << requests_per_second_average << "/сек) "
	    "</td></tr>\n";
    out << "<tr><td>Имя сервера: </td> <td>" <<
	    (getenv("SERVER_NAME")? getenv("SERVER_NAME"): "неизвестно") <<
	    "</td></tr>\n";
    out << "<tr><td>Текущее время: </td> <td>" <<
	    boost::posix_time::second_clock::local_time() <<
	    "</td></tr>\n";

    out << "<tr><td>Время запуска:</td> <td>" << time_service_started_ <<
	    "</td></tr>" <<
    out <<  "<tr><td>Сборка: </td><td>" << __DATE__ << " " << __TIME__ <<
	    "</td></tr>";
    out << "</table>\n";
    out << "</body>\n</html>\n";

    return out.str();
}

