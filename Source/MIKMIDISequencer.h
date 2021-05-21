//
//  MIKMIDISequencer.h
//  MIKMIDI
//
//  Created by Chris Flesner on 11/26/14.
//  Copyright (c) 2014 Mixed In Key. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h>
#import <MIKMIDI/MIKMIDICompilerCompatibility.h>

@class MIKMIDISequence;
@class MIKMIDITrack;
@class MIKMIDIMetronome;
@class MIKMIDICommand;
@class MIKMIDIDestinationEndpoint;
@class MIKMIDISynthesizer;
@class MIKMIDIClock;
@protocol MIKMIDICommandScheduler;

/**
 *  Types of click track statuses, that determine when the click track will be audible.
 *
 *  @see clickTrackStatus
 */
typedef NS_ENUM(NSInteger, MIKMIDISequencerClickTrackStatus) {
	/** The click track will not be heard during playback or recording. */
	MIKMIDISequencerClickTrackStatusDisabled,
	/** The click track will only be heard while recording. */
	MIKMIDISequencerClickTrackStatusEnabledInRecord,
	/** The click track will only be heard while recording and while the playback position is still in the pre-roll. */
	MIKMIDISequencerClickTrackStatusEnabledOnlyInPreRoll,
	/** The click track will always be heard during playback and recording. */
	MIKMIDISequencerClickTrackStatusAlwaysEnabled
};

typedef NS_OPTIONS(NSInteger, MIKMIDISequencerTimeConversionOptions) {
	/** Use default options (consider tempo override and looping, don't unroll loops) */
	MIKMIDISequencerTimeConversionOptionsNone = 0,
	/** Use the sequence's tempo events to calculate conversion, even if the sequencer has a tempo override set. The default is to use the overridden tempo for calculation if one is set.*/
	MIKMIDISequencerTimeConversionOptionsIgnoreTempoOverride = 1 << 0,
	/** Calculate conversion as if looping were disabled. The default is to take into account looping if it is enabled on the sequencer.*/
	MIKMIDISequencerTimeConversionOptionsIgnoreLooping = 1 << 1,
	/** When this option is set, conversion will return the time of events currently being played relative to the start of the sequence, and the result will never been greater than the end of the loop. The default, where this option is not set, is to calculate and return the absolute time since the sequence start.

	 For example, consider a sequence that is 16 beats long, the tempo is a constant 75 bpm and looping is enabled for first 8 beats. The sequence will be exactly 20 seconds long, and the loop will consist of the first 10 seconds.

	 If this option is *set*, and a time of 25 seconds is passed in, the result will be 4 beats, because the sequencer will be at the half way point of the loop on its third time through. If this option is *not set*, the result will be 20 beats, because 20 beats total will have elapsed since the start of the sequence.

	 Setting the option allows you to determine what part of the raw sequence is currently being played, while leaving it unset allows you to determine total playback time.

	 The same concept applies for conversion from beats to seconds.*/
	MIKMIDISequencerTimeConversionOptionsDontUnrollLoop = 1 << 2,
	/**
	 When this option is set, the sequencer's rate will be ignore, and the default rate of 1.0 will be used for time conversion calculations.
	 */
	MIKMIDISequencerTimeConversionOptionsIgnoreRate = 1 << 3,
};

NS_ASSUME_NONNULL_BEGIN

/**
 *  MIKMIDISequencer can be used to play and record to an MIKMIDISequence.
 *
 *  @note Recording and using the click track may not yet be fully functional, and should
 *	be considered experimental in the meantime. Please submit issues and/or pull requests
 *	when you find areas that don't work as expected.
 */
@interface MIKMIDISequencer : NSObject

#pragma mark - Creation

/**
 *  Convenience method for creating a new MIKMIDISequencer instance with an empty sequence.
 *
 *  @return An initialized MIKMIDISequencer.
 */
+ (instancetype)sequencer;

/**
 *  Initializes and returns  a new MIKMIDISequencer ready to playback and record to the
 *  specified sequence.
 *
 *  @param sequence The sequence to playback and record to.
 *
 *  @return An initialized MIKMIDISequencer.
 */
- (instancetype)initWithSequence:(MIKMIDISequence *)sequence;

