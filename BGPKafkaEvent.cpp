//
//  BGPKafkaEvent.cpp
//  BGPGeo
//
//  Created by zhangxinyi on 2019/2/23.
//  Copyright © 2019年 zhangxinyi. All rights reserved.
//

#include "BGPKafkaEvent.hpp"
void BGPKafkaEvent::init(int partition,char* brokers,char *topic){
    //Producer init;
    producer = new ProducerKafka;
    if (PRODUCER_INIT_SUCCESS == producer->init_kafka(partition, brokers,topic)){
        cout<<"Kafka producer init success"<<endl;
    }
    else{
        cout<<"Kafka producer init failed"<<endl;
        return ;
    }
    
    //Eventtable init
    return;
}

void BGPKafkaEvent::destory(){
    producer->destroy();
}

void  BGPKafkaEvent::BGPCheckOutage(AS *as,  PrefixPeer *prefixPeer, unsigned int time){
    event->checkOutage(as, prefixPeer, time, producer);
}
void  BGPKafkaEvent::BGPCheckHijack(AS *as, PrefixPeer *prefixPeer, unsigned int time){
    event->checkHijack(as,  prefixPeer, time, producer);
}
