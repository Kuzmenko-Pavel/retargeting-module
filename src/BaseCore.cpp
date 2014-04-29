#include "DB.h"
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
#include "base64.h"

#include "Informer.h"
#include "Campaign.h"
#include "Offer.h"
#include "Config.h"

BaseCore::BaseCore()
{
    struct sigaction actions;
    sigemptyset(&actions.sa_mask);
    actions.sa_flags = 0;
    actions.sa_handler = signal_handler;

    sigaction(SIGHUP,&actions,NULL);
    sigaction(SIGPIPE,&actions,NULL);

    time_service_started_ = boost::posix_time::second_clock::local_time();

    pdb = new ParentDB();

    LoadAllEntities();

    InitMessageQueue();
}

BaseCore::~BaseCore()
{
    delete amqp_;
}
std::string BaseCore::toString(AMQPMessage *m)
{
    unsigned len;
    char *pMes;

#ifdef AMQPCPP_OLD
    pMes = m->getMessage();
    len = strlen(pMes);
#else
    pMes = m->getMessage(&len);
#endif // AMQPCPP_OLD

    return std::string(pMes,len);
}

bool BaseCore::ProcessMQ()
{
    time_mq_check_ = boost::posix_time::second_clock::local_time();
    AMQPMessage *m;
    try
    {
        // Проверка сообщений campaign.#
        mq_campaign_->Get(AMQP_NOACK);
        m = mq_campaign_->getMessage();
        while(m->getMessageCount() > -1)
        {
            Log::gdb("BaseCore::ProcessMQ campaign: %s",m->getRoutingKey().c_str());
            mq_log_ = "BaseCore::ProcessMQ campaign: " + m->getRoutingKey();
            if(m->getRoutingKey() == "campaign.update")
            {
                pdb->CampaignLoad(toString(m));
            }
            else if(m->getRoutingKey() == "campaign.delete")
            {
                pdb->CampaignRemove(toString(m));
            }
            m = mq_campaign_->getMessage();
        }
        // Проверка сообщений advertise.#
        std::string m1, ofrId, cmgId;
        mq_advertise_->Get(AMQP_NOACK);
        m = mq_advertise_->getMessage();
        while(m->getMessageCount() > -1)
        {
            Log::gdb("BaseCore::ProcessMQ advertise: %s",m->getRoutingKey().c_str());
            mq_log_ = "BaseCore::ProcessMQ advertise: " + m->getRoutingKey();
            m1 = toString(m);
            if(m->getRoutingKey() == "advertise.update")
            {
                if(cmdParser(m1,ofrId,cmgId))
                {
                    pdb->OfferLoad(QUERY("guid" << ofrId));
                }
            }
            else if(m->getRoutingKey() == "advertise.delete")
            {
                if(cmdParser(m1,ofrId,cmgId))
                {
                    pdb->OfferRemove(ofrId);
                }
            }
            m = mq_advertise_->getMessage();
        }
        // Проверка сообщений informer.#
        mq_informer_->Get(AMQP_NOACK);
        m = mq_informer_->getMessage();
        while(m->getMessageCount() > -1)
        {
            Log::gdb("BaseCore::ProcessMQ informer: %s",m->getRoutingKey().c_str());
            mq_log_ = "BaseCore::ProcessMQ informer: " + m->getRoutingKey();
            if(m->getRoutingKey() == "informer.update")
            {
                pdb->InformerUpdate(toString(m));
            }
            else if(m->getRoutingKey() == "informer.delete")
            {
                pdb->InformerRemove(toString(m));
            }
            m = mq_informer_->getMessage();
        }
    }
    catch (AMQPException &ex)
    {
        Log::err("AMQPException: %s", ex.getMessage().c_str());
    }
    return false;
}


/*
*  Загружает из основной базы данных следующие сущности:
*
*  - рекламные предложения;
*  - рекламные кампании;
*  - информеры.
*
*  Если в кампании нет рекламных предложений, она будет пропущена.
*/
void BaseCore::LoadAllEntities()
{
    if(Config::Instance()->pDb->reopen)
        return;

    //Загрузили все информеры
    pdb->InformerLoadAll();

    //Загрузили все кампании
    pdb->CampaignLoad();

    //Загрузили все предложения
    pdb->OfferLoad(mongo::Query());

    //Загрузили все категории
    pdb->CategoriesLoad();

    Config::Instance()->pDb->indexRebuild();
}

/** \brief  Инициализация очереди сообщений (AMQP).

    Если во время инициализации произошла какая-либо ошибка, то сервис
    продолжит работу, но возможность оповещения об изменениях и горячего
    обновления будет отключена.
*/
void BaseCore::InitMessageQueue()
{
    try
    {
        // Объявляем точку обмена
        amqp_ = new AMQP(config->mq_path_);
        exchange_ = amqp_->createExchange();
        exchange_->Declare("getmyad", "topic", AMQP_AUTODELETE);
        //LogToAmqp("AMQP is up");

        // Составляем уникальные имена для очередей
        boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
        std::string postfix = to_iso_string(now);
        boost::replace_first(postfix, ".", ",");

        std::string mq_advertise_name( "getmyad.advertise." + postfix );
        std::string mq_campaign_name( "getmyad.campaign." + postfix );
        std::string mq_informer_name( "getmyad.informer." + postfix );

        // Объявляем очереди
        mq_campaign_ = amqp_->createQueue();
        mq_campaign_->Declare(mq_campaign_name, AMQP_AUTODELETE | AMQP_EXCLUSIVE);
        mq_informer_ = amqp_->createQueue();
        mq_informer_->Declare(mq_informer_name, AMQP_AUTODELETE | AMQP_EXCLUSIVE);
        mq_advertise_ = amqp_->createQueue();
        mq_advertise_->Declare(mq_advertise_name, AMQP_AUTODELETE | AMQP_EXCLUSIVE);

        // Привязываем очереди
        exchange_->Bind(mq_advertise_name, "advertise.#");
        exchange_->Bind(mq_campaign_name, "campaign.#");
        exchange_->Bind(mq_informer_name, "informer.#");

        Log::info("Created ampq queues: %s, %s, %s",
                  mq_campaign_name.c_str(),
                  mq_informer_name.c_str(),
                  mq_advertise_name.c_str());
    }
    catch (AMQPException &ex)
    {
        Log::err("Error in AMPQ init: %s, Feature will be disabled.", ex.getMessage().c_str());
    }
}

/** Возвращает данные о состоянии службы
 *  TODO Надоб переписать с учётом использования boost::formater красивее будет как некак :)
 */
std::string BaseCore::Status()
{
    std::stringstream out;

    return out.str();
}

bool BaseCore::cmdParser(const std::string &cmd, std::string &offerId, std::string &campaignId)
{
    boost::regex exp("Offer:(.*),Campaign:(.*)");
    boost::cmatch pMatch;

    if(boost::regex_match(cmd.c_str(), pMatch, exp))
    {
        offerId = pMatch[1];
        campaignId = pMatch[2];
        return true;
    }
    return false;
}

void BaseCore::signal_handler(int signum)
{
    switch(signum)
    {
    case SIGHUP:
        config->Load();
    case SIGPIPE:
        Log::err("sig pipe");
    }
}
