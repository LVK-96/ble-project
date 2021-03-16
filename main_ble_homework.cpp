/* mbed Microcontroller Library
 * Copyright (c) 2017 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ble_gap_sm.h"
#include "ble_gatt_alert_notification_service.h"
#include "ble_gatt_immedate_alert_service.h"
#include "ble_gatt_server.h"
#include "ble_utils.h"
#include <mbed.h>

#define PWM_PERIOD_US 100

/**
 * \brief The homework BLE device implementation class.
 *
 */
class CHomework {
  protected:
	CGapSecurity _gap;		  //!< The GAP implementation. This should be instantiated with
							  //!< SecurityManager::IO_CAPS_DISPLAY_ONLY capabilities:
	CGattServer _gatt_server; //!< This is the Gatt server which requires setting a set of services. The C++
							  //!< initializer list can be used.
	CAlertNotificationServiceServer
		_ans; //!< The alert notification service. This should be instantiated with
			  //!< CAlertNotificationServiceServer::ANS_TYPE_MASK_SIMPLE_ALERT as supported new alerts
	CImmediateAlertServiceServer _ias; //!< This is the Immedate alert service instance

	events::EventQueue *_event_queue; //!< A pointer to the system event queue
	BLE &_ble;						  //!< A reference to one and only system BLE instance

	InterruptIn _alert_button; //!< The alert button.
	PwmOut _alert_led_pwm;	   //!< The Alert LED pwm object

	/**
	 * \brief Button Press ISR implementation
	 *
	 */
	void onButtonPressed() {
		// TODO use event_queue call to dispatch button event pressed function handling to onButtonAlert
		// function
	}

	/**
	 * \brief Callback function of the Alert button pressed event dispatched by the system event queue
	 *
	 */
	void onButtonAlert() {
		/*
		 * TODO
		 * Indicate new alert to Alert Notification Service (_ans) with type
		 * CAlertNotificationServiceServer::ANS_TYPE_SIMPLE_ALERT
		 */
	}
	/**
	 * \brief Immediate Alert Service Alert Level characteristic written callback function
	 *
	 * \param level The new level
	 */
	void onAlertLevelChanged(uint8_t level) {
		float pulsewidth = 0.0f;
		level = (level > 2) ? 2 : level;
		switch (level) {
		case CImmediateAlertServiceServer::IAS_ALERT_LEVEL_NO_ALERT:
			// TODO  pulsewidth value to NO_ALERT state (LED should be off)
			break;
		case CImmediateAlertServiceServer::IAS_ALERT_LEVEL_MEDIUM:
			// TODO  pulsewidth value to MEDIUM ALERT state (LED should be half bright)
			break;
		case CImmediateAlertServiceServer::IAS_ALERT_LEVEL_HIGH:
			// TODO  pulsewidth value to HIGH ALERT state (LED should be bright)
			break;
		default:
			// TODO  pulsewidth value to HIGH ALERT state (LED should be bright)
			break;
		}
		// TODO update the PwmOut object pulsewidth
	}

	/**
	 * \brief The onConnection callback of the GAP.
	 *
	 */
	void onConnection() {
		_gatt_server.onConnection();
		// TODO set the Alert Level LED brightness to NO_ALERT level
	}
	/**
	 * \brief The onDisconnection callback of the GAP
	 *
	 */
	void onDisconnection() {
		_gatt_server.onDisconnection();
		// TODO set the Alert Level LED brightness to NO_ALERT level
		// TODO clear Alert Notification Service Alert Counts by using _ans->clearAlert()
	}

  public:
	/**
	 * \brief Construct a new CHomework object
	 *
	 * \param ble A reference to the BLE instance
	 * \param queue The system event queue
	 * \param deviceName The device name
	 * \param buttonPin Alert button pin name
	 * \param ledPin Alert LED pin
	 */
	CHomework(BLE &ble,
			  events::EventQueue *queue,
			  const char *deviceName,
			  PinName buttonPin = BUTTON1,
			  PinName ledPin = LED2)
		: _ble(ble), _event_queue(queue),
		  _gap(ble, *queue, deviceName, SecurityManager::IO_CAPS_DISPLAY_ONLY),
		  _ans(CAlertNotificationServiceServer::ANS_TYPE_MASK_SIMPLE_ALERT, 0),
		  _ias(), _gatt_server(ble, *queue, {&_ans, &_ias}), _alert_button(buttonPin),
		  _alert_led_pwm(ledPin) {

		_gap.setOnInitCallback(callback(&_gatt_server, &CGattServer::start));
		/*
		 * TODO
		 * 1. Configure _gap onConnection callback to use This object's onConnection function
		 * 2. Configure _gap onDisconnection callback to use This object's onDisconnection function
		 * 3. Configure _ias onAlertLevelWritten callback to use This object's onAlertLevelChanged function
		 * 4. Configure button rise ISR function to use This object's onButtonPress function
		 * 5. Configure LED pwm period to PWM_PERIOD_US
		 * 6. Turn off the Alert Level LED by seeting its pwm pulsewidth
		 * 7. Enable authentication requirement for Immediate Alert Service object _ias
		 * 8. Enable authentication requirement for Alert Notification Service object _ans
		 */

	}

	void run() { 
		// just let GAP class handle the event loops
		_gap.run(); 
	}
};

int main() {
	BLE &ble = BLE::Instance();
	events::EventQueue *event_queue = new events::EventQueue; // create the queue in the heap
	CHomework hw(ble, event_queue, "Homework");
	// bind the event queue to the ble interface, initialize the interface
	// and start advertising
	hw.run();

	return 0;
}
