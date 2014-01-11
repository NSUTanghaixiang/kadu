/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * %kadu copyright end%
 *
 * This file is derived from part of the KDE project
 * Copyright (C) 2007, 2006 Rafael Fernández López <ereslibre@kde.org>
 * Copyright (C) 2002-2003 Matthias Kretz <kretz@kde.org>
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

#define MARGIN 5

#include <QtGui/QApplication>
#include <QtGui/QBoxLayout>
#include <QtGui/QCheckBox>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPainter>
#include <QtGui/QPushButton>
#include <QtGui/QStyleOptionViewItemV4>

#include "configuration/configuration-manager.h"
#include "core/core.h"
#include "gui/widgets/categorized-list-view-painter.h"
#include "gui/widgets/categorized-list-view.h"
#include "gui/widgets/plugin-list/plugin-list-view-delegate.h"
#include "gui/widgets/plugin-list/plugin-list-widget.h"
#include "gui/windows/message-dialog.h"
#include "icons/kadu-icon.h"
#include "model/categorized-sort-filter-proxy-model.h"
#include "plugin/model/plugin-proxy-model.h"
#include "plugin/activation/plugin-activation-service.h"
#include "plugin/metadata/plugin-metadata.h"
#include "plugin/plugin-manager.h"

#include "plugin-model.h"

void PluginModel::loadPluginData()
{
        beginResetModel();
        Plugins.clear();
        QList<PluginEntry> listToAdd;

        for (auto const &pluginMetadata : Core::instance()->pluginManager())
        {
                PluginEntry pluginEntry;

                pluginEntry.category = !pluginMetadata.category().isEmpty()
                        ? pluginMetadata.category()
                        : "Misc";
                pluginEntry.name = !pluginMetadata.displayName().isEmpty()
                        ? pluginMetadata.displayName()
                        : pluginMetadata.name();
                pluginEntry.description = pluginMetadata.description();
                pluginEntry.author = pluginMetadata.author();
                pluginEntry.pluginName = pluginMetadata.name();
                pluginEntry.checked = m_pluginActivationService.data()->isActive(pluginMetadata.name());
                pluginEntry.isCheckable = true;

                listToAdd.append(pluginEntry);
        }

        Plugins << listToAdd;
        endResetModel();

        pluginSelector_d->Proxy->sort(0);
}

PluginModel::PluginModel(PluginListWidget *pluginSelector_d, QObject *parent)
                : QAbstractListModel(parent)
                , pluginSelector_d(pluginSelector_d)
                , Manager(Core::instance()->pluginManager())
{
}

PluginModel::~PluginModel()
{
}

void PluginModel::setPluginActivationService(PluginActivationService *pluginActivationService)
{
	m_pluginActivationService = pluginActivationService;
}

QModelIndex PluginModel::index(int row, int column, const QModelIndex &parent) const
{
        Q_UNUSED(parent)

        return createIndex(row, column, (row < Plugins.count()) ? (void*) &Plugins.at(row) : 0);
}

QVariant PluginModel::data(const QModelIndex &index, int role) const
{
        if (!index.isValid() || !index.internalPointer())
        {
                return QVariant();
        }

        if (index.row() < 0 || index.row() >= Plugins.count())
                return QVariant();

        PluginEntry *pluginEntry = static_cast<PluginEntry*>(index.internalPointer());


        switch (role)
        {

                case Qt::DisplayRole:
                        return pluginEntry->name;

                case PluginEntryRole:
                        return QVariant::fromValue(pluginEntry);

                case NameRole:
                        return pluginEntry->pluginName;

                case CommentRole:
                        return pluginEntry->description;

                case ServicesCountRole:

                case IsCheckableRole:
                        return true;

                case Qt::CheckStateRole:
                        return pluginEntry->checked;

                case CategorizedSortFilterProxyModel::CategoryDisplayRole: // fall through

                case CategorizedSortFilterProxyModel::CategorySortRole:
                        return pluginEntry->category;

                default:
                        return QVariant();
        }
}

bool PluginModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
        if (!index.isValid())
        {
                return false;
        }

        bool ret = false;

        if (role == Qt::CheckStateRole)
        {
                static_cast<PluginEntry*>(index.internalPointer())->checked = value.toBool();
                ret = true;
        }

        if (ret)
        {
                emit dataChanged(index, index);
        }

        return ret;
}

int PluginModel::rowCount(const QModelIndex &parent) const
{
        return parent.isValid() ? 0 : Plugins.count();
}

#include "moc_plugin-model.cpp"