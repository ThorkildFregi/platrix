#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QScrollArea>
#include <QSpinBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QDir>

class SettingsDialog : public QDialog 
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr) : QDialog(parent) {
        setWindowTitle("Settings");
        setMinimumSize(300, 200);

        fontSizeLabel = new QLabel;
        fontSizeLabel->setText("Editor: Font Size");
        fontSizeLabel->setMaximumHeight(20);
        fontSizeBox = new QSpinBox;
        fontSizeBox->setMaximumWidth(100);

        tabSizeLabel = new QLabel;
        tabSizeLabel->setText("Editor: Tab Size");
        tabSizeLabel->setMaximumHeight(20);
        tabSizeBox = new QSpinBox;
        tabSizeBox->setMaximumWidth(100);

        darkThemeLabel = new QLabel;
        darkThemeLabel->setText("Theme: Dark Theme");
        darkThemeLabel->setMaximumHeight(20);
        darkThemeCheck = new QCheckBox;

        settingsLayout = new QVBoxLayout;
        settingsLayout->setAlignment(Qt::AlignTop);
        settingsLayout->addWidget(fontSizeLabel);
        settingsLayout->addWidget(fontSizeBox);
        settingsLayout->addWidget(tabSizeLabel);
        settingsLayout->addWidget(tabSizeBox);
        settingsLayout->addWidget(darkThemeLabel);
        settingsLayout->addWidget(darkThemeCheck);
        settingsLayout->addStretch(1);

        settingsWidget = new QWidget;
        settingsWidget->setLayout(settingsLayout);
        
        settingsScrollArea = new QScrollArea;
        settingsScrollArea->setWidget(settingsWidget);
        settingsScrollArea->setWidgetResizable(true);

        applyButton = new QPushButton("Apply");
        applyButton->setMaximumWidth(100);

        connect(applyButton, &QPushButton::clicked, this, &SettingsDialog::applySettings);

        mainLayout = new QVBoxLayout;
        mainLayout->addWidget(settingsScrollArea, 1);
        mainLayout->addWidget(applyButton, 0, Qt::AlignLeft);

        loadSettings();

        setLayout(mainLayout);
    }

private:
    QLabel *fontSizeLabel;
    QSpinBox *fontSizeBox;

    QLabel *tabSizeLabel;
    QSpinBox *tabSizeBox;

    QLabel *darkThemeLabel;
    QCheckBox *darkThemeCheck;

    QPushButton *applyButton;

    QVBoxLayout *mainLayout;
    QVBoxLayout *settingsLayout;

    QWidget *settingsWidget;

    QScrollArea *settingsScrollArea;

    void loadSettings() {
        QFile file(MainWindow::getSettingsPath());
        
        if (file.open(QIODevice::ReadOnly)) {
            QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
            QJsonObject json = doc.object();
            
            fontSizeBox->setValue(json["fontSize"].toInt(14));
            tabSizeBox->setValue(json["tabSize"].toInt(4));
            darkThemeCheck->setChecked(json["darkTheme"].toBool(false));

            file.close();
        }
    }

    void applySettings() {
        QJsonObject json;

        json["fontSize"] = fontSizeBox->value();
        json["tabSize"] = tabSizeBox->value();
        json["darkTheme"] = darkThemeCheck->isChecked();

        QFile file(MainWindow::getSettingsPath());
        if (file.open(QIODevice::WriteOnly)) {
            QJsonDocument doc(json);
            
            file.write(doc.toJson());

            file.close();
        }

        accept();
    }
};

#endif