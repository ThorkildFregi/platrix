#include <QLabel>
#include <QWidget>
#include <QPushButton>

#include <QVBoxLayout>

#include "framelessdialog.h"

#include "extensionsdialog.h"

ExtensionsDialog::ExtensionsDialog(QWidget *parent) : FramelessDialog(parent)
{
    setWindowTitle("Extensions");
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

    connect(btnClose, &QPushButton::clicked, this, &ExtensionsDialog::close);

    QLabel *labelLoadBtn = new QLabel;
    labelLoadBtn->setText("Load an extension");

    loadBtn = new QPushButton("Load Extensions");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 0, 5, 5);

    mainLayout->addWidget(titleBar);
    mainLayout->addWidget(labelLoadBtn);
    mainLayout->addWidget(loadBtn);
    mainLayout->addStretch();

    setLayout(mainLayout);
}

void ExtensionsDialog::loadExtension(QString path)
{

}