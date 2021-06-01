//
//  MIKMIDIDevice.h
//  MIDI Testbed
//
//  Created by Andrew Madsen on 3/7/13.
//  Copyright (c) 2013 Mixed In Key. All rights reserved.
//

#import <MIKMIDI/MIKMIDIObject.h>
#import <MIKMIDI/MIKMIDICompilerCompatibility.h>

@class MIKMIDIEntity;
@class MIKMIDIEndpoint;

NS_ASSUME_NONNULL_BEGIN

/**
 *  MIKMIDIDevice represents a MIDI device such as a DJ controller, MIDI piano keyboard, etc.
 *  【MIKMIDIDevice 代表 MIDI 设备，例如 DJ 控制器、MIDI 钢琴键盘等。】
 *  Overview 【概述】
 *  --------
 *
 *  MIDI devices are for example, DJ controllers, MIDI piano keyboards, etc. For many applications,
 *  being able to discover, connect to, and receive messages from devices is fundamental to their use of MIDI.
 *  Instances of MIKMIDIDevice represent a MIDI device and MIKMIDIDevice includes methods for
 *  retrieving information about the device as well as obtaining its MIDI entities and endpoints in order
 *  to communicate with it. 【对于许多应用程序，能够发现、连接到设备并从设备接收消息是它们使用 MIDI 的基础。MIKMIDIDevice 的实例代表一个 MIDI 设备，并且 MIKMIDIDevice 包括用于检索有关设备的信息，并按顺序获取其MIDI实体和端点与之通信。】
 *
 *  MIDI devices can contain multiple entities, and each entity can contain multiple source and destination
 *  endpoints. Commonly, however, a device will contain only a single entity, which contains a single
 *  source endpoint, and a single destination endpoint.
 *  【 MID设备可以包含多个实体，每个实体可以包含多个源和目的地端点。然而，通常一个设备将只包含一个实体，该实体包含一个源端点和单个目标端点。】
 *
 *  Retrieving Available Devices 【检索可用设备】
 *  ----------------------------
 *
 *  To retrieve a list of the devices that are available, use -[MIKMIDIDeviceManager availableDevices].
 *  Note that some devices (e.g. some Native Instruments DJ controllers) have drivers that present
 *  them as pairs of virtual MIDI endpoints. These devices will not be available in the array returned
 *  by -availableDevices, and instead will be represented by virtual endpoints found in the arrays returned
 *  by -[MIKMIDIDeviceManager virtualSources] and -[MIKMIDIDeviceManager virtualDestinations]. MIKMIDIDevice
 *  can be used to "wrap" virtual sources so that it can be used with devices that present solely using virtual
 *  endpoings. See +deviceWithVirtualEndpoints: for more.
 *
 *  Connecting to a Device 【连接到设备】
 *  ----------------------
 *
 *  To connect a device and start receiving MIDI messages from it, you must first get the source endpoints
 *  you want to connect to. Often there will be only one. You can retrieve all of a devices source endpoints
 *  using the following:
 *
 *		NSArray *sources = [self.device.entities valueForKeyPath:@"@unionOfArrays.sources"];
 *      MIKMIDISourceEndpoint = [source firstObject]; // Or whichever source you want, but often there's only one.
 *
 *  Next, connect to that source using MIKMIDIDeviceManager:
 *
 *  	MIKMIDIDeviceManager *manager = [MIKMIDIDeviceManager sharedDeviceManager];
 *  	NSError *error = nil;
 *  	BOOL success = [manager connectInput:source error:&error eventHandler:^(MIKMIDISourceEndpoint *source, NSArray *commands) {
 *  		for (MIKMIDICommand *command in commands) {
 *  			// Handle each command
 *  		}
 *  	}];
 *  	if (!success) {
 *  		NSLog(@"Unable to connect to %@: %@", source, error);
 *  		// Handle the error
 *  	}
 *
 *
 *  @see MIKMIDIDeviceManager
 *  @see -[MIKMIDIDeviceManager availableDevices]
 *  @see +deviceWithVirtualEndpoints:
 */
@interface MIKMIDIDevice : MIKMIDIObject

/**
 *  Convenience method for creating a "virtual" MIKMIDIDevice instance from one or more virtual endpoints.
 *
 *  MIKMIDIDevices typically represent a physical, connected piece of MIDI hardware. However,
 *  some devices (e.g. some Native Instruments DJ controllers) have drivers that present
 *  them as pairs of virtual MIDI endpoints. These devices are not visible using the regular
 *  MIDI device API. To ease supporting them in code expecting to talk to devices, MIMKIDIDevice
 *  instances can be created with pairs of virtual endpoints, resulting in a "virtual" MIKMIDIDevice
 *  instance.
 *
 *  MIKMIDIDevice instances created with this method will return YES from their -isVirtual method.
 *
 *  @see -[MIKMIDIObject isVirtual]
 *
 *  @param endpoints An array of one or more virtual endpoints, including both source and destination endpoints.
 *
 *  @return An initialized MIKMIDIDevice instance.
 */
+ (instancetype)deviceWithVirtualEndpoints:(MIKArrayOf(MIKMIDIEndpoint *) *)endpoints;

/**
 *  Creates and initializes a "virtual" MIKMIDIDevice instance from one or more virtual endpoints.
 *
 *  MIKMIDIDevices typically represent a physical, connected piece of MIDI hardware. However,
 *  some devices (e.g. some Native Instruments DJ controllers) have drivers that present
 *  them as pairs of virtual MIDI endpoints. These devices are not visible using the regular
 *  MIDI device API. To ease supporting them in code expecting to talk to devices, MIMKIDIDevice
 *  instances can be created with pairs of virtual endpoints, resulting in a "virtual" MIKMIDIDevice
 *  instance.
 *
 *  MIKMIDIDevice instances created with this method will return YES from their -isVirtual method.
 *
 *  @see -[MIKMIDIObject isVirtual]
 *
 *  @param endpoints An array of one or more virtual endpoints, including both source and destination endpoints.
 *
 *  @return An initialized MIKMIDIDevice instance.
 */
- (instancetype)initWithVirtualEndpoints:(MIKArrayOf(MIKMIDIEndpoint *) *)endpoints;

/**
 *  The manufacturer of the MIDI device.
 */
@property (nonatomic, strong, readonly, nullable) NSString *manufacturer;

/**
 *  The model number of the MIDI device.
 */
@property (nonatomic, strong, readonly, nullable) NSString *model;

/**
 *  An NSArray containing instances of MIKMIDIEntity, representing the entities of the
 *  receiver. Entities contain logically related source and destination endpoints. Often
 *  a device will only have one entity.
 */
@property (nonatomic, strong, readonly) MIKArrayOf(MIKMIDIEntity *) *entities;

@end

NS_ASSUME_NONNULL_END
