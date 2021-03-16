#ifndef _ALERT_NOTIFICATION_SERVICE_H_
#define _ALERT_NOTIFICATION_SERVICE_H_

#include "ble/GattServer.h"
#include "ble/GattService.h"
#include "ble_gatt_characteristic.h"
#include "ble_gatt_service.h"
#include "ble_utils.h"

#include <set>


/**
 * \brief Alert notification service server class
 * \details This service is defined in
 * https://www.bluetooth.com/specifications/specs/alert-notification-service-1-0/ and composed of 5
 * characteristics. The purpose of this service is to implement notifying alert status to the client.
 */
class CAlertNotificationServiceServer : public CGattService{
  public:
	/**
	 * \brief Alert notification control point commands, as defined in the Alert Notification Specification.
	 * UUID: 0x2A44.
	 */
	enum CategoryId {
		ANS_TYPE_SIMPLE_ALERT = 0,			 /**< General text alert or non-text alert.*/
		ANS_TYPE_EMAIL = 1,					 /**< Email message arrives.*/
		ANS_TYPE_NEWS = 2,					 /**< News feeds such as RSS, Atom.*/
		ANS_TYPE_NOTIFICATION_CALL = 3,		 /**< Incoming call.*/
		ANS_TYPE_MISSED_CALL = 4,			 /**< Missed call.*/
		ANS_TYPE_SMS_MMS = 5,				 /**< SMS or MMS message arrives.*/
		ANS_TYPE_VOICE_MAIL = 6,			 /**< Voice mail.*/
		ANS_TYPE_SCHEDULE = 7,				 /**< Alert that occurs on calendar, planner.*/
		ANS_TYPE_HIGH_PRIORITIZED_ALERT = 8, /**< Alert to be handled as high priority.*/
		ANS_TYPE_INSTANT_MESSAGE = 9,		 /**< Alert for incoming instant messages.*/
		ANS_TYPE_ALL_ALERTS = 0xFF			 /**< Identifies all alerts. */
	};
	enum CategoryMaskId {
		ANS_TYPE_MASK_SIMPLE_ALERT = (1 << 0),			 /**< General text alert or non-text alert.*/
		ANS_TYPE_MASK_EMAIL = (1 << 1),					 /**< Email message arrives.*/
		ANS_TYPE_MASK_NEWS = (1 << 2),					 /**< News feeds such as RSS, Atom.*/
		ANS_TYPE_MASK_NOTIFICATION_CALL = (1 << 3),		 /**< Incoming call.*/
		ANS_TYPE_MASK_MISSED_CALL = (1 << 4),			 /**< Missed call.*/
		ANS_TYPE_MASK_SMS_MMS = (1 << 5),				 /**< SMS or MMS message arrives.*/
		ANS_TYPE_MASK_VOICE_MAIL = (1 << 6),			 /**< Voice mail.*/
		ANS_TYPE_MASK_SCHEDULE = (1 << 7),				 /**< Alert that occurs on calendar, planner.*/
		ANS_TYPE_MASK_HIGH_PRIORITIZED_ALERT = (1 << 8), /**< Alert to be handled as high priority.*/
		ANS_TYPE_MASK_INSTANT_MESSAGE = (1 << 9),		 /**< Alert for incoming instant messages.*/
		ANS_TYPE_MASK_ALL_ALERTS = 0x03FF				 /**< Identifies all alerts. */
	};
	/**
	 * \brief Alert notification control point commands, as defined in the Alert Notification Specification.
	 * UUID: 0x2A44
	 */
	enum CommandId {
		ANS_ENABLE_NEW_INCOMING_ALERT_NOTIFICATION = 0,		/**< Enable New Incoming Alert Notification.*/
		ANS_ENABLE_UNREAD_CATEGORY_STATUS_NOTIFICATION = 1, /**< Enable Unread Category Status Notification.*/
		ANS_DISABLE_NEW_INCOMING_ALERT_NOTIFICATION = 2,	/**< Disable New Incoming Alert Notification.*/
		ANS_DISABLE_UNREAD_CATEGORY_STATUS_NOTIFICATION =
			3,											   /**< Disable Unread Category Status Notification.*/
		ANS_NOTIFY_NEW_INCOMING_ALERT_IMMEDIATELY = 4,	   /**< Notify New Incoming Alert immediately.*/
		ANS_NOTIFY_UNREAD_CATEGORY_STATUS_IMMEDIATELY = 5, /**< Notify Unread Category Status immediately.*/
	};
	/**
	 * \brief The ANS control point commands
	 *
	 */
	union control_point_t {
		uint16_t value;
		struct control_point_fields {
			uint8_t command;
			uint8_t category;
		} fields;
	};
	/**
	 * \brief Alert status data structure
	 *
	 */
	union alert_status_t {
		uint16_t value;
		struct alert_status_fields {
			uint8_t category; //!< The alert category
			uint8_t count;	  //!< The number of unread alerts
		} fields;
	};

