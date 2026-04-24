#include <QLabel>
#include <QWidget>
#include <QPushButton>

#include <QMessageShow>

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

void ExtensionsDialog::loadExtension()
{
    QString filters = "Syntax rules file (*.strl)"

    QString srName = QFileDialog::getOpenFileName(this, "Open a file", QDir::homePath(), QFileDialog::DontResolveSymlinks);

    if (sr.isEmpty()) return;
    
    QFileInfo file (srName);
    QString dirPath = file.absolutePath();

    QFile file(getSettingsPath());
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Error", "File not found !");
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject json = doc.object();

    if (!json.contains("extension") || !json.contains("rules")) {
        QMessageBox::critical(this, "Error", "Missing 'extension' or 'rules'.");
        return;
    }

    if (!json["extension"].isString() || !json["rules"].isArray()) {
        QMessageBox::critical(this, "Error", "Invalid data types.");
        return;
    }

    if (json["extension"].toString().isEmpty() || json["rules"].toArray().isEmpty()) {
        QMessageBox::critical(this, "Error", "Fields cannot be empty.");
        return;
    }

    for (const QString &key : json.keys()) {
        if (key != "extension" && key != "rules") {
            QMessageBox::critical(this, "Error", QString("Unknown parameter: %1").arg(key));
            return;
        }
    }

    QJsonArray rulesArray = json["rules"].toArray();
    for (int i = 0; i < rulesArray.size(); ++i) {
        QJsonObject rule = rulesArray[i].toObject();

        if (!rule.contains("pattern") || !rule.contains("color")) {
            QMessageBox::critical(this, "Error", QString("Rule %1 is missing parameters.").arg(i));
            return;
        }

        if (!rule["pattern"].isString() || !rule["color"].isString()) {
            QMessageBox::critical(this, "Error", QString("Rule %1 has invalid types.").arg(i));
            return;
        }

        QRegularExpression re(rule["pattern"].toString());
        if (!re.isValid()) {
            QMessageBox::critical(this, "Error", QString("Regex error in rule %1: %2").arg(i).arg(re.errorString()));
            return;
        }

        QString hexColor = rule["color"].toString();
        if (!QColor::isValidColor(hexColor)) {
            QMessageBox::critical(this, "File not in accordance", QString("The color '%1' is not a valid hexadecimal format (ex: #RRGGBB).").arg(hexColor));
            return;
        }
    }
    
    file.close();

    QString path = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    QString destPath = QDir(path).filePath(file.fileName());

    int i = 1;
    while (QFile::exists(destPath)) {
        destPath = QDir(destDir).filePath(QString("%1_copy%2.%3").arg(sourceInfo.baseName()).arg(i++).arg(sourceInfo.suffix()));
    }

    if (copyRecursively(dirPath, destPath)) {
        statusBar()->showMessage("Loaded syntax rules file to: " + destPath, 2000);
    } else {
        QMessageBox::critical(this, "Error", "Could not copy syntax rules file !");
        return;
    }
}