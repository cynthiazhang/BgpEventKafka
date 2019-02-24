//
//  BGPKafkaEvent.hpp
//  BGPGeo
//
//  Created by zhangxinyi on 2019/2/23.
//  Copyright © 2019年 zhangxinyi. All rights reserved.
//

#ifndef BGPKafkaEvent_hpp
#define BGPKafkaEvent_hpp

#include <stdio.h>
#include "ProducerImpl.h"
#include "BGPevent.h"

using namespace std;
class BGPKafkaEvent{
public:
    ProducerKafka *producer;
    EventTable *event;
    BGPKafkaEvent(){}
    
    void init(int partition, char *brokers, char *topic);
    void destory();
    void BGPCheckOutage(AS *as,  PrefixPeer *prefixPeer, unsigned int time);
    void BGPCheckHijack(AS *as, PrefixPeer *prefixPeer, unsigned int time);
};

#endif /* BGPKafkaEvent_hpp */
