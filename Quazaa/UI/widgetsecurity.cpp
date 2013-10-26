﻿/*
** $Id$
**
** Copyright © Quazaa Development Team, 2009-2013.
** This file is part of QUAZAA (quazaa.sourceforge.net)
**
** Quazaa is free software; this file may be used under the terms of the GNU
** General Public License version 3.0 or later as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Quazaa is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** Please review the following information to ensure the GNU General Public
** License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** You should have received a copy of the GNU General Public License version
** 3.0 along with Quazaa; if not, write to the Free Software Foundation,
** Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "widgetsecurity.h"
#include "ui_widgetsecurity.h"

#include "securitytablemodel.h"
#include "dialogaddrule.h"
#include "dialogsecuritysubscriptions.h"
#include "dialogimportsecurity.h"

#include "quazaasettings.h"
#include "skinsettings.h"

#include "timedsignalqueue.h"

#include "debug_new.h"

#include <QMenu>
#include <QKeyEvent>

CWidgetSecurity::CWidgetSecurity(QWidget* parent) :
	QMainWindow( parent ),
	ui( new Ui::CWidgetSecurity )
{
	ui->setupUi( this );

	m_pSecurityMenu = new QMenu( this );
	m_pSecurityMenu->addAction(  ui->actionSecurityModifyRule  );
	m_pSecurityMenu->addAction(  ui->actionSecurityRemoveRule  );
	m_pSecurityMenu->addAction(  ui->actionSecurityExportRules );

	restoreState( quazaaSettings.WinMain.SecurityToolbars );

	tableViewSecurity = new CTableView();
	tableViewSecurity->verticalHeader()->setVisible( false );
	ui->verticalLayoutManual->addWidget(tableViewSecurity);

	connect(tableViewSecurity, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(tableViewSecurity_customContextMenuRequested(QPoint)));
	connect(tableViewSecurity, SIGNAL(clicked(QModelIndex)), this, SLOT(tableViewSecurity_clicked(QModelIndex)));
	connect(tableViewSecurity, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(tableViewSecurity_doubleClicked(QModelIndex)));

	tableViewSecurityAuto = new CTableView();
	tableViewSecurityAuto->verticalHeader()->setVisible( false );
	ui->verticalLayoutAuto->addWidget(tableViewSecurityAuto);

	connect(tableViewSecurityAuto, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(tableViewSecurityAuto_customContextMenuRequested(QPoint)));
	connect(tableViewSecurityAuto, SIGNAL(clicked(QModelIndex)), this, SLOT(tableViewSecurity_clicked(QModelIndex)));
	connect(tableViewSecurityAuto, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(tableViewSecurity_doubleClicked(QModelIndex)));

	m_lSecurity = new CSecurityTableModel( this );

	m_lManual = new SecurityFilterModel(m_lSecurity, false);
	m_lAutomatic = new SecurityFilterModel(m_lSecurity, true);

	tableViewSecurity->setModel( m_lManual );
	tableViewSecurityAuto->setModel( m_lAutomatic );
	m_lManual->sort( tableViewSecurity->horizontalHeader()->sortIndicatorSection(),
						   tableViewSecurity->horizontalHeader()->sortIndicatorOrder()    );
	m_lAutomatic->sort( tableViewSecurityAuto->horizontalHeader()->sortIndicatorSection(),
						   tableViewSecurityAuto->horizontalHeader()->sortIndicatorOrder()    );
	setSkin();
}

CWidgetSecurity::~CWidgetSecurity()
{
	delete ui; // Note: This does also take care of m_pSecurityMenu and m_pSecurityList.
}

void CWidgetSecurity::saveWidget()
{
	quazaaSettings.WinMain.SecurityToolbars = saveState();
}

void CWidgetSecurity::changeEvent(QEvent* e)
{
	switch ( e->type() )
	{
	case QEvent::LanguageChange:
	{
		ui->retranslateUi( this );
		break;
	}

	default:
		break;
	}

	QMainWindow::changeEvent( e );
}

void CWidgetSecurity::keyPressEvent(QKeyEvent *e)
{
	switch ( e->key() )
	{
	case Qt::Key_Delete:
	{
		on_actionSecurityRemoveRule_triggered();
		break;
	}

	case Qt::Key_Return:
	{
		on_actionSecurityModifyRule_triggered();
		break;
	}

	case Qt::Key_F5:
	{
		m_lSecurity->completeRefresh();
		break;
	}
	}

	QMainWindow::keyPressEvent( e );
}

void CWidgetSecurity::update()
{
	m_lSecurity->updateAll();
}

void CWidgetSecurity::on_actionSecurityAddRule_triggered()
{
	CDialogAddRule* dlgAddRule = new CDialogAddRule( this );
	connect(dlgAddRule, SIGNAL(accepted()), SLOT(update()));
	dlgAddRule->show();
}

void CWidgetSecurity::on_actionSecurityRemoveRule_triggered()
{
	if (ui->tabWidgetSecurity->currentIndex() == 1) {
		QModelIndexList selection = tableViewSecurityAuto->selectionModel()->selectedRows();

		foreach( QModelIndex i, selection )
		{
			if ( i.isValid() )
			{
				CSecureRule* pRule = m_lSecurity->ruleFromIndex( m_lAutomatic->mapToSource(i) );
				securityManager.remove( pRule );
			}
		}
	} else {
		QModelIndexList selection = tableViewSecurity->selectionModel()->selectedRows();

		foreach( QModelIndex i, selection )
		{
			if ( i.isValid() )
			{
				CSecureRule* pRule = m_lSecurity->ruleFromIndex( m_lManual->mapToSource(i) );
				securityManager.remove( pRule );
			}
		}
	}
}

void CWidgetSecurity::on_actionSecurityModifyRule_triggered()
{
	if (ui->tabWidgetSecurity->currentIndex() == 1) {
		QModelIndexList selection = tableViewSecurityAuto->selectionModel()->selectedRows();
		QModelIndex index = QModelIndex();

		// Get the highest selected row.
		foreach( QModelIndex i, selection )
		{
			if ( index.isValid() )
			{
				if ( index.row() > i.row() )
					index = i;
			}
			else
			{
				index = i;
			}
		}

		if ( index.isValid() )
		{
			CSecureRule* pRule = m_lSecurity->ruleFromIndex( m_lAutomatic->mapToSource(index) );
			CDialogAddRule* dlgAddRule = new CDialogAddRule( this, pRule );
			connect(dlgAddRule, SIGNAL(accepted()), SLOT(update()));

			dlgAddRule->show();
		}
	} else {
		QModelIndexList selection = tableViewSecurity->selectionModel()->selectedRows();
		QModelIndex index = QModelIndex();

		// Get the highest selected row.
		foreach( QModelIndex i, selection )
		{
			if ( index.isValid() )
			{
				if ( index.row() > i.row() )
					index = i;
			}
			else
			{
				index = i;
			}
		}

		if ( index.isValid() )
		{
			CSecureRule* pRule = m_lSecurity->ruleFromIndex( m_lManual->mapToSource(index) );
			CDialogAddRule* dlgAddRule = new CDialogAddRule( this, pRule );
			connect(dlgAddRule, SIGNAL(accepted()), SLOT(update()));

			dlgAddRule->show();
		}
	}
}

void CWidgetSecurity::on_actionSecurityImportRules_triggered()
{
	CDialogImportSecurity* dlgImportSecurity = new CDialogImportSecurity(this);
	dlgImportSecurity->exec();
}

void CWidgetSecurity::on_actionSecurityExportRules_triggered()
{
	// TODO: Implement.
}

void CWidgetSecurity::on_actionSubscribeSecurityList_triggered()
{
	CDialogSecuritySubscriptions* dlgSecuritySubscriptions = new CDialogSecuritySubscriptions( this );
	dlgSecuritySubscriptions->show();
}

void CWidgetSecurity::tableViewSecurity_doubleClicked(const QModelIndex& index)
{
	if ( index.isValid() )
	{
		on_actionSecurityModifyRule_triggered();
	}
	else
	{
		ui->actionSecurityExportRules->setEnabled( false );
		ui->actionSecurityModifyRule->setEnabled( false );
		ui->actionSecurityRemoveRule->setEnabled( false );
	}
}

void CWidgetSecurity::tableViewSecurity_clicked(const QModelIndex& index)
{
	if ( index.isValid() )
	{
		ui->actionSecurityExportRules->setEnabled( true );
		ui->actionSecurityModifyRule->setEnabled( true );
		ui->actionSecurityRemoveRule->setEnabled( true );
	}
	else
	{
		ui->actionSecurityExportRules->setEnabled( false );
		ui->actionSecurityModifyRule->setEnabled( false );
		ui->actionSecurityRemoveRule->setEnabled( false );
	}
}

void CWidgetSecurity::setSkin()
{
	tableViewSecurity->setStyleSheet( skinSettings.listViews );
	tableViewSecurityAuto->setStyleSheet( skinSettings.listViews );
}

void CWidgetSecurity::tableViewSecurity_customContextMenuRequested(const QPoint &pos)
{
	QModelIndex index = tableViewSecurity->indexAt( pos );

	if ( index.isValid() )
	{
		ui->actionSecurityExportRules->setEnabled( true );
		ui->actionSecurityModifyRule->setEnabled( true );
		ui->actionSecurityRemoveRule->setEnabled( true );
	}
	else
	{
		ui->actionSecurityExportRules->setEnabled( false );
		ui->actionSecurityModifyRule->setEnabled( false );
		ui->actionSecurityRemoveRule->setEnabled( false );
	}

	m_pSecurityMenu->popup( QCursor::pos() );
}

void CWidgetSecurity::tableViewSecurityAuto_customContextMenuRequested(const QPoint &pos)
{
	QModelIndex index = tableViewSecurityAuto->indexAt( pos );

	if ( index.isValid() )
	{
		ui->actionSecurityExportRules->setEnabled( true );
		ui->actionSecurityModifyRule->setEnabled( true );
		ui->actionSecurityRemoveRule->setEnabled( true );
	}
	else
	{
		ui->actionSecurityExportRules->setEnabled( false );
		ui->actionSecurityModifyRule->setEnabled( false );
		ui->actionSecurityRemoveRule->setEnabled( false );
	}

	m_pSecurityMenu->popup( QCursor::pos() );
}
