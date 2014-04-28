#include <iostream>
#include <fstream>

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

#include <string.h>
#include <stdlib.h>
#include <pwd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>

#include "Log.h"
#include "Config.h"

unsigned long request_processed_;
unsigned long last_time_request_processed;
unsigned long offer_processed_;
unsigned long social_processed_;

// Global static pointer used to ensure a single instance of the class.
Config* Config::mInstance = NULL;

Config* Config::Instance()
{
    if (!mInstance)   // Only allow one instance of class to be generated.
        mInstance = new Config();

    return mInstance;
}

Config::Config()
{
    mIsInited = false;
    mDoc = NULL;

    sphinx_field_names_ = (const char**)std::malloc(sizeof(const char**));
    sphinx_field_weights_ = (int *)std::malloc(sizeof(int*));
}

bool Config::LoadConfig(const std::string fName)
{
    mFileName = fName;
    return Load();
}

void Config::exit(const std::string &mes)
{
    std::cerr<<mes<<std::endl;
    Log::err(mes);
    ::exit(1);
}

bool Config::Load()
{
    TiXmlElement *mel, *mels;
    boost::filesystem::path p;

    p = boost::filesystem::path(mFileName);

    if(!boost::filesystem::is_regular_file(p))
    {
        exit("does not regular file: "+mFileName);
    }

    Log::info("Config::Load: load file: %s",mFileName.c_str());

    mIsInited = false;
    mDoc = new TiXmlDocument(mFileName);

    if(!mDoc)
    {
        exit("does not found config file: "+mFileName);
    }

    if(!mDoc->LoadFile())
    {
        exit("load file: "+mFileName+
             " error: "+ mDoc->ErrorDesc()+
             " row: "+std::to_string(mDoc->ErrorRow())+
             " col: "+std::to_string(mDoc->ErrorCol()));
    }

    if(p.has_parent_path())
    {
        cfgFilePath = p.parent_path().string() + "/";
    }
    else
    {
        cfgFilePath = "./";
    }
#ifdef DEBUG
    std::cout<<"config path: "<<cfgFilePath<<std::endl;
#endif // DEBUG

    mRoot = mDoc->FirstChildElement("root");

    if(!mRoot)
    {
        exit("does not found root section in file: "+mFileName);
    }

    instanceId = atoi(mRoot->Attribute("id"));

    //main config
    if( (mElem = mRoot->FirstChildElement("server")) )
    {
        if( (mel = mElem->FirstChildElement("ip")) && (mel->GetText()) )
        {
            server_ip_ = mel->GetText();
        }

        if( (mel = mElem->FirstChildElement("redirect_script")) && (mel->GetText()) )
        {
            redirect_script_ = mel->GetText();
        }

        if( (mel = mElem->FirstChildElement("geocity_path")) && (mel->GetText()) )
        {
            geocity_path_ = mel->GetText();

            if(!checkPath(geocity_path_, false, true, mes))
            {
                exit(mes);
            }
        }

        if( (mel = mElem->FirstChildElement("socket_path")) && (mel->GetText()) )
        {
            server_socket_path_ = mel->GetText();

            if(!checkPath(server_socket_path_, true, true, mes))
            {
                exit(mes);
            }

            if(boost::filesystem::exists(server_socket_path_))
            {
                unlink(server_socket_path_.c_str());
            }
        }

        if( (mel = mElem->FirstChildElement("children")) && (mel->GetText()) )
        {
            server_children_ = atoi(mel->GetText());
        }

        if( (mel = mElem->FirstChildElement("lock_file")) && (mel->GetText()) )
        {
            lock_file_ = mel->GetText();

            if(!checkPath(lock_file_,true, true, mes))
            {
                exit(mes);
            }
        }

        if( (mel = mElem->FirstChildElement("pid_file")) && (mel->GetText()) )
        {
            pid_file_ = mel->GetText();

            if(!checkPath(pid_file_,true, true, mes))
            {
                exit(mes);
            }
        }

        if( (mel = mElem->FirstChildElement("user")) && (mel->GetText()) )
        {
            user_ = mel->GetText();
        }


        if( (mel = mElem->FirstChildElement("templates")) )
        {
            if( (mels = mel->FirstChildElement("out")) && (mels->GetText()) )
            {
                if(!checkPath(cfgFilePath + mels->GetText(),false,true, mes))
                {
                    exit(mes);
                }

                template_out_ = getFileContents(cfgFilePath + mels->GetText());
            }
            else
            {
                exit("element template_out is not inited");
            }

            if( (mels = mel->FirstChildElement("error")) && (mels->GetText()) )
            {
                if(!checkPath(cfgFilePath + mels->GetText(),false,true, mes))
                {
                    exit(mes);
                }

                template_error_ = getFileContents(cfgFilePath + mels->GetText());
            }
            else
            {
                exit("element template_error is not inited");
            }
        }

        if( (mel = mElem->FirstChildElement("cookie")) )
        {
            if( (mels = mel->FirstChildElement("name")) && (mels->GetText()) )
                cookie_name_ = mels->GetText();
            else
            {
                Log::warn("element cookie_name is not inited");
            }
            if( (mels = mel->FirstChildElement("domain")) && (mels->GetText()) )
                cookie_domain_ = mels->GetText();
            else
            {
                Log::warn("element cookie_domain is not inited");
            }

            if( (mels = mel->FirstChildElement("path")) && (mels->GetText()) )
                cookie_path_ = mels->GetText();
            else
            {
                Log::warn("element cookie_path is not inited");
            }
        }
    }
    else
    {
        exit("no server section in config file. exit");
    }

    if( (mElem = mRoot->FirstChildElement("retargeting")) )
    {
        for(mel = mElem->FirstChildElement("redis"); mel; mel = mel->NextSiblingElement())
        {
            if( (mels = mel->FirstChildElement("host")) && (mels->GetText()) )
            {
                redis_server srv;
                srv.host = mels->GetText();
                if( (mels = mel->FirstChildElement("port")) && (mels->GetText()) )
                {
                    srv.port = mels->GetText();
                    redis_retargeting_.push_back(srv);
                }
            }
        }

        if( (mel = mElem->FirstChildElement("ttl")) && (mel->GetText()) )
        {
            redis_retargeting_ttl_ = getTime(mel->GetText());
        }
    }
    else
    {
        exit("no section: retargeting");
    }

    if( (mElem = mRoot->FirstChildElement("long_term")) )
    {
        for(mel = mElem->FirstChildElement("redis"); mel; mel = mel->NextSiblingElement())
        {
            if( (mels = mel->FirstChildElement("host")) && (mels->GetText()) )
            {
                redis_server srv;
                srv.host = mels->GetText();
                if( (mels = mel->FirstChildElement("port")) && (mels->GetText()) )
                {
                    srv.port = mels->GetText();
                    redis_long_term_.push_back(srv);
                }
            }
        }

        if( (mel = mElem->FirstChildElement("ttl")) && (mel->GetText()) )
        {
            redis_long_term_ttl_ = getTime(mel->GetText());
        }
    }
    else
    {
        exit("no section: long_term");
    }


    if( (mElem = mRoot->FirstChildElement("sphinx")) )
    {
        if( (mel = mElem->FirstChildElement("host")) && (mel->GetText()) )
        {
            sphinx_host_ = mel->GetText();
        }

        if( (mel = mElem->FirstChildElement("port")) && (mel->GetText()) )
        {
            sphinx_port_ = atoi(mel->GetText());
        }

        if( (mel = mElem->FirstChildElement("index")) && (mel->GetText()) )
        {
            sphinx_index_ = mel->GetText();
        }

        if( (mel = mElem->FirstChildElement("select")) && (mel->GetText()) )
        {
            sphinx_select_ = mel->GetText();
        }

        for(mel = mElem->FirstChildElement("mode"); mel; mel = mel->NextSiblingElement())
        {
            if( (mels = mel->FirstChildElement("match")) && (mels->GetText()) )
            {
                shpinx_mode *m = new shpinx_mode();

                m->match_ = mels->GetText();

                if( (mels = mel->FirstChildElement("rank")) && (mels->GetText()) )
                {
                    m->rank_ = mels->GetText();

                    if( (mels = mel->FirstChildElement("sort")) && (mels->GetText()) )
                    {
                        m->sort_ = mels->GetText();

                        shpinx_modes_.push_back(m);
                        Log::info("sphinx mach: %s, rank: %s sort: %s",
                                  m->match_.c_str(),
                                  m->rank_.c_str(),
                                  m->sort_.c_str());
                    }
                    else
                    {
                        delete m;
                    }
                }
                else
                {
                    delete m;
                }
            }
        }


        if((mel = mElem->FirstChildElement("fields")))
        {
            for(mElem = mel->FirstChildElement(); mElem; mElem = mElem->NextSiblingElement())
            {
                sphinx_fields_.push_back(sphinx_field(mElem->Value(), mElem->GetText()));

                sphinx_field_names_[sphinx_fields_.size()] = mElem->Value();
                sphinx_field_weights_[sphinx_fields_.size()] = atoi(mElem->GetText());
            }
        }

    }
    else
    {
        exit("no sphinx section in config file. exit");
    }

    request_processed_ = 0;
    last_time_request_processed = 0;

    mIsInited = true;
    return mIsInited;
}

