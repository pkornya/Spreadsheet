#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QAction;
class QLabel;
class FindDialog;
class Spreadsheet;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void find();
    void goToCell();
    void sort();
    void about();
    void openRecentFile();
    void updateStatusBar();
    void spreadsheetModified();

private:
    void createActions();
    void createMenus();
    void createContextMenu();
    void createToolBars();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    bool okToContinue();
    bool loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    void updateRecentFileActions();
    QString strippedName(const QString &fullFileName);

private:
    Spreadsheet *spreadsheet;
    FindDialog  *findDialog;
    QLabel      *locationLabel;
    QLabel      *formulaLabel;
    QStringList recentFiles;
    QString     curFile;

    enum { MaxRecentFiles = 5 };

    QAction     *recentFileActions[MaxRecentFiles];
    QAction     *separatorAction;
    QMenu       *fileMenu;
    QMenu       *editMenu;

    QToolBar    *fileToolBar;
    QToolBar    *editToolBar;
    QAction     *newAction;
    QAction     *openAction;
    QAction     *saveAction;
    QAction     *saveAsAction;

    //Created
    QAction     *selectAllAction;
    QAction     *showGridAction;
    QAction     *exitAction;
    QAction     *cutAction;
    QAction     *copyAction;
    QAction     *pasteAction;
    QAction     *deleteAction;
    QAction     *findAction;
    QAction     *goToCellAction;
    QAction     *selectRowAction;
    QAction     *selectColumnAction;
    QAction     *recalculateAction;
    QAction     *sortAction;
    QAction     *autoRecalcAction;
    QAction     *aboutAction;
    QAction     *closeAction;

    QMenu       *selectSubMenu;
    QMenu       *toolsMenu;
    QMenu       *optionsMenu;
    QMenu       *helpMenu;

    QAction     *aboutQtAction;
};

#endif // MAINWINDOW_H
