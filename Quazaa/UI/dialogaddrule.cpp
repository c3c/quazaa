/*
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

#include "dialogaddrule.h"
#include "ui_dialogaddrule.h"
#include "skinsettings.h"

#include <QListView>
#include <QMessageBox>

#include "debug_new.h"

CDialogAddRule::CDialogAddRule(CWidgetSecurity* parent, CSecureRule* pRule) :
	QDialog( parent ),
	ui( new Ui::CDialogAddRule ),
	m_pParent( parent )
{
	ui->setupUi( this );
	ui->comboBoxAction->setView( new QListView() );
	ui->comboBoxExpire->setView( new QListView() );
	ui->comboBoxRuleType->setView( new QListView() );


	m_pRule = pRule;

	if(m_pRule) {
		ui->comboBoxRuleType->setEnabled(false);
		ui->lineEditComment->setText( m_pRule->m_sComment );

		switch ( m_pRule->type() )
		{
		case RuleType::IPAddressRange:
		{
			ui->comboBoxRuleType->setCurrentIndex( 1 );
			ui->stackedWidgetType->setCurrentIndex( 1 );
			QStringList lAddressRange = m_pRule->getContentString().split("-");
			ui->lineEditStartIP->setText( lAddressRange.at(0) );
			ui->lineEditEndIP->setText( lAddressRange.at(1) );
			break;
		}
		case RuleType::Country:
			ui->comboBoxRuleType->setCurrentIndex( 2 );
			ui->stackedWidgetType->setCurrentIndex( 2 );
			ui->lineEditCountry->setText( m_pRule->getContentString() );
			break;
		case RuleType::Hash:
			ui->comboBoxRuleType->setCurrentIndex( 3 );
			ui->stackedWidgetType->setCurrentIndex( 3 );
			break;
		case RuleType::Content:
			ui->comboBoxRuleType->setCurrentIndex( 4 );
			ui->stackedWidgetType->setCurrentIndex( 4 );
			ui->lineEditContent->setText( m_pRule->getContentString() );
			break;
		case RuleType::RegularExpression:
			ui->comboBoxRuleType->setCurrentIndex( 5 );
			ui->stackedWidgetType->setCurrentIndex( 5 );
			ui->lineEditRegularExpression->setText( m_pRule->getContentString() );
			break;
		case RuleType::UserAgent:
			ui->comboBoxRuleType->setCurrentIndex( 6 );
			ui->stackedWidgetType->setCurrentIndex( 6 );
			ui->checkBoxUserAgentRegularExpression->setChecked( ((CUserAgentRule*)m_pRule)->getRegExp() );
			ui->lineEditUserAgent->setText( m_pRule->getContentString() );
			break;
		case RuleType::IPAddress:
		default:
			ui->comboBoxRuleType->setCurrentIndex( 0 );
			ui->stackedWidgetType->setCurrentIndex( 0 );
			ui->lineEditIP->setText( ((CIPRule*)m_pRule)->getContentString() );
			break;
		}

		switch (m_pRule->m_nAction) {
			case RuleAction::Accept:
				ui->comboBoxAction->setCurrentIndex(1);
				break;
			case RuleAction::Deny:
				ui->comboBoxAction->setCurrentIndex(2);
				break;
			default:
				ui->comboBoxAction->setCurrentIndex(0);
				break;
		}

		quint32 tExpire = m_pRule->getExpiryTime();

		switch ( tExpire )
		{
		case RuleTime::Forever:
			ui->comboBoxExpire->setCurrentIndex( 0 );
			ui->lineEditMinutes->setEnabled( false );
			ui->lineEditHours->setEnabled( false );
			ui->lineEditDays->setEnabled( false );
			break;

		case RuleTime::Session:
			ui->comboBoxExpire->setCurrentIndex( 1 );
			ui->lineEditMinutes->setEnabled( false );
			ui->lineEditHours->setEnabled( false );
			ui->lineEditDays->setEnabled( false );
			break;

		default:
			ui->comboBoxExpire->setCurrentIndex( 2 );
			ui->lineEditMinutes->setEnabled( true );
			ui->lineEditHours->setEnabled( true );
			ui->lineEditDays->setEnabled( true );

			tExpire -= common::getTNowUTC();

			tExpire /= 60; // minutes now
			ui->lineEditMinutes->setText( QString::number( tExpire % 60 ) );

			tExpire /= 60; // hours now
			ui->lineEditHours->setText(   QString::number( tExpire % 24 ) );
			ui->lineEditDays->setText(    QString::number( tExpire / 24 ) );
			break;
		}
	} else {
		ui->comboBoxRuleType->setCurrentIndex( 0 );
		ui->stackedWidgetType->setCurrentIndex( 0 );
		ui->comboBoxAction->setCurrentIndex(0);
		ui->comboBoxExpire->setCurrentIndex( 0 );
	}

	setSkin();
}

CDialogAddRule::~CDialogAddRule()
{
	delete ui;
}

void CDialogAddRule::changeEvent(QEvent* e)
{
	QDialog::changeEvent( e );
	switch ( e->type() )
	{
		case QEvent::LanguageChange:
			ui->retranslateUi( this );
			break;
		default:
			break;
	}
}

// TODO: change user interface for IP ranges and hashes.
void CDialogAddRule::on_pushButtonOK_clicked()
{
	bool bIsNewRule = false;

	if(!m_pRule) {
		bIsNewRule = true;
		switch ( ui->comboBoxRuleType->currentIndex() )
		{
			case 0:
				m_pRule = new CIPRule();
				break;
			case 1:
				m_pRule = new CIPRangeRule();
				break;
			case 2:
#if SECURITY_ENABLE_GEOIP
				m_pRule = new CCountryRule();
#endif // SECURITY_ENABLE_GEOIP
				break;
			case 3:
				m_pRule = new CHashRule();
				break;
			case 4:
				m_pRule = new CContentRule();
				break;
			case 5:
				m_pRule = new CRegularExpressionRule();
				break;
			case 6:
				m_pRule = new CUserAgentRule();
				break;
			default:
				Q_ASSERT( false );
		}
	}

	QString sTmp;

	switch ( m_pRule->type() )
	{
		case RuleType::IPAddress:
			sTmp = ui->lineEditIP->text();
			if ( !m_pRule->parseContent( sTmp ) )
			{
				QMessageBox::warning(this, tr("IP Address Rule Invalid"),
											   tr("The ip address rule is invalid. Please see the example for correct usage."),
											   QMessageBox::Ok,
											   QMessageBox::Ok);
				return;
			}
			break;
		case RuleType::IPAddressRange:
			sTmp = QString("%1-%2").arg(ui->lineEditStartIP->text()).arg(ui->lineEditEndIP->text());
			if ( !m_pRule->parseContent( sTmp ) )
			{
				QMessageBox::warning(this, tr("IP Address Range Rule Invalid"),
											   tr("The ip address range rule is invalid. Please see the example for correct usage."),
											   QMessageBox::Ok,
											   QMessageBox::Ok);
				return;
			}
			break;
		case RuleType::Country:
#if SECURITY_ENABLE_GEOIP
			sTmp = ui->lineEditCountry->text();
			if ( !m_pRule->parseContent( sTmp ) )
			{
				QMessageBox::warning(this, tr("Country Rule Invalid"),
											   tr("The country rule is invalid. Please see the example for correct usage."),
											   QMessageBox::Ok,
											   QMessageBox::Ok);
				return;
			}
#endif // SECURITY_ENABLE_GEOIP
			break;
		case RuleType::Hash:
			sTmp = ui->lineEditURI->text();
			if ( !m_pRule->parseContent( sTmp ) )
			{
				QMessageBox::warning(this, tr("Hash Rule Invalid"),
											   tr("The hash rule is invalid. Please see the example for correct usage."),
											   QMessageBox::Ok,
											   QMessageBox::Ok);
				return;
			}
			break;
		case RuleType::Content:
			sTmp = ui->lineEditContent->text();
			if ( !m_pRule->parseContent( sTmp ) )
			{
				QMessageBox::warning(this, tr("Content Rule Invalid"),
											   tr("The content rule is invalid. Please see the example for correct usage."),
											   QMessageBox::Ok,
											   QMessageBox::Ok);
				return;
			}
			((CContentRule*)m_pRule)->setAll( ui->radioButtonMatchAll->isChecked() );
			break;
		case RuleType::RegularExpression:
			sTmp = ui->lineEditRegularExpression->text();
			if ( !m_pRule->parseContent( sTmp ) )
			{
				QMessageBox::warning(this, tr("Regular Expression Rule Invalid"),
											   tr("The regular expression rule is invalid. Please see the example for correct usage."),
											   QMessageBox::Ok,
											   QMessageBox::Ok);
				return;
			}
			break;
		case RuleType::UserAgent:
			sTmp = ui->lineEditUserAgent->text();
			if ( !m_pRule->parseContent( sTmp ) )
			{
				QMessageBox::warning(this, tr("User Agent Rule Invalid"),
											   tr("The user agent rule is invalid. Please see the example for correct usage."),
											   QMessageBox::Ok,
											   QMessageBox::Ok);
				return;
			}
			((CUserAgentRule*)m_pRule)->setRegExp( ui->checkBoxUserAgentRegularExpression->isChecked() );
			break;
		default:
			Q_ASSERT( false );
	}

	switch (ui->comboBoxAction->currentIndex()) {
		case 0: // None
			m_pRule->m_nAction = RuleAction::None;
			break;
		case 1: // Accept
			m_pRule->m_nAction = RuleAction::Accept;
			break;
		case 2: // Deny
			m_pRule->m_nAction = RuleAction::Deny;
			break;
	}

	switch (ui->comboBoxExpire->currentIndex()) {
		case 0: // Forever
			m_pRule->m_tExpire = RuleTime::Forever;
		case 1: // Session
			m_pRule->m_tExpire = RuleTime::Session;
		case 2: // Set Time
		{
			quint32 tExpire = 0;
			tExpire += ui->lineEditMinutes->text().toUShort() * 60;
			tExpire += ui->lineEditHours->text().toUShort() * 3600;
			tExpire += ui->lineEditDays->text().toUShort() * 216000;
			tExpire += common::getTNowUTC();

			m_pRule->m_tExpire = tExpire;
			break;
		}
	}

	m_pRule->m_sComment = ui->lineEditComment->text();
	m_pRule->m_bAutomatic = false;

	if(bIsNewRule)
		securityManager.add(m_pRule);

	accept();
}

void CDialogAddRule::on_pushButtonCancel_clicked()
{
	reject();
}

void CDialogAddRule::on_comboBoxExpire_currentIndexChanged(int index)
{
	if ( index == 2 )
	{
		ui->lineEditDays->setEnabled( true );
		ui->lineEditHours->setEnabled( true );
		ui->lineEditMinutes->setEnabled( true );
	}
	else
	{
		ui->lineEditDays->setEnabled( false );
		ui->lineEditHours->setEnabled( false );
		ui->lineEditMinutes->setEnabled( false );
	}
}

void CDialogAddRule::setSkin()
{

}

void CDialogAddRule::on_lineEditDays_editingFinished()
{
	quint64 nDays = ui->lineEditDays->text().toULong();

	ui->lineEditDays->setText( QString::number( nDays ) );
}

void CDialogAddRule::on_lineEditHours_editingFinished()
{
	quint64 nHours = ui->lineEditHours->text().toULong();
	quint64 nDays = ui->lineEditDays->text().toULong();

	ui->lineEditHours->setText( QString::number( nHours % 24 ) );
	nDays += nHours / 24;
	ui->lineEditDays->setText( QString::number( nDays ) );
}

void CDialogAddRule::on_lineEditMinutes_editingFinished()
{
	quint64 nMinutes = ui->lineEditMinutes->text().toULong();
	quint64 nHours = ui->lineEditHours->text().toULong();
	quint64 nDays = ui->lineEditDays->text().toULong();

	ui->lineEditMinutes->setText( QString::number( nMinutes % 60 ) );
	nHours += nMinutes / 60;
	ui->lineEditHours->setText( QString::number( nHours % 24 ) );
	nDays += nHours / 24;
	ui->lineEditDays->setText( QString::number( nDays ) );
}
