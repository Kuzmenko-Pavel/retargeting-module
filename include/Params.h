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
    std::string target_;
    std::string action_;
    std::string retargeting_offer_id_;
    bool status;
    unsigned long long key_long;

    Params();
    bool parse(FCGX_Request *req);

    //*********************************************************************************************//
    std::string getIP() const;
    std::string getCookieId() const;
    std::string getUserKey() const;
    int getTimeCookie() const;
    int getSecondTimeCookie() const;
    unsigned long long getUserKeyLong() const;
    boost::posix_time::ptime getTime() const;
    std::string getScriptName() const;
    std::string getLocation() const;

private:
    std::string script_name_;
};

#endif // PARAMS_H
