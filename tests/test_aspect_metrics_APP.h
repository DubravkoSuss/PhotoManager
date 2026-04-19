#ifndef TEST_ASPECT_METRICS_H
#define TEST_ASPECT_METRICS_H

#include <QObject>
#include <QtTest>
#include "Aspect_APP.h"
#include "MetricsCollector_APP.h"

class AspectMetricsTests : public QObject {
    Q_OBJECT
private slots:
    void aspectRecordsMetrics();
    void metricsCollectorStoresCounters();
};

#endif // TEST_ASPECT_METRICS_H
