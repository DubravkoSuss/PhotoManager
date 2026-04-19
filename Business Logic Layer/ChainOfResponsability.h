//


#ifndef PHOTOMANAGER_CHAINOFRESPONSABILITY_H
#define PHOTOMANAGER_CHAINOFRESPONSABILITY_H

#include <QImage>
#include <memory>

class ImageHandler {
public:
    virtual ~ImageHandler() = default;
    
    void setNext(std::shared_ptr<ImageHandler> next) { next_ = next; }

    QImage handle(const QImage& image) {
        QImage out = process(image);
        if (next_) return next_->handle(out);
        return out;
    }
protected:
    virtual QImage process(const QImage& image) = 0;
private:
    std::shared_ptr<ImageHandler> next_;
};

class ResizeHandler : public ImageHandler {
public:
    ResizeHandler(int w, int h) : w_(w), h_(h) {}
protected:
    QImage process(const QImage& image) override;
private:
    int w_;
    int h_;
};

class SepiaHandler : public ImageHandler {
protected:
    QImage process(const QImage& image) override;
};

class BlurHandler : public ImageHandler {
protected:
    QImage process(const QImage& image) override;
};

#endif //PHOTOMANAGER_CHAINOFRESPONSABILITY_H