/**
 *  Convenience method for creating a new MIKMIDISequencer ready to playback and
 *  record to the specified sequence.
 *
 *  @param sequence The sequence to playback and record to.
 *
 *  @return An initialized MIKMIDISequencer.
 */
+ (instancetype)sequencerWithSequence:(MIKMIDISequence *)sequence;

#pragma mark - Playback

/**
 *  Starts playback from the beginning of the sequence. 【//从序列的开头开始播放】
 */
- (void)startPlayback;

/**
 *  Starts playback from the specified time stamp. 【从指定的时间戳开始播放】
 *
 *  @param timeStamp The position in the sequence to begin playback from.
 */
- (void)startPlaybackAtTimeStamp:(MusicTimeStamp)timeStamp;

/**
 *  Starts playback from the specified MusicTimeStamp at the specified MIDITimeStamp.  【在指定的MIDITimeStamp处从指定的MusicTimeStamp开始播放。】
 *  This could be useful if you need to synchronize the playback with another source 【在指定的MIDITimeStamp处从指定的MusicTimeStamp开始播放。】
 *  such as an audio track, or another MIKMIDISequencer instance. 【例如音轨或另一个MIKMIDISequencer实例。】
 *
 *  @param timeStamp The position in the sequence to begin playback from.
 *  @param midiTimeStamp The MIDITimeStamp to begin playback at.
 */
- (void)startPlaybackAtTimeStamp:(MusicTimeStamp)timeStamp MIDITimeStamp:(MIDITimeStamp)midiTimeStamp;

/**
 *  Starts playback from the position returned by -currentTimeStamp. 【从-currentTimeStamp返回的位置开始播放。】
 *
 *  @see currentTimeStamp
 */
- (void)resumePlayback;

/**
 *  Stops all playback and recording. 【停止所有播放和录制。】
 */
- (void)stop;

/**
 *	Sends any pending note offs for the command scheduler immeidately. 【立即发送命令调度程序的所有待处理注释。】
 *	This can be useful if you are changing the notes in the MIDI track and
 *	you want the old notes to immediately stop rather than play until their
 *	original end time stamp. 【如果您要更改MIDI轨道中的音符，并且希望旧音符立即停止播放而不是播放直到其原始结束时间戳记，则此功能很有用。】
 */
- (void)stopAllPlayingNotesForCommandScheduler:(id<MIKMIDICommandScheduler>)scheduler;

/**
 *	Allows subclasses to modify the MIDI commands that are about to be
 *	scheduled with a command scheduler. 【允许子类修改即将成为的MIDI命令使用命令调度程序进行调度。】
 *
 *	@param commandsToBeScheduled An array of MIKMIDICommands that are about
 *	to be scheduled. 【即将调度的MIKMIDICommand数组。】
 *
 *	@param scheduler The command scheduler the commands will be scheduled with
 *	after they are modified. 【修改命令后将与之一起调度的命令调度程序。】
 *
 *	@note You should not call this method directly. It is made public solely to
 *	give subclasses a chance to alter or override any MIDI commands parsed from the
 *	MIDI sequence before they get sent to their destination endpoint. 【您不应该直接调用此方法。 仅向在将子类发送到目标端点之前，让子类有机会更改或覆盖从MIDI序列解析的任何MIDI命令。】
 *
 */
- (MIKArrayOf(MIKMIDICommand *) *)modifiedMIDICommandsFromCommandsToBeScheduled:(MIKArrayOf(MIKMIDICommand *) *)commandsToBeScheduled forCommandScheduler:(id<MIKMIDICommandScheduler>)scheduler;

/**
 *	Sets the loopStartTimeStamp and loopEndTimeStamp properties.
 *
 *	@param loopStartTimeStamp The MusicTimeStamp to begin looping at.
 *
 *	@param loopEndTimeStamp The MusicTimeStamp to end looping at. To have
 *	the loop end at the end of the sequence, regardless of sequence length, 
 *	pass in MIKMIDISequencerEndOfSequenceLoopEndTimeStamp.
 *
 *	@see loopStartTimeStamp
 *	@see loopEndTimeStamp
 *	@see loop
 *	@see looping
 */
- (void)setLoopStartTimeStamp:(MusicTimeStamp)loopStartTimeStamp endTimeStamp:(MusicTimeStamp)loopEndTimeStamp;


#pragma mark - Recording

