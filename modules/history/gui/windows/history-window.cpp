/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QDateEdit>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLineEdit>
#include <QtGui/QMenu>
#include <QtGui/QPushButton>
#include <QtGui/QSplitter>
#include <QtGui/QStatusBar>
#include <QtGui/QVBoxLayout>

#include "buddies/model/buddies-model-base.h"
#include "chat/filter/chat-name-filter.h"
#include "chat/message/message.h"
#include "chat/type/chat-type.h"
#include "chat/type/chat-type-manager.h"
#include "chat/aggregate-chat-builder.h"
#include "gui/actions/actions.h"
#include "gui/widgets/buddies-list-view-menu-manager.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/delayed-line-edit.h"
#include "gui/windows/message-dialog.h"
#include "misc/misc.h"
#include "model/roles.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocol-menu-manager.h"
#include "activate.h"
#include "debug.h"
#include "icons-manager.h"

#include "model/chat-dates-model.h"
#include "model/history-chats-model.h"
#include "model/history-chats-model-proxy.h"
#include "storage/history-storage.h"

#include "history-window.h"
#include <QItemDelegate>
#include <chat/chat-details-aggregate.h>

HistoryWindow::HistoryWindow(QWidget *parent) :
		MainWindow(parent)
{
	kdebugf();

	setWindowTitle(tr("History"));
	setWindowIcon(IconsManager::instance()->loadIcon("History"));

	createGui();
	connectGui();

	loadWindowGeometry(this, "History", "HistoryWindowGeometry", 200, 200, 700, 500);
//TODO 0.6.6:
	DetailsPopupMenu = new QMenu(this);
//	DetailsPopupMenu->addAction(IconsManager::instance()->loadIcon("ClearHistory"), tr("&Remove entries"), this, SLOT(removeHistoryEntriesPerDate()));

	kdebugf2();
}

HistoryWindow::~HistoryWindow()
{
	kdebugf();

	saveWindowGeometry(this, "History", "HistoryDialogGeometry");

	//writeToolBarsToConfig("history");
	kdebugf2();
}

void HistoryWindow::createGui()
{
	QWidget *mainWidget = new QWidget(this);
	mainWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QVBoxLayout *layout = new QVBoxLayout(mainWidget);
	layout->setMargin(5);
	layout->setSpacing(5);

	QSplitter *splitter = new QSplitter(Qt::Horizontal, mainWidget);
	layout->addWidget(splitter);

	createChatTree(splitter);
	QSplitter *rightSplitter = new QSplitter(Qt::Vertical, splitter);

	QWidget *rightWidget = new QWidget(rightSplitter);
	QVBoxLayout *rightLayout = new QVBoxLayout(rightWidget);
	rightLayout->setSpacing(0);
	rightLayout->setMargin(0);

	QWidget *filterWidget = new QWidget(rightWidget);
	rightLayout->addWidget(filterWidget);

	createFilterBar(filterWidget);

	DetailsListView = new QTreeView(rightWidget);
	rightLayout->addWidget(DetailsListView);

	DetailsListView->setRootIsDecorated(false);
	DetailsListView->setModel(new ChatDatesModel(Chat::null, QList<QDate>(), this));
	DetailsListView->setUniformRowHeights(true);

	ContentBrowser = new ChatMessagesView(Chat::null, rightSplitter);
	ContentBrowser->setPruneEnabled(false);

	QList<int> sizes;
	sizes.append(100);
	sizes.append(300);
	splitter->setSizes(sizes);

	setCentralWidget(mainWidget);
}

void HistoryWindow::createChatTree(QWidget *parent)
{
	QWidget *chatsWidget = new QWidget(parent);
	QVBoxLayout *layout = new QVBoxLayout(chatsWidget);

	QLineEdit *filterLineEdit = new QLineEdit(chatsWidget);
	connect(filterLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(filterLineChanged(const QString &)));
	layout->addWidget(filterLineEdit);

	ChatsTree = new QTreeView(parent);
	ChatsTree->header()->hide();
	layout->addWidget(ChatsTree);

	ChatsModel = new HistoryChatsModel(this);

	ChatsModelProxy = new HistoryChatsModelProxy(this);
	ChatsModelProxy->setSourceModel(ChatsModel);

	NameFilter = new ChatNameFilter(this);
	ChatsModelProxy->addFilter(NameFilter);

	ChatsTree->setModel(ChatsModelProxy);
	ChatsModelProxy->sort(1);
	ChatsModelProxy->sort(0); // do the sorting
	ChatsTree->setRootIsDecorated(true);
}

