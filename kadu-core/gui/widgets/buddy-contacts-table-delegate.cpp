/*
 * %kadu copyright begin%
 * Copyright 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtGui/QComboBox>

#include "accounts/account.h"
#include "gui/widgets/accounts-combo-box.h"
#include "model/roles.h"

#include "buddy-contacts-table-delegate.h"

BuddyContactsTableDelegate::BuddyContactsTableDelegate(QObject *parent) :
		QStyledItemDelegate(parent)
{
}

BuddyContactsTableDelegate::~BuddyContactsTableDelegate()
{
}

QWidget * BuddyContactsTableDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	if (1 != index.column()) // not account
		return QStyledItemDelegate::createEditor(parent, option, index);

	return new AccountsComboBox(parent);
}

void BuddyContactsTableDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	Q_UNUSED(index)

	editor->setGeometry(option.rect); // use full rect, does not allow to display image next to combo-box
}

void BuddyContactsTableDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	AccountsComboBox *accountsComboBox = dynamic_cast<AccountsComboBox *>(editor);
	if (!accountsComboBox)
	{
		QStyledItemDelegate::setEditorData(editor, index);
		return;
	}

	accountsComboBox->setCurrentAccount(index.data(AccountRole).value<Account>());
}

void BuddyContactsTableDelegate::setModelData(QWidget* editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	AccountsComboBox *accountsComboBox = dynamic_cast<AccountsComboBox *>(editor);
	if (!accountsComboBox)
	{
		QStyledItemDelegate::setModelData(editor, model, index);
		return;
	}

	model->setData(index, QVariant::fromValue<Account>(accountsComboBox->currentAccount()), AccountRole);
}

bool BuddyContactsTableDelegate::eventFilter(QObject *editor, QEvent *event)
{
	QWidget *editorwidget = dynamic_cast<QWidget *>(editor);
	if (editorwidget)
		commitData(editorwidget);
	return QStyledItemDelegate::eventFilter(editor, event);
}
