#include "mainwindow.h"
#include "finddialog.h"
#include "gotocelldialog.h"
#include "spreadsheet.h"
#include "sortdialog.h"

#include <QLabel>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QCloseEvent>
#include <QStringList>
#include <QFileInfo>
#include <QTableWidgetSelectionRange>
#include <QSettings>
#include <QMutableListIterator>
#include <QDebug>

MainWindow::MainWindow()    //OK
{
    spreadsheet = new Spreadsheet;
    setCentralWidget(spreadsheet);

    createActions();
    createMenus();
    createContextMenu();
    createToolBars();
    createStatusBar();

    readSettings();

    findDialog = 0;

    setWindowIcon(QIcon(":/pictures/logo/logo.png"));
    setCurrentFile("");

    setAttribute(Qt::WA_DeleteOnClose);

    foreach (QWidget *win, QApplication::topLevelWidgets()) {
        if (MainWindow *mainWin = qobject_cast<MainWindow *>(win))
            mainWin->updateRecentFileActions(); 
    }
    // Write the same code for ShowGrid and Auto-Recalculate
}

void MainWindow::createActions()    // OK
{
    newAction = new QAction(tr("&New"), this);
    newAction->setIcon(QIcon(":/pictures/logo/new.png"));
    newAction->setShortcut(tr("Ctrl+N"));
    newAction->setStatusTip(tr("Create a new spreadsheet file"));
    connect(newAction, SIGNAL(triggered()),
            this, SLOT(newFile()));

    openAction = new QAction(tr("&Open"), this);
    openAction->setIcon(QIcon(":/pictures/logo/open.png"));
    openAction->setShortcut(tr("Ctrl+M"));
    openAction->setStatusTip(tr("Open a new spreadsheet file"));
    connect(openAction, SIGNAL(triggered()),
            this, SLOT(open()));

    saveAction = new QAction(tr("&Save"), this);
    saveAction->setIcon(QIcon(":/pictures/logo/save.png"));
    saveAction->setShortcut(tr("Ctrl+S"));
    saveAction->setStatusTip(tr("Save a new spreadsheet file"));
    connect(saveAction, SIGNAL(triggered()),
            this, SLOT(save()));

    saveAsAction = new QAction(tr("&SaveAs"), this);
    saveAsAction->setIcon(QIcon(":/pictures/logo/saveAs.png"));
    saveAsAction->setShortcut(tr("Ctrl+T"));
    saveAsAction->setStatusTip(tr("Save As a new spreadsheet file"));
    connect(saveAsAction, SIGNAL(triggered()),
            this, SLOT(saveAs()));

    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActions[i] = new QAction(this);
        recentFileActions[i]->setVisible(false);
        connect(recentFileActions[i], SIGNAL(triggered()),
                this, SLOT(openRecentFile()));
    }

    selectAllAction = new QAction(tr("&All"), this);
    selectAllAction->setShortcut(tr("Ctrl+A"));
    selectAllAction->setStatusTip(tr("Select all the cells in the spreadsheet"));
    connect(selectAllAction, SIGNAL(triggered()),
            spreadsheet, SLOT(selectAll()));

    showGridAction = new QAction(tr("&Show Grid"), this);
    showGridAction->setCheckable(true);
    showGridAction->setChecked(spreadsheet->showGrid());
    showGridAction->setStatusTip(tr("Show or hide the spreadsheet’s grid"));
    connect(showGridAction, SIGNAL(toggled(bool)),
            spreadsheet, SLOT(setShowGrid(bool)));

    aboutQtAction = new QAction(tr("About &Qt"), this);
    aboutQtAction->setStatusTip(tr("Show the Qt library’s About box"));
    connect(aboutQtAction, SIGNAL(triggered()),
            qApp, SLOT(aboutQt()));

    closeAction = new QAction(tr("&Close"), this);
    closeAction->setShortcut(tr("Ctrl+W"));
    closeAction->setStatusTip(tr("Close this window"));
    connect(closeAction, SIGNAL(triggered()),
            this, SLOT(close()));

    exitAction = new QAction(tr("&Exit"), this);
    exitAction->setIcon(QIcon(":/pictures/logo/exit.png"));
    exitAction->setShortcut(tr("Ctrl+Q"));
    exitAction->setStatusTip(tr("Exit the application"));
    connect(exitAction, SIGNAL(triggered()),
            qApp, SLOT(closeAllWindows()));

    cutAction = new QAction(tr("&Cut"), this);
    cutAction->setIcon(QIcon(":/pictures/logo/cut.png"));
    cutAction->setShortcut(tr("Ctrl+X"));
    connect(cutAction, SIGNAL(triggered(bool)),
            spreadsheet, SLOT(cut()));

    copyAction = new QAction(tr("&Copy"), this);
    copyAction->setIcon(QIcon(":/pictures/logo/copy.png"));
    copyAction->setShortcut(tr("Ctrl+C"));
    connect(copyAction, SIGNAL(triggered(bool)),
            spreadsheet, SLOT(copy()));

    pasteAction = new QAction(tr("&Paste"), this);
    pasteAction->setIcon(QIcon(":/pictures/logo/paste.png"));
    pasteAction->setShortcut(tr("Ctrl+V"));
    connect(pasteAction, SIGNAL(triggered(bool)),
            spreadsheet, SLOT(paste()));

    deleteAction = new QAction(tr("&Delete"), this);
    deleteAction->setIcon(QIcon(":/pictures/logo/delete.png"));
    deleteAction->setShortcut(tr("Delete"));
    connect(deleteAction, SIGNAL(triggered(bool)),
            spreadsheet, SLOT(del()));

    selectRowAction = new QAction(tr("&SelectRow"), this);
    connect(selectRowAction, SIGNAL(triggered(bool)),
            spreadsheet, SLOT(selectCurrentRow()));

    selectColumnAction = new QAction(tr("&SelectColumn"), this);
    connect(selectColumnAction, SIGNAL(triggered(bool)),
            spreadsheet, SLOT(selectCurrentColumn()));

    findAction = new QAction(tr("&Find"), this);
    findAction->setIcon(QIcon(":/pictures/logo/find.png"));
    findAction->setShortcut(tr("Ctrl+F"));
    findAction->setStatusTip(tr("Find"));
    connect(findAction, SIGNAL(triggered(bool)), this, SLOT(find()));

    goToCellAction = new QAction(tr("&GoToCell"), this);
    goToCellAction->setIcon(QIcon(":/pictures/logo/gotocell.png"));
    goToCellAction->setStatusTip(tr("Go To Cell"));
    connect(goToCellAction, SIGNAL(triggered(bool)),
            this, SLOT(goToCell()));

    recalculateAction = new QAction(tr("&Recalculate"), this);
    connect(recalculateAction, SIGNAL(triggered(bool)),
            spreadsheet, SLOT(recalculate()));

    sortAction = new QAction(tr("&Sort"), this);
    sortAction->setStatusTip(tr("Sort"));
    connect(sortAction, SIGNAL(triggered(bool)),
            this, SLOT(sort()));

    autoRecalcAction = new QAction(tr("&Auto-Recalculate"), this);
    //autoRecalcAction->setCheckable(true);
    connect(autoRecalcAction, SIGNAL(triggered(bool)),
            spreadsheet, SLOT(setAutoRecalculate(bool)));

    aboutAction = new QAction(tr("&About"), this);
    aboutAction->setStatusTip(tr("Brief information about program"));
    connect(aboutAction, SIGNAL(triggered(bool)),
            this, SLOT(about()));
}

