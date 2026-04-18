#include <QtWidgets>

#include "mainwindow.h"

MainWindow::MainWindow()
{
    QWidget *widget = new QWidget;
    setCentralWidget(widget);

    QString path = QCoreApplication::applicationDirPath();

    QFileSystemModel *model = new QFileSystemModel;
    model->setRootPath(path);

    QLabel *customHeader = new QLabel("Explorer", this);
    customHeader->setStyleSheet("background-color: #470242; color: white; padding: 2px;");
    customHeader->setFixedHeight(25);

    QModelIndex rootIndex = model->index(path);

    QTreeView *tree = new QTreeView();
    tree->setModel(model);
    tree->setRootIndex(rootIndex);
    tree->header()->hide();
    tree->hideColumn(1);
    tree->hideColumn(2);
    tree->hideColumn(3);

    infoLabel = new QLabel("<i>Choose a menu option, or right-click to invoke a context menu !</i>");

    infoLabel->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    infoLabel->setAlignment(Qt::AlignCenter);

    QVBoxLayout *fileSysLayout = new QVBoxLayout;
    fileSysLayout->addWidget(customHeader);
    fileSysLayout->addWidget(tree);

    QWidget *leftSideWidget = new QWidget;
    leftSideWidget->setLayout(fileSysLayout);

    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(leftSideWidget);
    splitter->addWidget(infoLabel);

    splitter->setSizes(QList<int>() << 200 << 600);
    
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setContentsMargins(5, 5, 5, 5);
    layout->addWidget(splitter);

    widget->setLayout(layout);

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

// Slot actions
void MainWindow::newFile()
{
    infoLabel->setText("Invoked <b>File|New<b>");
}

void MainWindow::open()
{
    infoLabel->setText("Invoked <b>File|Open<b>");
}

void MainWindow::save()
{
    infoLabel->setText("Invoked <b>File|Save<b>");
}

void MainWindow::undo()
{
    infoLabel->setText("Invoked <b>Edit|Undo<b>");
}

void MainWindow::redo()
{
    infoLabel->setText("Invoked <b>Edit|Redo<b>");
}

void MainWindow::cut()
{
    infoLabel->setText("Invoked <b>Edit|Cut<b>");
}

void MainWindow::copy()
{
    infoLabel->setText("Invoked <b>Edit|Copy<b>");
}

void MainWindow::paste()
{
    infoLabel->setText("Invoked <b>Edit|Paste<b>");
}

void MainWindow::about()
{
    infoLabel->setText(tr("Invoked <b>Help|About</b>"));
    QMessageBox::about(this, "About Menu", "The <b>Menu</b> example shows how to create menu-bar menus and context menus.");
}

void MainWindow::createActions()
{
    newAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentNew), "&New", this);

    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip("Create a new file");

    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);


    openAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen), "&Open...", this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip("Open an existing file");

    connect(openAct, &QAction::triggered, this, &MainWindow::open);


    saveAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentSave), "&Save", this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip("Save the document to disk");

    connect(saveAct, &QAction::triggered, this, &MainWindow::save);


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
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    
    editMenu = menuBar()->addMenu("&Edit");
    editMenu->addAction(undoAct);
    editMenu->addAction(redoAct);
    editMenu->addSeparator();
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);

    helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction(aboutAct);


}