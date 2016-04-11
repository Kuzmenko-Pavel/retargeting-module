#include <signal.h>

#include <boost/foreach.hpp>
#include <boost/date_time.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include "../config.h"

#include <ctime>
#include <cstdlib>

#include "Config.h"
#include "Log.h"
#include "Core.h"
#include "base64.h"

Core::Core()
{
}

Core::~Core()
{
}

std::string Core::Process(Params *prms)
{
    std::string html;
    params = prms;

    request_processed_++;
    std::string anal ="\
            (function(i,s,o,g,r,a,m){i['GoogleAnalyticsObject']=r;i[r]=i[r]||function(){\
            (i[r].q=i[r].q||[]).push(arguments)},i[r].l=1*new Date();a=s.createElement(o),\
            m=s.getElementsByTagName(o)[0];a.async=1;a.src=g;m.parentNode.insertBefore(a,m)\
            })(window,document,'script','//www.google-analytics.com/analytics.js','ga');\
            if (offer_type == 'retargeting')\
            {\
                ga('create', 'UA-69836571-1', {'alwaysSendReferrer': true});\
                ga('send', 'pageview', {\
                  'referrer': referrer,\
                  'hostname': hostname,\
                  'page': page,\
                  'title': title\
              });\
            }\
            ga('create', 'UA-69836571-2', {'alwaysSendReferrer': true, 'name': 'all'});\
            ga('all.send', 'pageview', {\
              'referrer': referrer,\
              'hostname': hostname,\
              'page': page,\
              'title': title\
            });";

    if ( params->account_id_ == "AEE4E3DD-957C-4E6B-A111-8B43318B78D3")
    {
        anal ="";
    }

    html = boost::str(boost::format(config->template_out_)
                       % params->offer_id_
                       % params->getSecondTimeCookie()
                       % params->account_id_
                       % params->referrer_
                       % params->location_
                       % params->title_
                       % params->gender_
                       % params->cost_
                       % anal
                        );
    return html;
}
