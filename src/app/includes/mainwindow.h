#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMap>
#include <QString>
#include <functional>

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QAction;

class FileModel;

class QMenu;
class QWidget;
class QTreeView;
class QTabWidget;

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

    void applyTheme(const QVariant &value);

protected:
#ifndef QT_NO_CONTEXTMENU
    void contextMenuEvent(QContextMenuEvent *event) override;
#endif
    bool eventFilter(QObject *obj, QEvent *event) override;
    bool copyRecursively(const QString &srcPath, const QString &dstPath);

private slots:
    void onSettingChanged(const QString &key, const QVariant &value);
    
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
    
    void updateFontSize(const QVariant &value);

private:
    void createActions();
    void createMenus();
    void createTextEditor();
    
    void setupSettingHandlers();
    void applyAllSettings();

    QMap<QString, std::function<void(QVariant)>> settingHandlers;

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