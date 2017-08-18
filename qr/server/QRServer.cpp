#include "MMPush.h"
#include "QRPeer.h"
#include "QRServer.h"
#include "QRDriver.h"
#include <iterator>
namespace QR {

	Server::Server() : INetServer() {
		
	}

	Server::~Server() {
		
	}
	void Server::init() {
		MM::SetupTaskPool(this);
	}
	void Server::tick() {
		NetworkTick();
	}
	void Server::shutdown() {

	}
	void Server::SetTaskPool(OS::TaskPool<MM::MMPushTask, MM::MMPushRequest> *pool) {
		const std::vector<MM::MMPushTask *> task_list = pool->getTasks();
		std::vector<MM::MMPushTask *>::const_iterator it = task_list.begin();
		while (it != task_list.end()) {
			MM::MMPushTask *task = *it;
			std::vector<INetDriver *>::iterator it2 = m_net_drivers.begin();
			while (it2 != m_net_drivers.end()) {
				QR::Driver *driver = (QR::Driver *)*it2;
				task->AddDriver(driver);
				it2++;
			}
			it++;
		}
	}

}