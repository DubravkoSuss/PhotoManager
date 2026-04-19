#ifndef ASPECT_H
#define ASPECT_H

#include <QString>
#include <QVariantMap>
#include <QElapsedTimer>
#include <QVector>
#include <memory>
#include "Logger_APP.h"
#include "MetricsCollector_APP.h"

/**
 * Aspect - Base interface for cross-cutting concerns
 * Implements AOP pattern for logging and metrics collection
 */
class Aspect {
public:
    virtual ~Aspect() = default;

    virtual void before(const QString& action, const QVariantMap& data) = 0;
    virtual void after(const QString& action, const QVariantMap& data, qint64 durationMs) = 0;
};

/**
 * LoggingAspect - Logs method entry and exit
 */
class LoggingAspect : public Aspect {
public:
    void before(const QString& action, const QVariantMap& data) override {
        Logger::getInstance().log("Aspect", QString("Before %1").arg(action));
    }

    void after(const QString& action, const QVariantMap& data, qint64 durationMs) override {
        Logger::getInstance().log("Aspect", QString("After %1 (%2 ms)").arg(action).arg(durationMs));
    }
};

/**
 * MetricsAspect - Collects performance metrics
 */
class MetricsAspect : public Aspect {
public:
    void before(const QString& action, const QVariantMap& data) override {
        MetricsCollector::instance().incrementCounter(action + ".calls");
    }

    void after(const QString& action, const QVariantMap& data, qint64 durationMs) override {
        MetricsCollector::instance().recordTiming(action + ".latency_ms", durationMs);
    }
};

/**
 * AspectManager - Singleton registry for aspects
 */
class AspectManager {
public:
    static AspectManager& instance() {
        static AspectManager manager;
        return manager;
    }

    void registerAspect(std::shared_ptr<Aspect> aspect) {
        aspects_.append(std::move(aspect));
    }

    const QVector<std::shared_ptr<Aspect>>& aspects() const { return aspects_; }

private:
    AspectManager() {
        aspects_.append(std::make_shared<LoggingAspect>());
        aspects_.append(std::make_shared<MetricsAspect>());
    }

    QVector<std::shared_ptr<Aspect>> aspects_;
};

/**
 * AspectScope - RAII wrapper for aspect execution
 */
class AspectScope {
public:
    AspectScope(const QString& action, const QVariantMap& data = {})
        : action_(action), data_(data) {
        timer_.start();
        for (const auto& aspect : AspectManager::instance().aspects()) {
            aspect->before(action_, data_);
        }
    }

    ~AspectScope() {
        qint64 durationMs = timer_.elapsed();
        for (const auto& aspect : AspectManager::instance().aspects()) {
            aspect->after(action_, data_, durationMs);
        }
    }

private:
    QString action_;
    QVariantMap data_;
    QElapsedTimer timer_;
};

#endif // ASPECT_H