void MainWindow::createMenus()  // OK
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);

    separatorAction = fileMenu->addSeparator();
    for (int i = 0; i < MaxRecentFiles; ++i)
        fileMenu->addAction(recentFileActions[i]);
    fileMenu->addSeparator();
    fileMenu->addAction(closeAction);
    fileMenu->addAction(exitAction);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(cutAction);
    editMenu->addAction(copyAction);
    editMenu->addAction(pasteAction);
    editMenu->addAction(deleteAction);

    selectSubMenu = editMenu->addMenu(tr("&Select"));
    selectSubMenu->addAction(selectRowAction);
    selectSubMenu->addAction(selectColumnAction);
    selectSubMenu->addAction(selectAllAction);

    editMenu->addSeparator();
    editMenu->addAction(findAction);
    editMenu->addAction(goToCellAction);

    toolsMenu = menuBar()->addMenu(tr("&Tools"));
    toolsMenu->addAction(recalculateAction);
    toolsMenu->addAction(sortAction);

    optionsMenu = menuBar()->addMenu(tr("&Options"));
    optionsMenu->addAction(showGridAction);
    optionsMenu->addAction(autoRecalcAction);

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAction);
    helpMenu->addAction(aboutQtAction);
}

void MainWindow::createContextMenu()    //OK
{
    spreadsheet->addAction(cutAction);
    spreadsheet->addAction(copyAction);
    spreadsheet->addAction(pasteAction);
    spreadsheet->setContextMenuPolicy(Qt::ActionsContextMenu);
}

