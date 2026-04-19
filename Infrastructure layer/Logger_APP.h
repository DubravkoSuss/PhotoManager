#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QDebug>
#include <memory>
#include "MetricsCollector_APP.h"

/**
 * Singleton + Observer Pattern for Logging
 */
class Logger {
public:
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    void log(const QString& actor, const QString& action) {
        QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
        QString logEntry = QString("[%1] %2: %3").arg(timestamp, actor, action);
        logs_.append(logEntry);
        qDebug() << logEntry;
        MetricsCollector::instance().incrementCounter("logs.total");
    }

    QStringList getLogs() const {
        return logs_;
    }

    QStringList getLogsByUser(const QString& userId) const {
        QStringList userLogs;
        for (const QString& log : logs_) {
            if (log.contains(userId)) {
                userLogs.append(log);
            }
        }
        return userLogs;
    }

    // Delete copy constructor and assignment operator
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

private:
    Logger() = default;
    QStringList logs_;
};

#endif // LOGGER_H