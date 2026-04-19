#ifndef IMAGE_PROCESSOR_H
#define IMAGE_PROCESSOR_H

#include "Logger.h"
#include <QImage>
#include <memory>

/**
 * Decorator Pattern for Image Processing
 */
class ImageProcessor {
public:
    virtual ~ImageProcessor() = default;
    virtual QImage process(const QImage& image) = 0;
};

/**
 * Base Image Processor
 */
class BaseImageProcessor : public ImageProcessor {
public:
    QImage process(const QImage& image) override {
        return image;
    }
};

/**
 * Abstract Decorator
 */
class ImageProcessorDecorator : public ImageProcessor {
protected:
    std::shared_ptr<ImageProcessor> wrapped_;

public:
    explicit ImageProcessorDecorator(std::shared_ptr<ImageProcessor> wrapped)
        : wrapped_(wrapped) {}
};

/**
 * Resize Decorator
 */
class ResizeDecorator : public ImageProcessorDecorator {
private:
    int width_;
    int height_;

public:
    ResizeDecorator(std::shared_ptr<ImageProcessor> wrapped, int width, int height)
        : ImageProcessorDecorator(wrapped), width_(width), height_(height) {}

    QImage process(const QImage& image) override {
        QImage processed = wrapped_->process(image);
        Logger::getInstance().log("ImageProcessor",
            QString("Image resized to %1x%2").arg(width_).arg(height_));
        return processed.scaled(width_, height_, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
};

/**
 * Grayscale/Sepia Decorator
 */
class SepiaDecorator : public ImageProcessorDecorator {
public:
    explicit SepiaDecorator(std::shared_ptr<ImageProcessor> wrapped)
        : ImageProcessorDecorator(wrapped) {}

    QImage process(const QImage& image) override {
        QImage processed = wrapped_->process(image);
        QImage result = processed.convertToFormat(QImage::Format_RGB32);

        for (int y = 0; y < result.height(); ++y) {
            for (int x = 0; x < result.width(); ++x) {
                QRgb pixel = result.pixel(x, y);
                int gray = qGray(pixel);
                // Simple sepia tone
                int r = qMin(255, static_cast<int>(gray * 1.2));
                int g = qMin(255, static_cast<int>(gray * 1.0));
                int b = qMin(255, static_cast<int>(gray * 0.8));
                result.setPixel(x, y, qRgb(r, g, b));
            }
        }

        Logger::getInstance().log("ImageProcessor", "Sepia filter applied");
        return result;
    }
};

/**
 * Blur Decorator
 */
class BlurDecorator : public ImageProcessorDecorator {
public:
    explicit BlurDecorator(std::shared_ptr<ImageProcessor> wrapped)
        : ImageProcessorDecorator(wrapped) {}

    QImage process(const QImage& image) override {
        QImage processed = wrapped_->process(image);
        // Simple blur implementation would go here
        Logger::getInstance().log("ImageProcessor", "Blur filter applied");
        return processed;
    }
};

#endif // IMAGE_PROCESSOR_H