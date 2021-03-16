#ifndef _BLE_GATT_SERVICE__H
#define _BLE_GATT_SERVICE__H

#include "BLE.h"
#include "ble_gatt_characteristic.h"
#include "mbed.h"

#include <set>
/**
 * \brief Pure virtual interface class for all services
 *
 */
class CGattService : protected mbed::NonCopyable<CGattService>, public GattService {
  protected:
	GattServer *_server; //!< The associated Gatt service
	std::set<GattCharacteristic *, decltype(compareCharacteristics) *>
		_characteristics_set; //!< The characteristics of the service

  public:
	/**
	 * \brief Construct a new CGattService object
	 *
	 * \param uuid The service UUID
	 * \param characteristics An array of service characteristics
	 * \param numCharacteristic Number of characteristics
	 */
	CGattService(const UUID &uuid, GattCharacteristic *characteristics[], unsigned numCharacteristic)
		: GattService(uuid, characteristics, numCharacteristic), _server(nullptr),
		  _characteristics_set(compareCharacteristics) {}

	/**
	 * \brief Set the Server object
	 *
	 * \param server The Gatt Server
	 */
	virtual void setServer(GattServer *server) { _server = server; }
	/**
	 * \brief Get the Server object
	 *
	 * \param server The Gatt server
	 */
	virtual GattServer *getServer(GattServer *server) { return _server; }

	/**
	 * \brief Get the Characteristics set object
	 *
	 * \return std::set<GattAttribute *, decltype(compareCharacteristics) *>&
	 */
	virtual std::set<GattCharacteristic *, decltype(compareCharacteristics) *> &getCharacteristics() {
		uint8_t charCount = GattService::getCharacteristicCount();
		_characteristics_set.clear();
		for (int ii = 0; ii < (int)charCount; ii++) {
			auto c = GattService::getCharacteristic((uint8_t)ii);
			_characteristics_set.insert(c);
		}

		return _characteristics_set;
	}
	/**
	 * \brief On connection to peer handler that must be implemented by the dervied class
	 *
	 */
	virtual void onConnection() = 0;
	/**
	 * \brief On disconnection from peer handler that must be implemented by the dervied class
	 *
	 */
	virtual void onDisconnection() = 0;

	/**
	 * \brief On write by the peer handler that must be implemented by the dervied class
	 * 
	 * \param handle the handle of the attribute
	 */
	virtual void onWrite(uint16_t handle) = 0;
	/**
	 * \brief On write by the peer handler that must be implemented by the dervied class
	 * 
	 * \param handle the handle of the attribute
	 */
	virtual void onRead(uint16_t handle) = 0;
	/**
	 * \brief Enable authentication that must be implemented by the dervied class
	 * 
	 * \param enable True to enable authentication, False to disable authentication 
	 */
	virtual void enableAuthentication(bool enable = true) = 0;

	/**
	 * \brief Checks whether service contains specfied characteristics value attribute handle 
	 * 
	 * \param handle The handle to to be checked
	 * \return true if service contains that handle
	 * \return false if service does not contains that handle
	 */
	bool contains(uint16_t handle) {
		if (_characteristics_set.size() != GattService::getCharacteristicCount()) {
			getCharacteristics();
		}

		for (auto c : _characteristics_set) {
			if (handle == c->getValueHandle()) {
				return true;
			}
		}
		return false;
	}
};

bool compareServices(const GattService &obj1, const GattService &obj2) {
	return (obj1.getHandle() <= obj2.getHandle());
}

typedef std::set<CGattService *> CGattServicesSet;
#endif //! _BLE_GATT_SERVICE__H
