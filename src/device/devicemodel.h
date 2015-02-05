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

#ifndef NS_DEVICEMODEL_H
#define NS_DEVICEMODEL_H

#include <QAbstractTableModel>

#include "device.h"

class DeviceModelPrivate;

/**
 * @brief Data model for discovered devices
 *
 * Devices continuously send out broadcast packets on all active network
 * interfaces. Once a packet is received, it is examined and an instance of
 * the Device class is created to represent the device.
 */
class DeviceModel : public QAbstractTableModel
{
    Q_OBJECT

public:

    /**
     * @brief Create a device model
     */
    DeviceModel();

    /**
     * @brief Retrieve the number of rows in the model
     * @param parent parent index
     * @return number of rows
     */
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

    /**
     * @brief Retrieve the number of columns in the model
     * @param parent parent index
     * @return number of columns
     */
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

    /**
     * @brief Retrieve data for the specified index
     * @param index index to retrieve
     * @param role role to retrieve
     * @return retrieved data
     */
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    /**
     * @brief Retrieve header data for the specified section
     * @param section section to retrieve
     * @param orientation orientation to retrieve
     * @param role role to retrieve
     * @return retrieved data
     */
   virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    /**
     * @brief Find a device by its UUID
     * @param uuid device UUID
     * @return device if found, otherwise nullptr
     */
    Device* find(const QString &uuid);

Q_SIGNALS:

    /**
     * @brief Indicate that a device has been added to the model
     * @param device device added
     *
     * The pointer is only guaranteed to exist for the duration of the slots
     * connected to this signal and should not be stored.
     */
    void deviceAdded(const Device *device);

    /**
     * @brief Indicate that a device has been removed from the model
     * @param device device removed
     *
     * The pointer is only guaranteed to exist for the duration of the slots
     * connected to this signal and should not be stored.
     */
    void deviceRemoved(const Device *device);

public Q_SLOTS:

    /**
     * @brief Update the model, removing expired devices
     */
    void update();

private:

    DeviceModelPrivate * const d;
    friend class DeviceModelPrivate;
};

#endif // NS_DEVICEMODEL_H
