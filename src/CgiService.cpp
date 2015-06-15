#include <sys/stat.h>

#include "Log.h"
#include "CgiService.h"
#include "UrlParser.h"
#include "Core.h"
#include "Server.h"
#include "Cookie.h"

#include <fcgi_stdio.h>

#define THREAD_STACK_SIZE PTHREAD_STACK_MIN + 10 * 1024

CgiService::CgiService()
{

    bcore = new BaseCore();

    FCGX_Init();

    mode_t old_mode = umask(0);

    socketId = FCGX_OpenSocket(config->server_socket_path_.c_str(), config->server_children_);
    if(socketId < 0)
    {
        Log::err("Error open socket. exit");
        exit(1);
    }
    umask(old_mode);

    pthread_attr_t* attributes = (pthread_attr_t*) malloc(sizeof(pthread_attr_t));
    pthread_attr_init(attributes);
    pthread_attr_setstacksize(attributes, THREAD_STACK_SIZE);

    struct sigaction actions;

    memset(&actions, 0, sizeof(actions));
    actions.sa_flags = 0;
    actions.sa_handler = SignalHandler;

    sigaction(SIGHUP,&actions,NULL);
    sigaction(SIGPIPE,&actions,NULL);

    threads = new pthread_t[config->server_children_ + 1];

    for(int i = 0; i < config->server_children_; i++)
    {
        if(pthread_create(&threads[i], attributes, &this->Serve, this))
        {
            Log::err("creating thread failed");
        }
    }
    pthread_attr_destroy(attributes);
}

CgiService::~CgiService()
{
    for(int i = 0; i < config->server_children_; i++)
    {
        pthread_join(threads[i], 0);
    }

    delete []threads;
}

void CgiService::run()
{
    int memSize = 0, newMemSize;
    //main loop
    for(;;)
    {
        //read mq and process
        bcore->ProcessMQ();

        newMemSize = Log::memUsage();
        if(newMemSize != memSize)
        {
            std::clog<<"VmSize:"<<newMemSize<<" diff:"<<newMemSize - memSize<<std::endl;
            memSize = newMemSize;
        }

        sleep(1);
    }
}

void CgiService::Response(FCGX_Request *req,
                          const std::string &out,
                          const std::string &c,
                          const std::string &rc)
{
    FCGX_FPrintF(req->out,"Content-type: text/html\r\n");
    FCGX_FPrintF(req->out,"Set-Cookie: %s\r\n", c.c_str());
    FCGX_FPrintF(req->out,"Set-Cookie: %s\r\n", rc.c_str());
    FCGX_FPrintF(req->out,"Status: 200 OK\r\n");
    FCGX_FFlush(req->out);
    FCGX_FPrintF(req->out,"\r\n%s\n", out.c_str());
    FCGX_Finish_r(req);
}

void CgiService::Response(FCGX_Request *req, int status)
{
    FCGX_FPrintF(req->out,"Content-type: text/html\r\n");

    FCGX_FPrintF(req->out,"Status: ");
    switch (status)
    {
    case 200:
        FCGX_FPrintF(req->out,"200 OK");
        break;
    case 301:
        FCGX_FPrintF(req->out,"301 Moved Permanently");
        break;
    case 302:
        FCGX_FPrintF(req->out,"302 Found");
        break;
    case 307:
        FCGX_FPrintF(req->out,"307 Temporary Redirect");
        break;
    case 400:
        FCGX_FPrintF(req->out,"400 Bad Request");
        break;
    case 403:
        FCGX_FPrintF(req->out,"403 Forbidden");
        break;
    case 500:
        FCGX_FPrintF(req->out,"500 Internal Server Error");
        break;
    case 503:
        FCGX_FPrintF(req->out,"503 Service Unavailable");
        break;
    default:
        FCGX_FPrintF(req->out,"200 OK");
        break;
    }
    FCGX_FPrintF(req->out,"\r\n");
    FCGX_FFlush(req->out);
    FCGX_Finish_r(req);
}


void *CgiService::Serve(void *data)
{
    CgiService *csrv = (CgiService*)data;

    Core *core = new Core();

    FCGX_Request request;

    if(FCGX_InitRequest(&request, csrv->socketId, 0) != 0)
    {
        Log::err("Can not init request");
        return nullptr;
    }

    for(;;)
    {
        static pthread_mutex_t accept_mutex = PTHREAD_MUTEX_INITIALIZER;
        pthread_mutex_lock(&accept_mutex);
        int rc = FCGX_Accept_r(&request);
        pthread_mutex_unlock(&accept_mutex);

        if(rc < 0)
        {
            Log::err("Can not accept new request");
            break;
        }

        csrv->ProcessRequest(&request, core);
    }

    return nullptr;
}


void CgiService::ProcessRequest(FCGX_Request *req, Core *core)
{
    Params *prm = new Params();

    if(!prm->parse(req))
    {
        Response(req, 503);
        delete prm;
        return;
    }

    if(prm->status)
    {
        Response(req, bcore->Status(), "");
        delete prm;
        return;
    }

    ClearSilver::Cookie c = ClearSilver::Cookie(config->cookie_name_,
                            prm->cookie_id_,
                            ClearSilver::Cookie::Credentials(
                                ClearSilver::Cookie::Authority(config->cookie_domain_),
                                ClearSilver::Cookie::Path(config->cookie_path_),
                                ClearSilver::Cookie::Expires(boost::posix_time::second_clock::local_time() + boost::gregorian::years(1))));
    
    ClearSilver::Cookie rc = ClearSilver::Cookie(config->cookie_tracking_name_,
                            prm->cookie_id_,
                            ClearSilver::Cookie::Credentials(
                                ClearSilver::Cookie::Authority(config->cookie_tracking_domain_),
                                ClearSilver::Cookie::Path(config->cookie_tracking_path_),
                                ClearSilver::Cookie::Expires(boost::posix_time::second_clock::local_time() + boost::gregorian::days(prm->time_cookies_))));
    
    std::string result = core->Process(prm);
    
    Response(req, result, c.to_string(), rc.to_string());

    delete prm;
}

void CgiService::SignalHandler(int signum)
{
    switch(signum)
    {
    case SIGHUP:
        std::clog<<"CgiService: sig hup"<<std::endl;
        config->ReLoad();
        break;
    case SIGPIPE:
        std::clog<<"CgiService: sig pipe"<<std::endl;
        break;
    }
}
