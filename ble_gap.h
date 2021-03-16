#ifndef _BLE_GAP_H
#define _BLE_GAP_H

#include <mbed.h>

#include "ble/BLE.h"
#include "ble/Gap.h"
#include "ble/GapAdvertisingData.h"
#include "ble/GapAdvertisingParams.h"
#include "ble_utils.h"
/**
 * \brief
 *
 */
class CGap : private mbed::NonCopyable<CGap>, public ble::Gap::EventHandler {
  protected:
	BLE &_ble;						 //!< The one and only BLE instance of the system
	events::EventQueue &_eventQueue; //!< The event queue of the system
	const char *_deviceName;		 //!< The name of the device
	DigitalOut _advertisementLed;	 //!< The Advertisement LED that blinks when the device is advertising
	DigitalOut
		_connectedLed; //!< The Connected LED that turns on/off when the device connection state changes

	uint8_t _advertisementDataBuffer[ble::LEGACY_ADVERTISING_MAX_SIZE]; //!< The advertisement data buffer to
																		//! be used by the builder
	ble::AdvertisingDataBuilder
		_advertisementDataBuilder; //!< The advertisement data builder to build advertisement data structures
	mbed::Callback<void(void)> _onInitComplete; //!< The user configurable callback to be called when the
												//! stack initialization completes
	mbed::Callback<void(void)>
		_onConnection; //!< The user configurable callback to be called when connection completes
	mbed::Callback<void(void)>
		_onDisconnection; //!< The user configurable function to be called when peer device disconnects

	bool _advertising; //!< The advertising flag. Set/Cleared when advertsing state changes.
	bool _connected;   //!< The connected flag. Set/Cleared when connection state changes

  protected:
	/**
	 * \brief Called when connection attempt ends or an advertising device has been connected.
	 *
	 * \param event The connection complete event object
	 */
	virtual void onConnectionComplete(const ble::ConnectionCompleteEvent &event) override {
		ble_utils::printError(event.getStatus(), "onConnectionComplete() ");
		ble_utils::printDeviceAddress(event.getPeerAddressType(), event.getPeerAddress());
		_connected = true;
		// call the user callback
		if(_onConnection){
			_onConnection();
		}
	}

	/**
	 * \brief Called when advertising ends.
	 *
	 * \details Advertising ends when the process timeout or if it is stopped by the
	 * application or if the local device accepts a connection request.
	 *
	 *
	 * \param event The advertisement end event object
	 */
	void onAdvertisingEnd(const ble::AdvertisingEndEvent &event) override {
		_advertising = false;
		// turn off the led
		_advertisementLed = 1;
		std::cout << "onAdvertisingEnd(). Connected " << event.isConnected() << std::endl;
	}

	/**
	 * \brief Called when a peer connected to this device is disconnected for some reason
	 *
	 * \param event Disconnection complete event
	 */
	void onDisconnectionComplete(const ble::DisconnectionCompleteEvent &event) override {
		std::cout << "onDisconnectionComplete(). Reason ";
		switch (event.getReason().value()) {
		case ble::disconnection_reason_t::type::AUTHENTICATION_FAILURE:
			std::cout << "AUTHENTICATION FAILURE" << std::endl;
			break;
		case ble::disconnection_reason_t::type::CONNECTION_TIMEOUT:
			std::cout << "CONNECTION TIMEOUT" << std::endl;
			break;
		case ble::disconnection_reason_t::type::REMOTE_USER_TERMINATED_CONNECTION:
			std::cout << "REMOTE USER TERMINATED CONNECTION" << std::endl;
			break;
		case ble::disconnection_reason_t::type::REMOTE_DEV_TERMINATION_DUE_TO_LOW_RESOURCES:
			std::cout << "REMOTE DEVICE HAS LOW RESOURCES" << std::endl;
			break;
		case ble::disconnection_reason_t::type::REMOTE_DEV_TERMINATION_DUE_TO_POWER_OFF:
			std::cout << "REMOTE DEVICE POWER OFF" << std::endl;
			break;
		case ble::disconnection_reason_t::type::LOCAL_HOST_TERMINATED_CONNECTION:
			std::cout << "LOCAL HOST TERMINATED CONNECTION" << std::endl;
			break;
		case ble::disconnection_reason_t::type::UNACCEPTABLE_CONNECTION_PARAMETERS:
			std::cout << "UNACCEPTABLE CONNECTION PARAMETERS" << std::endl;
			break;
		default:
			std::cout << "UNKNOWN" << std::endl;
			break;
		}
		// turn off the led
		_connectedLed = 1;
		_connected = false;
		// start advertising
		startAdvertising();
		// call the user callback
		if(_onDisconnection){
			_onDisconnection();
		}
	}

