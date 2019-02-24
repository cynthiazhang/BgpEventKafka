//
//  ConsumerImpl.hpp
//  BGPGeo
//
//  Created by zhangxinyi on 2019/2/20.
//  Copyright © 2019年 zhangxinyi. All rights reserved.
//

#ifndef ConsumerImpl_hpp
#define ConsumerImpl_hpp


#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <cstring>
#include <getopt.h>
#include <list>
#include "librdkafka/rdkafkacpp.h"
#include <vector>
#include<fstream>

using std::string;
using std::list;
using std::cout;
using std::endl;
using std::vector;
using std::fstream;

static bool run = true;
static bool exit_eof = true;

// 从kafka消费消息存到msg_data数组


struct protodata
{
    uint64_t uuid;
    uint64_t position;
    uint64_t next_position;
    string gtid;
};

static vector<protodata> fulltopic;



class MyEventCb : public RdKafka::EventCb {
public:
    void event_cb (RdKafka::Event &event) {
        switch (event.type())
        {
            case RdKafka::Event::EVENT_ERROR:
                std::cerr << "ERROR (" << RdKafka::err2str(event.err()) << "): " <<
                event.str() << std::endl;
                if (event.err() == RdKafka::ERR__ALL_BROKERS_DOWN)
                    run = false;
                break;
                
            case RdKafka::Event::EVENT_STATS:
                std::cerr << "\"STATS\": " << event.str() << std::endl;
                break;
                
            case RdKafka::Event::EVENT_LOG:
                fprintf(stderr, "LOG-%i-%s: %s\n",
                        event.severity(), event.fac().c_str(), event.str().c_str());
                break;
                
            default:
                std::cerr << "EVENT " << event.type() <<
                " (" << RdKafka::err2str(event.err()) << "): " <<
                event.str() << std::endl;
                break;
        }
    }
};


void msg_consume(RdKafka::Message* message, void* opaque) ;




class MyConsumeCb : public RdKafka::ConsumeCb {
public:
    void consume_cb (RdKafka::Message &msg, void *opaque) {
        msg_consume(&msg, opaque);
    }
};

static void sigterm (int sig) {
    run = false;
}

class ConsummerKafka
{
public:
    ConsummerKafka();;
    ~ConsummerKafka(){}
    
    int init_kafka(int partition, string brokers, string topic);
    int pull_data_from_kafka();
    void destroy();
    
private:
    RdKafka::Conf * global_conf;
    RdKafka::Conf * topic_conf;
    string brokers;
    string errstr;
    RdKafka::Consumer *consumer;
    string topic_name ;
    RdKafka::Topic *topic;
    int32_t partition;
    int64_t start_offset;
    RdKafka::Message *msg;
};


#endif /* ConsumerImpl_hpp */
