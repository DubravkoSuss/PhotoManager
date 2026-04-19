#ifndef SETTINGS_DIALOG_H
#define SETTINGS_DIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QLineEdit>

class SettingsDialog : public QDialog {
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget* parent = nullptr);
private slots:
    void onThemeChanged(int index);
    void onBrowsePath();
private:
    void setupUI();
    QComboBox* themeCombo_;
    QLineEdit* exportPathEdit_;
};

#endif // SETTINGS_DIALOG_H
