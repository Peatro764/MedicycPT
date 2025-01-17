

var EventEmitter = require('events').EventEmitter;
var util = require('util');
var q = require('q');
var driver_const = require('ljswitchboard-ljm_driver_constants');
var device_events = driver_const.device_curator_constants;

function createDevice(savedAttributes, deviceCallFunc, deviceSendFunc, closeDeviceFunc) {
	this.eventList = driver_const.device_curator_constants;
	this.savedAttributes = savedAttributes;
	this.device_comm_key = savedAttributes.device_comm_key;
	this.deviceCallFunc = deviceCallFunc;
	this.deviceSendFunc = deviceSendFunc;
	this.closeDeviceFunc = closeDeviceFunc;

	this.internalListeners = {};

	this.addInternalListener = function(funcName, func) {
		self.internalListeners[funcName] = func;
	};
	this.removeInternalListener = function(funcName) {
		if(self.internalListeners[funcName]) {
			self.internalListeners[funcName] = null;
			self.internalListeners[funcName] = undefined;
			delete self.internalListeners[funcName];
		}
	};
	this.oneWayListener = function(m) {
		if(self.internalListeners[m.func]) {
			self.internalListeners[m.func](m.data);
		} else {
			if((typeof(m.name) !== 'undefined') && (typeof(m.data) !== 'undefined')) {
				if(m.name === device_events.DEVICE_ATTRIBUTES_CHANGED) {
					// console.log('  ! (ljm_device) Updating saved attributes');
					var updatedKeys = Object.keys(m.data);
					updatedKeys.forEach(function(updatedKey) {
						self.savedAttributes[updatedKey] = m.data[updatedKey];
					});
				} else if(m.name === device_events.DEVICE_DISCONNECTED) {
					self.savedAttributes.isConnected = false;
				} else if(m.name === device_events.DEVICE_RECONNECTED) {
					self.savedAttributes.isConnected = true;
				}

				self.emit(m.name, m.data);
			} else {
				console.log(
					'Un-handled ljm_device.js oneWayListener message',
					self.device_comm_key,
					m
				);
			}
		}
	};
	this.callFunc = function(func, args) {
		return self.deviceCallFunc(self.device_comm_key, func, args);
	};

	this.getDeviceErrors = function() {
		return self.callFunc('getDeviceErrors');
	};
	this.getLatestDeviceErrors = function() {
		return self.callFunc('getLatestDeviceErrors');
	};
	this.clearDeviceErrors = function() {
		return self.callFunc('clearDeviceErrors');
	};
	
	this.getHandleInfo = function() {
		return self.callFunc('getHandleInfo');
	};
	this.getDeviceAttributes = function() {
		return self.callFunc('getDeviceAttributes');
	};
	this.updateSavedAttributes = function() {
		return self.callFunc('updateSavedAttributes');
	};
	this.readRaw = function(data) {
		return self.callFunc('readRaw', [data]);
	};
	this.read = function(address) {
		return self.callFunc('read', [address]);
	};
	this.readArray = function(address, numReads) {
		return self.callFunc('readArray', [address, numReads]);
	};
	this.readMultiple = function(addresses) {
		return self.callFunc('readMultiple', [addresses]);
	};
	this.readMany = function(addresses) {
		return self.callFunc('readMany', [addresses]);
	};
	this.writeRaw = function(data) {
		return self.callFunc('writeRaw', [data]);
	};
	this.write = function(address, value) {
		return self.callFunc('write', [address, value]);
	};
	this.writeArray = function(address, writeData) {
		return self.callFunc('writeArray', [address, writeData]);
	};
	this.writeMany = function(addresses, values) {
		return self.callFunc('writeMany', [addresses, values]);
	};
	this.rwMany = function(addresses, directions, numValues, values) {
		return self.callFunc('rwMany', [addresses, directions, numValues, values]);
	};
	this.readUINT64 = function(type) {
		return self.callFunc('readUINT64', [type]);
	};

	/**
	 * _DEFAULT safe functions
	 */
	this.qRead = function(address) {
		return self.callFunc('qRead', [address]);
	};
	this.qReadMany = function(addresses) {
		return self.callFunc('qReadMany', [addresses]);
	};
	this.qWrite = function(address, value) {
		return self.callFunc('qWrite', [address, value]);
	};
	this.qWriteMany = function(addresses, values) {
		return self.callFunc('qWriteMany', [addresses, values]);
	};
	this.qrwMany = function(addresses, directions, numValues, values) {
		return self.callFunc('qrwMany', [addresses, directions, numValues, values]);
	};
	this.qReadUINT64 = function(type) {
		return self.callFunc('qReadUINT64', [type]);
	};

	/**
	 * Special intelligent functions that automatically integrate the
	 * data_parser.
	 */
	this.iRead = function(address) {
		return self.callFunc('iRead', [address]);
	};
	this.iReadMany = function(addresses) {
		return self.callFunc('iReadMany', [addresses]);
	};
	this.iReadMultiple = function(addresses) {
		return self.callFunc('iReadMultiple', [addresses]);
	};
	this.iWrite = function(address, value) {
		return self.callFunc('iWrite', [address, value]);
	};
	this.iWriteMany = function(addresses, values) {
		return self.callFunc('iWriteMany', [addresses, values]);
	};
	this.iWriteMultiple = function(addresses, values) {
		return self.callFunc('iWriteMultiple', [addresses, values]);
	};

	/**
	 * Special "smart" functions that use the i functions and resolve any errors
	 * to register default values or last-valid cached values.
	 */
	this.sRead = function(address) {
		return self.callFunc('sRead', [address]);
	};
	this.sReadMany = function(addresses) {
		return self.callFunc('sReadMany', [addresses]);
	};
	this.sReadMultiple = function(addresses) {
		return self.callFunc('sReadMultiple', [addresses]);
	};

	/**
	 * Special function to set the device name
	 */
	this.writeDeviceName = function(newName) {
		return self.callFunc('writeDeviceName', [newName]);
	};

	/**
	 * Special functions to configure created mock devices
	 */
	this.configureMockDevice = function(deviceInfo) {
		return self.callFunc('configureMockDevice', [deviceInfo]);
	};
	/**
	 * Upgrade Function
	 */
	this.updateFirmware = function(firmwareFileLocation, percentListener, stepListener) {
		var defered = q.defer();

		self.addInternalListener('updateFirmware', function(m) {
			if(m.type === 'percent') {
				percentListener(m.data);
			} else {
				stepListener(m.data);
			}
		});
		self.callFunc('updateFirmware', [firmwareFileLocation])
		.then(function(res) {
			self.removeInternalListener('updateFirmware');
			defered.resolve(res);
		}, function(err) {
			self.removeInternalListener('updateFirmware');
			defered.reject(err);
		});

		return defered.promise;
	};
	
	this.getFunctions = function() {
		return self.callFunc('getFunctions');
	};

	/**
	 *	The close function calls the previously passed in close function of the
	 *	device_controller object to instruct the sub-process to delete the 
	 *	close & delete the device and remove it from the device_controller's 
	 *	local listing of devices.
	 */
	this.close = function() {
		var defered = q.defer();

		// Inform sub-process that the device should be closed
		self.closeDeviceFunc(self.device_comm_key)
		.then(defered.resolve, defered.reject);
		return defered.promise;
	};
	
	var self = this;
}
util.inherits(createDevice, EventEmitter);

exports.createDevice = createDevice;