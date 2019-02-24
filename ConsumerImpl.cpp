//
//  ConsumerImpl.cpp
//  BGPGeo
//
//  Created by zhangxinyi on 2019/2/20.
//  Copyright © 2019年 zhangxinyi. All rights reserved.
//

#include "ConsumerImpl.h"
#include <string.h>


ConsummerKafka::ConsummerKafka()
{
    
}
int ConsummerKafka::init_kafka(int _partition, string broker, string _topic)
{
    global_conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    topic_conf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);
    
    brokers = broker;
    partition = _partition;
    topic_name = _topic;
    start_offset = RdKafka::Topic::OFFSET_BEGINNING;
    global_conf->set("metadata.broker.list", brokers, errstr);
    
    MyEventCb ex_event_cb;
    global_conf->set("event_cb", &ex_event_cb, errstr);
    
    
    /*
     * Create consumer using accumulated global configuration.
     */
    consumer = RdKafka::Consumer::create(global_conf, errstr);
    if (!consumer) {
        std::cerr << "Failed to create consumer: " << errstr << std::endl;
        exit(1);
    }
    /* Create topic */
    topic = RdKafka::Topic::create(consumer, topic_name, topic_conf, errstr);
    if (!topic) {
        std::cerr << "Failed to create topic: " << errstr << std::endl;
        exit(1);
    }
    return 0;
}

void ConsummerKafka::destroy()
{
    consumer->stop(topic, partition);
    consumer->poll(1000);
    
    delete topic;
    delete consumer;
}

int ConsummerKafka::pull_data_from_kafka()
{
    RdKafka::ErrorCode resp = consumer->start(topic, partition, start_offset);
    if (resp != RdKafka::ERR_NO_ERROR) {
        std::cerr << "Failed to start consumer: " <<
        RdKafka::err2str(resp) << std::endl;
        exit(1);
    }
    
    /*
     * Consume messages
     */
    MyConsumeCb ex_consume_cb;
    int use_ccb = 0;
    while (run) {
        if (use_ccb) {
                  //consumer->consume_callback(topic, partition, 1000,
                  //                       &ex_consume_cb, &use_ccb);
        } else {
            RdKafka::Message *msg = consumer->consume(topic, partition, 1000);
            cout<<"consume message"<<endl;
            msg_consume(msg, NULL);
            delete msg;
        }
        consumer->poll(10000);
    }
    return 0;
}


void msg_consume(RdKafka::Message* message, void* opaque) {
    switch (message->err()) {
        case RdKafka::ERR__TIMED_OUT:
            break;
            
        case RdKafka::ERR_NO_ERROR:
            /* Real message */
            std::cout << "Read msg at offset " << message->offset() << std::endl;
            if (message->key()) {
                std::cout << "Key: " << *message->key() << std::endl;
            }
            cout << static_cast<const char *>(message->payload()) << endl;
            break;
            
        case RdKafka::ERR__PARTITION_EOF:
            cout << "reach last message" << endl;
            /* Last message */
            if (exit_eof) {
                run = false;
            }
            break;
            
        case RdKafka::ERR__UNKNOWN_TOPIC:
        case RdKafka::ERR__UNKNOWN_PARTITION:
            std::cerr << "Consume failed: " << message->errstr() << std::endl;
            run = false;
            break;
            
        default:
            /* Errors */
            std::cerr << "Consume failed: " << message->errstr() << std::endl;
            run = false;
    }
}