  private:
	uint16_t _supported_new_alert_category;	   //!< supported new alerts configuration
	uint16_t _supported_unread_alert_category; //!< supported unread alert configuration
	uint16_t _enabled_new_alert_category;	   //!< Enabled new alert alerts by the current client
	uint16_t _enabled_unread_alert_category;   //!< Enabled unread alerts by the current client

	alert_status_t _alert_status[10]; //!< Alert status for each supported alert

	/**
	 * \brief The service characteristics
	 * @{
	 */
	CReadOnlyCharacteristic<uint16_t> _supported_new_alert_category_characteristic;
	CReadOnlyCharacteristic<uint16_t> _supported_unread_alert_category_characteristic;
	CNotifyOnlyCharacteristic<uint16_t> _unread_alert_status_characteristic;
	CNotifyOnlyCharacteristic<uint16_t> _new_alert_characteristic;
	CWriteOnlyCharacteristic<uint16_t> _alert_notification_control_point_characteristic;

	GattCharacteristic *_characteristics[5];
	/** }@*/

	bool _connected; //!< Connected flag

  public:
	/**
	 * \brief Construct a new CAlertNotificationServiceServer object
	 *
	 * \param supportedNewAlerts A bit field of supported new alert notifications
	 * \param supportedUnreadAlerts A bit field of supported unread alert notifications
	 */
	CAlertNotificationServiceServer(const uint16_t supportedNewAlerts,
									const uint16_t supportedUnreadAlerts)
		: CGattService(GattService::UUID_ALERT_NOTIFICATION_SERVICE, _characteristics, 5),
		  _supported_new_alert_category_characteristic(
			  GattCharacteristic::UUID_SUPPORTED_NEW_ALERT_CATEGORY_CHAR,
			  supportedNewAlerts),
		  _supported_unread_alert_category_characteristic(
			  GattCharacteristic::UUID_SUPPORTED_UNREAD_ALERT_CATEGORY_CHAR,
			  supportedUnreadAlerts),
		  _unread_alert_status_characteristic(GattCharacteristic::UUID_UNREAD_ALERT_CHAR, 0),
		  _new_alert_characteristic(GattCharacteristic::UUID_NEW_ALERT_CHAR, 0),
		  _alert_notification_control_point_characteristic(
			  GattCharacteristic::UUID_ALERT_NOTIFICATION_CONTROL_POINT_CHAR,
			  0) {
		_characteristics[0] = &_supported_new_alert_category_characteristic;
		_characteristics[1] = &_supported_unread_alert_category_characteristic;
		_characteristics[2] = &_unread_alert_status_characteristic;
		_characteristics[3] = &_new_alert_characteristic;
		_characteristics[4] = &_alert_notification_control_point_characteristic;

		_supported_new_alert_category = supportedNewAlerts;
		_supported_unread_alert_category = supportedUnreadAlerts;

		for (int ii = 0; ii < 10; ii++) {
			_alert_status[ii].fields.category = (uint8_t)ii;
			_alert_status[ii].fields.count = 0;
		}
		_connected = false;
	}
	/**
	 * \brief Adds a new alert
	 *
	 * \param category
	 * \\return true if the category is not supported by the service
	 * \\return false otherwise
	 */
	bool newAlert(CAlertNotificationServiceServer::CategoryId category) {
		if ((int)category >= 10) {
			return false;
		}
		uint16_t mask = _supported_new_alert_category | _supported_unread_alert_category;
		uint16_t categoryMask = (uint16_t)(1 << category);
		// check if we are supporting this category
		if ((mask & categoryMask) == 0) {
			std::cout << "\t ANS new Alert for an unsupported category " << category << std::endl;
			return false;
		}
		_alert_status[(int)category].fields.count++;
		// check if this alert category is enabled for notification
		mask = _enabled_new_alert_category;
		if ((mask & categoryMask) != 0) {
			ble_error_t error = _new_alert_characteristic.set(_server, _alert_status[(int)category].value);
			ble_utils::printError(error, "CCharacteristic.set() ");
		}
		mask = _enabled_unread_alert_category;
		if ((mask & categoryMask) != 0) {
			ble_error_t error =
				_unread_alert_status_characteristic.set(_server, _alert_status[(int)category].value);
			//_alert_status[(int)category].fields.count = 0;
			ble_utils::printError(error, "CCharacteristic.set() ");
		}
		mask = _enabled_new_alert_category | _enabled_unread_alert_category;
		if ((mask & categoryMask) == 0) {
			std::cout << "\t ANS new Alert for disabled category " << category << " count "
					  << (int)_alert_status[(int)category].fields.count << std::endl;
		} else {
			std::cout << "\t ANS new Alert for enabled category " << category << " count "
					  << (int)_alert_status[(int)category].fields.count << std::endl;
		}
		return true;
	}