/**
 *  Starts playback from the beginning of the sequence minus the value returned
 *  by -preRoll, and enables recording of incoming events to the record enabled tracks.
 *
 *  @see preRoll
 *  @see recordEnabledTracks
 */
- (void)startRecording;

/**
 *  Starts playback from the specified time stamp minus the value returned by
 *  -preRoll, and enables recording of incoming events to the record enabled tracks.
 *
 *  @see preRoll
 *  @see recordEnabledTracks
 */
- (void)startRecordingAtTimeStamp:(MusicTimeStamp)timeStamp;

/**
 *  Starts playback from the specified MusicTimeStamp minus the value returned by -preRoll
 *  at the specified MIDITimeStamp, and enables recording of incoming events to the
 *  record enabled tracks.
 *
 *  @see preRoll
 *  @see recordEnabledTracks
 */
- (void)startRecordingAtTimeStamp:(MusicTimeStamp)timeStamp MIDITimeStamp:(MIDITimeStamp)midiTimeStamp;

/**
 *  Starts playback from the position returned by -currentTimeStamp minus the
 *  value returned by -preRoll, and enables recording of incoming events to the
 *  record enabled tracks.
 *
 *  @see preRoll
 *  @see recordEnabledTracks
 */
- (void)resumeRecording;

/**
 *  Records a MIDI command to the record enabled tracks.
 *
 *  @param command The MIDI command to record to the record enabled tracks.
 *
 *  @note When recording is NO, calls to this method will do nothing.
 *
 *  @see recording
 *  @see recordEnabledTracks
 */
- (void)recordMIDICommand:(MIKMIDICommand *)command;

#pragma mark - Configuration

/**
 *  Sets the command scheduler for a track in the sequencer's sequence. 【为定序器序列中的音轨设置命令调度器。】
 *  Calling this method is optional. By default, the sequencer will setup internal synthesizers
 *	so that playback "just works". 【调用此方法是可选的。 默认情况下，音序器将设置内部合成器使播放“正常”。】
 *
 *  @note If track is not contained by the receiver's sequence, this method does nothing. 【如果接收方的序列中不包含磁道，则此方法不执行任何操作。】
 *
 *  @param commandScheduler	An object that conforms to MIKMIDICommandScheduler with which events
 *	in track should be scheduled during playback. MIKMIDIDestinationEndpoint and MIKMIDISynthesizer both conform to MIKMIDICommandScheduler, so they can be used here. Pass nil to remove an existing command scheduler. 【符合MIKMIDICommandScheduler的对象，通过它可以在播放期间安排轨道中的事件。MIKMIDIDestinationEndpoint和MIKMIDISynthesizer都符合MIKMIDICommandScheduler，因此可以在此处使用。 传递nil删除现有的命令调度程序。】
 *  @param track	An MIKMIDITrack instance. 【一个MIKMIDITrack实例。】
 */
- (void)setCommandScheduler:(nullable id<MIKMIDICommandScheduler>)commandScheduler forTrack:(MIKMIDITrack *)track;

/**
 *  Returns the command scheduler for a track in the sequencer's sequence.  【返回音序器序列中音轨的命令调度器。】
 *
 *  MIKMIDISequencer will automatically create its own default synthesizers connected 
 *	for any tracks not configured manually. This means that even if you haven't called
 *	-setCommandScheduler:forTrack:, you can use this method to retrieve
 *  the default command scheduler for a given track.
 *
 *  @note If track is not contained by the receiver's sequence, this method returns nil.
 *
 *  @param track An MIKMIDITrack instance.
 *
 *  @return The command scheduler associated with track, or nil if one can't be found.
 *
 *  @see -setCommandScheduler:forTrack:
 *  @see -builtinSynthesizerForTrack:
 *	@see createSynthsIfNeeded
 */
- (nullable id<MIKMIDICommandScheduler>)commandSchedulerForTrack:(MIKMIDITrack *)track;

