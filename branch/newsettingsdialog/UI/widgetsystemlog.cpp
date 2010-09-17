//
// widgetsystemlog.cpp
//
// Copyright © Quazaa Development Team, 2009-2010.
// This file is part of QUAZAA (quazaa.sourceforge.net)
//
// Quazaa is free software; you can redistribute it
// and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3 of
// the License, or (at your option) any later version.
//
// Quazaa is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Quazaa; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include "widgetsystemlog.h"
#include "ui_widgetsystemlog.h"

#include "quazaasettings.h"
#include "QSkinDialog/qskinsettings.h"

WidgetSystemLog::WidgetSystemLog(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::WidgetSystemLog)
{
	ui->setupUi(this);
	quazaaSettings.loadLogSettings();
	ui->actionPauseLogDisplay->setChecked(quazaaSettings.SystemLog.IsPaused);
	logMenu = new QMenu(ui->textEditSystemLog);
	logMenu->addAction(ui->actionShowInformation);
	ui->actionShowInformation->setChecked(quazaaSettings.SystemLog.ShowInformation);
	logMenu->addAction(ui->actionShowSecurity);
	ui->actionShowSecurity->setChecked(quazaaSettings.SystemLog.ShowSecurity);
	logMenu->addAction(ui->actionShowNotice);
	ui->actionShowNotice->setChecked(quazaaSettings.SystemLog.ShowNotice);
	logMenu->addAction(ui->actionShowDebug);
	ui->actionShowDebug->setChecked(quazaaSettings.SystemLog.ShowDebug);
	logMenu->addAction(ui->actionShowWarnings);
	ui->actionShowWarnings->setChecked(quazaaSettings.SystemLog.ShowWarnings);
	logMenu->addAction(ui->actionShowError);
	ui->actionShowError->setChecked(quazaaSettings.SystemLog.ShowError);
	logMenu->addAction(ui->actionShowCritical);
	ui->actionShowCritical->setChecked(quazaaSettings.SystemLog.ShowCritical);
	restoreState(quazaaSettings.WinMain.SystemLogToolbar);
	ui->actionToggleTimestamp->setChecked(quazaaSettings.Logging.LogShowTimestamp);
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	connect(&systemLog, SIGNAL(logPosted(QString,LogSeverity::Severity)), this, SLOT(appendLog(QString,LogSeverity::Severity)));
	skinChangeEvent();
}

WidgetSystemLog::~WidgetSystemLog()
{
	delete ui;
}