	/**
	 * \brief This function is called when the MTU size negotiation completes.
	 *
	 * \param connectionHandle The connection handle
	 * \param txSize The new MTU size for transmissions
	 * \param rxSize The new MTU size for receptions
	 */
	void onDataLengthChange(ble::connection_handle_t connectionHandle,
							uint16_t txSize,
							uint16_t rxSize) override {
		std::cout << "BLE Connection Data Length negotiated for connection: " << connectionHandle
				  << " txSize " << (int)txSize << " rxSize " << (int)rxSize << std::endl;
	}
	/**
	 * \brief Starts the advertisement process. This function is called when the system is initialized
	 * 		  or when the device state is changed to disconnected.
	 *
	 */
	void startAdvertising() {
		// adjust the advertisement parameters, set the advertisement events,
		// and start advertising
		ble::AdvertisingParameters adv_parameters(ble::advertising_type_t::CONNECTABLE_UNDIRECTED,
												  ble::adv_interval_t(ble::millisecond_t(100)));

		_advertisementDataBuilder.setFlags();
		_advertisementDataBuilder.setName(_deviceName);

		/* Setup advertising */

		ble_error_t error =
			_ble.gap().setAdvertisingParameters(ble::LEGACY_ADVERTISING_HANDLE, adv_parameters);
		ble_utils::printError(error, "_ble.gap().setAdvertisingParameters() ");

		error = _ble.gap().setAdvertisingPayload(ble::LEGACY_ADVERTISING_HANDLE,
												 _advertisementDataBuilder.getAdvertisingData());
		ble_utils::printError(error, "_ble.gap().setAdvertisingPayload() ");

		/* Start advertising */

		error = _ble.gap().startAdvertising(ble::LEGACY_ADVERTISING_HANDLE);
		ble_utils::printError(error, "_ble.gap().startAdvertising() ");
		if (error == BLE_ERROR_NONE) {
			_advertising = true;
			_connectedLed = 1;
			_advertisementLed = 0;
		}
	}

	/**
	 * The device state LED display callback
	 */
	void showDeviceState() {
		if (_connected) {
			_connectedLed = 0;
		} else {
			if (_advertising) {
				_advertisementLed = !_advertisementLed;
			}
		}
	}

	/**
	 * \brief Called by BLE stack when there are some BLE events to be processed.
	 *
	 * \param context The event context
	 */
	void scheduleBLEEvents(BLE::OnEventsToProcessCallbackContext *context) {
		_eventQueue.call(Callback<void()>(&context->ble, &BLE::processEvents));
	}

  public:
	/**
	 * \brief Construct a new CGap object
	 *
	 * \param ble A reference to one only BLE instance of the system
	 * \param eventQueue The event queue of the application
	 * \param deviceName The Bluetooth Device Name
	 * \param advLed Advertisement LED to be blinked when advertising
	 * \param connectedLed Connected LED to be lit when connected.
	 */
	CGap(BLE &ble,
		 events::EventQueue &eventQueue,
		 const char *deviceName,
		 PinName advLed = LED1,
		 PinName connectedLed = LED1)
		: ble::Gap::EventHandler(), _ble(ble), _eventQueue(eventQueue), _deviceName(deviceName),
		  _advertisementLed(advLed, 1), _connectedLed(connectedLed, 1),
		  _advertisementDataBuilder(_advertisementDataBuffer), _onInitComplete(), _onConnection(),
		  _onDisconnection(), _advertising(false), _connected(false) {}
	~CGap() {
		if (_ble.hasInitialized()) {
			_ble.shutdown();
		}
	}
	/**
	 * \brief Callback called when the BLE stack initialization completes
	 * \param context Initialization complete context
	 */
	virtual void onBleStackInitComplete(BLE::InitializationCompleteCallbackContext *context) {
		ble_utils::printError(context->error, "BLE Stack initialization completed with code ");
		if (context->error != BLE_ERROR_NONE) {
			std::cout << "BLE stack initialization completed with error!" << std::endl;
		} else {
			Gap::AddressType_t addr_type;
			Gap::Address_t address;
			_ble.gap().getAddress(&addr_type, address);
			ble_utils::printDeviceAddress(addr_type, address);

			// set the devicename characteristics of the GAP
			_ble.gap().setDeviceName(reinterpret_cast<const std::uint8_t *>(_deviceName));

			startAdvertising();
			if (_onInitComplete) {
				_onInitComplete();
			}
		}
	}
	/**
	 * The run function of the GAP implementation
	 */
	void run() {
		ble_error_t error;

		_ble.onEventsToProcess(makeFunctionPointer(this, &CGap::scheduleBLEEvents));
		// register BLE init complete callback to the function of this class
		error = _ble.init(this, &CGap::onBleStackInitComplete);
		if (error != BLE_ERROR_NONE) {
			std::cout << "BLE stack initialization completed with error " << error << std::endl;
			return;
		}

		// set the GAP event handler
		_ble.gap().setEventHandler(this);
		_eventQueue.call_every(500, this, &CGap::showDeviceState);
		// dispatch the event queue forever
		_eventQueue.dispatch_forever();
	}

	/**
	 * \brief Sets the setOnInitCallback function to be called when initialization completes.
	 * \details This function is sets a callback object, which is called
	 * 			when the stack initialization completes. The callback can be used by the
	 * 			application to trigger certain operations in the application side.
	 *
	 * \param callback The callback object. If this is nullptr, it disables callback calling.
	 */
	void setOnInitCallback(mbed::Callback<void(void)> callback) { _onInitComplete = callback; }

	void setOnConnection(mbed::Callback<void(void)> callback) { _onConnection = callback; }

	void setOnDisconnection(mbed::Callback<void(void)> callback) { _onDisconnection = callback; }
};

#endif //!_BLE_GAP_H
