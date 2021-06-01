//
//  MIKMIDISourceEndpoint.h
//  MIDI Testbed
//
//  Created by Andrew Madsen on 3/8/13.
//  Copyright (c) 2013 Mixed In Key. All rights reserved.
//

#import <MIKMIDI/MIKMIDIEndpoint.h>
#import <MIKMIDI/MIKMIDICompilerCompatibility.h>

@class MIKMIDISourceEndpoint;
@class MIKMIDICommand;

NS_ASSUME_NONNULL_BEGIN

/**
 *  Block used by various MIKMIDI APIs that deliver incoming MIDI messages.
 *
 *  @param source	The source endpoint from which MIDI messagse were received.
 *  @param commands	An NSArray containing received MIKMIDICommand instances.
 */
typedef void(^MIKMIDIEventHandlerBlock)(MIKMIDISourceEndpoint *source, MIKArrayOf(MIKMIDICommand *) *commands); // commands in an array of MIKMIDICommands

/**
 *  MIKMIDISourceEndpoint represents a source (input) MIDI endpoint. 【MIKMIDISourceEndpoint 表示源（输入）MIDI 端点。】
 *  It is essentially an Objective-C wrapper for instances of CoreMIDI's MIDIEndpoint class
 *  which are kMIDIObjectType_Source type endpoints. 【它本质上是 CoreMIDI 的 MIDIEndpoint 类实例的 Objective-C 包装器，这些实例是 kMIDIObjectType_Source 类型的端点。】
 *
 *  MIDI source endpoints are contained by MIDI entities, which are in turn contained by MIDI devices. 【MIDI 源端点包含在 MIDI 实体中，而 MIDI 实体又包含在 MIDI 设备中。】
 *  MIDI sources can be connected in order to receive data from them using MIKMIDIDeviceManager's
 *  -connectInput:error:eventHandler:virtualDestinations method.
 *
 *  Note that MIKMIDISourceEndpoint does not declare any methods of its own. All its methods can be
 *  found on its superclasses: MIKMIDIEndpoint and MIKMIDIObject.
 *  
 *  @see -[MIKMIDIDeviceManager connectInput:error:eventHandler:virtualDestinations]
 *  @see -[MIKMIDIDeviceManager disconnectInput:]
 */
@interface MIKMIDISourceEndpoint : MIKMIDIEndpoint

@end

NS_ASSUME_NONNULL_END