//---------------------------------------------------------------------------------------------------------------
Config::~Config()
{
    if (mDoc != NULL)
    {
        delete mDoc;
        mDoc = NULL;
    }

    std::free(sphinx_field_names_);
    std::free(sphinx_field_weights_);

    mInstance = NULL;
}
//---------------------------------------------------------------------------------------------------------------
int Config::getTime(const char *p)
{
    struct tm t;
    int ret;
    strptime(p, "%H:%M:%S", &t);
    ret = t.tm_hour * 3600;
    ret = ret + t.tm_min * 60;
    return ret + t.tm_sec;
}
//---------------------------------------------------------------------------------------------------------------
std::string Config::getFileContents(const std::string &fileName)
{
    std::ifstream in(fileName, std::ios::in | std::ios::binary);

    if(in)
    {
        std::string cnt;
        in.seekg(0, std::ios::end);
        cnt.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&cnt[0], cnt.size());
        in.close();
        return(cnt);
    }

    Log::err("error open file: %s: %d",fileName.c_str(), errno);
    return std::string();
}
//---------------------------------------------------------------------------------------------------------------
/*
                stat = boost::filesystem::status(test, errcode);
                if(errcode)
                {
                  Log::err("file system error: object: %s value: %d message: %s",
                           test.string().c_str(),
                           errcode.value(),
                           errcode.message().c_str());
                    return false;
                }

*/
bool Config::checkPath(const std::string &path_, bool checkWrite, bool isFile, std::string &mes)
{
    boost::filesystem::path path, test;
    boost::system::error_code errcode;
    boost::filesystem::path::iterator toEnd;
    struct stat info;
    uid_t uid;
    gid_t gid;

    uid = getuid();
    gid = getgid();


    path = boost::filesystem::path(path_);

    toEnd = path.end();
    if(isFile)
    {
        toEnd--;
    }

    for (boost::filesystem::path::iterator it = path.begin(); it != toEnd; ++it)
    {
        test /= *it;

        if(test.string()=="" || test.string()=="/")
            continue;

        if(boost::filesystem::exists(test))
        {
            ::stat(test.string().c_str(), &info);

            if (boost::filesystem::is_regular_file(test))
            {
                if(info.st_uid != uid && info.st_gid != gid)
                {
                    if(info.st_mode &(S_IROTH | S_IWOTH))
                    {
                        continue;
                    }
                    else
                    {
                        mes = "file system error: path: "+test.string()+" message: cann't write";
                        return false;
                    }
                }
                else if(info.st_uid == uid && info.st_gid != gid)
                {
                    if(info.st_mode & (S_IRUSR | S_IWUSR))
                    {
                        continue;
                    }
                    else
                    {
                        mes = "file system error: path: "+test.string()+" message: cann't write";
                        return false;
                    }
                }
                else if(info.st_uid != uid && info.st_gid == gid)
                {
                    if(info.st_mode & (S_IRGRP | S_IWGRP))
                    {
                        continue;
                    }
                    else
                    {
                        mes = "file system error: path: "+test.string()+" message: cann't write";
                        return false;
                    }
                }
                else
                {
                    if(info.st_mode & (S_IRUSR | S_IWUSR))
                    {
                        continue;
                    }
                    else
                    {
                        mes = "file system error: path: "+test.string()+" message: cann't write";
                        return false;
                    }
                }
            }
            else if (boost::filesystem::is_directory(test))
            {
                if(info.st_uid != uid && info.st_gid != gid)
                {
                    if(info.st_mode & (S_IROTH | S_IXOTH))
                    {
                        continue;
                    }
                    else
                    {
                        mes = "file system error: path: "+test.string()+" message: cann't write";
                        return false;
                    }
                }
                else if(info.st_uid == uid && info.st_gid != gid)
                {
                    if(info.st_mode & (S_IRUSR | S_IXUSR))
                    {
                        continue;
                    }
                    else
                    {
                        mes = "file system error: path: "+test.string()+" message: cann't write";
                        return false;
                    }
                }
                else if(info.st_uid != uid && info.st_gid == gid)
                {
                    if(info.st_mode & (S_IRGRP | S_IXGRP))
                    {
                        continue;
                    }
                    else
                    {
                        mes = "file system error: path: "+test.string()+" message: cann't write";
                        return false;
                    }
                }
                else
                {
                    if(info.st_mode & (S_IRUSR | S_IXUSR))
                    {
                        continue;
                    }
                    else
                    {
                        mes = "file system error: path: "+test.string()+" message: cann't write";
                        return false;
                    }
                }
                continue;
            }
            else
            {
                if(checkWrite)
                {
                    try
                    {
                        if(!boost::filesystem::create_directories(test))
                        {
                            return false;
                        }
                    }
                    catch(const boost::filesystem::filesystem_error &ex)
                    {
                        mes = "file system error: path: "+test.string()+" message: "+ex.what();
                        return false;
                    }
                }
                else
                {
                    return false;
                }
            }
        }
        else//does not exists
        {
            if(checkWrite)
            {
                try
                {
                    if(!boost::filesystem::create_directories(test))
                    {
                        return false;
                    }
                }
                catch(const boost::filesystem::filesystem_error &ex)
                {
                    mes = "file system error: path: "+test.string()+" message: "+ex.what();
                    return false;
                }
            }
            else
            {
                mes = "file system error: path: "+test.string()+" message: does not exists";
                return false;
            }
        }
    }

    if(isFile && checkWrite)
    {
        int lfp = open((path_+"test").c_str(),O_RDWR|O_CREAT,0640);

		if(lfp < 0)
		{
			exit("unable to create file: "+path_+", "+strerror(errno));
		}
		else
        {
            close(lfp);
            unlink((path_+"test").c_str());
        }
    }

    return true;
}