/**
 *  Returns synthesizer the receiver will use to synthesize MIDI during playback 【返回合成器，接收机在播放期间将用来合成MIDI】
 *  for any tracks whose MIDI has not been routed to a custom scheduler using
 *  -setCommandScheduler:forTrack:. For tracks where a custom scheduler has
 *  been set, this method returns nil.
 *
 *  The caller is free to reconfigure the synthesizer(s) returned by this method,
 *  e.g. to load a custom soundfont file or select a different instrument.  【*调用者可以自由地重新配置此方法返回的合成器，例如 加载自定义声音字体文件或选择其他乐器。】
 *
 *  @param track The track for which the builtin synthesizer is desired.
 *
 *  @return An MIKMIDISynthesizer instance, or nil if a builtin synthesizer for track doesn't exist.
 */
- (nullable MIKMIDISynthesizer *)builtinSynthesizerForTrack:(MIKMIDITrack *)track;

#pragma mark - Time Conversion


/** Returns the time in seconds for a given MusicTimeStamp (time in beats).  【返回给定MusicTimeStamp的时间（以秒为单位）。】
 *
 *  This method converts a time in beats to the corresponding time in seconds on the sequencer, taking into account the tempo of the sequence, including tempo changes.
 *  By default, looping and an overridden tempo, if enabled, will be considered when calculating the result. This behavior can be changed by passing in the appropriate options.
 *  【*该方法在考虑到音序的速度（包括速度变化）的情况下，将拍子中的时间转换为音序器中相应的时间（以秒为单位）。默认情况下，在计算结果时将考虑循环和覆盖的速度（如果启用）。 可以通过传入适当的选项来更改此行为。】
 *
 * @param musicTimeStamp The time in beats you want to convert to seconds.
 * @param options Options to control the details of the conversion algorithm. See MIKMIDISequencerTimeConversionOptions for a list of possible options.
 *
 * @return A time in seconds as an NSTimeInterval.
 *
 * @see -musicTimeStampForTimeInSeconds:options:
 * @see -[MIKMIDISequence musicTimeStampForTimeInSeconds:]
 */
- (NSTimeInterval)timeInSecondsForMusicTimeStamp:(MusicTimeStamp)musicTimeStamp options:(MIKMIDISequencerTimeConversionOptions)options;

/** Returns the time in beats for a given time in seconds.  【以秒为单位返回给定时间的节拍时间。】
 *
 * @param timeInSeconds The time in seconds you want to convert to a MusicTimeStamp (beats).
 * @param options Options to control the details of the conversion algorithm. See MIKMIDISequencerTimeConversionOptions for a list of possible options.
 *
 * @return A time in beats as a MusicTimeStamp.
 *
 * @see -timeInSecondsForMusicTimeStamp:options:
 * @see -[MIKMIDISequence timeInSecondsForMusicTimeStamp:]
 */
- (MusicTimeStamp)musicTimeStampForTimeInSeconds:(NSTimeInterval)timeInSeconds options:(MIKMIDISequencerTimeConversionOptions)options;

#pragma mark - Properties

/**
 *  The sequence to playback and record to.
 */
@property (nonatomic, strong) MIKMIDISequence *sequence;

/**
 *	Whether or not the sequencer is currently playing. This can be observed with KVO.
 *
 *  @see recording
 */
@property (readonly, nonatomic, getter=isPlaying) BOOL playing;

/**
 *  Whether or not the sequence is currently playing and is record enabled.
 *  This can be observed with KVO.
 *
 *  @note When recording is YES, events will only be recorded to the tracks
 *  specified by -recordEnabledTracks.
 *
 *  @see playing
 *  @see recordEnabledTracks
 */
@property (readonly, nonatomic, getter=isRecording) BOOL recording;

/**
 * @property rate
 * @abstract The playback rate of the sequencer. For example, if rate is 2.0, the sequencer will play twice as fast as normal.
 *  Unlike the tempo property, this does not override the tempos in the sequence's tempo track. Rather, they are adjusted by multiplying by this rate.
 * @discussion
 * 	1.0 is normal playback rate.  Rate must be > 0.0.
*/
@property (nonatomic) float rate;

/**
 *  The tempo the sequencer should play its sequence at. When set to 0, the sequence will be played using 
 *  the tempo events from the sequence's tempo track. Default is 0.
 */
@property (nonatomic) Float64 tempo;

/**
 *  The length the that the sequencer should consider its sequence to be. When set to 0, the sequencer
 *  will use sequence.length instead.  【定序器应考虑其序列的长度。 设置为0时，定序器将改用sequence.length。】
 *
 *  This can be handy if you want to alter the duration of playback to be shorter or longer
 *  than the sequence's length without affecting the sequence itself. 【如果要在不影响序列本身的情况下将播放时间更改为短于或长于序列的长度，这可能很方便。】
 */
