#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QWidget;
class QFileSystemModel;
class QTreeView;
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

private slots:
    void newFile();
    void open();
    void save();
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void about();

private:
    void createActions();
    void createMenus();
    void createTextEditor();

    QWidget *centralWidget;
    QWidget *leftWidget;
    QWidget *rightWidget;

    QFileSystemModel *model;

    QLabel *header;
    QTreeView *explorer;

    QTabWidget *tabs;
    QMap<QString, int> openedFiles;

    QSplitter *splitter;

    QVBoxLayout *fileSysLayout;
    QHBoxLayout *layout;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *helpMenu;

    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *undoAct;
    QAction *redoAct;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;
    QAction *aboutAct;
};

#endif