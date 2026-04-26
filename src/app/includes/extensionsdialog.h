#include "framelessdialog.h"

class ExtensionsDialog : public FramelessDialog
{
    Q_OBJECT

public:
    explicit ExtensionsDialog(QWidget *parent = nullptr);

    static bool verifyFileAccordance(QString fileName);

private slots:
    void loadSyntaxRules();

private:
    QPushButton *loadSRBtn;

    QWidget *titleBar;
};