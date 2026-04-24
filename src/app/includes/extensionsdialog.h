#include "framelessdialog.h"

class ExtensionsDialog : public FramelessDialog
{
    Q_OBJECT

public:
    explicit ExtensionsDialog(QWidget *parent = nullptr);

private slots:
    void loadSyntaxRules();

private:
    QPushButton *loadBtn;

    QWidget *titleBar;
};