//
//  ProducerImpl.h
//  BGPGeo
//
//  Created by zhangxinyi on 2019/2/21.
//  Copyright © 2019年 zhangxinyi. All rights reserved.
//

#ifndef BGPKafkaProducer_hpp
#define BGPKafkaProducer_hpp

#include "librdkafka/rdkafkacpp.h"
#include "json.hpp"

#include <ctype.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>
#include <sys/time.h>
#include "rdkafka.h"
#include <syslog.h>
#include <errno.h>


using json = nlohmann::json;
using namespace std;

const int PRODUCER_INIT_FAILED = -1;
const int PRODUCER_INIT_SUCCESS = 0;
const int PUSH_DATA_FAILED = -1;
const int PUSH_DATA_SUCCESS = 0;


class ProducerKafka
{
public:
    ProducerKafka(){}
    ~ProducerKafka(){}
    
    int init_kafka(int partition, char *brokers, char *topic);
    int push_data_to_kafka(const char* buf, const int buf_len);
    void destroy();
    

    
private:
    int partition_;
    
    //rd
    rd_kafka_t* handler_;
    rd_kafka_conf_t *conf_;
    
    //topic
    rd_kafka_topic_t *topic_;
    rd_kafka_topic_conf_t *topic_conf_;
};



#endif /* ProducerImp_h */

