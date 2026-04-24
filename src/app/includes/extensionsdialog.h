#include "framelessdialog.h"

class ExtensionsDialog : public FramelessDialog
{
    Q_OBJECT

public:
    explicit ExtensionsDialog(QWidget *parent = nullptr);

    bool verifFileAccordance(QString fileName);

private slots:
    void loadSyntaxRules();

private:
    QPushButton *loadBtn;

    QWidget *titleBar;
};