#ifndef STORAGE_STRATEGY_H
#define STORAGE_STRATEGY_H

#include "Logger_APP.h"
#include <QString>
#include <QFile>
#include <QDir>
#include "MetricsCollector_APP.h"

/**
 * Strategy Pattern for Storage
 */
class StorageStrategy {
public:
    virtual ~StorageStrategy() = default;

    virtual QString upload(const QString& filePath, const QString& userId) = 0;
    virtual QString download(const QString& path) = 0;
    virtual void remove(const QString& path) = 0;
};

class LocalStorageStrategy : public StorageStrategy {
public:
    QString upload(const QString& filePath, const QString& userId) override {
        QString storagePath = "./photos/" + userId;
        MetricsCollector::instance().incrementCounter("storage.uploads");
        QDir dir;
        dir.mkpath(storagePath);

        QFileInfo fileInfo(filePath);
        QString destPath = storagePath + "/" + fileInfo.fileName();

        QFile::copy(filePath, destPath);
        Logger::getInstance().log("LocalStorage", "File uploaded: " + destPath);
        return destPath;
    }

    QString download(const QString& path) override {
        return path;
    }

    void remove(const QString& path) override {
        QFile::remove(path);
        Logger::getInstance().log("LocalStorage", "File deleted: " + path);
    }
};

/**
 * Cloud Storage Strategy
 */
class CloudStorageStrategy : public StorageStrategy {
public:
    QString upload(const QString& filePath, const QString& userId) override {
        QFileInfo fileInfo(filePath);
        QString cloudPath = QString("s3://bucket/%1/%2").arg(userId, fileInfo.fileName());
        Logger::getInstance().log("CloudStorage", "File uploaded to cloud: " + cloudPath);
        return cloudPath;
    }

    QString download(const QString& path) override {
        qDebug() << "Downloading from cloud:" << path;
        return path;
    }

    void remove(const QString& path) override {
        Logger::getInstance().log("CloudStorage", "File deleted from cloud: " + path);
    }
};

class ThirdPartyCloudClient {
public:
    QString uploadFile(const QString& localPath, const QString& destKey) {
        return QString("tp://%1").arg(destKey);
    }
    QString downloadFile(const QString& key) { return key; }
    void deleteFile(const QString& key) {}
};

class CloudClientAdapter : public StorageStrategy {
public:
    CloudClientAdapter(std::shared_ptr<ThirdPartyCloudClient> client) : client_(client) {}
    QString upload(const QString& filePath, const QString& userId) override {
        QFileInfo f(filePath);
        QString key = QString("bucket/%1/%2").arg(userId, f.fileName());
        QString path = client_->uploadFile(filePath, key);
        Logger::getInstance().log("CloudAdapter", "File uploaded: " + path);
        return path;
    }
    QString download(const QString& path) override { return client_->downloadFile(path); }
    void remove(const QString& path) override { client_->deleteFile(path); }
private:
    std::shared_ptr<ThirdPartyCloudClient> client_;
};

#endif // STORAGE_STRATEGY_H
