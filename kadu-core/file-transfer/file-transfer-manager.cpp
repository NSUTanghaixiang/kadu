/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#include "file-transfer-manager.h"

#include "accounts/account.h"
#include "chat/chat-manager.h"
#include "chat/chat.h"
#include "chat/type/chat-type-contact.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "contacts/contact-set.h"
#include "core/application.h"
#include "core/core.h"
#include "file-transfer/file-transfer-direction.h"
#include "file-transfer/file-transfer-handler-manager.h"
#include "file-transfer/file-transfer-handler.h"
#include "file-transfer/file-transfer-notifications.h"
#include "file-transfer/file-transfer-status.h"
#include "file-transfer/file-transfer.h"
#include "file-transfer/gui/file-transfer-actions.h"
#include "file-transfer/gui/file-transfer-window.h"
#include "gui/widgets/chat-widget/chat-widget-manager.h"
#include "gui/widgets/chat-widget/chat-widget-repository.h"
#include "gui/widgets/chat-widget/chat-widget.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/message-dialog.h"
#include "protocols/protocol.h"
#include "protocols/services/file-transfer-service.h"
#include "storage/storage-point.h"
#include "activate.h"

#include <QtCore/QFileInfo>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

FileTransferManager::FileTransferManager(QObject *parent) :
		QObject{parent}
{
	m_actions = new FileTransferActions{this};
	NewFileTransferNotification::registerEvents();

	triggerAllAccountsRegistered();
}

FileTransferManager::~FileTransferManager()
{
	m_window.data()->deleteLater();

	triggerAllAccountsUnregistered();

	NewFileTransferNotification::unregisterEvents();
}

void FileTransferManager::setFileTransferHandlerManager(FileTransferHandlerManager *fileTransferHandlerManager)
{
	m_fileTransferHandlerManager = fileTransferHandlerManager;
}

void FileTransferManager::addFileTransferService(Account account)
{
	auto protocol = account.protocolHandler();
	if (!protocol)
		return;

	auto service = protocol->fileTransferService();
	if (!service)
		return;

	connect(service, SIGNAL(incomingFileTransfer(FileTransfer)),
			this, SLOT(incomingFileTransfer(FileTransfer)));
}

void FileTransferManager::removeFileTransferService(Account account)
{
	auto protocol = account.protocolHandler();
	if (!protocol)
		return;

	auto service = protocol->fileTransferService();
	if (!service)
		return;

	disconnect(service, 0, this, 0);
}

void FileTransferManager::accountRegistered(Account account)
{
	QMutexLocker locker(&mutex());

	addFileTransferService(account);
}

void FileTransferManager::accountUnregistered(Account account)
{
	QMutexLocker locker(&mutex());

	removeFileTransferService(account);
}

void FileTransferManager::cleanUp()
{
	QMutexLocker locker(&mutex());

	auto toRemove = QList<FileTransfer>{};

	for (auto &&fileTransfer : items())
		if (FileTransferStatus::Finished == fileTransfer.transferStatus())
			toRemove.append(fileTransfer);

	for (auto &&fileTransfer : toRemove)
		removeItem(fileTransfer);
}

void FileTransferManager::acceptFileTransfer(FileTransfer transfer)
{
	QMutexLocker locker(&mutex());

	if (!m_fileTransferHandlerManager->ensureHandler(transfer))
		return;

	auto fileName = transfer.localFileName();

	auto haveFileName = !fileName.isEmpty();
	// auto resumeTransfer = haveFileName;

	QFileInfo fi;

	while (true)
	{
		if (fileName.isEmpty())
			fileName = QFileDialog::getSaveFileName(Core::instance()->kaduWindow(), tr("Select file location"),
					Application::instance()->configuration()->deprecatedApi()->readEntry("Network", "LastDownloadDirectory") + transfer.remoteFileName(),
							QString(), 0, QFileDialog::DontConfirmOverwrite);

		if (fileName.isEmpty())
		{
			if (transfer.handler())
				transfer.handler()->reject();
			return;
		}

		Application::instance()->configuration()->deprecatedApi()->writeEntry("Network", "LastDownloadDirectory", QFileInfo(fileName).absolutePath() + '/');
		fi.setFile(fileName);

		if (!haveFileName && fi.exists())
		{
			QWidget *parent = nullptr;
			auto chat = ChatTypeContact::findChat(transfer.peer(), ActionReturnNull);
			if (chat)
				parent = Core::instance()->chatWidgetRepository()->widgetForChat(chat);

			auto question = tr("File %1 already exists.").arg(fileName);
			switch (QMessageBox::question(parent, tr("Save file"), question, tr("Overwrite"), //tr("Resume"),
			                                 tr("Select another file"), 0, 2))
			{
				case 0:
					//resumeTransfer = false;
					break;

				case 1:
					//resumeTransfer = true;
					//break;

				//case 2:
					fileName = QString{};
					haveFileName = false;
					continue;
			}
		}

		if (fi.exists() && !fi.isWritable())
		{
			MessageDialog::show(KaduIcon("dialog-warning"), tr("Kadu"), tr("Could not open file. Select another one."));
			fileName.clear();
			continue;
		}

		if (transfer.handler())
		{
			auto file = new QFile{fileName};
			if (!file->open(QFile::WriteOnly | QIODevice::Truncate))
			{
				MessageDialog::show(KaduIcon("dialog-warning"), tr("Kadu"), tr("Could not open file. Select another one."));
				fileName.clear();
				file->deleteLater();
				continue;
			}

			transfer.setLocalFileName(fileName);
			transfer.handler()->accept(file);
		}

		break;
	}

	transfer.setTransferStatus(FileTransferStatus::Transfer);

	showFileTransferWindow();
}

void FileTransferManager::rejectFileTransfer(FileTransfer transfer)
{
	QMutexLocker locker(&mutex());

	if (m_fileTransferHandlerManager->ensureHandler(transfer))
		transfer.handler()->reject();
}

void FileTransferManager::showFileTransferWindow()
{
	QMutexLocker locker(&mutex());

	if (!m_window)
		m_window = new FileTransferWindow{this};
	_activateWindow(m_window.data());
}

void FileTransferManager::incomingFileTransfer(FileTransfer fileTransfer)
{
	QMutexLocker locker(&mutex());
	addItem(fileTransfer);
	NewFileTransferNotification::notifyIncomingFileTransfer(fileTransfer);
}

void FileTransferManager::itemAboutToBeAdded(FileTransfer fileTransfer)
{
	emit fileTransferAboutToBeAdded(fileTransfer);
}

void FileTransferManager::itemAdded(FileTransfer fileTransfer)
{
	emit fileTransferAdded(fileTransfer);
}

void FileTransferManager::itemAboutToBeRemoved(FileTransfer fileTransfer)
{
	emit fileTransferAboutToBeRemoved(fileTransfer);
}

void FileTransferManager::itemRemoved(FileTransfer fileTransfer)
{
	emit fileTransferRemoved(fileTransfer);
}

#include "moc_file-transfer-manager.cpp"
