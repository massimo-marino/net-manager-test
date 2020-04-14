#pragma once

#include <QCoreApplication>
#include <QNetworkConfigurationManager>
#include <QDebug>

class myNetworkManager : public QNetworkConfigurationManager
{
  Q_OBJECT

  private:
   mutable QList<QNetworkConfiguration> _configList;

  public:
    myNetworkManager (QObject* parent = nullptr) :
    QNetworkConfigurationManager(parent),
    _configList()
    {}

    void doUpdateConfigs() const
    {
      QObject::connect(this, &QNetworkConfigurationManager::updateCompleted, this, &myNetworkManager::updateConfigList);
    }

    void doNotUpdateConfigs() const
    {
      QObject::disconnect(this, &QNetworkConfigurationManager::updateCompleted, this, &myNetworkManager::updateConfigList);
    }

    void logWLANConfigs ()
    {
      QList<QNetworkConfiguration> configList {getWLANList()};
      if ( 0 == configList.size() ) {
          qDebug() << "WLAN Config list is empty.";
          return;
      }
      for (const QNetworkConfiguration& wlanConfig : configList) {
        qDebug() << "WLAN Name:" << wlanConfig.name()
                 << "state:" << wlanConfig.state()
                 << "bearer type name:" << wlanConfig.bearerTypeName()
                 << "bearer type family:" << wlanConfig.bearerTypeFamily()
                 << "identifier:" << wlanConfig.identifier();
      }
      qDebug() << "";
    }

    // https://bugreports.qt.io/browse/QTBUG-66461
    QList<QNetworkConfiguration> getWLANList ()
    {
      updateNow();

      QList<QNetworkConfiguration> list;

      for (const QNetworkConfiguration& netConfig : allConfigurations()) {
        if ( QNetworkConfiguration::BearerWLAN == netConfig.bearerType() ) {
          list.append(netConfig);
        }
      }
      return list;
    }

    void updateNow ()
    {
      QEventLoop eventLoop;

      QObject::connect(this, &myNetworkManager::updated, &eventLoop, &QEventLoop::quit, Qt::QueuedConnection);

      updateConfigurations();
      eventLoop.exec();
    }

    void updateConfigurations ()
    {
      QEventLoop eventLoop;

      QObject::connect(this, &myNetworkManager::updateCompleted, &eventLoop, &QEventLoop::quit);
      QNetworkConfigurationManager::updateConfigurations();
      eventLoop.exec();
      emit updated();
    }

    void logConfigs () const
    {
      for (const QNetworkConfiguration& netConfig : _configList) {
        qDebug() << "logConfigs >"
                 << "Name:" << netConfig.name()
                 << "state:" << netConfig.state()
                 << "bearer type name:" << netConfig.bearerTypeName()
                 << "bearer type family:" << netConfig.bearerTypeFamily()
                 << "identifier:" << netConfig.identifier();
      }
      qDebug() << "";
    }

  public slots:
    // called every 10 seconds
    void updateConfigList () const
    {
      _configList.clear();
      for (const QNetworkConfiguration& netConfig : allConfigurations()) {
        _configList.append(netConfig);
      }
      logConfigs();
    }

  signals:
    void updated() const;

};  // class myNetworkManager
