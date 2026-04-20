#include <QDialog>

QT_BEGIN_NAMESPACE
class QWidget;
QT_END_NAMESPACE

class SettingsDialog : public QDialog {
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget *parent = nullptr) : QDialog(parent) {
        setWindowTitle("Setings");
        setMinimumSize(300, 200);




    }
};