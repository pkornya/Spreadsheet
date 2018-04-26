#include "cell.h"
#include "spreadsheet.h"

#include <QFile>
#include <QDataStream>
#include <QIODevice>
#include <QMessageBox>
#include <QApplication>
#include <QClipboard>

Spreadsheet::Spreadsheet(QWidget *parent)   // OK
    : QTableWidget(parent)
{
    autoRecalc = true;

    setItemPrototype(new Cell);
    setSelectionMode(ContiguousSelection);

    connect(this, SIGNAL(itemChanged(QTableWidgetItem *)),
            this, SLOT(somethingChanged()));

    clear();
}

void Spreadsheet::clear()   //OK
{
    setRowCount(0);
    setColumnCount(0);
    setRowCount(RowCount);
    setColumnCount(ColumnCount);

    for (int i = 0; i < ColumnCount; ++i) {
        QTableWidgetItem *item = new QTableWidgetItem;
        item->setText(QString(QChar('A' + i)));
        setHorizontalHeaderItem(i, item);
    }

    setCurrentCell(0, 0);
}

Cell *Spreadsheet::getCell(int row, int column) const   // OK
{
    return static_cast<Cell *>(item(row, column));
}

QString Spreadsheet::text(int row, int column) const    // OK
{
    Cell *c = getCell(row, column);
    return c ? c->text() : "";
}

QString Spreadsheet::formula(int row, int column) const // OK
{
    Cell *c = getCell(row, column);
    return c ? c->formula() : "";
}

void Spreadsheet::setFormula(int row, int column, const QString &formula) // OK
{
    Cell *c = getCell(row, column);
    if (!c) {
        c = new Cell;
        setItem(row, column, c);
    }
    c->setFormula(formula);
}

QString Spreadsheet::currentLocation() const    // OK
{
    return QChar('A' + currentColumn())
            + QString::number(currentRow() + 1);
}

QString Spreadsheet::currentFormula() const // OK
{
    return formula(currentRow(), currentColumn());
}

void Spreadsheet::somethingChanged()    // OK
{
    if (autoRecalc)
        recalculate();
    emit modified();
}

bool Spreadsheet::writeFile(const QString &fileName)    // OK
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, tr("Spreadsheet"),
                             tr("Cannot write file %1:\n%2")
                             .arg(file.fileName())
                             .arg(file.errorString()));
        return false;
    }

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_8);

    QApplication::setOverrideCursor(Qt::WaitCursor);
    for (int row = 0; row < RowCount; ++row) {
        for (int column = 0; column < ColumnCount; ++column) {
            QString str = formula(row, column);
            if (!str.isEmpty())
                out << row << column << str;
        }
    }
    QApplication::restoreOverrideCursor();
    return true;
}

bool Spreadsheet::readFile(const QString &fileName) //OK
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("Spreadsheet"),
                             tr("Cannot read file %1:\n%2")
                             .arg(file.fileName())
                             .arg(file.errorString()));
        return false;
    }

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_5_8);

    clear();

    quint32 row;
    quint32 column;
    QString str;

    QApplication::setOverrideCursor(Qt::WaitCursor);
    while (!in.atEnd()) {
        in >> row >> column >> str;
        setFormula(row, column, str);
    }

    QApplication::restoreOverrideCursor();
    return true;
}

void Spreadsheet::cut() // OK
{
    copy();
    del();
}

void Spreadsheet::copy()    // OK
{
    QTableWidgetSelectionRange range = selectedRange();

    QString str;

    for (int i = 0; i < range.rowCount(); ++i) {
        if (i > 0)
            str += "\n";
        for (int j = 0; j < range.columnCount(); ++j) {
             if (j > 0)
                 str += "\t";
             str += formula(range.topRow() + i, range.leftColumn() + j);
        }
    }
    QApplication::clipboard()->setText(str);
}

QTableWidgetSelectionRange Spreadsheet::selectedRange() const   // OK
{
    QList<QTableWidgetSelectionRange> ranges = selectedRanges();
    if (ranges.isEmpty())
        return QTableWidgetSelectionRange();
    return ranges.first();
}

