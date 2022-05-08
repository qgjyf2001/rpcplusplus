#ifndef KAFKA_H
#define KAFKA_H
#include <iostream>
#include <map>
#include <memory>
#include <future>
#include <shared_mutex>

#include <librdkafka/rdkafkacpp.h>

#include "config.h"
class kafkaProducer
{
private:
    static std::shared_ptr<kafkaProducer> instance_;
    std::shared_mutex mutex;
    std::map<std::string,std::shared_ptr<RdKafka::Producer>> kafkaMap;
public:
    static std::shared_ptr<kafkaProducer> instance()
    {
        if (instance_==nullptr) {
            instance_=std::make_shared<kafkaProducer>();
        }
        return instance_;
    }
    std::future<int> produce(std::string kafkaName,std::string message);
    int createKafkaInstance(std::string kafkaName);
    ~kafkaProducer()
    {
        for (auto &[name,producer]:kafkaMap)
        {
            std::cout<<"flush kafka producer:"<<name<<std::endl;
            producer->flush(10*1000);
        }
    }
};

class kafkaConsumer
{
private:
    const int partition=RdKafka::Topic::PARTITION_UA;
    std::shared_ptr<RdKafka::KafkaConsumer> consumer;
    std::shared_ptr<RdKafka::Topic> topic;
public:
    kafkaConsumer(std::string kafkaName);
    std::future<std::string> consume(int timeout);
    ~kafkaConsumer();
};
#endif