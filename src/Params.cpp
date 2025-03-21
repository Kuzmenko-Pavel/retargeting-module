#include <sstream>

#include <boost/algorithm/string/trim.hpp>
#include <boost/regex/icu.hpp>
#include <boost/regex.hpp>
#include <boost/date_time.hpp>
#include <boost/algorithm/string.hpp>

#include <string>

#include "Params.h"
#include "Log.h"
#include "UrlParser.h"
#include "Config.h"
const unsigned long STDIN_MAX = 6000000;

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
    char * content_length_str = FCGX_GetParam("CONTENT_LENGTH", req->envp);
    UrlParser *post_parser;
    unsigned long content_length = STDIN_MAX;
    std::string postq;
    if (content_length_str) {
        content_length = strtol(content_length_str, &content_length_str, 10);
        if (*content_length_str) {
            Log::warn("Can't Parse 'CONTENT_LENGTH'");
        }
        if (content_length > STDIN_MAX) {
            content_length = STDIN_MAX;
        }
        if (content_length > 0)
        {
            char * content = new char[content_length];
            memset(content, 0, content_length);
            FCGX_GetStr(content, content_length, req->in);
            postq = std::string(content);
            delete [] content;
        }
    }
    post_parser = new UrlParser(postq);

    referrer_ = post_parser->param("referrer");
    location_ = post_parser->param("url");
    title_ = post_parser->param("title");
    gender_ = post_parser->param("gender");
    cost_ = post_parser->param("cost");
    account_id_ = post_parser->param("ac");
    offer_id_ = post_parser->param("offer_id");

    std::string s_time_cookies_ = post_parser->param("time");
    if(s_time_cookies_.empty())
    {
        time_cookies_ = 365;
    }
    else
    {
        time_cookies_ = atoi(s_time_cookies_.c_str());
    }


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
        delete post_parser;
        delete query_parser;
        return true;
    }
    else
    {
        status = false;
    }

    if( !(tmp_str = FCGX_GetParam("REMOTE_ADDR", req->envp)) )
    {
        std::clog<<"remote address is not set"<<std::endl;
        delete post_parser;
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

    /// ID посетителя, cookie
    if(cookie_id_.empty())
    {
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
    delete post_parser;
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

int Params::getTimeCookie() const
{
    return time_cookies_;
}

int Params::getSecondTimeCookie() const
{
    return time_cookies_ * 24 * 60 * 60;
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
std::string Params::getTitle() const
{
    return title_;
}
std::string Params::getReferrer() const
{
    return referrer_;
}
