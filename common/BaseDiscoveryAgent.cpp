#include <common/BaseDiscoveryAgent.h>

void BaseDiscoveryAgent::query() {
	QMap<QString, BluetoothAddress>::iterator i;
	for (i = _agentCache.begin(); i != _agentCache.end(); ++i) {
		emit deviceDiscovered(i.value());
	}
}