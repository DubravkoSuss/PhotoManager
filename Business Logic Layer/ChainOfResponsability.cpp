//

#include "ChainOfResponsability.h"
#include "Logger.h"

QImage ResizeHandler::process(const QImage& image) {
    QImage scaled = image.scaled(w_, h_, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    Logger::getInstance().log("ImagePipeline", QString("Resize %1x%2").arg(w_).arg(h_));
    return scaled;
}

QImage SepiaHandler::process(const QImage& image) {
    QImage result = image.convertToFormat(QImage::Format_RGB32);
    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            QRgb pixel = result.pixel(x, y);
            int gray = qGray(pixel);
            int r = qMin(255, static_cast<int>(gray * 1.2));
            int g = qMin(255, static_cast<int>(gray * 1.0));
            int b = qMin(255, static_cast<int>(gray * 0.8));
            result.setPixel(x, y, qRgb(r, g, b));
        }
    }
    Logger::getInstance().log("ImagePipeline", "Sepia");
    return result;
}

QImage BlurHandler::process(const QImage& image) {
    Logger::getInstance().log("ImagePipeline", "Blur");
    return image;
}
