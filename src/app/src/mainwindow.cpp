#include <QtWidgets>

#include "mainwindow.h"

#include "filemodel.h"
#include "codeeditor.h"

#include "extensionsdialog.h"
#include "settingsdialog.h"
#include "settingsmanager.h"

MainWindow::MainWindow()
{   
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);

    createTextEditor();
    createActions();
    createMenus();

    fileSystemWatcher = new QFileSystemWatcher();
    fileSystemWatcher->addPath(SettingsManager::getSettingsPath());

    connect(fileSystemWatcher, &QFileSystemWatcher::fileChanged, this, [this](const QString &path) {
        this->applyAllSettings();
        fileSystemWatcher->addPath(path);
    });
    connect(&SettingsManager::instance(), &SettingsManager::settingChanged, this, &MainWindow::onSettingChanged);

    setupSettingHandlers();
    applyAllSettings();

    QString message = "A context menu is available by right-clicking";
    statusBar()->showMessage(message);

    setWindowTitle("Platrix");
    setMinimumSize(800, 600);
}

#ifndef QT_NO_CONTEXTMENU
void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);

    menu.addAction(newAct);
    menu.addAction(newFAct);
    menu.addSeparator();

    menu.exec(event->globalPos());
}
#endif

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == explorer->viewport() && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);

        QModelIndex index = explorer->indexAt(mouseEvent->pos());

        if (!index.isValid()) {
            explorer->clearSelection();
            explorer->setCurrentIndex(QModelIndex());
        }
    }
    
    if (event->type() == QEvent::MouseMove) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        this->mouseMoveEvent(mouseEvent);
    }

    return QMainWindow::eventFilter(obj, event);
}

bool MainWindow::copyRecursively(const QString &srcPath, const QString &dstPath)
{
    QDir srcDir(srcPath);
    if (!srcDir.exists()) return false;

    if (!QDir().mkpath(dstPath)) return false;

    foreach (QString fileName, srcDir.entryList(QDir::Files)) {
        QFile::copy(srcDir.filePath(fileName), QDir(dstPath).filePath(fileName));
    }

    foreach (QString dirName, srcDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        copyRecursively(srcDir.filePath(dirName), QDir(dstPath).filePath(dirName));
    }

    return true;
}