void HistoryWindow::createFilterBar(QWidget *parent)
{
	QGridLayout *layout = new QGridLayout(parent);
	layout->setSpacing(0);
	layout->setMargin(0);

	layout->setColumnStretch(1, 2);
	layout->setColumnStretch(2, 1);

	QLabel *filterLabel = new QLabel(tr("Filter") + ": ", parent);
	layout->addWidget(filterLabel, 0, 0, 1, 1);
	
	DelayedLineEdit *searchLineEdit = new DelayedLineEdit(parent);
	layout->addWidget(searchLineEdit, 0, 1, 1, 1);

	QCheckBox *filterByDate = new QCheckBox(tr("by date"), parent);
	filterByDate->setChecked(false);
	layout->addWidget(filterByDate, 0, 2, 1, 1);
	
	FromDateLabel = new QLabel(tr("From") + ": ", parent);
	FromDateLabel->setEnabled(false);
	layout->addWidget(FromDateLabel, 0, 3, 1, 1);
	
	FromDate = new QDateEdit(parent);
	FromDate->setEnabled(false);
	FromDate->setCalendarPopup(true);
	layout->addWidget(FromDate, 0, 4, 1, 1);

	ToDateLabel = new QLabel(tr("To") + ": ", parent);
	ToDateLabel->setEnabled(false);
	layout->addWidget(ToDateLabel, 0, 5, 1, 1);
	
	ToDate = new QDateEdit(parent);
	ToDate->setEnabled(false);
	ToDate->setCalendarPopup(true);
	layout->addWidget(ToDate, 0, 6, 1, 1);
	
	connect(filterByDate, SIGNAL(stateChanged(int)),
			this, SLOT(dateFilteringEnabled(int)));

	connect(searchLineEdit, SIGNAL(delayedTextChanged(const QString &)),
			this, SLOT(searchTextChanged(const QString &)));
	connect(FromDate, SIGNAL(dateChanged(const QDate &)),
			this, SLOT(fromDateChanged(const QDate &)));
	connect(ToDate, SIGNAL(dateChanged(const QDate &)),
			this, SLOT(toDateChanged(const QDate &)));
}

void HistoryWindow::connectGui()
{
	connect(ChatsTree, SIGNAL(activated(const QModelIndex &)),
			this, SLOT(chatActivated(const QModelIndex &)));
	connect(DetailsListView, SIGNAL(activated(const QModelIndex &)),
			this, SLOT(dateActivated(const QModelIndex &)));

	ChatsTree->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ChatsTree, SIGNAL(customContextMenuRequested(QPoint)),
			this, SLOT(showMainPopupMenu(QPoint)));

	DetailsListView->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(DetailsListView, SIGNAL(customContextMenuRequested(QPoint)),
			this, SLOT(showDetailsPopupMenu(QPoint)));
}

void HistoryWindow::updateData()
{
	kdebugf();

	QModelIndex index = ChatsTree->selectionModel()->currentIndex();
	Chat chat = index.data(ChatRole).value<Chat>();

	ChatsModel->clear();
	QList<Chat> usedChats;
	QList<Chat> chatsList = History::instance()->chatsList(Search);
	QList<Chat> result;

	foreach (Chat chat, chatsList)
	{
		if (usedChats.contains(chat))
			continue;
		Chat aggregate = AggregateChatBuilder::buildAggregateChat(chat.contacts().toBuddySet());
		if (aggregate)
		{
			ChatDetailsAggregate *details = dynamic_cast<ChatDetailsAggregate *>(aggregate.details());

			if (details)
				foreach (Chat usedChat, details->chats())
					usedChats.append(usedChat);

			result.append(aggregate);
		}
		else
		{
			result.append(chat);
			usedChats.append(chat);
		}
	}

	ChatsModel->addChats(result);

	if (result.contains(chat))
		selectChat(chat);
	else
		selectChat(Chat::null);
}

