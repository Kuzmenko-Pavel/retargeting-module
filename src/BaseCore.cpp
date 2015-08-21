#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <boost/format.hpp>
#include <boost/regex.hpp>

#include <ctime>
#include <cstdlib>
#include <sstream>

#include <mongo/util/net/hostandport.h>

#include "../config.h"

#include "Log.h"
#include "BaseCore.h"
#include "Config.h"
#define MAXCOUNT 1000

BaseCore::BaseCore()
{
    time_service_started_ = boost::posix_time::second_clock::local_time();
}

BaseCore::~BaseCore()
{
}

/** Возвращает данные о состоянии службы
 *  TODO Надоб переписать с учётом использования boost::formater красивее будет как некак :)
 */
std::string BaseCore::Status()
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
    out.str("");
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
        "<body>\n<h1>Состояние службы Yottos  retargeting</h1>\n"
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
        "</td></tr>";

    out <<  "<tr><td>Сборка: </td><td>" << __DATE__ << " " << __TIME__ <<
        "</td></tr>";

    out << "</table>\n";

    out << "</body>\n</html>\n";

    return out.str();
}
