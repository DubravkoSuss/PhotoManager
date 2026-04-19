#ifndef TAG_FLYWEIGHT_H
#define TAG_FLYWEIGHT_H

#include <QString>
#include <QSet>
#include <QMap>
#include <memory>

/**
 * FLYWEIGHT PATTERN (Structural)
 * 
 * Intent: Use sharing to support large numbers of fine-grained objects efficiently.
 *
 */
class TagFlyweightFactory {
public:
    static std::shared_ptr<QString> getTag(const QString& tag) {
        static QMap<QString, std::shared_ptr<QString>> pool;
        
        if (!pool.contains(tag)) {
            pool[tag] = std::make_shared<QString>(tag);
        }
        return pool[tag];
    }
    
    // Helper to clear pool if needed
    static void clear() {
        // Implementation dependent
    }
};

#endif // TAG_FLYWEIGHT_H
