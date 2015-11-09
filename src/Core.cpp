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

#define CMD_SIZE 8192
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
    if(params->account_id_.empty())
    {
        return html;
    }

    if(params->retargeting_offer_id_.empty())
    {
        return html;
    }
    if(config->logAccountId)
        std::clog<<" account id:"<<params->account_id_;

    if(config->logKey)
        std::clog<<" key:"<<params->getUserKey();


    request_processed_++;

    html = boost::str(boost::format(config->template_out_)
                       % params->retargeting_offer_id_
                       % params->getSecondTimeCookie()
                       % params->account_id_
                       % params->target_
                       % params->action_
                       % params->referrer_
                       % params->location_
                       % params->title_);
    return html;
}
