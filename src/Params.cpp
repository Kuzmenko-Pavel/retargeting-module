#include <sstream>

#include <boost/algorithm/string/trim.hpp>
#include <boost/regex/icu.hpp>
#include <boost/regex.hpp>
#include <boost/date_time.hpp>
#include <boost/algorithm/string.hpp>

#include <string>

#include "Params.h"
#include "GeoIPTools.h"
#include "Log.h"
#include "UrlParser.h"
#include "Config.h"


Params::Params()
{
    time_ = boost::posix_time::second_clock::local_time();
}

std::string time_t_to_string(time_t t)
{
    std::stringstream sstr;
    sstr << t;
    return sstr.str();
}


bool Params::parse(FCGX_Request *req)
{
    char *tmp_str = nullptr;
    UrlParser *query_parser;

    if (!(tmp_str = FCGX_GetParam("QUERY_STRING", req->envp)))
    {
        std::clog<<"query string is not set"<<std::endl;
        return false;
    }
    else
    {
        query_parser = new UrlParser(tmp_str);
    }

    if(query_parser->param("show") == "status")
    {
        status = true;
        delete query_parser;
        return true;
    }
    else
    {
        status = false;
    }

    account_id_ = query_parser->param("ac");
    if(account_id_.empty())
    {
        std::clog<<__func__<<" miss account id"<<std::endl;
        delete query_parser;
        return false;
    }

    std::string s_time_cookies_ = query_parser->param("time");
    if(s_time_cookies_.empty())
    {
        std::clog<<__func__<<" miss time"<<std::endl;
        time_cookies_ = 365;
    }
    else
    {
        time_cookies_ = atoi(s_time_cookies_.c_str());
    }

    retargeting_offer_id_ = query_parser->param("offer_id");
    if(account_id_.empty())
    {
        std::clog<<__func__<<" miss offer id"<<std::endl;
        delete query_parser;
        return false;
    }

    if( !(tmp_str = FCGX_GetParam("REMOTE_ADDR", req->envp)) )
    {
        std::clog<<"remote address is not set"<<std::endl;
        delete query_parser;
        return false;
    }
    else
    {
        ip_ = std::string(tmp_str);
    }
    
    if((tmp_str = FCGX_GetParam("HTTP_COOKIE", req->envp)))
    {
        std::string visitor_cookie = std::string(tmp_str);

        std::vector<std::string> strs;
        boost::split(strs, visitor_cookie, boost::is_any_of(";"));

        for (unsigned int i=0; i<strs.size(); i++)
        {
            if(strs[i].find(config->cookie_name_) != std::string::npos)
            {
                std::vector<std::string> name_value;

                boost::split(name_value, strs[i], boost::is_any_of("="));

                if (name_value.size() == 2)
                {
                    cookie_id_ = name_value[1];
                    break;
                }

            }
        }
    }

    //get country code
    country_ = query_parser->param("country");
    if(country_.empty())
    {
        country_ = geoip->country_code_by_addr(ip_);
    }

    //get region name
    region_ = query_parser->param("region");
    if(region_.empty())
    {
        region_ = geoip->region_code_by_addr(ip_);
    }

    search_ = query_parser->param("search");
    //context_ = query_parser->param("context");
    //location_ = query_parser->param("location");

    /// ID посетителя, cookie
    if(cookie_id_.empty())
    {
        //make new session id
        //if(inet_pton(AF_INET, ip_.c_str(), &ipval))
        //{
        //    key_long = ipval.s_addr;
        //}
        //else
        //{
        //    key_long = 0;
        //}

        //key_long = key_long << 32;
        key_long = time(NULL);
        cookie_id_ = time_t_to_string(key_long);
    }
    else
    {
        cookie_id_ = cookie_id_;
        boost::u32regex replaceSymbol = boost::make_u32regex("[^0-9]");
        cookie_id_ = boost::u32regex_replace(cookie_id_ ,replaceSymbol,"");
        boost::trim(cookie_id_);
        key_long = atol(cookie_id_.c_str());
    }

    delete query_parser;
    return true;
}

std::string Params::getIP() const
{
    return ip_;
}
std::string Params::getCookieId() const
{
    return cookie_id_;
}

std::string Params::getUserKey() const
{
    return cookie_id_;
}

int Params::getTimeCookie() const
{
    return time_cookies_;
}

int Params::getSecondTimeCookie() const
{
    return time_cookies_ * 24 * 60 * 60;
}

unsigned long long Params::getUserKeyLong() const
{
    return key_long;
}

std::string Params::getCountry() const
{
    return country_;
}
std::string Params::getRegion() const
{
    return region_;
}

boost::posix_time::ptime Params::getTime() const
{
    return time_;
}
std::string Params::getScriptName() const
{
    return script_name_;
}

std::string Params::getLocation() const
{
    return location_;
}

std::string Params::getContext() const
{
    return context_;
}

std::string Params::getSearch() const
{
    return search_;
}

std::string Params::getUrl() const
{
    std::stringstream url;
    if (!country_.empty())
        url << "&country=" << country_;
    if (!region_.empty())
        url << "&region=" << region_;
    url << "&";

    return url.str();
}
