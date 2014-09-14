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
    bool newClient;
    bool test_mode_;
    bool json_;
    std::string ip_;
    std::string cookie_id_;
    std::string location_;
    boost::posix_time::ptime time_;
    std::string informer_id_;
    std::string account_id_;
    std::string retargeting_offer_id_;
    bool status;

    Params();
    bool parse(FCGX_Request *req);

    //*********************************************************************************************//
    std::string getIP() const;
    std::string getCookieId() const;
    std::string getUserKey() const;
    unsigned long long getUserKeyLong() const;
    std::string getCountry() const;
    std::string getRegion() const;
    std::string getInformerId() const;
    boost::posix_time::ptime getTime() const;
    bool isTestMode() const;
    bool isJson() const;
    std::vector<std::string> getExcludedOffers();
    std::string getScriptName() const;
    std::string getLocation() const;
    std::string getW() const;
    std::string getH() const;
    std::string getContext() const;
    std::string getSearch() const;
    std::string getUrl() const;

private:
    unsigned long long key_long;
    std::string country_;
    std::string countryByIp_;
    std::string region_;
    std::vector<std::string> excluded_offers_;
    std::string script_name_;
    std::string w_;
    std::string h_;
    std::string context_;//строка содержашяя контекст страницы
    std::string search_;
};

#endif // PARAMS_H
