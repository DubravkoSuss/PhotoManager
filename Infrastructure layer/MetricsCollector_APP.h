#ifndef METRICS_COLLECTOR_H
#define METRICS_COLLECTOR_H

#include <QString>
#include <QMap>
#include <QList>
#include <QDateTime>
#include <QMutex>
#include <QMutexLocker>
#include <QVector>

/**
 * MetricsCollector - Singleton for collecting application metrics
 * Supports counters, gauges, timings, histograms, and memory snapshots
 */
class MetricsCollector {
public:
    static MetricsCollector& instance() {
        static MetricsCollector collector;
        return collector;
    }

    void incrementCounter(const QString& name, qint64 amount = 1) {
        QMutexLocker locker(&mutex_);
        counters_[name] += amount;
    }

    void setGauge(const QString& name, double value) {
        QMutexLocker locker(&mutex_);
        gauges_[name] = value;
    }

    void recordTiming(const QString& name, qint64 durationMs) {
        QMutexLocker locker(&mutex_);
        timings_[name].append(durationMs);
        // Simple latency buckets
        if (durationMs < 10) histogram_[name + ".lt10ms"]++;
        else if (durationMs < 50) histogram_[name + ".lt50ms"]++;
        else if (durationMs < 100) histogram_[name + ".lt100ms"]++;
        else if (durationMs < 250) histogram_[name + ".lt250ms"]++;
        else histogram_[name + ".gte250ms"]++;
    }

    void recordMemorySnapshot(const QString& name, qint64 bytes) {
        QMutexLocker locker(&mutex_);
        memorySnapshots_[name].append(bytes);
    }

    qint64 counterValue(const QString& name) const {
        return counters_.value(name, 0);
    }

    double gaugeValue(const QString& name) const {
        return gauges_.value(name, 0.0);
    }

    double averageTiming(const QString& name) const {
        auto values = timings_.value(name);
        if (values.isEmpty()) return 0.0;
        qint64 total = 0;
        for (auto v : values) total += v;
        return static_cast<double>(total) / values.size();
    }

    qint64 histogramValue(const QString& name) const {
        return histogram_.value(name, 0);
    }

    double averageMemory(const QString& name) const {
        auto values = memorySnapshots_.value(name);
        if (values.isEmpty()) return 0.0;
        qint64 total = 0;
        for (auto v : values) total += v;
        return static_cast<double>(total) / values.size();
    }

    QStringList counters() const { return counters_.keys(); }
    QStringList gauges() const { return gauges_.keys(); }
    QStringList timings() const { return timings_.keys(); }
    QStringList histograms() const { return histogram_.keys(); }
    QStringList memorySnapshots() const { return memorySnapshots_.keys(); }

private:
    MetricsCollector() = default;

    mutable QMutex mutex_;
    QMap<QString, qint64> counters_;
    QMap<QString, double> gauges_;
    QMap<QString, QList<qint64>> timings_;
    QMap<QString, qint64> histogram_;
    QMap<QString, QList<qint64>> memorySnapshots_;
};

#endif // METRICS_COLLECTOR_H
