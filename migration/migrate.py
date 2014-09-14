#!/usr/bin/python

import redis

r = redis.StrictRedis(host='localhost', port=6379, db=0)

for key in r.keys():
    print(key.decode("utf-8"),"->",key.decode("utf-8").split('-')[0]);
    r.rename(key,key.decode("utf-8").split('-')[0])
