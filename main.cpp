#include "net-manager-test.h"

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);

  myNetworkManager mynmgr;

  mynmgr.logWLANConfigs();
  mynmgr.doUpdateConfigs();

  return a.exec();
}
