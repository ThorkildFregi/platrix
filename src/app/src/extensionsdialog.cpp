#include <QLabel>
#include <QWidget>
#include <QPushButton>
#include <QMessageBox>

#include <QVBoxLayout>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QStandardPaths>

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QRegularExpression>

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

    QLabel *labelLoadSRBtn = new QLabel;
    labelLoadSRBtn->setText("Load a syntax rules");

    loadSRBtn = new QPushButton("Load Syntax Rules");

    connect(loadSRBtn, &QPushButton::clicked, this, &ExtensionsDialog::loadSyntaxRules);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 0, 5, 5);

    mainLayout->addWidget(titleBar);
    mainLayout->addWidget(labelLoadSRBtn);
    mainLayout->addWidget(loadSRBtn);
    mainLayout->addStretch();

    setLayout(mainLayout);
}

bool ExtensionsDialog::verifyFileAccordance(QString path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(nullptr, "Error", "File not found !");
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject json = doc.object();

    if (!json.contains("extension") || !json.contains("rules")) {
        QMessageBox::critical(nullptr, "Error", "Missing 'extension' or 'rules'.");
        return false;
    }

    if (!json["extension"].isString() || !json["rules"].isArray()) {
        QMessageBox::critical(nullptr, "Error", "Invalid data types.");
        return false;
    }

    if (json["extension"].toString().isEmpty() || json["rules"].toArray().isEmpty()) {
        QMessageBox::critical(nullptr, "Error", "Fields cannot be empty.");
        return false;
    }

    for (const QString &key : json.keys()) {
        if (key != "extension" && key != "rules") {
            QMessageBox::critical(nullptr, "Error", QString("Unknown parameter: %1").arg(key));
            return false;
        }
    }

    QJsonArray rulesArray = json["rules"].toArray();
    for (int i = 0; i < rulesArray.size(); ++i) {
        QJsonObject rule = rulesArray[i].toObject();

        if (!rule.contains("pattern") || !rule.contains("color")) {
            QMessageBox::critical(nullptr, "Error", QString("Rule %1 is missing parameters.").arg(i));
            return false;
        }

        if (!rule["pattern"].isString() || !rule["color"].isString()) {
            QMessageBox::critical(nullptr, "Error", QString("Rule %1 has invalid types.").arg(i));
            return false;
        }

        QRegularExpression re(rule["pattern"].toString());
        if (!re.isValid()) {
            QMessageBox::critical(nullptr, "Error", QString("Regex error in rule %1: %2").arg(i).arg(re.errorString()));
            return false;
        }

        QString hexColor = rule["color"].toString();
        if (!QColor::isValidColorName(hexColor)) {
            QMessageBox::critical(nullptr, "File not in accordance", QString("The color '%1' is not a valid hexadecimal format (ex: #RRGGBB).").arg(hexColor));
            return false;
        }
    }
    
    file.close();

    return true;
} 

void ExtensionsDialog::loadSyntaxRules()
{
    QString filters = "Syntax rules file (*.strl)";

    QString filePath = QFileDialog::getOpenFileName(this, "Open a file", QDir::homePath(), filters, nullptr, QFileDialog::DontResolveSymlinks);

    if (filePath.isEmpty()) return;

    QFileInfo fileInfo(filePath);
    
    if (!verifyFileAccordance(filePath)) return;

    QString syntaxPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/syntax/";

    QDir dir(syntaxPath);
    if (!dir.exists()) dir.mkpath(".");

    QString destPath = QDir(syntaxPath).filePath(fileInfo.fileName());

    int i = 1;
    while (QFile::exists(destPath)) {
        destPath = QDir(destPath).filePath(QString("%1_copy%2.%3").arg(fileInfo.baseName()).arg(i++).arg(fileInfo.suffix()));
    }

    if (QFile::copy(filePath, destPath)) {
        QMessageBox::information(this, "Success", "Loaded syntax rules file to: " + destPath);
    } else {
        QMessageBox::critical(this, "Error", "Could not copy syntax rules file !");
        return;
    }
}