#ifndef SEARCH_RESULTS_WINDOW_H
#define SEARCH_RESULTS_WINDOW_H

#include <QWidget>
#include <QList>
#include <memory>
#include <QGridLayout>
#include "Photo.h"

class SearchResultsWindow : public QWidget {
    Q_OBJECT

public:
    explicit SearchResultsWindow(QList<std::shared_ptr<Photo>> photos, QWidget *parent = nullptr);

private:
    void setupUI();
    void addPhotoThumbnail(std::shared_ptr<Photo> photo);
    QList<std::shared_ptr<Photo>> photos_;
    QGridLayout* photoGridLayout_;
};

#endif // SEARCH_RESULTS_WINDOW_H
