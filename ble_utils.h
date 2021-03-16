#ifndef _BLE_UTILS_H_
#define _BLE_UTILS_H_
#include "BLE.h"
#include "mbed.h"

#include <iomanip>
#include <iostream>

namespace ble_utils {

#define HEX_BYTE_IOSTREAM(__value) setw(2) << setfill('0') << hex << (int)(__value)
#define HEX_SHORT_IOSTREAM(__value) setw(4) << setfill('0') << hex << (int)(__value)
void printDeviceAddress(const ble::peer_address_type_t type, const ble::address_t &address);
void printDeviceAddress(const Gap::Address_t &address);
void printDeviceAddress(const ble::address_t &address);
/**
 * \brief Prints the Bluetooth Device Address.
 *
 * \param type The type of the address.
 * \param address The address.
 */
void printDeviceAddress(Gap::AddressType_t type, const Gap::Address_t &address) {
	/* Print out device MAC address to the console*/
	std::cout << "LOCAL BLUETOOTH DEVICE ADDRESS ";
	switch (type) {
	case BLEProtocol::AddressType::PUBLIC:
		std::cout << "PUBLIC ";
		break;
	case BLEProtocol::AddressType::RANDOM_STATIC:
		std::cout << "RANDOM STATIC ";
		break;
	case BLEProtocol::AddressType::RANDOM_PRIVATE_RESOLVABLE:
		std::cout << "RANDOM PRIVATE RESOLVABLE ";
		break;
	case BLEProtocol::AddressType::RANDOM_PRIVATE_NON_RESOLVABLE:
		std::cout << "RANDOM PRIVATE NON-RESOLVABLE ";
		break;
	default:
		std::cout << "UNKNOWN";
		break;
	}
	ble_utils::printDeviceAddress(address);
}

/**
 * \brief Prints the Bluetooth address of a peer device.
 *
 * \param type The peer device address type
 * \param address The peer device address
 */
void printDeviceAddress(const ble::peer_address_type_t type, const ble::address_t &address) {
	/* Print out device MAC address to the console*/
	std::cout << "PEER BLUETOOTH DEVICE ADDRESS ";
	switch (type.value()) {
	case ble::peer_address_type_t::PUBLIC:
		std::cout << "PUBLIC ";
		break;
	case ble::peer_address_type_t::RANDOM:
		std::cout << "RANDOM ";
		break;
	case ble::peer_address_type_t::RANDOM_STATIC_IDENTITY:
		std::cout << "RANDOM STATIC IDENTITY ";
		break;
	case ble::peer_address_type_t::ANONYMOUS:
		std::cout << "ANONYMOUS BROADCASTER ";
		break;
	default:
		std::cout << "UNKNOWN ";
		break;
	}
	ble_utils::printDeviceAddress(address);
}

/**
 * \brief Prints just the device address but not the type
 *
 * \param address The address to be printed.
 */
void printDeviceAddress(const Gap::Address_t &address) {
	/* Print out device MAC address to the console*/
	std::cout << HEX_BYTE_IOSTREAM(address[5]) << HEX_BYTE_IOSTREAM(address[4])
			  << HEX_BYTE_IOSTREAM(address[3]) << HEX_BYTE_IOSTREAM(address[2])
			  << HEX_BYTE_IOSTREAM(address[1]) << HEX_BYTE_IOSTREAM(address[0]) << std::endl;
}

/**
 * \brief PRints just the address
 *
 * \param address address to be printed
 */
void printDeviceAddress(const ble::address_t &address) {
	/* Print out device MAC address to the console*/
	std::cout << HEX_BYTE_IOSTREAM(address[5]) << HEX_BYTE_IOSTREAM(address[4])
			  << HEX_BYTE_IOSTREAM(address[3]) << HEX_BYTE_IOSTREAM(address[2])
			  << HEX_BYTE_IOSTREAM(address[1]) << HEX_BYTE_IOSTREAM(address[0]) << std::endl;
}
/**
 * \brief Print error code helper function
 *
 * \param error The error code
 * \param message The message to be prepended before the error code description.
 */
void printError(ble_error_t error, const char *message) {
	std::cout << message;
	switch (error) {
	case BLE_ERROR_NONE:
		std::cout << "BLE_ERROR_NONE: No error";
		break;
	case BLE_ERROR_BUFFER_OVERFLOW:
		std::cout << "BLE_ERROR_BUFFER_OVERFLOW: The requested action would "
					 "cause a buffer overflow and has been aborted";
		break;
	case BLE_ERROR_NOT_IMPLEMENTED:
		std::cout << "BLE_ERROR_NOT_IMPLEMENTED: Requested a feature that "
					 "isn't yet implement or isn't supported by the target HW";
		break;
	case BLE_ERROR_PARAM_OUT_OF_RANGE:
		std::cout << "BLE_ERROR_PARAM_OUT_OF_RANGE: One of the supplied "
					 "parameters is outside the valid range";
		break;
	case BLE_ERROR_INVALID_PARAM:
		std::cout << "BLE_ERROR_INVALID_PARAM: One of the supplied parameters "
					 "is invalid";
		break;
	case BLE_STACK_BUSY:
		std::cout << "BLE_STACK_BUSY: The stack is busy";
		break;
	case BLE_ERROR_INVALID_STATE:
		std::cout << "BLE_ERROR_INVALID_STATE: Invalid state";
		break;
	case BLE_ERROR_NO_MEM:
		std::cout << "BLE_ERROR_NO_MEM: Out of Memory";
		break;
	case BLE_ERROR_OPERATION_NOT_PERMITTED:
		std::cout << "BLE_ERROR_OPERATION_NOT_PERMITTED";
		break;
	case BLE_ERROR_INITIALIZATION_INCOMPLETE:
		std::cout << "BLE_ERROR_INITIALIZATION_INCOMPLETE";
		break;
	case BLE_ERROR_ALREADY_INITIALIZED:
		std::cout << "BLE_ERROR_ALREADY_INITIALIZED";
		break;
	case BLE_ERROR_UNSPECIFIED:
		std::cout << "BLE_ERROR_UNSPECIFIED: Unknown error";
		break;
	case BLE_ERROR_INTERNAL_STACK_FAILURE:
		std::cout << "BLE_ERROR_INTERNAL_STACK_FAILURE: internal stack faillure";
		break;
	case BLE_ERROR_NOT_FOUND:
		std::cout << "BLE_ERROR_NOT_FOUND: The data not found or there is "
					 "nothing to return";
		break;
	}
	std::cout << std::endl;
}


} // namespace ble_utils
#endif //! _BLE_UTILS_H_