void Spreadsheet::paste()   // OK
{
    QTableWidgetSelectionRange range = selectedRange();
    QString str = QApplication::clipboard()->text();
    QStringList rows = str.split('\n');
    int numRows = rows.count();
    int numColumns = rows.first().count('\t') + 1;

    if (range.rowCount() * range.columnCount() != 1
            && (range.rowCount() != numRows
                || range.columnCount() != numColumns)) {
        QMessageBox::information(this, tr("Spreadsheet"),
            tr("The information cannot be pasted because the copy "
               "and paste areas aren’t the same size."));
        return;
    }

    for (int i = 0; i < numRows; ++i) {
        QStringList columns = rows[i].split('\t');
        for (int j = 0; j < numColumns; ++j) {
            int row = range.topRow() + i;
            int column = range.leftColumn() + j;
            if (row < RowCount && column < ColumnCount)
                setFormula(row, column, columns[j]);
        }
    }
    somethingChanged();
}

void Spreadsheet::del() // OK
{
    foreach (QTableWidgetItem *item, selectedItems()) {
       delete item;
    }
}

void Spreadsheet::selectCurrentRow()    // OK
{
    selectRow(currentRow());
}

void Spreadsheet::selectCurrentColumn() // OK
{
    selectColumn(currentColumn());
}

void Spreadsheet::findNext(const QString& str, Qt::CaseSensitivity cs) // OK
{
    int row = currentRow();
    int column = currentColumn() + 1;

    while (row < RowCount) {
        while (column < ColumnCount) {
            if (text(row, column).contains(str, cs)) {
                clearSelection();
                setCurrentCell(row, column);
                activateWindow();
                return;
             }
             ++column;
         }
         column = 0;
         ++row;
    }
    QMessageBox::warning(this, "Unsuccessful search",
                         "Could not find anything by your request");
}

void Spreadsheet::findPrevious(const QString &str, Qt::CaseSensitivity cs) // OK
{

    int row = currentRow();
    int column = currentColumn() - 1;

    while (row >= 0) {
        while (column >= 0) {
            if (text(row, column).contains(str, cs)) {
                clearSelection();
                setCurrentCell(row, column);
                activateWindow();
                return;
            }
            --column;
        }
        column = ColumnCount - 1;
        --row;
    }
    QMessageBox::warning(this, "Unsuccessful search",
                         "Could not find anything by your request");
}

void Spreadsheet::recalculate() // OK
{
    for (int row = 0; row < RowCount; ++row) {
        for (int column = 0; column < ColumnCount; ++column) {
            if (getCell(row, column))
                getCell(row, column)->setDirty();
        }
    }
    viewport()->update();
}

void Spreadsheet::setAutoRecalculate(bool recalc)   // OK
{
    autoRecalc = recalc;
    if (autoRecalc)
        recalculate();
}

void Spreadsheet::sort(const SpreadsheetCompare &compare)   // OK
{
    QList<QStringList> rows;
    QTableWidgetSelectionRange range = selectedRange();
    int i;

    for (i = 0; i < range.rowCount(); ++i) {
        QStringList row;
        for (int j = 0; j < range.columnCount(); ++j)
            row.append(formula(range.topRow() + i,
                               range.leftColumn() + j));
        rows.append(row);
    }

    std::sort(rows.begin(), rows.end(), compare);

    for (i = 0; i < range.rowCount(); ++i) {
        for (int j = 0; j < range.columnCount(); ++j)
            setFormula(range.topRow() + i, range.leftColumn() + j,
                       rows[i][j]);
    }

    clearSelection();
    somethingChanged();
}

bool SpreadsheetCompare::operator ()(const QStringList &row1,
                                     const QStringList &row2) const
{
    for (int i = 0; i < KeyCount; ++i) {
        int column = keys[i];
        if (column != -1) {
            if (row1[column] != row2[column]) {
                if (ascending[i]) {
                    return row1[column] < row2[column];
                } else {
                    return row1[column] > row2[column];
                }
            }
        }
    }
    return false;
}