void MainWindow::createToolBars()   //OK
{
    fileToolBar = addToolBar(tr("&File"));
    fileToolBar->addAction(newAction);
    fileToolBar->addAction(openAction);
    fileToolBar->addAction(saveAction);
    fileToolBar->setMovable(false);

    editToolBar = addToolBar(tr("&Edit"));
    editToolBar->addAction(cutAction);
    editToolBar->addAction(copyAction);
    editToolBar->addAction(pasteAction);
    editToolBar->addSeparator();
    editToolBar->addAction(findAction);
    editToolBar->addAction(goToCellAction);
    editToolBar->setMovable(false);
}

void MainWindow::createStatusBar()  // OK
{
    locationLabel = new QLabel(" W999 ");
    locationLabel->setAlignment(Qt::AlignHCenter);
    locationLabel->setMinimumSize(locationLabel->sizeHint());

    formulaLabel = new QLabel;
    formulaLabel->setIndent(1);

    statusBar()->addWidget(locationLabel);
    statusBar()->addWidget(formulaLabel, 1);

    connect(spreadsheet, SIGNAL(currentCellChanged(int, int, int, int)),
            this, SLOT(updateStatusBar()));
    connect(spreadsheet, SIGNAL(modified()),
            this, SLOT(spreadsheetModified()));

    updateStatusBar();
}

void MainWindow::updateStatusBar()  // OK
{
    locationLabel->setText(spreadsheet->currentLocation());
    formulaLabel->setText(spreadsheet->currentFormula());
}

void MainWindow::spreadsheetModified()  // OK
{
    setWindowModified(true);
    updateStatusBar();
}