void MainWindow::mousePressEvent(QMouseEvent *event) 
{
    if (event->button() == Qt::LeftButton) {
        Qt::Edges edges = getMouseEdges(event->pos());
        
        if (edges != Qt::Edges()) {
            windowHandle()->startSystemResize(edges);
            event->accept();
        }
        else if (auto window = windowHandle()) {
            window->startSystemMove();
            event->accept();
        }
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    Qt::Edges edges = getMouseEdges(event->pos());

    if ((edges & Qt::LeftEdge && edges & Qt::TopEdge) || (edges & Qt::RightEdge && edges & Qt::BottomEdge))
        setCursor(Qt::SizeFDiagCursor);
    else if ((edges & Qt::RightEdge && edges & Qt::TopEdge) || (edges & Qt::LeftEdge && edges & Qt::BottomEdge))
        setCursor(Qt::SizeBDiagCursor);
    else if (edges & Qt::LeftEdge || edges & Qt::RightEdge)
        setCursor(Qt::SizeHorCursor);
    else if (edges & Qt::TopEdge || edges & Qt::BottomEdge)
        setCursor(Qt::SizeVerCursor);
    else
        setCursor(Qt::ArrowCursor);
}

Qt::Edges MainWindow::getMouseEdges(const QPoint &pos)
{
    Qt::Edges edges = Qt::Edges();
    int x = pos.x();
    int y = pos.y();
    int w = width();
    int h = height();

    if (x < borderWidth) edges |= Qt::LeftEdge;
    if (x > w - borderWidth) edges |= Qt::RightEdge;
    if (y < borderWidth) edges |= Qt::TopEdge;
    if (y > h - borderWidth) edges |= Qt::BottomEdge;

    return edges;
}

void MainWindow::onSettingChanged(const QString &key, const QVariant &value)
{
    if (settingHandlers.contains(key)) {
        settingHandlers[key](value);
    }
}

void MainWindow::openSettings()
{
    SettingsDialog dialog;

    if (dialog.exec() == QDialog::Accepted) {
        statusBar()->showMessage("Settings applied !", 2000);
    }
}

void MainWindow::openExtensions()
{
    ExtensionsDialog dialog;

    dialog.exec();
}

void MainWindow::newFile()
{
    QModelIndex currentIndex = explorer->currentIndex();
    QString parentPath;

    if (currentIndex.isValid()) {
        if (model->isDir(currentIndex)) {
            parentPath = model->filePath(currentIndex);
        } else {
            parentPath = model->fileInfo(currentIndex).absolutePath();
        }
    } else {
        parentPath = model->rootPath();
    }

    QDir dir(parentPath);
    QString fileName = "new_file";
    if (dir.exists(fileName)) {
        int i = 1;
        while (dir.exists(QString("new_folder_%1").arg(i))) {
            i++;
        }
        fileName = QString("new_folder_%1").arg(i);
    }

    QString filePath = dir.absoluteFilePath(fileName);
    QFile file(filePath);

    if (file.open(QIODevice::WriteOnly)) {
        file.close();

        QModelIndex newIndex = model->index(filePath);

        if (newIndex.isValid()) {
            explorer->scrollTo(newIndex);
            explorer->setCurrentIndex(newIndex);
            explorer->edit(newIndex);
        }
    }
}

void MainWindow::newFolder()
{
    QModelIndex currentIndex = explorer->currentIndex();
    QString parentPath;

    if (currentIndex.isValid()) {
        if (model->isDir(currentIndex)) {
            parentPath = model->filePath(currentIndex);
        } else {
            parentPath = model->fileInfo(currentIndex).absolutePath();
        }
    } else {
        parentPath = model->rootPath();
    }

    QDir dir(parentPath);
    QString folderName = "new_folder";
    if (dir.exists(folderName)) {
        int i = 1;
        while (dir.exists(QString("new_folder_%1").arg(i))) {
            i++;
        }
        folderName = QString("new_folder_%1").arg(i);
    }

    if (dir.mkdir(folderName)) {
        QString newFolderPath = dir.absoluteFilePath(folderName);
        QModelIndex newIndex = model->index(newFolderPath);

        if (newIndex.isValid()) {
            explorer->scrollTo(newIndex);
            explorer->setCurrentIndex(newIndex);
            explorer->edit(newIndex);
        }
    }
}

void MainWindow::open()
{
    QString dirPath = QFileDialog::getExistingDirectory(this, "Choose a folder", QDir::homePath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    model->setRootPath(dirPath);

    QModelIndex rootIndex = model->index(dirPath);
    explorer->setRootIndex(rootIndex.parent());

    QModelIndex parentIndex = rootIndex.parent();
    int rowCount = model->rowCount(parentIndex);

    for (int i = 0; i < rowCount; ++i) {
        QModelIndex sibling = model->index(i, 0, parentIndex);
        if (sibling != rootIndex) {
            explorer->setRowHidden(i, parentIndex, true);
        }
    }

    explorer->setRootIndex(model->index(dirPath).parent());

    QSettings settings;
    settings.setValue("rootPath", dirPath);

    statusBar()->showMessage("Folder opened: " + dirPath, 2000);
}

void MainWindow::save()
{
    int currentIndex = tabs->currentIndex();
    if (currentIndex == -1) return;

    QString filePath = openedFiles.key(currentIndex);

    CodeEditor *currentEditor = qobject_cast<CodeEditor*>(tabs->currentWidget());

    if (currentEditor && !filePath.isEmpty()) {
        QFile file(filePath);

        if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            QTextStream out(&file);

            out << currentEditor->toPlainText();

            statusBar()->showMessage("Saved: " + filePath, 2000);

            currentEditor->document()->setModified(false);
            
            tabs->setTabIcon(tabs->currentIndex(), QIcon());

            file.close();
        }
    }
}

void MainWindow::deleteF()
{
    QModelIndexList selected = explorer->selectionModel()->selectedRows();
    if (selected.isEmpty()) return;
    
    QStringList paths;
    for (const QModelIndex &index : selected) {
        paths << model->filePath(index);
    }

    for (const QString &path : paths) {
        QDir dir(path);
        QFile file(path);

        if (dir.exists()) {
            dir.removeRecursively();
        } 
        else if (file.exists()) {
            if (openedFiles.contains(path)){
                int key = openedFiles.value(path);

                tabs->removeTab(key);
            }

            file.remove();
        } 
        else {
            QMessageBox::critical(this, "Error", "File/Folder can't be found !");
        }
    }
}

void MainWindow::rename()
{
    QModelIndex currentIndex = explorer->currentIndex();
    if (currentIndex.isValid()) {
        explorer->edit(currentIndex);
    }
}

void MainWindow::cut()
{
    QString cutCachePath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/cache/cut/"; 
    
    QDir dir(cutCachePath);
    if (!dir.exists()) {dir.mkpath(".");}

    QModelIndexList selected = explorer->selectionModel()->selectedRows();
    if (selected.isEmpty()) return;
    
    foreach (QString fName, dir.entryList(QDir::Files)) {
        QFile file(QDir(cutCachePath).filePath(fName));
        file.remove();
    }

    QStringList paths;
    QList<bool> is_dirs;
    for (const QModelIndex &index : selected) {
        paths << model->filePath(index);
        is_dirs << model->isDir(index);
    }
    
    QStringList destPaths;
    for (const QString &path : paths) {
        QString fileName = QFileInfo(path).fileName();
        QString destPath = QDir(cutCachePath).filePath(fileName);

        if (openedFiles.contains(path)){
            int key = openedFiles.value(path);

            tabs->removeTab(key);
        }

        if (QFile::rename(path, destPath)) {
            destPaths << destPath;
            
        } else {
            QMessageBox::critical(this, "Error", "Could not cut file !");
        }
    }

    QSettings settings;
    settings.setValue("clipboard_paths", destPaths);
    settings.setValue("clipboard_is_dirs", QVariant::fromValue(is_dirs));

    statusBar()->showMessage("Cut", 2000);
}

void MainWindow::copy()
{
    QModelIndexList selected = explorer->selectionModel()->selectedRows();
    if (selected.isEmpty()) return;

    QStringList paths;
    QList<bool> is_dirs;
    for (const QModelIndex &index : selected) {
        paths << model->filePath(index);
        is_dirs << model->isDir(index);
    }
    
    QSettings settings;
    settings.setValue("clipboard_paths", paths);
    settings.setValue("clipboard_is_dirs", QVariant::fromValue(is_dirs));
    
    statusBar()->showMessage("Copied", 2000);
}

void MainWindow::paste()
{
    QSettings settings;
    QStringList sourcePaths = settings.value("clipboard_paths").toStringList();
    QList<bool> isDirs = settings.value("clipboard_is_dirs").value<QList<bool>>(); 

    if (sourcePaths.isEmpty()) return;

    QModelIndex currentIndex = explorer->currentIndex();
    QString destDir;

    if (currentIndex.isValid() && model->isDir(currentIndex)) {
        destDir = model->filePath(currentIndex);
    } else {
        destDir = model->rootPath();
    }

    for (int i = 0; i < sourcePaths.size(); i++) {
        QFileInfo sourceInfo(sourcePaths.at(i));
        QString destPath = QDir(destDir).filePath(sourceInfo.fileName());
        
        int j = 1;
        while (QFile::exists(destPath)) {
            destPath = QDir(destDir).filePath(QString("%1_copy%2.%3").arg(sourceInfo.baseName()).arg(j++).arg(sourceInfo.suffix()));
        }

        if (!isDirs.at(i)) {
            if (QFile::copy(sourcePaths.at(i), destPath)) {
                statusBar()->showMessage("Pasted: " + destPath, 2000);
            } else {
                QMessageBox::critical(this, "Error", "Could not paste file !");
            }
        } else {
            if (copyRecursively(sourcePaths.at(i), destPath)) {
                statusBar()->showMessage("Pasted folder: " + destPath, 2000);
            } else {
                QMessageBox::critical(this, "Error", "Could not paste folder !");
            }
        }
    }
}

void MainWindow::about()
{
    QMessageBox::about(this, "About Platrix", "<b>Platrix</b> is a programming langage specialized in math operations and statistics. \n <a href='https://github.com/ThorkildFregi/platrix/tree/main'>GitHub<a>");
}

void MainWindow::updateFontSize(const QVariant &value)
{
    int size = value.toInt();

    for (int i = 0; i < tabs->count(); i++) {
        CodeEditor *editor = qobject_cast<CodeEditor*>(tabs->widget(i));

        if (editor) {
            QFont f = editor->font();
            f.setPointSize(size);
            editor->setFont(f);
        }
    }
}

void MainWindow::applyTheme(const QVariant &value) {
    int isDark = value.toBool();

    QString filename = isDark ? ":/dark.qss" : ":/light.qss";
    QFile file(filename);
    
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream ts(&file);

        qApp->setStyleSheet(ts.readAll());

        file.close();
    } else {
        QMessageBox::critical(this, "Error", "Style can't be found !");
    }
}

