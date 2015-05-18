#include <vector>
#include <boost/algorithm/string.hpp>

#include "ParentDB.h"
#include "Log.h"
#include "KompexSQLiteStatement.h"
#include "Config.h"

ParentDB::ParentDB()
{
    pdb = Config::Instance()->pDb->pDatabase;
    fConnectedToMainDatabase = false;
    ConnectMainDatabase();
}

ParentDB::~ParentDB()
{
    //dtor
}


bool ParentDB::ConnectMainDatabase()
{
    if(fConnectedToMainDatabase)
        return true;

    std::vector<mongo::HostAndPort> hvec;
    for(auto h = config->mongo_main_host_.begin(); h != config->mongo_main_host_.end(); ++h)
    {
        hvec.push_back(mongo::HostAndPort(*h));
        Log::info("Connecting to: %s",(*h).c_str());
    }

    try
    {
        std::string err;
        if(!config->mongo_main_set_.empty())
        {
            monga_main = new mongo::DBClientReplicaSet(config->mongo_main_set_, hvec);
            monga_main->connect();
            fConnectedToMainDatabase = true;
        }
        else
        {
            Log::err("ParentDB mongo error:");
            return false;
        }
    }
    catch (mongo::UserException &ex)
    {
        Log::err("ParentDB mongo error: %s", ex.what());
        return false;
    }

    return true;
}

/** Загружает все товарные предложения из MongoDb */
void ParentDB::OfferLoad(const std::string &id)
{
    if(!fConnectedToMainDatabase)
        return;

    Kompex::SQLiteStatement *pStmt;
    char *pData;
    int sz, i = 0;
    int skipped = 0;

    std::unique_ptr<mongo::DBClientCursor> cursor;
    mongo::Query q;
    if(!id.size())
    {
        q = mongo::Query("{$and: [{ \"retargeting\" : true}, {\"type\" : \"teaser\"}]}");
        cursor = monga_main->query(config->mongo_main_db_ + ".offer",q);
    }
    else
    {
        q = mongo::Query("{$and: [{ \"retargeting\" : true}, {\"type\" : \"teaser\"}, {\"campaignId\":\""+ id +"\"}]}");
        cursor = monga_main->query(config->mongo_main_db_ + ".offer",q);
    }

    pStmt = new Kompex::SQLiteStatement(pdb);


    bzero(buf,sizeof(buf));
    snprintf(buf,sizeof(buf),"INSERT INTO Offer(id,guid,campaignId,accountId,retargetingId) VALUES(");
    sz = strlen(buf);
    pData = buf + sz;
    sz = sizeof(buf) - sz;

    pStmt->BeginTransaction();
    while (cursor->more())
    {
        mongo::BSONObj x = cursor->next();
        std::string id = x.getStringField("guid");
        if (id.empty())
        {
            skipped++;
            continue;
        }

        std::string image = x.getStringField("image");
        std::string swf = x.getStringField("swf");
        if (image.empty())
        {
            if (swf.empty())
            {
                skipped++;
                continue;
            }
        }

        bzero(pData,sz);
        sqlite3_snprintf(sz,pData,
                         "%llu,'%q','%q','%q',%llu);",
                         x.getField("guid_int").numberLong(),
                         id.c_str(),
                         x.getStringField("campaignId"),
                         x.getStringField("accountId"),
                         strtol(x.getStringField("RetargetingID"),NULL,10)
                        );

        try
        {
            pStmt->SqlStatement(buf);
        }
        catch(Kompex::SQLiteException &ex)
        {
            logDb(ex);
            skipped++;
        }

        i++;

    }

    pStmt->CommitTransaction();
    pStmt->FreeQuery();
    delete pStmt;

    Log::info("Loaded %d offers", i);
    if (skipped)
        Log::warn("Offers with empty id or image skipped: %d", skipped);
}

void ParentDB::OfferRemove(const std::string &id)
{
    Kompex::SQLiteStatement *pStmt;

    if(id.empty())
    {
        return;
    }

    pStmt = new Kompex::SQLiteStatement(pdb);
    pStmt->BeginTransaction();
    sqlite3_snprintf(sizeof(buf),buf,"DELETE FROM Offer WHERE campaignId='%s';",id.c_str());
    try
    {
        pStmt->SqlStatement(buf);
    }
    catch(Kompex::SQLiteException &ex)
    {
        logDb(ex);
    }
    pStmt->CommitTransaction();
    pStmt->FreeQuery();

    delete pStmt;

    Log::info("offer %s removed",id.c_str());
}



void ParentDB::logDb(const Kompex::SQLiteException &ex) const
{
    Log::err("ParentDB::logDb request:(%s) error: %s", buf, ex.GetString().c_str());
}
