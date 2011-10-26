/*
** $Id$
**
** Copyright © Quazaa Development Team, 2009-2011.
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


#include "widgetsearchtemplate.h"
#include "ui_widgetsearchtemplate.h"
#include "NetworkCore/searchmanager.h"
#include "NetworkCore/managedsearch.h"
#include "NetworkCore/query.h"
#include "NetworkCore/queryhit.h"
#include "quazaasettings.h"
#if defined(_MSC_VER) && defined(_DEBUG)
	#define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
	#define new DEBUG_NEW
#endif
WidgetSearchTemplate::WidgetSearchTemplate(QString searchString, QWidget* parent) :
	QWidget(parent),
	m_ui(new Ui::WidgetSearchTemplate)
{
	m_ui->setupUi(this);
	sSearchString = searchString;
	m_pSearch = 0;
	nFiles = 0;
	nHits = 0;
	nHubs = 0;
	nLeaves = 0;
	searchState = SearchState::Default;
	sortModel = new QSortFilterProxyModel(this);
	searchModel = new SearchTreeModel();
	sortModel->setSourceModel(searchModel);
	m_ui->treeViewSearchResults->setModel(sortModel);
	sortModel->setDynamicSortFilter(false);
	connect(searchModel, SIGNAL(sort()), this, SLOT(Sort()));
	connect(searchModel, SIGNAL(updateStats()), this, SLOT(OnStatsUpdated()));
	loadHeaderState();
	connect(m_ui->treeViewSearchResults->header(), SIGNAL(sectionMoved(int,int,int)), this, SLOT(saveHeaderState()));
	connect(m_ui->treeViewSearchResults->header(), SIGNAL(sectionResized(int,int,int)), this, SLOT(saveHeaderState()));
	connect(m_ui->treeViewSearchResults->header(), SIGNAL(sectionClicked(int)), this, SLOT(saveHeaderState()));
}
WidgetSearchTemplate::~WidgetSearchTemplate()
{
	if(m_pSearch != 0)
	{
		StopSearch();
	}
	delete m_ui;
}
void WidgetSearchTemplate::changeEvent(QEvent* e)
{
	QWidget::changeEvent(e);
	switch(e->type())
	{
		case QEvent::LanguageChange:
			m_ui->retranslateUi(this);
			break;
		default:
			break;
	}
}
void WidgetSearchTemplate::StartSearch(CQuery* pQuery)
{
	if(m_pSearch && m_pSearch->m_pQuery != pQuery)
	{
		delete m_pSearch;
		m_pSearch = 0;
	}
	if(!m_pSearch)
	{
		m_pSearch = new CManagedSearch(pQuery);
		connect(m_pSearch, SIGNAL(OnHit(QueryHitSharedPtr)), searchModel, SLOT(addQueryHit(QueryHitSharedPtr)));
		connect(m_pSearch, SIGNAL(StatsUpdated()), this, SLOT(OnStatsUpdated()));
		connect(m_pSearch, SIGNAL(StateChanged()), this, SLOT(OnStateChanged()));
	}
	searchState = SearchState::Searching;
	m_pSearch->Start();
	sSearchString = m_pSearch->m_pQuery->DescriptiveName();
}
void WidgetSearchTemplate::StopSearch()
{
	Q_ASSERT(m_pSearch != 0);
	searchState = SearchState::Stopped;
	m_pSearch->Stop();
	delete m_pSearch;
	m_pSearch = 0;
}
void WidgetSearchTemplate::PauseSearch()
{
	Q_ASSERT(m_pSearch != 0);
	searchState = SearchState::Paused;
	m_pSearch->Pause();
}
void WidgetSearchTemplate::ClearSearch()
{
	//qDebug() << "Clear search captured in widget search template.";
        searchState = SearchState::Default;
	searchModel->clear();
	qApp->processEvents();
}
void WidgetSearchTemplate::OnStatsUpdated()
{
	nFiles = searchModel->nFileCount;
	nHits = m_pSearch->m_nHits;
	nHubs = m_pSearch->m_nHubs;
	nLeaves = m_pSearch->m_nLeaves;
	emit statsUpdated(this);
}
QModelIndex WidgetSearchTemplate::CurrentItem()
{
	QModelIndex idx = m_ui->treeViewSearchResults->currentIndex();
	const QSortFilterProxyModel* pModel = static_cast<const QSortFilterProxyModel*>(idx.model());
	return pModel->mapToSource(idx);
}
void WidgetSearchTemplate::OnStateChanged()
{
	if( m_pSearch )
	{
		if( m_pSearch->m_bPaused )
		{
			searchState = SearchState::Paused;
		}
		else if( m_pSearch->m_bActive && !m_pSearch->m_bPaused )
		{
			searchState = SearchState::Searching;
		}
		else
		{
			searchState = SearchState::Stopped;
		}
	}
	else
	{
                searchState = SearchState::Stopped;
	}
	emit stateChanged();
}
void WidgetSearchTemplate::Sort()
{
	sortModel->sort(sortModel->sortColumn(), sortModel->sortOrder());
}
void WidgetSearchTemplate::saveHeaderState()
{
	quazaaSettings.WinMain.SearchHeader = m_ui->treeViewSearchResults->header()->saveState();
}
void WidgetSearchTemplate::loadHeaderState()
{
	m_ui->treeViewSearchResults->header()->restoreState(quazaaSettings.WinMain.SearchHeader);
}

