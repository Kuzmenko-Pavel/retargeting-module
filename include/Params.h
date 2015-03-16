#ifndef PARAMS_H
#define PARAMS_H

#include <sstream>
#include <string>
#include <boost/date_time.hpp>

#include <fcgi_stdio.h>

/** \brief Параметры, которые определяют показ рекламы */
class Params
{
public:
    std::string ip_;
    std::string cookie_id_;
    std::string location_;
    boost::posix_time::ptime time_;
    int time_cookies_;
    std::string account_id_;
    std::string retargeting_offer_id_;
    bool status;

    Params();
    bool parse(FCGX_Request *req);

    //*********************************************************************************************//
    std::string getIP() const;
    std::string getCookieId() const;
    std::string getUserKey() const;
    int getTimeCookie() const;
    int getSecondTimeCookie() const;
    unsigned long long getUserKeyLong() const;
    std::string getCountry() const;
    std::string getRegion() const;
    boost::posix_time::ptime getTime() const;
    std::string getScriptName() const;
    std::string getLocation() const;
    std::string getContext() const;
    std::string getSearch() const;
    std::string getUrl() const;

private:
    unsigned long long key_long;
    std::string country_;
    std::string countryByIp_;
    std::string region_;
    std::string script_name_;
    std::string context_;//строка содержашяя контекст страницы
    std::string search_;
};

#endif // PARAMS_H
