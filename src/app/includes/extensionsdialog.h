#include "framelessdialog.h"

class ExtensionsDialog : public FramelessDialog
{
    Q_OBJECT

public:
    explicit ExtensionsDialog(QWidget *parent = nullptr);

private slots:
    void loadExtension(QString path);

private:
    QPushButton *loadBtn;

    QWidget *titleBar;
};