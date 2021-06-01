//
//  MIKMIDIPort.h
//  MIDI Testbed
//
//  Created by Andrew Madsen on 3/8/13.
//  Copyright (c) 2013 Mixed In Key. All rights reserved.
//

#import <MIKMIDI/MIKMIDIObject.h>
#import <CoreMIDI/CoreMIDI.h>
#import <MIKMIDI/MIKMIDICompilerCompatibility.h>

@class MIKMIDIEndpoint;

NS_ASSUME_NONNULL_BEGIN

/**
 *  MIKMIDIPort is an Objective-C wrapper for CoreMIDI's MIDIPort class. It is not intended for use by clients/users of
 *  of MIKMIDI. Rather, it should be thought of as an MIKMIDI private class.
 *  【MIKMIDIPort 是 CoreMIDI 的 MIDIPort 类的 Objective-C 包装器。 它不适用于 MIKMIDI 的客户/用户。 相反，它应该被认为是一个 MIKMIDI 私有类。】
 */
@interface MIKMIDIPort : NSObject

- (nullable instancetype)initWithClient:(MIDIClientRef)clientRef name:(NSString *)name;

@property (nonatomic, readonly) MIDIPortRef portRef;

@end

NS_ASSUME_NONNULL_END
