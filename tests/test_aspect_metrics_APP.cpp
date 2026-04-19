#include "test_aspect_metrics_APP.h"

void AspectMetricsTests::aspectRecordsMetrics() {
    MetricsCollector::instance().incrementCounter("reset", -MetricsCollector::instance().counterValue("reset"));
    {
        AspectScope scope("TestAction");
    }
    QVERIFY(MetricsCollector::instance().counterValue("TestAction.calls") >= 1);
    QVERIFY(MetricsCollector::instance().histogramValue("TestAction.latency_ms.lt50ms") >= 0);
}

void AspectMetricsTests::metricsCollectorStoresCounters() {
    MetricsCollector::instance().incrementCounter("CounterA", 2);
    QCOMPARE(MetricsCollector::instance().counterValue("CounterA"), 2);

    MetricsCollector::instance().recordMemorySnapshot("mem", 100);
    MetricsCollector::instance().recordMemorySnapshot("mem", 200);
    QVERIFY(MetricsCollector::instance().averageMemory("mem") >= 100);
}
