#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>

#include <tinyxml.h>
#include "shpinx_mode.h"
#include "redis_server.h"
#include "sphinx_field.h"

extern unsigned long request_processed_;
extern unsigned long last_time_request_processed;
extern unsigned long offer_processed_;
extern unsigned long social_processed_;

class Config
{
public:
    std::vector<redis_server> redis_long_term_;
    int redis_long_term_ttl_;
    std::vector<redis_server> redis_retargeting_;
    int redis_retargeting_ttl_;

    //new params
    std::string server_ip_;
    std::string redirect_script_;
    std::string server_socket_path_;
    int server_children_;
    std::string cookie_name_;
    std::string cookie_domain_;
    std::string cookie_path_;
    std::string cookie_tracking_name_;
    std::string cookie_tracking_domain_;
    std::string cookie_tracking_path_;

    std::string geocity_path_;

    std::string sphinx_host_;
    int         sphinx_port_;
    std::string sphinx_index_;
    std::string sphinx_select_;
    std::vector<sphinx_field> sphinx_fields_;
    std::vector<shpinx_mode*> shpinx_modes_;
    const char**sphinx_field_names_;
    int *sphinx_field_weights_;

    int         instanceId;
    std::string lock_file_;
    std::string pid_file_;
    std::string user_;

    std::string template_out_;
    std::string template_error_;
    int time_update_;

    static Config* Instance();
    bool LoadConfig(const std::string fName);
    bool Load();
    virtual ~Config();

    bool to_bool(std::string const& s)
    {
        return s != "false";
    }
    float to_float(std::string const& s)
    {
        return atof(s.c_str());
    }
    int to_int(std::string const& s)
    {
        return atoi(s.c_str());
    }

protected:
private:
    static Config* mInstance;
    Config();
    bool mIsInited;
    std::string mes;
    std::string mFileName;
    std::string cfgFilePath;

    TiXmlDocument *mDoc;
    TiXmlElement* mRoot;
    TiXmlElement* mElem;

    int getTime(const char *p);
    std::string getFileContents(const std::string &fileName);
    void exit(const std::string &mes);
    bool checkPath(const std::string &path_, bool checkWrite, bool isFile, std::string &mes);
};

extern Config *config;

#endif // CONFIG_H

