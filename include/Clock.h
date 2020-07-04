#pragma once

#include <Arduino.h>

#include "Config/ClockConfig.h"
#include "Utils/TimeUtils.h"
#include "Utils/PrintMessage.h"

#ifdef ESP8266
#include "sntp.h"
#endif

class Clock {
    const char* MODULE = "Clock";

   private:
    Time_t _time_local;
    Time_t _time_utc;
    unsigned long _uptime;
    unsigned long _unixtime;
    bool _hasSynced;
    bool _configured;
    ClockConfig* _cfg;

   public:
    Clock() : _uptime{millis()}, _hasSynced{false}, _configured{false} {};

    void setConfig(ClockConfig* cfg) {
        _cfg = cfg;
    }

    void loop() {
        unsigned long passed = millis_since(_uptime);
        if (passed < ONE_SECOND_ms) {
            return;
        }
        _uptime += passed;

        // world time
        time_t now = getSystemTime();
        time_t estimated = _unixtime + (passed / ONE_SECOND_ms);
        double drift = difftime(now, estimated);
        if (drift > 1) {
            // Обработать ситуации c дрифтом времени на значительные величины
        }
        // TODO сохранять время на флеше

        _unixtime = now;

        breakEpochToTime(_unixtime, _time_utc);

        breakEpochToTime(_unixtime + getOffsetInSeconds(_cfg->getTimezone()), _time_local);
    }

    bool hasSync() {
        if (!_hasSynced) {
            startSync();
        }
        return _hasSynced;
    }

    void startSync() {
        if (!_configured) {
            pm.info("ntp: " + _cfg->getNtp() + " timezone: " + String(_cfg->getTimezone()));
            setupSntp();
            _configured = true;
            // лучше не ждать, проверим в следующий раз
            return;
        }
        _hasSynced = hasTimeSynced();
        if (_hasSynced) {
            pm.info(getDateDotFormated() + " " + getTime());
        } else {
            pm.error("failed to obtain");
        }
    }

    void setupSntp() {
        configTime(0, 0, _cfg->getNtp().c_str(), "ru.pool.ntp.org", "pool.ntp.org");
    }

    bool hasTimeSynced() const {
        return _unixtime > MIN_DATETIME;
    }

    time_t getSystemTime() const {
        timeval tv{0, 0};
        timezone tz = timezone{0, 0};
        time_t epoch = 0;
        if (gettimeofday(&tv, &tz) != -1) {
            epoch = tv.tv_sec;
        }
        return epoch;
    }
    const unsigned long getEpoch() {
        return _unixtime;
    }
    const String getTimeUnix() {
        return String(_unixtime);
    }

    /*
    * Локальное время "дд.ММ.гг"
    */
    const String getDateDotFormated() {
        char buf[32];
        sprintf(buf, "%02d.%02d.%02d", _time_local.day_of_month, _time_local.month, _time_local.year);
        return String(buf);
    }

    /*
    * Локальное дата время "дд.ММ.гг чч.мм.cc"
    */
    const String getDateTimeDotFormated() {
        char buf[32];
        sprintf(buf, "%02d.%02d.%02d %02d:%02d:%02d", _time_local.day_of_month, _time_local.month, _time_local.year, _time_local.hour, _time_local.minute, _time_local.second);
        return String(buf);
    }

    /*
    * Локальное время "чч:мм:cc"
    */
    const String getTime() {
        char buf[32];
        sprintf(buf, "%02d:%02d:%02d", _time_local.hour, _time_local.minute, _time_local.second);
        return String(buf);
    }

    const String getTimeJson() {
        char buf[32];
        sprintf(buf, "%02d-%02d-%02d", _time_local.hour, _time_local.minute, _time_local.second);
        return String(buf);
    }

    /*
    * Локальное время "чч:мм"
    */
    const String getTimeWOsec() {
        char buf[32];
        sprintf(buf, "%02d:%02d", _time_local.hour, _time_local.minute);
        return String(buf);
    }

    /*
    * Время с момента запуска "чч:мм:cc" далее "дд чч:мм"
    */
    const String getUptime() {
        return prettyMillis(_uptime);
    }
};