void WidgetSystemLog::changeEvent(QEvent *e)
{
	QMainWindow::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void WidgetSystemLog::skinChangeEvent()
{
	ui->toolBar->setStyleSheet(skinSettings.toolbars);
}

void WidgetSystemLog::appendLog(QString message, LogSeverity::Severity severity)
{
	if (!ui->actionPauseLogDisplay->isChecked())
	{
		if (ui->actionToggleTimestamp->isChecked())
		{
			timeStamp = QTime::currentTime();
			switch (severity)
			{
			case LogSeverity::Information:
				if (ui->actionShowInformation->isChecked())
					ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3: %4</span>").arg(skinSettings.logWeightInformation).arg(skinSettings.logColorInformation.name()).arg(timeStamp.toString("hh:mm:ss.zzz")).arg(message));
				break;
			case LogSeverity::Security:
				if (ui->actionShowSecurity->isChecked())
					ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3: %4</span>").arg(skinSettings.logWeightSecurity).arg(skinSettings.logColorSecurity.name()).arg(timeStamp.toString("hh:mm:ss.zzz")).arg(message));
				break;
			case LogSeverity::Notice:
				if (ui->actionShowNotice->isChecked())
					ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3: %4</span>").arg(skinSettings.logWeightNotice).arg(skinSettings.logColorNotice.name()).arg(timeStamp.toString("hh:mm:ss.zzz")).arg(message));
				break;
			case LogSeverity::Debug:
				if (ui->actionShowDebug->isChecked())
					ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3: %4</span>").arg(skinSettings.logWeightDebug).arg(skinSettings.logColorDebug.name()).arg(timeStamp.toString("hh:mm:ss.zzz")).arg(message));
				break;
			case LogSeverity::Warning:
				if (ui->actionShowWarnings->isChecked())
					ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3: %4</span>").arg(skinSettings.logWeightWarning).arg(skinSettings.logColorWarning.name()).arg(timeStamp.toString("hh:mm:ss.zzz")).arg(message));
				break;
			case LogSeverity::Error:
				if (ui->actionShowError->isChecked())
						ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3: %4</span>").arg(skinSettings.logWeightError).arg(skinSettings.logColorError.name()).arg(timeStamp.toString("hh:mm:ss.zzz")).arg(message));
				break;
			case LogSeverity::Critical:
				if (ui->actionShowCritical->isChecked())
					ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3: %4</span>").arg(skinSettings.logWeightCritical).arg(skinSettings.logColorCritical.name()).arg(timeStamp.toString("hh:mm:ss.zzz")).arg(message));
				break;
			}
		} else {
			switch (severity)
			{
			case LogSeverity::Information:
				if (ui->actionShowInformation->isChecked())
					ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3</span>").arg(skinSettings.logWeightInformation).arg(skinSettings.logColorInformation.name()).arg(message));
				break;
			case LogSeverity::Security:
				if (ui->actionShowSecurity->isChecked())
					ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3</span>").arg(skinSettings.logWeightSecurity).arg(skinSettings.logColorSecurity.name()).arg(message));
				break;
			case LogSeverity::Notice:
				if (ui->actionShowNotice->isChecked())
					ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3</span>").arg(skinSettings.logWeightNotice).arg(skinSettings.logColorNotice.name()).arg(message));
				break;
			case LogSeverity::Debug:
				if (ui->actionShowDebug->isChecked())
					ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3</span>").arg(skinSettings.logWeightDebug).arg(skinSettings.logColorDebug.name()).arg(message));
				break;
			case LogSeverity::Warning:
				if (ui->actionShowWarnings->isChecked())
					ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3</span>").arg(skinSettings.logWeightWarning).arg(skinSettings.logColorWarning.name()).arg(message));
				break;
			case LogSeverity::Error:
				if (ui->actionShowError->isChecked())
					ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3</span>").arg(skinSettings.logWeightError).arg(skinSettings.logColorError.name()).arg(message));
				break;
			case LogSeverity::Critical:
				if (ui->actionShowCritical->isChecked())
					ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3</span>").arg(skinSettings.logWeightCritical).arg(skinSettings.logColorCritical.name()).arg(message));
				break;
			}
		}
	}
}

void WidgetSystemLog::saveWidget()
{
	quazaaSettings.WinMain.SystemLogToolbar = saveState();
	quazaaSettings.Logging.LogShowTimestamp = ui->actionToggleTimestamp->isChecked();
	quazaaSettings.SystemLog.ShowWarnings = ui->actionShowWarnings->isChecked();
	quazaaSettings.SystemLog.ShowInformation = ui->actionShowInformation->isChecked();
	quazaaSettings.SystemLog.ShowSecurity = ui->actionShowSecurity->isChecked();
	quazaaSettings.SystemLog.ShowNotice = ui->actionShowNotice->isChecked();
	quazaaSettings.SystemLog.ShowDebug = ui->actionShowDebug->isChecked();
	quazaaSettings.SystemLog.ShowError = ui->actionShowError->isChecked();
	quazaaSettings.SystemLog.ShowCritical = ui->actionShowCritical->isChecked();
	quazaaSettings.SystemLog.IsPaused = ui->actionPauseLogDisplay->isChecked();
	quazaaSettings.saveLogSettings();
}

void WidgetSystemLog::on_actionClearBuffer_triggered()
{
	ui->textEditSystemLog->clear();
}

void WidgetSystemLog::on_textEditSystemLog_customContextMenuRequested(QPoint pos)
{
	Q_UNUSED(pos);
	logMenu->exec(QCursor::pos());
}
