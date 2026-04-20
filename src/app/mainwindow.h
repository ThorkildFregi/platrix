#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QWidget;
class QTreeView;
class FileModel;
class QTabWidget;
class QString;
class QSplitter;
class QHBoxLayout;
class QVBoxLayout;
class QLabel;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

protected:
#ifndef QT_NO_CONTEXTMENU
    void contextMenuEvent(QContextMenuEvent *event) override;
#endif
    bool eventFilter(QObject *obj, QEvent *event) override;
    bool copyRecursively(const QString &srcPath, const QString &dstPath);

private slots:
    void openSettings();
    void newFile();
    void newFolder();
    void open();
    void save();
    void deleteF();
    void rename();
    void cut();
    void copy();
    void paste();
    void about();

private:
    void createActions();
    void createMenus();
    void createTextEditor();
    void loadSettings();

    QWidget *centralWidget;
    QWidget *leftWidget;
    QWidget *rightWidget;

    FileModel *model;

    QLabel *header;
    QTreeView *explorer;

    QTabWidget *tabs;
    QMap<QString, int> openedFiles;

    QSplitter *splitter;

    QVBoxLayout *fileSysLayout;
    QHBoxLayout *layout;

    QMenu *fileMenu;
    QMenu *preferencesMenu;
    QMenu *editMenu;
    QMenu *helpMenu;

    QAction *openSettingsAct;
    QAction *newAct;
    QAction *newFAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *deleteAct;
    QAction *renameAct;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;
    QAction *aboutAct;
};

#endif