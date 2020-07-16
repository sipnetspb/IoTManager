#pragma once

#include <PubSubClient.h>

#include "Base/Writer.h"

class MqttWriter : public Writer {
   public:
    MqttWriter(PubSubClient* client, const char* topic);
    ~MqttWriter();

    bool begin(size_t length) override;
    bool write(const char* data) override;
    bool end() override;

    bool print(const char* data) override;

   private:
    char* _topic;
    size_t _length;
    PubSubClient* _client;
};