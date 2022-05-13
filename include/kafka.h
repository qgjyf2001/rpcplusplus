#ifndef KAFKA_H
#define KAFKA_H
#include <iostream>
#include <map>
#include <memory>
#include <future>
#include <mutex>

#include <librdkafka/rdkafkacpp.h>

#include "safeQueue.h"
#include "config.h"
class kafkaProducer
{
private:
    static std::shared_ptr<kafkaProducer> instance_;
    std::map<std::string,std::shared_ptr<RdKafka::Producer>> kafkaMap;
    safeQueue<std::pair<std::string,std::string>> produceQueue;
    std::mutex mutex;
    std::condition_variable consumer;
    bool isShutdown=false;
    std::thread thread;
public:
    kafkaProducer()
    {
        thread=std::thread(&kafkaProducer::syncThread,this);
    }
    static std::shared_ptr<kafkaProducer> instance()
    {
        if (instance_==nullptr) {
            instance_=std::make_shared<kafkaProducer>();
        }
        return instance_;
    }
    void syncThread();
    int produce_(std::string kafkaName,std::string message);
    int produce(std::string kafkaName,std::string message);
    int createKafkaInstance(std::string kafkaName);
    ~kafkaProducer()
    {
        for (auto &[name,producer]:kafkaMap)
        {
            std::cout<<"flush kafka producer:"<<name<<std::endl;
            producer->flush(10*1000);
        }
        isShutdown=true;
        thread.join();
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