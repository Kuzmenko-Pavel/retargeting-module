#ifndef PARAMS_H
#define PARAMS_H

#include <sstream>
#include <string>
#include <boost/date_time.hpp>

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

    Params();
    /// IP посетителя.
    Params &ip(const std::string &);
    /// ID посетителя, взятый из cookie
    Params &cookie_id(const std::string &);
    /// ID информера.
    Params &informer_id(const std::string &);
    Params &account_id(const std::string &);
    Params &retargeting_offer_id(const std::string &);

    /** \brief  Двухбуквенный код страны посетителя.

        Если не задан, то страна будет определена по IP.

        Этот параметр используется служебными проверками работы информеров
        в разных странах и в обычной работе не должен устанавливаться.

        \see region()
        \see ip()
    */
    Params &country(const std::string &country);
    /** \brief  Гео-политическая область в написании MaxMind GeoCity.

        Если не задана, то при необходимости будет определена по IP.

        Вместе с параметром country() используется служебными проверками
        работы информеров в разных странах и в обычной работе не должен
        устанавливаться.

        \see country()
        \see ip()
    */
    Params &region(const std::string &region);
    /** \brief  Тестовый режим работы, в котором показы не записываются и переходы не записываються.
        По умолчанию равен false.
    */
    Params &test_mode(bool test_mode);
    /// Выводить ли предложения в формате json.
    Params &json(bool json);
    /// ID предложений, которые следует исключить из показа.
    Params &excluded_offers(const std::vector<std::string> &excluded);
    /** \brief  Виртуальный путь и имя вызываемого скрипта.

        Используется для построения ссылок на самого себя. Фактически,
        сюда должна передаваться сервреная переменная SCRIPT_NAME.
    */
    Params &script_name(const char *script_name);
    /** \brief  Адрес страницы, на которой показывается информер.

        Обычно передаётся javascript загрузчиком.
    */
    Params &location(const char *location);
    /** \brief  Адрес страницы, на которой показывается информер.

        Обычно передаётся javascript загрузчиком.
    */
    Params &location(const std::string &location);
    Params &w(const std::string &w);
    Params &h(const std::string &h);
    /**
     * строка, содержашяя контекст страницы
     */
    Params &context(const std::string &context);
    Params &context(const char *context);
    /**
     * строка, содержашяя поисковый запрос
     */
    Params &search(const std::string &search);
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
