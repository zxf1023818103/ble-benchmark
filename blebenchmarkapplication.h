#ifndef BLEBENCHMARKAPPLICATION_H
#define BLEBENCHMARKAPPLICATION_H

#include <QObject>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QLowEnergyController>
#include <QTimer>

class BleBenchmarkApplication : public QObject
{
    Q_OBJECT
public:
    explicit BleBenchmarkApplication(QObject *parent = nullptr);

public slots:
    void start();

private slots:
    void onDeviceDiscovered(const QBluetoothDeviceInfo &info);

    void onScanFinished();

    void onConnected();

    void onDisconnected();

    void onConnectionUpdated(const QLowEnergyConnectionParameters &parameters);

    void onServiceDiscoveryFinished();

    void onServiceDiscovered(const QBluetoothUuid &newService);

    void onControllerErrorOccurred(QLowEnergyController::Error newError);

    void onMtuChanged(int mtu);

    void onControllerStateChanged(QLowEnergyController::ControllerState state);

    void onServiceStateChanged(QLowEnergyService::ServiceState newState);

    void onCharacteristicChanged(const QLowEnergyCharacteristic &info, const QByteArray &value);

    void onCharacteristicRead(const QLowEnergyCharacteristic &info, const QByteArray &value);

    void onCharacteristicWritten(const QLowEnergyCharacteristic &info, const QByteArray &value);

    void onDescriptorRead(const QLowEnergyDescriptor &info, const QByteArray &value);

    void onDescriptorWritten(const QLowEnergyDescriptor &info, const QByteArray &value);

    void onServiceErrorOccurred(QLowEnergyService::ServiceError error);

    void onControllerDestroyed(QObject *obj);

    void onTimerTimeout();

private:

    static const quint16 SERVICE_UUID = 0xFFF0;

    static const quint16 P2E_UUID = 0xFFF1;

    static const quint16 E2P_UUID = 0xFFF2;

    QBluetoothDeviceDiscoveryAgent *m_discoveryAgent = nullptr;

    QBluetoothDeviceInfo m_targetDeviceInfo;

    QLowEnergyController *m_controller = nullptr;

    QLowEnergyService *m_service = nullptr;

    QTimer *m_timer = nullptr;

    QLowEnergyCharacteristic m_p2eCharacteristic;

    QLowEnergyCharacteristic m_e2pCharacteristic;

    quint64 m_sentBytes = 0;

signals:

};

#endif // BLEBENCHMARKAPPLICATION_H
