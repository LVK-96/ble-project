#ifndef _BLE_CHARACTERISTIC_H_
#define _BLE_CHARACTERISTIC_H_

#include <iostream>
#include <mbed.h>

#include "ble/BLE.h"
/**
 * \brief General Characteristic class
 *
 * \tparam T The value type of the characteristic
 */
template <typename T> class CCharacteristic : public GattCharacteristic {
  private:
	T _value;

  public:
	/**
	 * Construct a characteristic that can be read or written and emit
	 * notification or indication.
	 *
	 * @param[in] uuid The UUID of the characteristic.
	 * @param[in] initial_value Initial value contained by the characteristic.
	 */
	//
	/**
	 * \brief Construct a new CCharacteristic object
	 *
	 * \param uuid The UUID of the characteristic
	 * \param initialValue The initial value of the characteristic
	 * \param properties The properties of the characteristic. This is a bitfield.
	 * \param descriptors The characteristics descriptors
	 * \param numOfDescriptos number of descriptors
	 */
	CCharacteristic(const UUID &uuid,
					const T &initialValue,
					const uint8_t properties,
					GattAttribute *descriptors[] = NULL,
					int numOfDescriptors = 0)
		: _value(initialValue), GattCharacteristic(/* UUID */ uuid,
												   /* Initial value */ reinterpret_cast<uint8_t *>(&_value),
												   /* Value size */ sizeof(T),
												   /* Value capacity */ sizeof(T),
												   /* Properties */ properties,
												   /* Descriptors */ descriptors,
												   /* Num descriptors */ numOfDescriptors,
												   /* variable len */ false) {}

	/**
	 * Get the value of this characteristic.
	 *
	 * @param[in] server GattServer instance that contain the characteristic
	 * value.
	 * @param[in] dst Variable that will receive the characteristic value.
	 *
	 * @return BLE_ERROR_NONE in case of success or an appropriate error code.
	 */
	ble_error_t get(GattServer *server, T &dst) const {
		uint16_t value_length = sizeof(dst);
		return server->read(getValueHandle(), (uint8_t *)&dst, &value_length);
	}

	/**
	 * Assign a new value to this characteristic.
	 *
	 * @param[in] server GattServer instance that will receive the new value.
	 * @param[in] value The new value to set.
	 * @param[in] local_only Flag that determine if the change should be kept
	 * locally or forwarded to subscribed clients.
	 */
	ble_error_t set(GattServer *server, const T &value, bool localOnly = false) {
		_value = value;
		return server->write(getValueHandle(),
							 reinterpret_cast<uint8_t *>(&_value),
							 sizeof(T),
							 localOnly);
	}
};
/**
 * \brief Characteristics comparison function
 *
 * \param obj1 The first object to compared
 * \param obj2  The second object of the comparison
 * \return true if the first object should placed before object 2
 * \return false otherwise
 */
static bool compareCharacteristics(const GattCharacteristic *obj1, const GattCharacteristic *obj2) {
	return (obj1->getValueAttribute().getUUID().getShortUUID() <=
			obj2->getValueAttribute().getUUID().getShortUUID());
}

/**
 * \brief  Read only characteristic
 *
 * \tparam T The value type of the characteristic
 */
template <typename T> class CReadOnlyCharacteristic : public CCharacteristic<T> {
  public:
	/**
	 * \brief Construct a new CReadOnlyCharacteristic object
	 *
	 * \param uuid The UUID of the characteristic
	 * \param initialValue The initial value of the characteristic
	 * \param descriptors The characteristics descriptors
	 * \param numOfDescriptos number of descriptors
	 */
	CReadOnlyCharacteristic(const UUID &uuid,
							const T &initialValue,
							GattAttribute *descriptors[] = NULL,
							int numOfDescriptors = 0)
		: CCharacteristic<T>(uuid,
							 initialValue,
							 (uint8_t)GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ,
							 descriptors,
							 numOfDescriptors) {}
};

template <typename T> class CWriteOnlyCharacteristic : public CCharacteristic<T> {
  public:
	/**
	 * \brief Construct a new CReadOnlyCharacteristic object
	 *
	 * \param uuid The UUID of the characteristic
	 * \param initialValue The initial value of the characteristic
	 * \param descriptors The characteristics descriptors
	 * \param numOfDescriptos number of descriptors
	 */
	CWriteOnlyCharacteristic(const UUID &uuid,
							 const T &initialValue,
							 GattAttribute *descriptors[] = NULL,
							 int numOfDescriptors = 0)
		: CCharacteristic<T>(uuid,
							 initialValue,
							 GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE,
							 descriptors,
							 numOfDescriptors) {}
};

