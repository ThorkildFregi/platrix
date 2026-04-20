#include <QDialog>

QT_BEGIN_NAMESPACE
class QWidget;
QT_END_NAMESPACE

class ThemesDialog : public QDialog {
    Q_OBJECT
public:
    explicit ThemesDialog(QWidget *parent = nullptr) : QDialog(parent) {
        setWindowTitle("Themes");
        setMinimumSize(300, 200);




    }
};