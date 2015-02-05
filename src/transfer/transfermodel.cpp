/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Nathan Osman
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 **/

#include <QApplication>
#include <QIcon>
#include <QStyle>

#include "transfermodel.h"
#include "transfermodel_p.h"

enum {
    ColumnDeviceName = 0,
    ColumnProgress,
    ColumnStatus,
    ColumnCount
};

TransferModelPrivate::TransferModelPrivate(QObject *parent)
    : QObject(parent)
{
}

TransferModelPrivate::~TransferModelPrivate()
{
    qDeleteAll(transfers);
}

TransferModel::TransferModel()
    : d(new TransferModelPrivate(this))
{
}

int TransferModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : d->transfers.count();
}

int TransferModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : ColumnCount;
}

QVariant TransferModel::data(const QModelIndex &index, int role) const
{
    if(index.row() < d->transfers.count() && index.column() < ColumnCount) {
        Transfer *transfer = d->transfers.at(index.row());

        switch(role) {
        case Qt::DisplayRole:
            switch(index.column()) {
            case ColumnDeviceName:
                return transfer->deviceName();
            case ColumnProgress:
                return transfer->progress();
            }
        case Qt::DecorationRole:
            if(index.column() == 0) {
                switch(transfer->direction()) {
                case Transfer::Send:
                    return QVariant::fromValue(QApplication::style()->standardIcon(QStyle::SP_ArrowUp));
                case Transfer::Receive:
                    return QVariant::fromValue(QApplication::style()->standardIcon(QStyle::SP_ArrowDown));
                }
            }
        case Qt::UserRole:
            return QVariant::fromValue(transfer);
        }
    }

    return QVariant();
}

QVariant TransferModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch(section) {
        case ColumnDeviceName:
            return tr("Device Name");
        case ColumnProgress:
            return tr("Progress");
        case ColumnStatus:
            return tr("Status");
        }
    }

    return QVariant();
}

QModelIndex TransferModel::indexOf(Transfer *transfer, int column) const
{
    return index(d->transfers.indexOf(transfer), column);
}

void TransferModel::add(Transfer *transfer)
{
    beginInsertRows(QModelIndex(), d->transfers.count(), d->transfers.count());
    d->transfers.append(transfer);
    endInsertRows();

    // Whenever properties of the transfer change, emit the appropriate signal
    connect(transfer, &Transfer::deviceNameChanged, [this, transfer]() {
        QModelIndex index = indexOf(transfer, ColumnDeviceName);
        emit dataChanged(index, index);
    });

    connect(transfer, &Transfer::progressChanged, [this, transfer]() {
        QModelIndex index = indexOf(transfer, ColumnProgress);
        emit dataChanged(index, index);
    });

    connect(transfer, &Transfer::statusChanged, [this, transfer]() {
        QModelIndex index = indexOf(transfer, ColumnStatus);
        emit dataChanged(index, index);
    });

    transfer->start();
}

void TransferModel::clear()
{
    // Iterate over the list in reverse to preserve indices when items are removed
    for(int i = d->transfers.count() - 1; i >= 0; --i) {
        Transfer *transfer = d->transfers.at(i);

        if(transfer->status() == Transfer::Completed || transfer->status() == Transfer::Error) {
            beginRemoveRows(QModelIndex(), i, i);
            delete d->transfers.takeAt(i);
            endRemoveRows();
        }
    }
}
