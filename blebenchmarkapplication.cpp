#include "blebenchmarkapplication.h"

BleBenchmarkApplication::BleBenchmarkApplication(QObject *parent)
    : QObject{parent}
{
    m_discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &BleBenchmarkApplication::onDeviceDiscovered);
    connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished, this, &BleBenchmarkApplication::onScanFinished);
    connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::canceled, this, &BleBenchmarkApplication::onScanFinished);
}

void BleBenchmarkApplication::start()
{
    m_discoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}

void BleBenchmarkApplication::onDeviceDiscovered(const QBluetoothDeviceInfo &info)
{
    qDebug() << (QString(metaObject()->className()) + "::" + __func__) << info.address() << info.name();

    if (info.name() == "BAT") {
        m_discoveryAgent->stop();
        m_targetDeviceInfo = info;
    }
}

void BleBenchmarkApplication::onScanFinished()
{
    if (m_targetDeviceInfo.isValid()) {
        m_controller = QLowEnergyController::createCentral(m_targetDeviceInfo, this);
        connect(m_controller, &QLowEnergyController::connected, this, &BleBenchmarkApplication::onConnected);
        connect(m_controller, &QLowEnergyController::disconnected, this, &BleBenchmarkApplication::onDisconnected);
        connect(m_controller, &QLowEnergyController::connectionUpdated, this, &BleBenchmarkApplication::onConnectionUpdated);
        connect(m_controller, &QLowEnergyController::discoveryFinished, this, &BleBenchmarkApplication::onServiceDiscoveryFinished);
        connect(m_controller, &QLowEnergyController::serviceDiscovered, this, &BleBenchmarkApplication::onServiceDiscovered);
        connect(m_controller, &QLowEnergyController::errorOccurred, this, &BleBenchmarkApplication::onControllerErrorOccurred);
        connect(m_controller, &QLowEnergyController::mtuChanged, this, &BleBenchmarkApplication::onMtuChanged);
        connect(m_controller, &QLowEnergyController::stateChanged, this, &BleBenchmarkApplication::onControllerStateChanged);
        connect(m_controller, &QLowEnergyController::destroyed, this, &BleBenchmarkApplication::onControllerDestroyed);
        m_timer = new QTimer(m_controller);
        m_timer->setInterval(200);
        connect(m_timer, &QTimer::timeout, this, &BleBenchmarkApplication::onTimerTimeout);
        m_controller->connectToDevice();
    }
    else {
        m_discoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
    }
}

void BleBenchmarkApplication::onConnected()
{
    qDebug() << m_controller->remoteName() << m_controller->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__);

    m_controller->discoverServices();
}

void BleBenchmarkApplication::onDisconnected()
{
    qDebug() << m_controller->remoteName() << m_controller->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__);

    m_controller->deleteLater();
}

void BleBenchmarkApplication::onConnectionUpdated(const QLowEnergyConnectionParameters &parameters)
{
    qDebug() << m_controller->remoteName() << m_controller->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__)
             << "latency" << parameters.latency()
             << "maximiumInterval" << parameters.maximumInterval()
             << "minimiumInterval" << parameters.minimumInterval()
             << "supervisionTimeout" << parameters.supervisionTimeout();
}

void BleBenchmarkApplication::onServiceDiscoveryFinished()
{
    qDebug() << m_controller->remoteName() << m_controller->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__);

    if (m_service != nullptr) {
        m_service->discoverDetails();
    }
    else {
        m_controller->deleteLater();
    }
}

void BleBenchmarkApplication::onServiceDiscovered(const QBluetoothUuid &newService)
{
    if (m_service == nullptr) {
        bool ok;
        quint16 uuid = newService.toUInt16(&ok);
        qDebug() << m_controller->remoteName() << m_controller->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__) << newService;
        if (ok && uuid == SERVICE_UUID) {
            qDebug() << m_controller->remoteName() << m_controller->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__) << QString::asprintf("%04X", uuid);
            m_service = m_controller->createServiceObject(newService, this);
            connect(m_service, &QLowEnergyService::stateChanged, this, &BleBenchmarkApplication::onServiceStateChanged);
            connect(m_service, &QLowEnergyService::characteristicChanged, this, &BleBenchmarkApplication::onCharacteristicChanged);
            connect(m_service, &QLowEnergyService::characteristicRead, this, &BleBenchmarkApplication::onCharacteristicRead);
            connect(m_service, &QLowEnergyService::characteristicWritten, this, &BleBenchmarkApplication::onCharacteristicWritten);
            connect(m_service, &QLowEnergyService::descriptorRead, this, &BleBenchmarkApplication::onDescriptorRead);
            connect(m_service, &QLowEnergyService::descriptorWritten, this, &BleBenchmarkApplication::onDescriptorWritten);
            connect(m_service, &QLowEnergyService::errorOccurred, this, &BleBenchmarkApplication::onServiceErrorOccurred);
        }
    }
}