void MainWindow::createActions()
{
    openSettingsAct = new QAction("S&ettings", this);
    openSettingsAct->setStatusTip("Open settings dialog");

    connect(openSettingsAct, &QAction::triggered, this, &MainWindow::openSettings);


    openExtensionsAct = new QAction("&Extensions", this);
    openExtensionsAct->setStatusTip("Open extensions dialog");

    connect(openExtensionsAct, &QAction::triggered, this, &MainWindow::openExtensions);


    newAct = new QAction("&New File", this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip("Create a new file");

    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);


    newFAct = new QAction("New &Folder", this);
    newFAct->setStatusTip("Create a new folder");

    connect(newFAct, &QAction::triggered, this, &MainWindow::newFolder);


    openAct = new QAction("&Open Folder...", this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip("Open an existing folder");

    connect(openAct, &QAction::triggered, this, &MainWindow::open);


    saveAct = new QAction("&Save", this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip("Save the file to disk");

    connect(saveAct, &QAction::triggered, this, &MainWindow::save);


    deleteAct = new QAction("&Delete", this);
    deleteAct->setShortcut(Qt::Key_Delete);
    deleteAct->setStatusTip("Delete a file or a folder");

    connect(deleteAct, &QAction::triggered, this, &MainWindow::deleteF);


    renameAct = new QAction("&Rename", this);
    renameAct->setShortcut(QKeySequence(Qt::Key_F2));
    renameAct->setStatusTip("Rename a file or a folder");

    connect(renameAct, &QAction::triggered, this, &MainWindow::rename);

    explorer->addAction(renameAct);


    connect(model, &QFileSystemModel::fileRenamed, this, [this](const QString &path, const QString &oldName, const QString &newName) {
        QString oldPath = QDir(path).filePath(oldName);
        QString newPath = QDir(path).filePath(newName);

        if (openedFiles.contains(oldPath)) {
            int key = openedFiles.value(oldPath);

            openedFiles.remove(oldPath);
            openedFiles.insert(newPath, key);

            tabs->setTabText(key, newName);

            statusBar()->showMessage("Index updated: " + newName, 2000);
        }
    });


    cutAct = new QAction("Cu&t", this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip("Cut the current selection's contents to the clipboard");

    connect(cutAct, &QAction::triggered, this, &MainWindow::cut);


    copyAct = new QAction("&Copy", this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip("Copy the current selection's contents to the clipboard");

    connect(copyAct, &QAction::triggered, this, &MainWindow::copy);


    pasteAct = new QAction("&Paste", this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip("Paste the clipboard's contents into the current selection");

    connect(pasteAct, &QAction::triggered, this, &MainWindow::paste);


    aboutAct = new QAction("&About", this);
    aboutAct->setStatusTip("Show the application's About box");

    connect(aboutAct, &QAction::triggered, this, &MainWindow::about);


    connect(tabs, &QTabWidget::tabCloseRequested, [this](int index) {
        QWidget *w = tabs->widget(index);

        QString path = openedFiles.key(index);
        openedFiles.remove(path);

        tabs->removeTab(index);
        delete w;
    });


    connect(explorer, &QTreeView::customContextMenuRequested, this, [this](const QPoint &pos) {
        QModelIndex index = explorer->indexAt(pos);

        QMenu menu(this);

        menu.addAction(newAct);
        menu.addAction(newFAct);
        menu.addSeparator();

        if (index.isValid()) {
            menu.addAction(deleteAct);
            menu.addSeparator();
            menu.addAction(renameAct);
            menu.addSeparator();
            menu.addAction(cutAct);
            menu.addAction(copyAct);
            menu.addAction(pasteAct);
        } else {
            explorer->clearSelection();
            explorer->setCurrentIndex(QModelIndex());
        }

        menu.exec(explorer->viewport()->mapToGlobal(pos));
    });
    

    connect(explorer, &QTreeView::doubleClicked, [this](const QModelIndex &index) {
        QString filePath = model->filePath(index);
        QFileInfo info(filePath);

        if (info.isFile()) {
            if (openedFiles.contains(filePath)) {
                tabs->setCurrentIndex(openedFiles.value(filePath));
                return;
            }

            QFile file(filePath);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                auto &manager = SettingsManager::instance();

                CodeEditor *newEditor = new CodeEditor(tabs);
                
                int newIndex = tabs->addTab(newEditor, info.fileName());
                tabs->setCurrentIndex(newIndex);

                newEditor->setPlainText(file.readAll());
                newEditor->document()->setModified(false);

                tabs->setTabIcon(tabs->currentIndex(), QIcon());

                openedFiles.insert(filePath, newIndex);

                statusBar()->showMessage("File loaded: " + filePath, 2000);

                file.close();
            }
        }
    });


    connect(model, &QFileSystemModel::directoryLoaded, this, [this](const QString &path) {
        QSettings settings;
        QString projectPath = settings.value("rootPath").toString();

        QModelIndex rootIndex = model->index(projectPath);
        QModelIndex parentIndex = rootIndex.parent();

        if (path == model->filePath(parentIndex)) {
            int rowCount = model->rowCount(parentIndex);
            for (int i = 0; i < rowCount; ++i) {
                QModelIndex sibling = model->index(i, 0, parentIndex);
                if (sibling != rootIndex) {
                    explorer->setRowHidden(i, parentIndex, true);
                }
            }
        }
    });
}

void MainWindow::createMenus()
{
    fileMenu = customMenuBar->addMenu("&File");
    fileMenu->addAction(newAct);
    fileMenu->addAction(newFAct);
    fileMenu->addSeparator();
    fileMenu->addAction(openAct);
    fileMenu->addSeparator();
    fileMenu->addAction(saveAct);
    fileMenu->addSeparator();
    preferencesMenu = fileMenu->addMenu("&Preferences");
    preferencesMenu->addAction(openSettingsAct);
    preferencesMenu->addAction(openExtensionsAct);
    
    editMenu = customMenuBar->addMenu("&Edit");
    editMenu->addAction(deleteAct);
    editMenu->addSeparator();
    editMenu->addAction(renameAct);
    editMenu->addSeparator();
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);

    helpMenu = customMenuBar->addMenu("&Help");
    helpMenu->addAction(aboutAct);
}

void MainWindow::createTextEditor()
{
    centralWidget = new QWidget;
    setCentralWidget(centralWidget);
    
    titleBar = new QWidget();
    titleBar->setObjectName("titleBar");
    titleBar->setFixedHeight(35);
    titleBar->setMouseTracking(true);
    this->setMouseTracking(true);

    QHBoxLayout *titleLayout = new QHBoxLayout(titleBar);
    titleLayout->setContentsMargins(5, 0, 0, 0);
    titleLayout->setSpacing(0);

    customMenuBar = new QMenuBar(titleBar); 
    customMenuBar->setNativeMenuBar(false);
    customMenuBar->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

    titleLayout->addWidget(customMenuBar);
    titleLayout->addStretch();

    QLabel *appName = new QLabel();
    appName->setText("Platrix");

    titleLayout->addWidget(appName);
    titleLayout->addStretch();

    QPushButton *btnMin = new QPushButton("-");
    QPushButton *btnMax = new QPushButton("□");
    QPushButton *btnClose = new QPushButton("✕");

    btnMin->setObjectName("btnMin");
    btnMax->setObjectName("btnMax");
    btnClose->setObjectName("btnClose");

    btnMin->setFixedSize(45, 35);
    btnMax->setFixedSize(45, 35);
    btnClose->setFixedSize(45, 35);

    titleLayout->addWidget(btnMin);
    titleLayout->addWidget(btnMax);
    titleLayout->addWidget(btnClose);

    connect(btnClose, &QPushButton::clicked, this, &MainWindow::close);
    connect(btnMin, &QPushButton::clicked, this, &MainWindow::showMinimized);
    connect(btnMax, &QPushButton::clicked, this, [this](){
        if (isMaximized()) showNormal();
        else showMaximized();
    });

    QSettings settings;
    QString rootPath;
    if (settings.value("rootPath").toString() == "") {
        rootPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/defaultFolderSystem/";
        settings.setValue("rootPath", rootPath);
    }
    else {
        rootPath = settings.value("rootPath").toString();
    }
    
    QDir dir(rootPath);

    if (!dir.exists()) {dir.mkpath(".");}

    QString targetPath = dir.absolutePath();

    model = new FileModel;
    model->setRootPath(targetPath);
    model->setReadOnly(false);

    header = new QLabel("Explorer", this);
    header->setFixedHeight(25);

    explorer = new QTreeView();
    explorer->setModel(model);

    QModelIndex rootIndex = model->index(targetPath);
    explorer->setRootIndex(rootIndex.parent());

    explorer->header()->hide();
    explorer->hideColumn(1);
    explorer->hideColumn(2);
    explorer->hideColumn(3);
    explorer->setContextMenuPolicy(Qt::CustomContextMenu);
    explorer->setEditTriggers(QAbstractItemView::NoEditTriggers);
    explorer->viewport()->installEventFilter(this);
    explorer->setDragEnabled(true);
    explorer->setAcceptDrops(true);
    explorer->setDropIndicatorShown(true);
    explorer->setDragDropMode(QAbstractItemView::InternalMove);
    explorer->setDefaultDropAction(Qt::MoveAction);
    explorer->setSelectionMode(QAbstractItemView::ExtendedSelection);
    explorer->clearSelection();
    explorer->setCurrentIndex(QModelIndex());

    fileSysLayout = new QVBoxLayout;
    fileSysLayout->addWidget(header);
    fileSysLayout->addWidget(explorer);

    leftWidget = new QWidget;
    leftWidget->setLayout(fileSysLayout);

    tabs = new QTabWidget();
    tabs->setTabsClosable(true);
    tabs->setMovable(true);

    splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(leftWidget);
    splitter->addWidget(tabs);
    splitter->setStretchFactor(1, 1);

    splitter->setSizes(QList<int>() << 200 << 800);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(5, 0, 5, 5);

    mainLayout->addWidget(titleBar);
    mainLayout->addWidget(splitter);
}

void MainWindow::setupSettingHandlers() 
{
    settingHandlers["fontSize"] = [this](QVariant v) { updateFontSize(v.toInt()); };
    settingHandlers["darkTheme"] = [this](QVariant v) { applyTheme(v.toBool()); };
}

void MainWindow::applyAllSettings()
{
    auto &manager = SettingsManager::instance();

    for (auto it = manager.settings.begin(); it != manager.settings.end(); ++it) {
        if (settingHandlers.contains(it.key())) {
            settingHandlers[it.key()](it.value());
        }
    }
}