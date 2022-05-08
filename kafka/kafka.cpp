#include "kafka.h"
#include "unistd.h"
std::shared_ptr<kafkaProducer> kafkaProducer::instance_=nullptr;
std::future<int> kafkaProducer::produce(std::string kafkaName,std::string message)
{
    auto func=std::async([&](){
        bool result;
        {
            std::shared_lock<std::shared_mutex> lck(mutex);
            result=kafkaMap.find(kafkaName) == kafkaMap.end();
        }
        if (result)
        {
            std::unique_lock<std::shared_mutex> lck(mutex);
            createKafkaInstance(kafkaName);
        }
        std::shared_lock<std::shared_mutex> lck(mutex);
        auto &producer=kafkaMap[kafkaName];
        auto resp=producer->produce(kafkaName,RdKafka::Topic::PARTITION_UA,RdKafka::Producer::RK_MSG_COPY,
                                    const_cast<char *>(message.data()), message.length(),nullptr,0,0,nullptr,nullptr);
        return producer->poll(0);
    });
    return func;
}
int kafkaProducer::createKafkaInstance(std::string kafkaName)
{
    std::shared_ptr<RdKafka::Conf> conf;
    std::shared_ptr<RdKafka::Producer> producer;
    std::string errstr;
    conf.reset(RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL));
    bool result=conf->set("bootstrap.servers",config::kafkaBrokers,errstr);
    producer.reset(RdKafka::Producer::create(conf.get(),errstr));
    kafkaMap[kafkaName]=producer;
    return result;
}
kafkaConsumer::kafkaConsumer(std::string kafkaName)
{
    std::shared_ptr<RdKafka::Conf> conf;
    std::shared_ptr<RdKafka::Conf> topicConf;
    std::string errstr;
    conf.reset(RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL));
    topicConf.reset(RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC));
    bool result=conf->set("metadata.broker.list",config::kafkaBrokers,errstr);
    conf->set("group.id","pressure",errstr);
    consumer.reset(RdKafka::KafkaConsumer::create(conf.get(),errstr));
    auto code=consumer->subscribe({kafkaName});
}
std::future<std::string> kafkaConsumer::consume(int timeout)
{
    auto func=std::async([=]()->std::string{
        while (true) {
            std::shared_ptr<RdKafka::Message> msg;
            msg.reset(consumer->consume(timeout));
            if (msg->err()!=RdKafka::ERR_NO_ERROR)
            {
                return "";
            }
            return static_cast<char*>(msg->payload());
        }
    });
    return func;
}
kafkaConsumer::~kafkaConsumer()
{
    alarm(10);
    consumer->close();
    RdKafka::wait_destroyed(5000);
}