#include <QLabel>
#include <QWidget>
#include <QPushButton>

#include <QMessageBox>

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

    QLabel *labelLdSRBtn = new QLabel;
    labelLoadBtn->setText("Load a syntax rules");

    loadSRBtn = new QPushButton("Load Syntax Rules");

    connect(loadSRBtn, &QPushButton::clicked, this, &ExtensionsDialog::loadSyntaxRules)

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 0, 5, 5);

    mainLayout->addWidget(titleBar);
    mainLayout->addWidget(labelLoadBtn);
    mainLayout->addWidget(loadBtn);
    mainLayout->addStretch();

    setLayout(mainLayout);
}

bool ExtensionsDialog::verifFileAccordance(QString path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Error", "File not found !");
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject json = doc.object();

    if (!json.contains("extension") || !json.contains("rules")) {
        QMessageBox::critical(this, "Error", "Missing 'extension' or 'rules'.");
        return false;
    }

    if (!json["extension"].isString() || !json["rules"].isArray()) {
        QMessageBox::critical(this, "Error", "Invalid data types.");
        return false;
    }

    if (json["extension"].toString().isEmpty() || json["rules"].toArray().isEmpty()) {
        QMessageBox::critical(this, "Error", "Fields cannot be empty.");
        return false;
    }

    for (const QString &key : json.keys()) {
        if (key != "extension" && key != "rules") {
            QMessageBox::critical(this, "Error", QString("Unknown parameter: %1").arg(key));
            return false;
        }
    }

    QJsonArray rulesArray = json["rules"].toArray();
    for (int i = 0; i < rulesArray.size(); ++i) {
        QJsonObject rule = rulesArray[i].toObject();

        if (!rule.contains("pattern") || !rule.contains("color")) {
            QMessageBox::critical(this, "Error", QString("Rule %1 is missing parameters.").arg(i));
            return false;
        }

        if (!rule["pattern"].isString() || !rule["color"].isString()) {
            QMessageBox::critical(this, "Error", QString("Rule %1 has invalid types.").arg(i));
            return false;
        }

        QRegularExpression re(rule["pattern"].toString());
        if (!re.isValid()) {
            QMessageBox::critical(this, "Error", QString("Regex error in rule %1: %2").arg(i).arg(re.errorString()));
            return false;
        }

        QString hexColor = rule["color"].toString();
        if (!QColor::isValidColor(hexColor)) {
            QMessageBox::critical(this, "File not in accordance", QString("The color '%1' is not a valid hexadecimal format (ex: #RRGGBB).").arg(hexColor));
            return false;
        }
    }
    
    file.close();

    return true;
} 

void ExtensionsDialog::loadSyntaxRules()
{
    QString filters = "Syntax rules file (*.strl)"

    QString filePath = QFileDialog::getOpenFileName(this, "Open a file", QDir::homePath(), QFileDialog::DontResolveSymlinks);

    if (filePath.isEmpty()) return;

    QFileInfo file(filePath);
    
    if (!verifFileAccordance(filePath)) return;

    QString syntaxPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/syntax/";

    QDir dir(syntaxPath);
    if (!dir.exists()) dir.mkpath(".");

    QString destPath = QDir(syntaxPath).filePath(file.fileName());

    int i = 1;
    while (QFile::exists(destPath)) {
        destPath = QDir(destPath).filePath(QString("%1_copy%2.%3").arg(file.baseName()).arg(i++).arg(file.suffix()));
    }

    if (QFile::copy(filePath, destPath)) {
        statusBar()->showMessage("Loaded syntax rules file to: " + destPath, 2000);
    } else {
        QMessageBox::critical(this, "Error", "Could not copy syntax rules file !");
        return;
    }
}