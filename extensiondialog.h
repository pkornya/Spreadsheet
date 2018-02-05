#ifndef EXTENSIONDIALOG_H
#define EXTENSIONDIALOG_H

#include <QDialog>
#include "ui_extensiondialog.h"

class ExtensionDialog : public QDialog, public Ui::ExtensionDialog
{
    Q_OBJECT

public:
    ExtensionDialog(QWidget *parent = 0);
    void setColumnRange(QChar first, QChar last);
};

#endif // EXTENSIONDIALOG_H
