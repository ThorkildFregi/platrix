#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QWidget>
#include <QDialog>

#include <QMap>
#include <QString>
#include <QVariant>
#include <QStringList>

#include "framelessdialog.h"

QT_BEGIN_NAMESPACE
class QHBoxLayout;
class QVBoxLayout;
class QPushButton;

class QScrollArea;
QT_END_NAMESPACE

class SettingsDialog : public FramelessDialog 
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);

private slots:
    void applySettings();

private:
    QPushButton *applyButton;

    QVBoxLayout *mainLayout;
    QVBoxLayout *settingsLayout;

    QWidget *settingsWidget;
    QWidget *titleBar;

    QScrollArea *settingsScrollArea;

    QStringList settingsList = {"fontSize", "tabSize", "darkTheme"};

    QMap<QString, QWidget*> uiWidgets;
    QMap<QString, QVariant> settingsLabel = {
        {"fontSize", "Editor: Font Size"},
        {"tabSize", "Editor: Tab Size"},
        {"darkTheme", "Theme: Dark Theme"}
    };
};

#endif