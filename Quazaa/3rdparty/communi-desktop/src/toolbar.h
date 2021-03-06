/*
* Copyright (C) 2008-2013 The Communi Project
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*/

#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <QToolBar>

class ToolBar : public QToolBar
{
	Q_OBJECT

public:
	explicit ToolBar(QWidget* parent = 0);

signals:
	void settingsTriggered();
	void connectTriggered();
	void joinTriggered();

private slots:
	void showHelp();
	void applySettings();

private:
	struct Private {
		QAction* helpAction;
		QAction* settingsAction;
		QAction* connectAction;
		QAction* newViewAction;
	} d;
};

#endif // TOOLBAR_H
