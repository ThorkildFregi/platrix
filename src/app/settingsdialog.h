#include <QDialog>
#include <QWidget>
#include <QScrollArea>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>

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
        fontSizeEdit = new QLineEdit;
        fontSizeEdit->setMaximumWidth(100);

        tabSizeLabel = new QLabel;
        tabSizeLabel->setText("Editor: Tab Size");
        tabSizeLabel->setMaximumHeight(20);
        tabSizeEdit = new QLineEdit;
        tabSizeEdit->setMaximumWidth(100);

        settingsLayout = new QVBoxLayout;
        settingsLayout->setAlignment(Qt::AlignTop);
        settingsLayout->addWidget(fontSizeLabel);
        settingsLayout->addWidget(fontSizeEdit);
        settingsLayout->addWidget(tabSizeLabel);
        settingsLayout->addWidget(tabSizeEdit);
        settingsLayout->addStretch(1);

        settingsWidget = new QWidget;
        settingsWidget->setLayout(settingsLayout);
        
        settingsScrollArea = new QScrollArea;
        settingsScrollArea->setWidget(settingsWidget);
        settingsScrollArea->setWidgetResizable(true);

        applyButton = new QPushButton("Apply");
        applyButton->setMaximumWidth(100);

        mainLayout = new QVBoxLayout;
        mainLayout->addWidget(settingsScrollArea, 1);
        mainLayout->addWidget(applyButton, 0, Qt::AlignLeft);

        setLayout(mainLayout);
    }

private:
    QLabel *fontSizeLabel;
    QLineEdit *fontSizeEdit;

    QLabel *tabSizeLabel;
    QLineEdit *tabSizeEdit;

    QPushButton *applyButton;

    QVBoxLayout *mainLayout;
    QVBoxLayout *settingsLayout;

    QWidget *settingsWidget;

    QScrollArea *settingsScrollArea;
};