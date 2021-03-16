#ifndef _BLE_GAP_SM_H_
#define _BLE_GAP_SM_H_
#include "ble/SecurityManager.h"
#include "ble_gap.h"

/**
 * \brief Security manager event handler class derived from CGap class
 *
 */
class CGapSecurity : private mbed::NonCopyable<CGapSecurity>,
					 public CGap,
					 public SecurityManager::EventHandler {
  protected:
	SecurityManager::SecurityIOCapabilities_t _io_capability; //!< The device IO capability
  public:
	/**
	 * \brief Construct a new CGapSecurity object
	 * \param ble A reference to one only BLE instance of the system
	 * \param eventQueue The event queue of the application
	 * \param deviceName The Bluetooth Device Name
	 * \param advLed Advertisement LED to be blinked when advertising
	 * \param connectedLed Connected LED to be lit when connected.
	 * \param ioCapability IO capability of the device
	 */
	CGapSecurity(BLE &ble,
				 events::EventQueue &eventQueue,
				 const char *deviceName,
				 SecurityManager::SecurityIOCapabilities_t ioCapability = SecurityManager::IO_CAPS_NONE,
                 PinName advLed = LED1,
				 PinName connectedLed = LED1)
		: CGap(ble, eventQueue, deviceName, advLed, connectedLed), _io_capability(ioCapability) {}
	/**
	 * \brief Override of the Stack initialization complete function
	 *
	 * \param context Initialization complete context
	 */
	void onBleStackInitComplete(BLE::InitializationCompleteCallbackContext *context) override {
		ble_error_t error;
		if (context->error) {
			std::cout << "Error during the initialisation" << std::endl;
			return;
		}
		/* If the security manager is required this needs to be called before any
		 * calls to the Security manager happen. */
		error = CGap::_ble.securityManager().init(false /* Enable bonding*/,
												  true /*Require MITM protection*/,
												  _io_capability /*IO capabilities*/,
												  NULL /*Passkey*/,
												  false /*Support data signing*/);
		ble_utils::printError(error, "_ble.securityManager().init() ");
		if (error != BLE_ERROR_NONE) {
			return;
		}
		// enable legacy pairing
		_ble.securityManager().allowLegacyPairing(true);
		// set the event handler to this object
		_ble.securityManager().setSecurityManagerEventHandler(this);
		/** This tells the stack to generate a pairingRequest event
		 * which will require this application to respond before pairing
		 * can proceed. Setting it to false will automatically accept
		 * pairing. */
		_ble.securityManager().setPairingRequestAuthorisation(true);

		CGap::onBleStackInitComplete(context);
		/*Enable privacy so we can find the keys */
		error = _ble.gap().enablePrivacy(true);

		ble_utils::printError(error, "_ble.gap().enablePrivacy() ");
		Gap::PeripheralPrivacyConfiguration_t configuration_p = {
			/* use_non_resolvable_random_address */ false,
			Gap::PeripheralPrivacyConfiguration_t::REJECT_NON_RESOLVED_ADDRESS};
		_ble.gap().setPeripheralPrivacyConfiguration(&configuration_p);
	}
	/**
	 * \brief Override of connection complete function
	 *
	 * \param event The connection complete event
	 */
	virtual void onConnectionComplete(const ble::ConnectionCompleteEvent &event) override {
		// ble_error_t error;
		ble_utils::printError(event.getStatus(), "onConnectionComplete() ");
		ble_utils::printDeviceAddress(event.getPeerAddressType(), event.getPeerAddress());
		ble::connection_handle_t handle = event.getConnectionHandle();
		/* Request a change in link security. This will be done
		 * indirectly by asking the master of the connection to
		 * change it. Depending on circumstances different actions
		 * may be taken by the master which will trigger events
		 * which the applications should deal with. */
		ble_error_t error =
			_ble.securityManager().setLinkSecurity(handle,
												   SecurityManager::SECURITY_MODE_ENCRYPTION_WITH_MITM);

		ble_utils::printError(error, "_ble.securityManager().setLinkSecuirty() ");
	}
	/**
	 * \brief Request application to accept or reject pairing. Application should respond by
	 * calling the appropriate function: acceptPairingRequest or cancelPairingRequest
	 *
	 * \param connectionHandle The connection handle of the link associated with the pairing process
	 */
	virtual void pairingRequest(ble::connection_handle_t connectionHandle) override {
		std::cout << "Pairing requested - authorising" << std::endl;
		_ble.securityManager().acceptPairingRequest(connectionHandle);
	}
	/**
	 * \brief Inform the device of the encryption state of a given link.
	 *
	 * \param connectionHandle The connection handle of the link
	 * \param result the encryption process result
	 */
	virtual void linkEncryptionResult(ble::connection_handle_t connectionHandle,
									  ble::link_encryption_t result) override {
		if (result == ble::link_encryption_t::ENCRYPTED) {
			std::cout << "Link ENCRYPTED" << std::endl;
		} else if (result == ble::link_encryption_t::ENCRYPTED_WITH_MITM) {
			std::cout << "Link ENCRYPTED_WITH_MITM" << std::endl;
		} else if (result == ble::link_encryption_t::NOT_ENCRYPTED) {
			std::cout << "Link NOT_ENCRYPTED" << std::endl;
		}
	}

	/**
	 * \brief Display the given passkey on the local device.
	 *
	 * \param connectionHandle The handle of the connection
	 * \param passkey The passkey to be displayed
	 */
	virtual void passkeyDisplay(ble::connection_handle_t connectionHandle,
								const SecurityManager::Passkey_t passkey) override {
		std::cout << "Input passKey: ";
		for (unsigned i = 0; i < Gap::ADDR_LEN; i++) {
			printf("%c ", passkey[Gap::ADDR_LEN - 1 - i]);
		}
		std::cout << std::endl;
	}
	/**
	 * \brief Indicate to the application that a confirmation is required. This is used
	 * when the device does not have a keyboard but has a yes/no button. The device
	 * displays numbers on its display in response to passkeyDisplay and the user
	 * checks if they are the same on both devices. The application should proceed
	 * by supplying the confirmation using the confirmationEntered function
	 *
	 * \param connectionHandle The handle of the connection
	 */
	virtual void confirmationRequest(ble::connection_handle_t connectionHandle) override {
		std::cout << "Confirmation required!" << std::endl;
		_ble.securityManager().confirmationEntered(connectionHandle, true);
	}

	/**
	 * \brief Indicate to the application that a passkey is required. The application should
	 * proceed by supplying the passkey through the passkeyEntered function.
	 *
	 * \param connectionHandle The handle of the connection
	 */
	virtual void passkeyRequest(ble::connection_handle_t connectionHandle) override {
		std::cout << "passkeyRequest" << std::endl;
	}

	/**
	 * \brief Notify the application that a key was pressed by the peer during passkey entry.
	 *
	 * \param connectionHandle The handle of the connection
	 * \param keypress The keypress event type
	 */
	virtual void keypressNotification(ble::connection_handle_t connectionHandle,
									  SecurityManager::Keypress_t keypress) override {
		std::cout << "keypressNotification" << std::endl;
	}

	/**
	 * \brief Delivers the CSRK to the application
	 *
	 * \param connectionHandle The handle of the connection
	 * \param csrk The Connection Signature Resolving Key
	 * \param authenticated True if authenticated
	 */
	virtual void signingKey(ble::connection_handle_t connectionHandle,
							const ble::csrk_t *csrk,
							bool authenticated) override {
		std::cout << "signingKey" << std::endl;
	}

	/**
	 * \brief Indicate to the application that pairing has completed
	 *
	 * \param connectionHandle The handle of the connect
	 * \param result The pairing completion result
	 */
	virtual void pairingResult(ble::connection_handle_t connectionHandle,
							   SecurityManager::SecurityCompletionStatus_t result) override {
		printf("Security status 0x%02x\r\n", result);
		if (result == SecurityManager::SEC_STATUS_SUCCESS) {
			std::cout << "Security success" << std::endl;
		} else {
			std::cout << "Security failed" << std::endl;
		}
	}
};
#endif //! _BLE_GAP_SM_H_
