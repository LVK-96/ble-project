#ifndef _BLE_GATT_SERVER_H_
#define _BLE_GATT_SERVER_H_

#include "BLE.h"
#include "ble_utils.h"
#include "mbed.h"

#include <ble_gatt_service.h>
#include <sstream>
/**
 * The GATT server class used by the system. This class has all the services the system has implemented.
 */
class CGattServer {
  protected:
	CGattServicesSet _services;

	//!< the GATT server
	GattServer *_server;
	//!< the event queue of the application
	events::EventQueue &_eventQueue;
	//!< the one only BLE instance
	BLE &_ble;

  private:
	/**
	 * Handler called when a notification or an indication has been sent.
	 */
	void onDataSent(unsigned count) { std::cout << "onDataSent() for " << count << " updates" << std::endl; }

	/**
	 * Handler called after an attribute has been written.
	 */
	void onDataWritten(const GattWriteCallbackParams *e) {
		std::cout << "onDataWritten() using Conn. Handle 0x" << HEX_SHORT_IOSTREAM(e->connHandle)
				  << " for Att. Handle 0x" << HEX_SHORT_IOSTREAM(e->handle) << std::endl;
		std::cout << "\twrite operation: " << e->writeOp << std::endl;
		std::cout << "\toffset: " << e->offset << std::endl;
		std::cout << "\tlength: " << e->len << std::endl;
		std::cout << "\tdata: ";

		for (size_t i = 0; i < e->len; ++i) {
			printf("%02X ", e->data[i]);
		}

		std::cout << std::endl;

		for (auto s : _services) {
			s->onWrite(e->handle);
		}
	}

	/**
	 * Handler called after an attribute has been read.
	 */
	void onDataRead(const GattReadCallbackParams *e) {
		std::cout << "onDataRead() using Conn. Handle 0x" << HEX_SHORT_IOSTREAM(e->connHandle)
				  << " for Att. Handle 0x" << HEX_SHORT_IOSTREAM(e->handle) << std::endl;
	}

	/**
	 * Handler called after a client has subscribed to notification or indication.
	 *
	 * @param handle Handle of the characteristic value affected by the change.
	 */
	void onUpdatesEnabled(GattAttribute::Handle_t handle) {
		std::cout << "Updates enabled on handle 0x" << HEX_SHORT_IOSTREAM(handle) << std::endl;
	}

	/**
	 * Handler called after a client has cancelled his subscription from
	 * notification or indication.
	 *
	 * @param handle Handle of the characteristic value affected by the change.
	 */
	void onUpdatesDisabled(GattAttribute::Handle_t handle) {
		std::cout << "Updates disabled on handle 0x" << HEX_SHORT_IOSTREAM(handle) << std::endl;
	}

	/**
	 * Handler called when an indication confirmation has been received.
	 *
	 * @param handle Handle of the characteristic value that has emitted the
	 * indication.
	 */
	void onConfirmationReceived(GattAttribute::Handle_t handle) {
		std::cout << "Confirmation received on handle 0x" << HEX_SHORT_IOSTREAM(handle) << std::endl;
	}

  public:
	/**
	 * The full constructor
	 */
	CGattServer(BLE &ble, events::EventQueue &eventQueue, CGattServicesSet &&services)
		: _server(nullptr), _services(services), _eventQueue(eventQueue), _ble(ble) {}
	/**
	 * Starts the GATT service. This function is should be called when the
	 * the BLE stack is initialized
	 */
	void start() {
		_server = &_ble.gattServer();

		// register the service
		std::cout << "Adding the service" << std::endl;
		int ii = 0;
		for (auto s : _services) {
			s->setServer(_server);
			ble_error_t err = _server->addService(*s);
			std::ostringstream sstr;
			sstr << "GATTServer->addService() " << ii << " ";
			ble_utils::printError(err, sstr.str().c_str());
		}

		// read write handler
		_server->onDataSent(makeFunctionPointer(this, &CGattServer::onDataSent));
		_server->onDataWritten(makeFunctionPointer(this, &CGattServer::onDataWritten));
		_server->onDataRead(makeFunctionPointer(this, &CGattServer::onDataRead));

		// updates subscribtion handlers
		_server->onUpdatesEnabled(makeFunctionPointer(this, &CGattServer::onUpdatesEnabled));
		_server->onUpdatesDisabled(makeFunctionPointer(this, &CGattServer::onUpdatesDisabled));
		_server->onConfirmationReceived(makeFunctionPointer(this, &CGattServer::onConfirmationReceived));

		// print the handles
		int ss = 0;
		for (auto s : _services) {
			std::cout << "\tService " << ss << " Handle 0x" << HEX_SHORT_IOSTREAM(s->getHandle())
					  << " registered." << std::endl;
			auto chars = s->getCharacteristics();
			int ii = 0;
			for (auto c : chars) {
				std::cout << "\t\tCharacteristic " << ii << " UUID 0x"
						  << HEX_SHORT_IOSTREAM(c->getValueAttribute().getUUID().getShortUUID())
						  << " value handle 0x" << HEX_SHORT_IOSTREAM(c->getValueHandle()) << std::endl;
				ii++;
			}
			ss++;
		}
	}

	/**
	 * \brief Get the Service object
	 *
	 * \return CGattServicesSet&
	 */
	CGattServicesSet &getService() { return _services; }

	/**
	 * \brief Called when a peer is connected
	 *
	 */
	void onConnection() {
		for (auto s : _services) {
			s->onConnection();
		}
	}
	/**
	 * \brief Called when a peer is disconnected
	 *
	 */
	void onDisconnection() {
		for (auto s : _services) {
			s->onDisconnection();
		}
	}
};

#endif //! _BLE_GATT_SERVER_H_