void HistoryWindow::selectChat(Chat chat)
{
	QString typeName = chat.type();
	ChatType *type = ChatTypeManager::instance()->chatType(typeName);

	if (!type)
	{
		chatActivated(QModelIndex());
		return;
	}

	QModelIndex chatTypeIndex = ChatsModelProxy->chatTypeIndex(type);
	if (!chatTypeIndex.isValid())
	{
		chatActivated(QModelIndex());
		return;
	}

	ChatsTree->collapseAll();
	ChatsTree->expand(chatTypeIndex);

	QModelIndex chatIndex = ChatsModelProxy->chatIndex(chat);
	ChatsTree->selectionModel()->select(chatIndex, QItemSelectionModel::ClearAndSelect);

	chatActivated(chat);
}

void HistoryWindow::chatActivated(Chat chat)
{
	kdebugf();

	ChatDatesModel *model = dynamic_cast<ChatDatesModel *>(DetailsListView->model());
	if (!model)
		return;

	QModelIndex selectedIndex = DetailsListView->selectionModel()->currentIndex();
	QDate date = selectedIndex.data(DateRole).toDate();

	QList<QDate> chatDates = History::instance()->datesForChat(chat, Search);
	model->setChat(chat);
	model->setDates(chatDates);

	QModelIndex select = model->indexForDate(date);
	if (!select.isValid())
	{
		int lastRow = model->rowCount(QModelIndex()) - 1;
		if (lastRow >= 0)
			select = model->index(lastRow);
	}

	DetailsListView->selectionModel()->setCurrentIndex(select, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);

	dateActivated(select);

	kdebugf2();
}

void HistoryWindow::chatActivated(const QModelIndex &index)
{
	kdebugf();

	chatActivated(index.data(ChatRole).value<Chat>());

	kdebugf2();
}

void HistoryWindow::dateActivated(const QModelIndex &index)
{
	kdebugf();

	Chat chat = index.data(ChatRole).value<Chat>();
	QDate date = index.data(DateRole).value<QDate>();

	QList<Message> messages;
	if (chat && date.isValid())
		messages = History::instance()->messages(chat, date);

	ContentBrowser->setChat(chat);
	ContentBrowser->clearMessages();
	ContentBrowser->appendMessages(messages);

	kdebugf2();
}

void HistoryWindow::filterLineChanged(const QString &filterText)
{
	NameFilter->setName(filterText);
}

void HistoryWindow::searchTextChanged(const QString &searchText)
{
	Search.setQuery(searchText);
	updateData();
}

void HistoryWindow::fromDateChanged(const QDate &date)
{
	Search.setFromDate(date);
	updateData();
}

void HistoryWindow::toDateChanged(const QDate &date)
{
	Search.setToDate(date);
	updateData();
}

