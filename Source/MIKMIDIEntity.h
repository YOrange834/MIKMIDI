//
//  MIKMIDIEntity.h
//  MIDI Testbed
//
//  Created by Andrew Madsen on 3/7/13.
//  Copyright (c) 2013 Mixed In Key. All rights reserved.
//

#import <MIKMIDI/MIKMIDIObject.h>
#import <MIKMIDI/MIKMIDICompilerCompatibility.h>

@class MIKMIDIDevice;
@class MIKMIDIEndpoint;
@class MIKMIDISourceEndpoint;
@class MIKMIDIDestinationEndpoint;

NS_ASSUME_NONNULL_BEGIN

/**
 *  MIKMIDIEntity represents a logical grouping of endpoints within a MIDI device. It essentially
 *  acts as a simple container for endpoints. 【MIKMIDIEntity 表示 MIDI 设备中端点的逻辑分组。 它本质上充当端点的简单容器。】
 *
 *  As part of MIKMIDIDevice's support for wrapping virtual endpoints, an MIKMIDIEntity can also
 *  be created using virtual MIDI endpoints. 【作为 MIKMIDIDevice 对包装虚拟端点的支持的一部分，还可以使用虚拟 MIDI 端点创建 MIKMIDIEntity。】
 */
@interface MIKMIDIEntity : MIKMIDIObject

/**
 *  Convenience method for creating a "virtual" MIKMIDIEntity instance from one or more virtual endpoints.
 *  This method is typically not called directly by clients of MIKMIDI. Rather it's used by MIKMIDIDevice's
 *  internal machinery for creating virtual devices.
 *
 *  @param endpoints An array of one or more virtual endpoints, including both source and destination endpoints.
 *
 *  @return An initialized MIKMIDIEntity instance.
 *
 *  @see +[MIKMIDIDevice deviceWithVirtualEndpoints:]
 */
+ (nullable instancetype)entityWithVirtualEndpoints:(MIKArrayOf(MIKMIDIEndpoint *) *)endpoints;

/**
 *  Creates and initializes a "virtual" MIKMIDIEntity instance from one or more virtual endpoints.
 *  This method is typically not called directly by clients of MIKMIDI. Rather it's used by MIKMIDIDevice's
 *  internal machinery for creating virtual devices.
 *
 *  @param endpoints An array of one or more virtual endpoints, including both source and destination endpoints.
 *
 *  @return An initialized MIKMIDIEntity instance.
 *
 *  @see -[MIKMIDIDevice initWithVirtualEndpoints:]
 */
- (nullable instancetype)initWithVirtualEndpoints:(MIKArrayOf(MIKMIDIEndpoint *) *)endpoints;

/**
 *  The device that contains the receiver. May be nil if the receiver is a virtual entity not contained
 *  by a virtual device.  【包含接收器的设备。 如果接收者是一个不包含在虚拟设备中的虚拟实体，则可能为零。】
 */
@property (nonatomic, weak, readonly, nullable) MIKMIDIDevice *device;

/**
 *  The source (input) endpoints contained by the receiver.   【接收器包含的源（输入）端点。】
 *  An array of MIKMIDISourceEndpoint instances.  【MIKMIDISourceEndpoint 实例数组。】
 */
@property (nonatomic, readonly) MIKArrayOf(MIKMIDISourceEndpoint *) *sources;

/**
 *  The destination (output) endpoints contained by the receiver.   【接收方包含的目标（输出）端点。】
 *  An array of MIKMIDIDestinationEndpoint instances.  【MIKMIDIDestinationEndpoint 实例的数组。】
 */
@property (nonatomic, readonly) MIKArrayOf(MIKMIDIDestinationEndpoint *) *destinations;

@end

NS_ASSUME_NONNULL_END