	/**
	 * \brief should be called when a peer is connected to the server
	 *
	 */
	virtual void onConnection() override {
		_enabled_new_alert_category = 0;
		_enabled_unread_alert_category = 0;
		_connected = 0;
	}
	/**
	 * \brief should be called when the connected peer is disconnected
	 *
	 */
	virtual void onDisconnection() override { _connected = false; }
	/**
	 * \brief Should be called when data is written to Gatt Server Attributes
	 *
	 * \param handle
	 */
	virtual void onWrite(uint16_t handle) override {
		if (handle == _alert_notification_control_point_characteristic.getValueHandle()) {
			control_point_t controlPointValue;
			uint16_t value;
			CategoryId category;
			_alert_notification_control_point_characteristic.get(_server, value);
			controlPointValue.value = value;
			category = (CategoryId)controlPointValue.fields.category;
			std::cout << "\tANS Control Point Written: Command " << (int)controlPointValue.fields.command
					  << " Category " << (int)controlPointValue.fields.category << std::endl;
			switch ((CommandId)controlPointValue.fields.command) {
			case ANS_ENABLE_NEW_INCOMING_ALERT_NOTIFICATION:
				if (category == ANS_TYPE_ALL_ALERTS) {
					_enabled_new_alert_category = 0x03FF;
				} else {
					_enabled_new_alert_category |= (1 << (int)category);
				}
				std::cout << "\tANS New Incoming Alert Enabled for Category "
						  << (int)controlPointValue.fields.category << std::endl;
				break;
			case ANS_ENABLE_UNREAD_CATEGORY_STATUS_NOTIFICATION:
				if (category == ANS_TYPE_ALL_ALERTS) {
					_enabled_unread_alert_category = 0x03FF;
				} else {
					_enabled_unread_alert_category |= (1 << (int)category);
				}
				std::cout << "\tANS Unread Alert Enabled for Category "
						  << (int)controlPointValue.fields.category << std::endl;
				break;
			case ANS_DISABLE_NEW_INCOMING_ALERT_NOTIFICATION:
				if (category == ANS_TYPE_ALL_ALERTS) {
					_enabled_new_alert_category = 0x0;
				} else {
					_enabled_new_alert_category &= ~(1 << (int)category);
				}
				std::cout << "\tANS New Incoming Alert Disabled for Category "
						  << (int)controlPointValue.fields.category << std::endl;
				break;
			case ANS_DISABLE_UNREAD_CATEGORY_STATUS_NOTIFICATION:
				if (category == ANS_TYPE_ALL_ALERTS) {
					_enabled_unread_alert_category = 0x0;
				} else {
					_enabled_unread_alert_category &= ~(1 << (int)category);
				}
				std::cout << "\tANS Unread Alert Disabled for Category "
						  << (int)controlPointValue.fields.category << std::endl;
				break;
			case ANS_NOTIFY_NEW_INCOMING_ALERT_IMMEDIATELY:
				if (category == ANS_TYPE_ALL_ALERTS) {
					for (int ii = 0; ii < 10; ii++) {
						uint16_t mask = _enabled_new_alert_category;
						if ((mask & ((uint16_t)ii)) != 0) {
							_new_alert_characteristic.set(_server, _alert_status[ii].value);
						}
					}
				} else {
					uint16_t mask = _enabled_new_alert_category;
					if ((mask & ((uint16_t)category)) != 0) {
						_new_alert_characteristic.set(_server, _alert_status[(int)category].value);
					}
				}
				std::cout << "\tANS Immediate New Incoming Alert Requested for Category "
						  << (int)controlPointValue.fields.category << std::endl;
				break;
			case ANS_NOTIFY_UNREAD_CATEGORY_STATUS_IMMEDIATELY:
				if (category == ANS_TYPE_ALL_ALERTS) {
					for (int ii = 0; ii < 10; ii++) {
						uint16_t mask = _enabled_unread_alert_category;
						if ((mask & ((uint16_t)ii)) != 0) {
							_unread_alert_status_characteristic.set(_server, _alert_status[ii].value);
						}
					}
				} else {
					uint16_t mask = _enabled_unread_alert_category;
					if ((mask & ((uint16_t)category)) != 0) {
						_unread_alert_status_characteristic.set(_server, _alert_status[(int)category].value);
					}
				}
				std::cout << "\tANS Immediate Unread Alert Requested for Category "
						  << (int)controlPointValue.fields.category << std::endl;
				break;
			default:
				break;
			}
			std::cout << "\tANS New Incoming Alert Enabled Categories 0x"
					  << HEX_SHORT_IOSTREAM(_enabled_new_alert_category) << std::endl;
			std::cout << "\tANS Unread Alert Enabled Categories 0x"
					  << HEX_SHORT_IOSTREAM(_enabled_unread_alert_category) << std::endl;
		}
	}

