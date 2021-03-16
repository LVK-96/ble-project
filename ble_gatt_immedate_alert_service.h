#ifndef _IMMEDIATE_ALERT_SERVICE_H_
#define _IMMEDIATE_ALERT_SERVICE_H_
#include "ble_gatt_service.h"
#include "ble_utils.h"

class CImmediateAlertServiceServer : protected mbed::NonCopyable<CImmediateAlertServiceServer>,
									 public CGattService {

  public:
	/**
	 * \brief Immediate Alert Service alert levels
	 * 
	 */
	enum AlertLevel { IAS_ALERT_LEVEL_NO_ALERT = 0, IAS_ALERT_LEVEL_MEDIUM = 1, IAS_ALERT_LEVEL_HIGH = 2 };

  protected:
	// TODO declare WriteOnly characteristic with value type uint8_t. UUID should be
	// GattCharacteristic::UUID_ALERT_LEVEL_CHAR. Use appropriate class in ble_gatt_characteristic.h.
	GattCharacteristic *_characteristics[1]; //!< The characteristics of the service
	//TODO declare a callback with type void(uint8_t) to be called when alert level written 

  public:
	/**
	 * \brief Construct a new CImmediateAlertServiceServer object
	 */
	CImmediateAlertServiceServer()
		: CGattService(GattService::UUID_IMMEDIATE_ALERT_SERVICE, _characteristics, 1){
		_characteristics[0] = nullptr; //TODO write here the address of your characteristic object
	}
	/**
	 * \brief on Connection handler of the service
	 *
	 */
	virtual void onConnection() override {
		// not required for this service. Leave it empty if your implementation does not require handling onConnection event.
	}
	/**
	 * \brief on Disconnection handler of the service
	 *
	 */
	virtual void onDisconnection() override {
		// not required for this service. Leave it empty if your implementation does not require handling onDisconnection event.
	}
	/**
	 * \brief on Read handler of the service
	 *
	 * \param handle The attribute handle of the characteristic value attribute
	 */
	virtual void onRead(uint16_t handle) override { 
		(void)handle; 
		// not required for this service. Leave it empty if your implementation does not require handling onRead of your characteristic.
	}
	/**
	 * \brief onWrite handler of the service
	 *
	 * \param handle The attribute handle of the characteristic value attribute
	 */
	virtual void onWrite(uint16_t handle) override {
		/* TODO
		 * 1. Check whether the handle is equal to the handle of your characteric using .getValueHandle() member function.
		 * 2. Read characteristic value using .get() member of the charecteristic. Print the result code of get() using ble_utils::printError() function.
		 * 3. if your onAlertLevel callback is configured, call that function
		 */
	}

	/**
	 * \brief Enables/disables authentication for alert level characteristics
	 *
	 * \param enable True enable, False to disable authentication
	 */
	virtual void enableAuthentication(bool enable = true) override {
		if (enable != false) {
			// TODO set the Write Security Requirement of your characteristic to ble::att_security_requirement_t::AUTHENTICATED
		} else {
			// TODO set the Write Security Requirement of your characteristic to ble::att_security_requirement_t::NONE
		}
	}

	void setOnAlertLevelWritten(const mbed::Callback<void(uint8_t)> &callback) {
		//TODO assign your onAlertLevel callback object to callback
	}
};

#endif //!_IMMEDIATE_ALERT_SERVICE_H_