@property (nonatomic) MusicTimeStamp overriddenSequenceLength;

/**
 *  The current playback position in the sequence. 【序列中的当前播放位置。】
 *
 *  @note This property is *not* observable using Key Value Observing.
 */
@property (nonatomic) MusicTimeStamp currentTimeStamp;


/**
 *  The amount of time (in beats) to pre-roll the sequence before recording.
 *  For example, if preRoll is set to 4 and you begin recording, the sequence
 *  will start 4 beats ahead of the specified recording position.
 * 【在录制之前预滚动序列的时间（以拍子为单位）。例如，如果将preRoll设置为4，然后开始录制，则序列将在指定的录制位置之前开始4拍。】
 *  The default is 4.
 */
@property (nonatomic) MusicTimeStamp preRoll;

/**
 *  Whether or not playback should loop when between loopStartTimeStamp and loopEndTimeStamp.【在loopStartTimeStamp和loopEndTimeStamp之间播放时是否应循环播放。】
 *
 *  @see loopStartTimeStamp
 *  @see loopEndTimeStamp
 *  @see looping
 */
@property (nonatomic, getter=shouldLoop) BOOL loop;

/**
 *  Whether or not playback is currently looping between loopStartTimeStamp and loopEndTimeStamp.
 *
 *  @note If loop is YES, and playback starts before loopStartTimeStamp, looping will be NO until
 *  currentTimeStamp reaches loopStartTimeStamp. At that point, looped playback will begin and
 *  the looping property will become YES. Conversely, if playback starts after loopEndTimeStamp,
 *  then the looped area of playback will never be reached and looping will remain NO.
 *
 *  @see loop
 *  @see loopStartTimeStamp
 *  @see loopEndTimeStamp
 *	@see -setLoopStartTimeStamp:loopEndTimeStamp:
 *  @see currentTimeStamp
 */
@property (readonly, nonatomic, getter=isLooping) BOOL looping;

/**
 *  The loop's beginning time stamp during looped playback.
 *
 *	@see -setLoopStartTimeStamp:loopEndTimeStamp:
 */
@property (readonly, nonatomic) MusicTimeStamp loopStartTimeStamp;

/**
 *  The loop's ending time stamp during looped playback, or MIKMIDISequencerEndOfSequenceLoopEndTimeStamp.
 *
 *	@note When this is set to MIKMIDISequencerEndOfSequenceLoopEndTimeStamp
 *	the loopEndTimeStamp will be treated as if it is set to the length of the
 *	sequence. The default is MIKMIDISequencerEndOfSequenceLoopEndTimeStamp.
 *
  *	@see effectiveLoopEndTimeStamp
 *	@see -setLoopStartTimeStamp:loopEndTimeStamp:
 */
@property (readonly, nonatomic) MusicTimeStamp loopEndTimeStamp;

/**
 *	The loop's ending time stamp during looped playback.
 *
 *	@note When loopEndTimeStamp is set to MIKMIDISequencerEndOfSequenceLoopEndTimeStamp,
 *	this will return the same length as the sequence.length. Otherwise loopEndTimeStamp
 *	will be returned.
 */
@property (readonly, nonatomic) MusicTimeStamp effectiveLoopEndTimeStamp;


/**
 *	Whether or not the sequencer should create synthesizers and endpoints
 *	for MIDI tracks that are not assigned an endpoint.  【音序器是否应为未分配端点的MIDI轨道创建合成器和端点。】
 *
 *	When this property is YES, -commandSchedulerForTrack: will create a 
 *	synthesizer for any track that has MIDI commands scheduled for it
 *	and doesn't already have an assigned scheduler. The default for this property
 *	is YES.
 *  【当此属性为YES时，-commandSchedulerForTrack：将为已为其安排MIDI命令且尚未分配调度程序的任何轨道创建一个合成器。 此属性的默认值为YES。】
 *	@see -commandSchedulerForTrack:
 */
@property (nonatomic, getter=shouldCreateSynthsIfNeeded) BOOL createSynthsIfNeeded;

/**
 *  The metronome to send click track events to.  【节拍器向其发送点击跟踪事件。】
 */