	virtual void onRead(uint16_t handle) override {
		
	};

	/**
	 * \brief Enables/disables authentication requirement for the service.
	 *
	 * \param enable True to enable, False to disable authentication requirement.
	 */
	virtual void enableAuthentication(bool enable = true) override {
		if (enable != false) {
			_alert_notification_control_point_characteristic.setWriteSecurityRequirement(
				ble::att_security_requirement_t::AUTHENTICATED);
			_supported_new_alert_category_characteristic.setReadSecurityRequirement(
				ble::att_security_requirement_t::AUTHENTICATED);
			_supported_unread_alert_category_characteristic.setReadSecurityRequirement(
				ble::att_security_requirement_t::AUTHENTICATED);

		} else {
			_alert_notification_control_point_characteristic.setWriteSecurityRequirement(
				ble::att_security_requirement_t::NONE);
			_supported_new_alert_category_characteristic.setReadSecurityRequirement(
				ble::att_security_requirement_t::NONE);
			_supported_unread_alert_category_characteristic.setReadSecurityRequirement(
				ble::att_security_requirement_t::NONE);
		}
	}
	/**
	 * \brief Set the Supported New Alerts object
	 *
	 * \param supportedNewAlerts the supported new alerts bitfield.
	 * \return true if peer of not connected
	 * \return false if peer is connected
	 */
	bool setSupportedNewAlerts(uint16_t supportedNewAlerts) {
		if (_connected != false) {
			return false;
		} else {
			_supported_new_alert_category = supportedNewAlerts;
			_supported_new_alert_category_characteristic.set(_server, supportedNewAlerts);
			return true;
		}
	}

	/**
	 * \brief Set the Supported Unread Alerts object
	 *
	 * \param supportedUnreadAlerts the supported unread alerts bitfields
	 * \return true if peer of not connected
	 * \return false if peer is connected
	 */
	bool setSupportedUnreadAlerts(uint16_t supportedUnreadAlerts) {
		if (_connected != false) {
			return false;
		} else {
			_supported_unread_alert_category = supportedUnreadAlerts;
			_supported_unread_alert_category_characteristic.set(_server, supportedUnreadAlerts);
			return true;
		}
	}

	/**
	 * \brief Clears the alert counts
	 *
	 * \param catgory Category to clear. if ANS_TYPE_ALL_ALERTS, all alert types are cleared.
	 */
	void clearAlert(CategoryId category) {
		if (category == ANS_TYPE_ALL_ALERTS) {
			for (int ii = 0; ii < 10; ii++) {
				_alert_status[ii].fields.count = 0;
			}
		} else {
			_alert_status[(int)category].fields.count = 0;
		}
	}
};

#endif //!_ALERT_NOTIFICATION_SERVICE_H_