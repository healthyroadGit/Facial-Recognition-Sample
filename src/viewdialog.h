#ifndef VIEWDIALOG_H
#define VIEWDIALOG_H

#include <QDialog>
#include "ui_viewdialog.h"
#include "HrDLib.h"

class ViewDialog : public QDialog
{
	Q_OBJECT

public:
	ViewDialog(HrDLib* hrd, void* imgx, QWidget *parent = 0);
	~ViewDialog();

private:
	Ui::ViewDialog ui;
	HrDLib* hrd;
	void* img;

	void updateList();

public slots:
	void on_btnRemove_clicked();
};

#endif // VIEWDIALOG_H
