#ifndef CORE_H
#define CORE_H

#include <list>
#include <vector>
#include <map>
#include <utility>
#include <algorithm>

#include <boost/date_time.hpp>
#include <boost/algorithm/string.hpp>

#include "Params.h"
#include "Searcher.h"
#include "RedisClient.h"

/// Класс, который связывает воедино все части системы.
class Core
{
public:
    /** \brief  Создаёт ядро.
     *
     * Производит все необходимые инициализации:
     *
     * - Загружает все сущности из базы данных;
     * - Подключается к RabbitMQ и создаёт очереди сообщений;
     * - При необходимости создаёт локальную базу данных MongoDB с нужными
     *   параметрами.
     */
    Core();

    /** Пытается красиво закрыть очереди RabbitMQ, но при работе с FastCGI
     *  никогда не вызывается (как правило, процессы просто снимаются).
     */
    ~Core();

    /** \brief  Обработка запроса на показ рекламы.
     * Самый главный метод. Возвращает HTML-строку, которую нужно вернуть
     * пользователю.
     */
    void Process(Params *params);
    std::string Status();

private:
    /// Время запуска службы
    boost::posix_time::ptime time_service_started_;
    /// Время начала последнего запроса
    boost::posix_time::ptime time_request_started_;
    pthread_t tid;
    Searcher *searcher;
    std::list<RedisClient*> rc;
};

#endif // CORE_H