void BleBenchmarkApplication::onControllerErrorOccurred(QLowEnergyController::Error newError)
{
    qDebug() << m_controller->remoteName() << m_controller->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__)
             << newError;
    m_controller->deleteLater();
}

void BleBenchmarkApplication::onMtuChanged(int mtu)
{
    qDebug() << m_controller->remoteName() << m_controller->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__)
             << mtu;
}

void BleBenchmarkApplication::onControllerStateChanged(QLowEnergyController::ControllerState state)
{
    qDebug() << m_controller->remoteName() << m_controller->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__)
             << state;

    if (state == QLowEnergyController::UnconnectedState) {
        m_controller->deleteLater();
    }
}

void BleBenchmarkApplication::onServiceStateChanged(QLowEnergyService::ServiceState newState)
{
    qDebug() << m_controller->remoteName() << m_controller->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__)
             << newState;

    if (newState == QLowEnergyService::RemoteServiceDiscovered) {
        auto characteristics = m_service->characteristics();
        for (auto i = characteristics.constBegin(); i != characteristics.constEnd(); i++) {
            bool ok;
            quint16 uuid = i->uuid().toUInt16(&ok);

            qDebug() << m_controller->remoteName() << m_controller->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__)
                     << i->uuid() << QString::asprintf("%04X", uuid);

            if (uuid == E2P_UUID) {
                m_e2pCharacteristic = *i;
            }
            else if (uuid == P2E_UUID) {
                m_p2eCharacteristic = *i;
            }
        }

        if (m_e2pCharacteristic.isValid() && m_p2eCharacteristic.isValid()) {
            m_service->writeDescriptor(m_e2pCharacteristic.clientCharacteristicConfiguration(), QLowEnergyCharacteristic::CCCDEnableNotification);
        }
        else {
            m_controller->deleteLater();
        }
    }
}

void BleBenchmarkApplication::onCharacteristicChanged(const QLowEnergyCharacteristic &info, const QByteArray &value)
{
    qDebug() << m_controller->remoteName() << m_controller->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__)
             << info.uuid() << value.toHex(' ');

    if (info == m_e2pCharacteristic) {
//        emit dataReceived(value);
    }
}

void BleBenchmarkApplication::onCharacteristicRead(const QLowEnergyCharacteristic &info, const QByteArray &value)
{
    qDebug() << m_controller->remoteName() << m_controller->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__)
             << info.uuid() << value.toHex(' ');
}

void BleBenchmarkApplication::onCharacteristicWritten(const QLowEnergyCharacteristic &info, const QByteArray &value)
{
    qDebug() << m_controller->remoteName() << m_controller->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__)
             << info.uuid() << value.toHex(' ');

    if (info == m_e2pCharacteristic) {
//        emit dataReceived(value);
    }
}

void BleBenchmarkApplication::onDescriptorRead(const QLowEnergyDescriptor &info, const QByteArray &value)
{
    qDebug() << m_controller->remoteName() << m_controller->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__)
             << info.uuid() << value.toHex(' ');
}

void BleBenchmarkApplication::onDescriptorWritten(const QLowEnergyDescriptor &info, const QByteArray &value)
{
    qDebug() << m_controller->remoteName() << m_controller->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__)
             << info.uuid() << value.toHex(' ');

    if (info == m_e2pCharacteristic.clientCharacteristicConfiguration() && value == QLowEnergyCharacteristic::CCCDEnableNotification) {
        m_timer->start();
    }
}

void BleBenchmarkApplication::onServiceErrorOccurred(QLowEnergyService::ServiceError error)
{
    qDebug() << m_controller->remoteName() << m_controller->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__)
             << error;

    m_controller->deleteLater();
}

void BleBenchmarkApplication::onControllerDestroyed(QObject *obj)
{
    m_service = nullptr;
    m_controller = nullptr;
    m_targetDeviceInfo = QBluetoothDeviceInfo();
    m_discoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}

void BleBenchmarkApplication::onTimerTimeout()
{
    QByteArray a(240, 0x55);
    m_service->writeCharacteristic(m_p2eCharacteristic, a, QLowEnergyService::WriteWithoutResponse);
    m_sentBytes += a.size();
    qDebug() << m_controller->remoteName() << m_controller->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__) << m_sentBytes;
}
