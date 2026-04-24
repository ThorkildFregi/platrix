#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>

#include <QFrame>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QMessageBox>

#include <QString>
#include <QVariant>
#include <QMetaType>

#include "framelessdialog.h"

#include "settingsdialog.h"
#include "settingsmanager.h"

SettingsDialog::SettingsDialog(QWidget *parent) : FramelessDialog(parent) 
{
    setWindowTitle("Settings");
    setMinimumSize(800, 600);

    titleBar = new QWidget();
    titleBar->setObjectName("titleBar");
    titleBar->setFixedHeight(35);
    titleBar->setMouseTracking(true);
    this->setMouseTracking(true);

    QHBoxLayout *titleLayout = new QHBoxLayout(titleBar);
    titleLayout->setContentsMargins(5, 0, 0, 0);
    titleLayout->setSpacing(0);

    titleLayout->addStretch();

    QLabel *appName = new QLabel();
    appName->setText("Settings");

    titleLayout->addWidget(appName);
    titleLayout->addStretch();

    QPushButton *btnClose = new QPushButton("✕");
    btnClose->setObjectName("btnClose");
    btnClose->setFixedSize(45, 35);
    
    titleLayout->addWidget(btnClose);

    connect(btnClose, &QPushButton::clicked, this, &SettingsDialog::close);

    settingsLayout = new QVBoxLayout;
    settingsLayout->setAlignment(Qt::AlignTop);

    auto &settings = SettingsManager::instance().settings;

    uiWidgets.clear();

    for (const QString &key : settingsList) {
        if (settings.value(key).typeId() == QMetaType::Int) {
            QLabel *label = new QLabel;
            label->setText(settingsLabel.value(key).toString());
            label->setMaximumHeight(20);

            QSpinBox *widget = new QSpinBox;
            widget->setValue(settings.value(key).toInt());
            widget->setMaximumWidth(100);

            uiWidgets[key] = widget;

            QFrame *line = new QFrame();
            line->setObjectName("settingsSeparator");
            line->setFrameShape(QFrame::HLine);
            line->setFrameShadow(QFrame::Sunken);

            settingsLayout->addWidget(label);
            settingsLayout->addWidget(widget);
            settingsLayout->addWidget(line);
        } 
        else if (settings.value(key).typeId() == QMetaType::Bool) {
            QLabel *label = new QLabel;
            label->setText(settingsLabel.value(key).toString());
            label->setMaximumHeight(20);
            
            QCheckBox *widget = new QCheckBox;
            widget->setChecked(settings.value(key).toBool());
            widget->setMaximumWidth(100);

            uiWidgets[key] = widget;

            QFrame *line = new QFrame();
            line->setObjectName("settingsSeparator");
            line->setFrameShape(QFrame::HLine);
            line->setFrameShadow(QFrame::Sunken);

            settingsLayout->addWidget(label);
            settingsLayout->addWidget(widget);
            settingsLayout->addWidget(line);
        } 
        else {
            QMessageBox::critical(this, "Error", "A setting is not of type int or bool !");
        }
    }

    settingsLayout->addStretch(1);

    settingsWidget = new QWidget;
    settingsWidget->setObjectName("settingsWidget");
    settingsWidget->setLayout(settingsLayout);
    
    settingsScrollArea = new QScrollArea;
    settingsScrollArea->setWidget(settingsWidget);
    settingsScrollArea->setWidgetResizable(true);

    applyButton = new QPushButton("Apply");
    applyButton->setMaximumWidth(100);

    connect(applyButton, &QPushButton::clicked, this, &SettingsDialog::applySettings);

    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 0, 5, 5);

    mainLayout->addWidget(titleBar);
    mainLayout->addWidget(settingsScrollArea, 1);
    mainLayout->addWidget(applyButton, 0, Qt::AlignLeft);

    setLayout(mainLayout);
}


void SettingsDialog::applySettings() 
{
    auto &manager = SettingsManager::instance();

    for (auto it = uiWidgets.begin(); it != uiWidgets.end(); ++it) {
        QString key = it.key();
        QWidget *widget = it.value();

        if (QSpinBox* sb = qobject_cast<QSpinBox*>(widget)) {
            manager.set(key, sb->value());
        } 
        else if (QCheckBox* cb = qobject_cast<QCheckBox*>(widget)) {
            manager.set(key, cb->isChecked());
        }
    }

    accept();
}