@property (nonatomic, strong, nullable) MIKMIDIMetronome *metronome;

/**
 *  When the click track should be heard.
 *  The default is MIKMIDISequencerClickTrackStatusEnabledInRecord. 【*当应该听到点击声时。默认值为MIKMIDISequencerClickTrackStatusEnabledInRecord。】
 */
@property (nonatomic) MIKMIDISequencerClickTrackStatus clickTrackStatus;

/**
 *  The tracks to record incoming MIDI events to while recording is enabled. 【*启用了在录制时将传入的MIDI事件录制到的音轨。】
 *
 *  Each incoming event is added to every track in this set.
 *
 *  @see recording
 *
 */
@property (nonatomic, copy, nullable) MIKSetOf(MIKMIDITrack *) *recordEnabledTracks;

/**
 *  An MIKMIDIClock that is synced with the sequencer's internal clock. 【与音序器的内部时钟同步的MIKMIDIClock。】
 *
 *  @  @see -[MIKMIDIClock syncedClock]
 */
@property (nonatomic, readonly) MIKMIDIClock *syncedClock;


/**
 *  The latest MIDITimeStamp the sequencer has looked ahead to to schedule MIDI events.
 */
@property (nonatomic, readonly) MIDITimeStamp latestScheduledMIDITimeStamp;


/**
 *	The maximum amount the sequencer will look ahead to schedule MIDI events. (0.05 to 1s). 【音序器可以预先安排MIDI事件的最大数量。 （0.05至1秒）。】
 *
 *	The default of 0.1s should suffice for most uses. You may however, need a longer time
 *	if your sequencer needs to playback on iOS while the device is locked.
 *   【*默认值0.1s足以满足大多数用途。 但是，您可能需要更长的时间如果您的音序器需要在设备锁定时在iOS上播放。】
 */
@property (nonatomic) NSTimeInterval maximumLookAheadInterval;


#pragma mark - 扩充的功能

/// 移调
@property (nonatomic) SInt8 moveNote;


#pragma mark - Deprecated

/**
 *	@deprecated Use -setCommandScheduler:forTrack: instead.
 *
 *  Sets the destination endpoint for a track in the sequencer's sequence.
 *  Calling this method is optional. By default, the sequencer will setup internal default endpoints
 *  connected to synthesizers so that playback "just works".
 *
 *  @note If track is not contained by the receiver's sequence, this method does nothing.
 *
 *  @param endpoint The MIKMIDIDestinationEndpoint instance to which events in track should be sent during playback.
 *  @param track    An MIKMIDITrack instance.
 */
- (void)setDestinationEndpoint:(MIKMIDIDestinationEndpoint *)endpoint forTrack:(MIKMIDITrack *)track __attribute((deprecated("use -setCommandScheduler:forTrack: instead")));

/**
 *	@deprecated Use -commandSchedulerForTrack: instead.
 *
 *  Returns the destination endpoint for a track in the sequencer's sequence.
 *
 *  MIKMIDISequencer will automatically create its own default endpoints connected to
 *  MIKMIDISynthesizers for any tracks not configured manually. This means that even if you
 *  haven't called -setDestinationEndpoint:forTrack:, you can use this method to retrieve
 *  the default endpoint for a given track.
 *
 *  @note If track is not contained by the receiver's sequence, this method returns nil.
 *
 *  @param track An MIKMIDITrack instance.
 *
 *  @return The destination endpoint associated with track, or nil if one can't be found.
 *
 *  @see -setDestinationEndpoint:forTrack:
 *  @see -builtinSynthesizerForTrack:
 *	@see createSynthsAndEndpointsIfNeeded
 */
- (nullable MIKMIDIDestinationEndpoint *)destinationEndpointForTrack:(MIKMIDITrack *)track __attribute((deprecated("use -setCommandScheduler:forTrack: instead")));

@end


/**
 *  Sent out shortly before playback loops.
 */
FOUNDATION_EXPORT NSString * const MIKMIDISequencerWillLoopNotification;

/**
 *	Set loopEndTimeStamp to this to have the loop end at the end of the
 *	sequence regardless of sequence length.
 */
FOUNDATION_EXPORT const MusicTimeStamp MIKMIDISequencerEndOfSequenceLoopEndTimeStamp;

NS_ASSUME_NONNULL_END