void MainWindow::closeEvent(QCloseEvent *event) // OK
{
    if (okToContinue()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::newFile()  // OK
{
//    MainWindow *mainWin = new MainWindow;
//    mainWin->show();
    (new MainWindow())->show();
}

bool MainWindow::okToContinue() // OK
{
    if (isWindowModified()) {
        int r = QMessageBox::warning(this, tr("Spreadsheet"),
                tr("The document has been modified.\n"
                   "Do you want to save your changes?"),
                QMessageBox::Yes | QMessageBox::Default,
                QMessageBox::No,
                QMessageBox::Cancel | QMessageBox::Escape);
        if (r == QMessageBox::Yes) {
            return save();
        } else if (r == QMessageBox::Cancel) {
            return false;                             
        }
    }
    return true;
}

void MainWindow::open() // OK
{
    if(okToContinue()) {
        QString filter = tr("Spreadsheet files (*.sp)");
        QString fileName = QFileDialog::getOpenFileName(this,
                                   tr("Open Spreadsheet"), ".",
                                   filter);
        if (!fileName.isEmpty())
            loadFile(fileName);
    }
}

bool MainWindow::loadFile(const QString &fileName)  // OK
{
    if (!spreadsheet->readFile(fileName)) {
        statusBar()->showMessage(tr("Loading cancelled"), 2000);
        return false;
    }

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File loaded"), 2000);
    return true;
}

bool MainWindow::save() // OK
{
    if(curFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool MainWindow::saveFile(const QString &fileName)  // OK
{
    if (!spreadsheet->writeFile(fileName)) {
        statusBar()->showMessage(tr("Saving canceled"), 2000);
        return false;
    }

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

bool MainWindow::saveAs()   // OK
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                tr("Save Spreadsheet"), ".",
                                tr("Spreadsheet files (*.sp)"));
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

void MainWindow::setCurrentFile(const QString &fileName)    // OK
{
    curFile = fileName;
    setWindowModified(false);

    QString shownName = "Untitled";
    if (!curFile.isEmpty()) {
        shownName = strippedName(curFile);
        recentFiles.removeAll(curFile);
        recentFiles.prepend(curFile);
        updateRecentFileActions();
    }
    setWindowTitle(tr("%1[*] - %2").arg(shownName)
                                   .arg(tr("Spreadsheet")));
}

QString MainWindow::strippedName(const QString &fullFileName)   // OK
{
    return QFileInfo(fullFileName).fileName();
}

void MainWindow::updateRecentFileActions()  // OK
{
    QMutableListIterator<QString> i(recentFiles);
    while (i.hasNext()) {
        if (!QFile::exists(i.next()))
            i.remove();
    }

    for (int j = 0; j < MaxRecentFiles; ++j) {
        if (j < recentFiles.count()) {
            QString text = tr("%1")
                           .arg(strippedName(recentFiles[j]));
            recentFileActions[j]->setText(text);
            recentFileActions[j]->setData(recentFiles[j]);
            recentFileActions[j]->setVisible(true);

        } else {
            recentFileActions[j]->setVisible(false);
        }
        separatorAction->setVisible(!recentFiles.isEmpty());
    }
}

void MainWindow::find() //OK
{
    if (!findDialog) {
        findDialog = new FindDialog(this);
        connect(findDialog, SIGNAL(findNext(const QString&,
                                            Qt::CaseSensitivity)),
                spreadsheet, SLOT(findNext(const QString&,
                                           Qt::CaseSensitivity)));
        connect(findDialog, SIGNAL(findPrevious(const QString&,
                                            Qt::CaseSensitivity)),
                spreadsheet, SLOT(findPrevious(const QString&,
                                           Qt::CaseSensitivity)));
    }

    findDialog->show();
    //findDialog->raise();
    findDialog->activateWindow();
}

void MainWindow::goToCell() // OK
{
    GoToCellDialog dialog(this);
    if (dialog.exec()) {
        QString str = dialog.lineEdit->text().toUpper();
        spreadsheet->setCurrentCell(str.mid(1).toInt() - 1,
                                    str[0].unicode() - 'A');
    }
}

void MainWindow::sort()
{
    SortDialog dialog(this);
    QTableWidgetSelectionRange range = spreadsheet->selectedRange();
    dialog.setColumnRange('A' + range.leftColumn(),
                          'A' + range.rightColumn());
    if (dialog.exec()) {
        SpreadsheetCompare compare;
        compare.keys[0] =
            dialog.primaryColumnCombo->currentIndex();
        compare.keys[1] =
            dialog.secondaryColumnCombo->currentIndex() - 1;
        compare.keys[2] =
            dialog.tertiaryColumnCombo->currentIndex() - 1;
        compare.ascending[0] =
            (dialog.primaryOrderCombo->currentIndex() == 0);
        compare.ascending[1] =
            (dialog.secondaryOrderCombo->currentIndex() == 0);
        compare.ascending[2] =
            (dialog.tertiaryOrderCombo->currentIndex() == 0);
        spreadsheet->sort(compare);
    }
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Spreadsheet"),
            tr("<h2>Spreadsheet 1.1</h2>"
            "<p>Copyright &copy; 2017 Software Inc."
            "<p>Spreadsheet is a small application that "
            "demonstrates QAction, QMainWindow, QMenuBar, "
            "QStatusBar, QTableWidget, QToolBar, and many other "
            "Qt classes."));
}

void MainWindow::openRecentFile()
{
    if (okToContinue()) {
        QAction *action = qobject_cast<QAction *>(sender());
        if(action)
            loadFile(action->data().toString());
    }
}

void MainWindow::writeSettings()
{
    QSettings settings("Software Inc.", "Spreadsheet");

    settings.setValue("geometry", geometry());
    settings.setValue("recentFiles", recentFiles);
    settings.setValue("showGrid", showGridAction->isChecked());
    settings.setValue("autoRecalc", autoRecalcAction->isChecked());
}

void MainWindow::readSettings()
{
    QSettings settings("Software Inc.", "Spreadsheet");

    QRect rect = settings.value("geometry",
                                QRect(200, 200, 400, 400)).toRect();
    move(rect.topLeft());
    resize(rect.size());

    recentFiles = settings.value("recentFiles").toStringList();
    updateRecentFileActions();

    bool showGrid = settings.value("showGrid", true).toBool();
    showGridAction->setChecked(showGrid);

    bool autoRecalc = settings.value("autoRecalc", true).toBool();
    autoRecalcAction->setChecked(autoRecalc);
}
