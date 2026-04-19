#ifndef FILE_SIZE_FORMATTER_H
#define FILE_SIZE_FORMATTER_H

#include <QString>

/**
 * FileSizeFormatter - Utility for formatting file sizes
 */
class FileSizeFormatter {
public:
    static QString format(qint64 size) {
        const qint64 KB = 1024;
        const qint64 MB = KB * 1024;
        const qint64 GB = MB * 1024;

        if (size >= GB) {
            return QString::number(size / (double)GB, 'f', 2) + " GB";
        } else if (size >= MB) {
            return QString::number(size / (double)MB, 'f', 2) + " MB";
        } else if (size >= KB) {
            return QString::number(size / (double)KB, 'f', 2) + " KB";
        }
        return QString::number(size) + " bytes";
    }
};

#endif // FILE_SIZE_FORMATTER_H
