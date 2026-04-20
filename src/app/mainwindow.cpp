#include <QtWidgets>

#include "mainwindow.h"
#include "filemodel.h"
#include "settingsdialog.h"
#include "themesdialog.h"

MainWindow::MainWindow()
{   
    createTextEditor();
    createActions();
    createMenus();

    QString message = "A context menu is available by right-clicking";
    statusBar()->showMessage(message);

    setWindowTitle("Platrix");
    setMinimumSize(160, 160);
    resize(800, 600);
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

void MainWindow::openSettings()
{
    SettingsDialog dialog;

    dialog.exec();
}

void MainWindow::openThemes()
{
    ThemesDialog dialog;

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
    explorer->setRootIndex(model->index(dirPath));

    QSettings settings;
    settings.setValue("rootPath", dirPath);

    statusBar()->showMessage("Folder opened: " + dirPath, 2000);
}

void MainWindow::save()
{
    int currentIndex = tabs->currentIndex();
    if (currentIndex == -1) return;

    QString filePath = openedFiles.key(currentIndex);

    QTextEdit *currentEditor = qobject_cast<QTextEdit*>(tabs->currentWidget());

    if (currentEditor && !filePath.isEmpty()) {
        QFile file(filePath);

        if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            QTextStream out(&file);

            out << currentEditor->toPlainText();

            statusBar()->showMessage("Saved: " + filePath, 2000);

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

void MainWindow::createActions()
{
    openSettingsAct = new QAction("Settings", this);
    openSettingsAct->setStatusTip("Open settings dialog");

    connect(openSettingsAct, &QAction::triggered, this, &MainWindow::openSettings);


    openThemesAct = new QAction("Themes", this);
    openThemesAct->setStatusTip("Open settings dialog");

    connect(openThemesAct, &QAction::triggered, this, &MainWindow::openThemes);
    

    newAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentNew), "New File", this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip("Create a new file");

    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);


    newFAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::FolderNew), "New Folder", this);
    newFAct->setStatusTip("Create a new folder");

    connect(newFAct, &QAction::triggered, this, &MainWindow::newFolder);


    openAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::FolderOpen), "Open Folder...", this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip("Open an existing folder");

    connect(openAct, &QAction::triggered, this, &MainWindow::open);


    saveAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentSave), "Save", this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip("Save the file to disk");

    connect(saveAct, &QAction::triggered, this, &MainWindow::save);


    deleteAct = new QAction("Delete", this);
    deleteAct->setShortcut(Qt::Key_Delete);
    deleteAct->setStatusTip("Delete a file or a folder");

    connect(deleteAct, &QAction::triggered, this, &MainWindow::deleteF);


    renameAct = new QAction("Rename", this);
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


    cutAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::EditCut), "Cut", this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip("Cut the current selection's contents to the clipboard");

    connect(cutAct, &QAction::triggered, this, &MainWindow::cut);


    copyAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::EditCopy), "Copy", this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip("Copy the current selection's contents to the clipboard");

    connect(copyAct, &QAction::triggered, this, &MainWindow::copy);


    pasteAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::EditPaste), "Paste", this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip("Paste the clipboard's contents into the current selection");

    connect(pasteAct, &QAction::triggered, this, &MainWindow::paste);


    aboutAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::HelpAbout), "About", this);
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
                QTextEdit *newEditor = new QTextEdit();
                newEditor->setPlainText(file.readAll());
                
                int newIndex = tabs->addTab(newEditor, info.fileName());
                tabs->setCurrentIndex(newIndex);

                openedFiles.insert(filePath, newIndex);

                statusBar()->showMessage("File loaded: " + filePath, 2000);

                file.close();
            }
        }
    });
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu("File");
    fileMenu->addAction(newAct);
    fileMenu->addAction(newFAct);
    fileMenu->addSeparator();
    fileMenu->addAction(openAct);
    fileMenu->addSeparator();
    fileMenu->addAction(saveAct);
    fileMenu->addSeparator();
    preferencesMenu = fileMenu->addMenu("Preferences");
    preferencesMenu->addAction(openSettingsAct);
    preferencesMenu->addAction(openThemesAct);
    
    editMenu = menuBar()->addMenu("Edit");
    editMenu->addAction(deleteAct);
    editMenu->addSeparator();
    editMenu->addAction(renameAct);
    editMenu->addSeparator();
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);

    helpMenu = menuBar()->addMenu("Help");
    helpMenu->addAction(aboutAct);
}

void MainWindow::createTextEditor()
{
    centralWidget = new QWidget;
    setCentralWidget(centralWidget);

    QSettings settings;
    QString rootPath;
    if (settings.value("rootPath").toString() == "") {
        rootPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/defaultFolderSystem/";
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
    header->setStyleSheet("background-color: #470242; color: white; padding: 2px;");
    header->setFixedHeight(25);

    QModelIndex rootIndex = model->index(targetPath);

    explorer = new QTreeView();
    explorer->setModel(model);
    explorer->setRootIndex(rootIndex);
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
    
    layout = new QHBoxLayout;
    layout->setContentsMargins(5, 5, 5, 5);
    layout->addWidget(splitter);

    centralWidget->setLayout(layout);
}