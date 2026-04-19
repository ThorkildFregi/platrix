#include <QtWidgets>

#include "mainwindow.h"

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

    menu.addAction(cutAct);
    menu.addAction(copyAct);
    menu.addAction(pasteAct);

    menu.exec(event->globalPos());
}
#endif

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

            statusBar()->showMessage("Saved : " + filePath, 2000);

            file.close();
        }
    }
}

void MainWindow::deleteF()
{
    QModelIndex currentIndex = explorer->currentIndex();
    if (!currentIndex.isValid()) return;
    
    QString path = model->filePath(currentIndex);
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

void MainWindow::rename()
{
    QModelIndex currentIndex = explorer->currentIndex();
    if (currentIndex.isValid()) {
        explorer->edit(currentIndex);
    }
}

void MainWindow::undo()
{
}

void MainWindow::redo()
{
}

void MainWindow::cut()
{
}

void MainWindow::copy()
{
}

void MainWindow::paste()
{

}

void MainWindow::about()
{
    QMessageBox::about(this, "About Menu", "The <b>Menu</b> example shows how to create menu-bar menus and context menus.");
}

void MainWindow::createActions()
{
    newAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentNew), "&New File", this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip("Create a new file");

    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);


    newFAct = new QAction("&New Folder", this);
    newFAct->setStatusTip("Create a new folder");

    connect(newFAct, &QAction::triggered, this, &MainWindow::newFolder);


    openAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen), "&Open...", this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip("Open an existing file");

    connect(openAct, &QAction::triggered, this, &MainWindow::open);


    saveAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentSave), "&Save", this);
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

            statusBar()->showMessage("Index updated : " + newName, 5000);
        }
    });


    undoAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::EditUndo), "&Undo", this);
    undoAct->setShortcuts(QKeySequence::Undo);
    undoAct->setStatusTip("Undo the last operation");

    connect(undoAct, &QAction::triggered, this, &MainWindow::undo);


    redoAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::EditRedo), "&Redo", this);
    redoAct->setShortcuts(QKeySequence::Redo);
    redoAct->setStatusTip("Redo the last operation");

    connect(redoAct, &QAction::triggered, this, &MainWindow::redo);


    cutAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::EditCut), "Cu&t", this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip("Cut the current selection's contents to the clipboard");

    connect(cutAct, &QAction::triggered, this, &MainWindow::cut);


    copyAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::EditCopy), "&Copy", this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip("Copy the current selection's contents to the clipboard");

    connect(copyAct, &QAction::triggered, this, &MainWindow::copy);


    pasteAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::EditPaste), "&Paste", this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip("Paste the clipboard's contents into the current selection");

    connect(pasteAct, &QAction::triggered, this, &MainWindow::paste);


    aboutAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::HelpAbout), "&About", this);
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

        QMenu *menu = new QMenu();

        menu->addAction(newAct);
        menu->addAction(newFAct);
        menu->addSeparator();

        if (index.isValid()) {
            menu->addAction(deleteAct);
            menu->addSeparator();
            menu->addAction(renameAct);
            menu->addSeparator();
            menu->addAction(cutAct);
            menu->addAction(copyAct);
            menu->addAction(pasteAct);
        } else {
            explorer->clearSelection();
            explorer->setCurrentIndex(QModelIndex());
        }

        menu->exec(explorer->viewport()->mapToGlobal(pos));
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

                statusBar()->showMessage("File loaded : " + filePath, 5000);

                file.close();
            }
        }
    });
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction(newAct);
    fileMenu->addAction(newFAct);
    fileMenu->addSeparator();
    fileMenu->addAction(openAct);
    fileMenu->addSeparator();
    fileMenu->addAction(saveAct);
    
    editMenu = menuBar()->addMenu("&Edit");
    editMenu->addAction(deleteAct);
    editMenu->addSeparator();
    editMenu->addAction(undoAct);
    editMenu->addAction(redoAct);
    editMenu->addSeparator();
    editMenu->addAction(renameAct);
    editMenu->addSeparator();
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);

    helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction(aboutAct);
}

void MainWindow::createTextEditor()
{
    centralWidget = new QWidget;
    setCentralWidget(centralWidget);

    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    QDir dir(appDataPath);

    if (!dir.exists()) {dir.mkpath(".");}

    QString targetPath = dir.absolutePath();

    model = new QFileSystemModel;
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