void HistoryWindow::showMainPopupMenu(const QPoint &pos)
{	
	Chat chat = ChatsTree->indexAt(pos).data(ChatRole).value<Chat>();
	if (!chat)
		return;

	bool first = true;
	QMenu *menu = new QMenu(this);

	QMenu *actions = new QMenu(tr("Actions"));
	foreach (ActionDescription *actionDescription, BuddiesListViewMenuManager::instance()->buddyListActions())
		if (actionDescription)
		{
			Action *action = actionDescription->createAction(this);
			actions->addAction(action);
			action->checkState();
		}
		else
			actions->addSeparator();

	foreach (ActionDescription *actionDescription, BuddiesListViewMenuManager::instance()->buddiesContexMenu())
	{
		if (actionDescription)
		{

			Action *action = actionDescription->createAction(this);
			menu->addAction(action);
			action->checkState();
		}
		else
		{
			menu->addSeparator();
			if (first)
			{
				menu->addMenu(actions);
				first = false;
			}
		}
	}

	QMenu *management = menu->addMenu(tr("Buddy Options"));

	foreach (ActionDescription *actionDescription, BuddiesListViewMenuManager::instance()->managementActions())
		if (actionDescription)
		{
			Action *action = actionDescription->createAction(this);
			management->addAction(action);
			action->checkState();
		}
		else
			management->addSeparator();
		
	QList<Account> accounts;
	foreach (Contact con, chat.contacts())
		accounts.append(con.contactAccount());
	
	foreach (Account account, accounts)
	{
		if (account.isNull())
			continue;

		ProtocolFactory *protocolFactory = account.protocolHandler()->protocolFactory();

		if (!protocolFactory || !protocolFactory->protocolMenuManager())
			continue;

		QMenu *account_menu = menu->addMenu(account.name());
		if (!protocolFactory->iconName().isEmpty())
			account_menu->setIcon(IconsManager::instance()->loadIcon(protocolFactory->iconName()));

		if (protocolFactory->protocolMenuManager()->protocolActions(account, (*chat.contacts().toBuddySet().begin())).size() == 0)
			continue;

		foreach (ActionDescription *actionDescription, protocolFactory->protocolMenuManager()->protocolActions(account, (*chat.contacts().toBuddySet().begin())))
			if (actionDescription)
			{
				Action *action = actionDescription->createAction(this);
				account_menu->addAction(action);
				action->checkState();
			}
			else
				account_menu->addSeparator();
	}
	menu->addAction(IconsManager::instance()->loadIcon("ClearHistory"), tr("&Clear history"), this, SLOT(clearHistory()));
	menu->exec(QCursor::pos());
}

void HistoryWindow::showDetailsPopupMenu(const QPoint &pos)
{
	bool isValid = true;
	Chat chat = DetailsListView->indexAt(pos).data(ChatRole).value<Chat>();
	QDate date = DetailsListView->indexAt(pos).data(DateRole).value<QDate>();

	if (!chat || !date.isValid())
		isValid = false;

	foreach (QAction *action, DetailsPopupMenu->actions())
		action->setEnabled(isValid);

	DetailsPopupMenu->exec(QCursor::pos());
}

void HistoryWindow::show(Chat chat)
{
	if (!History::instance()->currentStorage())
	{
		MessageDialog::msg(tr("There is no history storage module loaded!"), false, "Warning");
		return;
	}

	updateData();
	selectChat(chat);

	QWidget::show();
	_activateWindow(this);
}

void HistoryWindow::openChat()
{
	kdebugf();
	Chat chat = ChatsTree->currentIndex().data(ChatRole).value<Chat>();
	if (!chat)
		return;

	ChatWidgetManager::instance()->openChatWidget(chat, true);

	kdebugf2();
}

void HistoryWindow::clearHistory()
{
	kdebugf();
	Chat chat = ChatsTree->currentIndex().data(ChatRole).value<Chat>();
	if (!chat)
		return;

	History::instance()->currentStorage()->clearChatHistory(chat);
	updateData();
	kdebugf2();
}

bool HistoryWindow::supportsActionType(ActionDescription::ActionType type)
{
	return (type == ActionDescription::TypeGlobal || type == ActionDescription::TypeChat || type == ActionDescription::TypeHistory);
}

void HistoryWindow::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape)
	{
		e->accept();
		hide();
	}
	else
		QWidget::keyPressEvent(e);
}

ContactSet HistoryWindow::contacts()
{
	Chat chat = ChatsTree->currentIndex().data(ChatRole).value<Chat>();
	if (!chat)
		return ContactSet();
	return chat.contacts();
}

BuddySet HistoryWindow::buddies()
{
	return contacts().toBuddySet();
}

void HistoryWindow::dateFilteringEnabled(int state)
{
	bool enabled = state == 2;
	FromDateLabel->setEnabled(enabled);
	FromDate->setEnabled(enabled);
	ToDateLabel->setEnabled(enabled);
	ToDate->setEnabled(enabled);
}

HistoryWindow *historyDialog = 0;