template <typename T> class CNotifyOnlyCharacteristic : public CCharacteristic<T> {
  public:
	/**
	 * \brief Construct a new CReadOnlyCharacteristic object
	 *
	 * \param uuid The UUID of the characteristic
	 * \param initialValue The initial value of the characteristic
	 * \param descriptors The characteristics descriptors
	 * \param numOfDescriptos number of descriptors
	 */
	CNotifyOnlyCharacteristic(const UUID &uuid,
							  const T &initialValue,
							  GattAttribute *descriptors[] = NULL,
							  int numOfDescriptors = 0)
		: CCharacteristic<T>(uuid,
							 initialValue,
							 GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY,
							 descriptors,
							 numOfDescriptors) {}
};

template <typename T> class CReadWriteCharacteristic : public CCharacteristic<T> {
  public:
	/**
	 * \brief Construct a new CReadOnlyCharacteristic object
	 *
	 * \param uuid The UUID of the characteristic
	 * \param initialValue The initial value of the characteristic
	 * \param descriptors The characteristics descriptors
	 * \param numOfDescriptos number of descriptors
	 */
	CReadWriteCharacteristic(const UUID &uuid,
							 const T &initialValue,
							 GattAttribute *descriptors[] = NULL,
							 int numOfDescriptors = 0)
		: CCharacteristic<T>(uuid,
							 initialValue,
							 GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ |
								 GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE,
							 descriptors,
							 numOfDescriptors) {}
};

template <typename T> class CReadNotifyCharacteristic : public CCharacteristic<T> {
  public:
	/**
	 * \brief Construct a new CReadOnlyCharacteristic object
	 *
	 * \param uuid The UUID of the characteristic
	 * \param initialValue The initial value of the characteristic
	 * \param descriptors The characteristics descriptors
	 * \param numOfDescriptos number of descriptors
	 */
	CReadNotifyCharacteristic(const UUID &uuid,
							  const T &initialValue,
							  GattAttribute *descriptors[] = NULL,
							  int numOfDescriptors = 0)
		: CCharacteristic<T>(uuid,
							 initialValue,
							 GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ |
								 GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY,
							 descriptors,
							 numOfDescriptors) {}
};

template <typename T> class CReadWriteNotifyCharacteristic : public CCharacteristic<T> {
  public:
	/**
	 * \brief Construct a new CReadOnlyCharacteristic object
	 *
	 * \param uuid The UUID of the characteristic
	 * \param initialValue The initial value of the characteristic
	 * \param descriptors The characteristics descriptors
	 * \param numOfDescriptos number of descriptors
	 */
	CReadWriteNotifyCharacteristic(const UUID &uuid,
								   const T &initialValue,
								   GattAttribute *descriptors[] = NULL,
								   int numOfDescriptors = 0)
		: CCharacteristic<T>(uuid,
							 initialValue,
							 GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ |
								 GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE |
								 GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY,
							 descriptors,
							 numOfDescriptors) {}
};

template <typename T> class CReadIndicateCharacteristic : public CCharacteristic<T> {
  public:
	/**
	 * \brief Construct a new CReadOnlyCharacteristic object
	 *
	 * \param uuid The UUID of the characteristic
	 * \param initialValue The initial value of the characteristic
	 * \param descriptors The characteristics descriptors
	 * \param numOfDescriptos number of descriptors
	 */
	CReadIndicateCharacteristic(const UUID &uuid,
								const T &initialValue,
								GattAttribute *descriptors[] = NULL,
								int numOfDescriptors = 0)
		: CCharacteristic<T>(uuid,
							 initialValue,
							 GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ |
								 GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_INDICATE,
							 descriptors,
							 numOfDescriptors) {}
};

template <typename T> class CReadWriteIndicateCharacteristic : public CCharacteristic<T> {
  public:
	/**
	 * \brief Construct a new CReadOnlyCharacteristic object
	 *
	 * \param uuid The UUID of the characteristic
	 * \param initialValue The initial value of the characteristic
	 * \param descriptors The characteristics descriptors
	 * \param numOfDescriptos number of descriptors
	 */
	CReadWriteIndicateCharacteristic(const UUID &uuid,
									 const T &initialValue,
									 GattAttribute *descriptors[] = NULL,
									 int numOfDescriptors = 0)
		: CCharacteristic<T>(uuid,
							 initialValue,
							 GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ |
								 GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE |
								 GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_INDICATE,
							 descriptors,
							 numOfDescriptors) {}
};

#endif //! _BLE_CHARACTERISTIC_H_
