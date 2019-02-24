//
// Created by Kave Salamatian on 24/11/2018.
//

#ifndef BGPGEOPOLITICS_BGPGEOPOLITICS_H
#define BGPGEOPOLITICS_BGPGEOPOLITICS_H
#include "stdint.h"
extern "C" {
#include "bgpstream.h"
}
#include <boost/range/combine.hpp>
#include "tbb/concurrent_unordered_set.h"
#include <set>
#include <mutex>
#include <condition_variable>
#include <list>
#include <vector>

#include <boost/thread/shared_mutex.hpp>
#include "tbb/concurrent_hash_map.h"

using namespace std;
using namespace tbb;


enum Category{None, AADiff,AADup, WADup, WWDup, Flap, Withdrawn};


enum Status{CONNECTED=1, DISCONNECTED=2, OUTAGE=4, HIJACKED=8, HIJACKING=16};



template < class T> class ThreadSafeSet{
public:
    std::set<T> set;
    bool insert(const T& val){
        boost::unique_lock<boost::shared_mutex> lock(mutex_);
        return set.insert(val).second;
    }

    int erase(const T& val){
        boost::unique_lock<boost::shared_mutex> lock(mutex_);
        return set.erase(val);
    }

    int size(){
        boost::shared_lock<boost::shared_mutex> lock(mutex_);
        return set.size();
    }

    typename std::set<T>::iterator find(const T& val){
        boost::shared_lock<boost::shared_mutex> lock(mutex_);
        return set.find(val);
    }

    typename std::set<T>::iterator end(){
        boost::shared_lock<boost::shared_mutex> lock(mutex_);
        return set.end();
    }

    typename std::set<T>::iterator begin(){
        return set.begin();
    }


private:
    mutable boost::shared_mutex mutex_;
};

class PrefixPeer{
public:
//    string str;
    bgpstream_pfx_storage_t *pfx;
    ThreadSafeSet<short int> peerSet;
    ThreadSafeSet<unsigned int> asSet;

    PrefixPeer(bgpstream_pfx_storage_t *prefix, unsigned int time): cTime(time){
        pfx = new bgpstream_pfx_storage_t();
        memcpy(pfx, prefix, sizeof(bgpstream_pfx_storage_t));
    }
    int addPeer(short int peerStrIndex, unsigned int time){
        peerSet.insert(peerStrIndex);
        cTime=time;
        return peerSet.size();
    }

    int addAS(unsigned int asn){
        asSet.insert(asn);
        return asSet.size();
    }

    int removePeer(short int peerStrIndex, unsigned int time){
        auto it = peerSet.find(peerStrIndex);
        if (it != peerSet.end()){
            peerSet.erase(peerStrIndex);
            cTime = time;
        }
        return peerSet.size();
    }

    int removeAS(unsigned int asn){
        if (asSet.find(asn) != asSet.end()){
            asSet.erase(asn);
        }
        return asSet.size();
    }

    bgpstream_addr_version_t getVersion(){
        return pfx->address.version;
    }

    bool checkHijack(){
        if (asSet.size()>2) {
            return true;
        }
        return false;
    }

    int size_of(){
        int size =0;
        string str;
        for(auto it=peerSet.begin();it!=peerSet.end();it++){
            size +=2;
        }
        size += asSet.size()*4;
        return size;
    }

    string str(){
        char buf[20];
        bgpstream_pfx_snprintf(buf, 20, (bgpstream_pfx_t *)pfx);
        string str(buf);
        return str;
    }

private:
    unsigned int cTime;
};


class AS{
public:
    unsigned int asNum;
    string name="??";
    string country="??";
    string RIR="??";
    double risk=0.0, geoRisk=0.0, secuRisk=0.0,  otherRisk=0.0;
    unsigned int activePrefixNum=0;
    unsigned int allPrefixNum=0;
    concurrent_hash_map<string, PrefixPeer *> activePrefixMap;
    ThreadSafeSet<PrefixPeer *> inactivePrefixSet;
    unsigned int cTime=0;
    bool observed=false;
    int status=0;

    AS(int asn): asNum(asn){}
    AS(int asn, string inname, string incountry, string(inRIR), float risk): asNum(asn){
        name = inname;
        country = incountry;
        RIR = RIR;
    }
    bool checkOutage(){
        activePrefixNum = activePrefixMap.size() ;
        allPrefixNum =  activePrefixNum+ inactivePrefixSet.size();
        if ((activePrefixNum<0.3*allPrefixNum) &&(allPrefixNum>10))  {
            status |= OUTAGE;
            return true;
        }   else {
            if (status & OUTAGE) {
                status &= ~OUTAGE;
            }
            return false;
        }
    }

    void disconnect(){
        status |= DISCONNECTED;
    }

    void reconnect(){
        status &= ~DISCONNECTED;
    }

    int size_of(){
        int size =4+4*8+4*4+1;
        size += name.length();
        size += country.length();
        size += RIR.length();
        for(auto it=activePrefixMap.begin();it!=activePrefixMap.end();it++)
            size += it->first.length()+4;
        return size;
    }
};



#endif //BGPGEOPOLITICS_BGPGEOPOLITICS_H
