/**
 * This header is generated by class-dump-z 0.2a.
 * class-dump-z is Copyright (C) 2009 by KennyTM~, licensed under GPLv3.
 *
 * Source: /System/Library/PrivateFrameworks/BackRow.framework/BackRow
 */

#import "BackRow-Structs.h"
#import "BRControl.h"

@class BRWaitSpinnerControl, BRImage, NSString, BRAsyncTask;
@protocol BRImageProxy;

@interface BRAsyncImageControl : BRControl {
@private
	id<BRImageProxy> _imageProxy;	// 40 = 0x28
	BRWaitSpinnerControl *_spinner;	// 44 = 0x2c
	BRImage *_image;	// 48 = 0x30
	BRImage *_defaultImage;	// 52 = 0x34
	BRAsyncTask *_imageProcessingTask;	// 56 = 0x38
	NSString *_imageID;	// 60 = 0x3c
	BOOL _cropAndFill;	// 64 = 0x40
	BOOL _useLoadingSpinner;	// 65 = 0x41
	BOOL _loadFailure;	// 66 = 0x42
	BOOL _imageLoadRequested;	// 67 = 0x43
	BOOL _useLoadFailureImage;	// 68 = 0x44
	BOOL _loadOnActivation;	// 69 = 0x45
	BOOL _okToLoadImage;	// 70 = 0x46
	BOOL _useFadeInAnimation;	// 71 = 0x47
	int _requestedSize;	// 72 = 0x48
}
@property(retain) BRImage *image;	// G=0x315dc005; S=0x315dc8fd; converted property
@property(retain) id imageProxy;	// G=0x315dc911; S=0x315dc941; converted property
+ (id)imageControlWithImage:(id)image;	// 0x315dca81
+ (id)imageControlWithImageProxy:(id)imageProxy;	// 0x315dcb29
- (id)initWithImage:(id)image;	// 0x315dca31
- (id)initWithImageProxy:(id)imageProxy;	// 0x315dcacd
- (void)_cropImage:(id)image;	// 0x315dd4c9
- (void)_enableFadeInAnimation;	// 0x315dc62d
- (void)_enableSpinner;	// 0x315dc5f9
- (void)_fetchCachedThumbnailImage;	// 0x315dd255
- (void)_fetchPreferredSizeImage;	// 0x315dd0b9
- (id)_getImageFromImageManagerNamed:(id)imageManagerNamed withSize:(CGSize)size;	// 0x315dc411
- (id)_imageOfSize:(int)size;	// 0x315dd36d
- (void)_imageTaskComplete:(id)complete;	// 0x315dc491
- (void)_imageUnavailable:(id)unavailable;	// 0x315dc379
- (void)_imageUpdated:(id)updated;	// 0x315dcb69
- (void)_scaleImage:(id)image;	// 0x315dc52d
- (void)_setImage:(id)image;	// 0x315dc311
- (void)_setImageID:(id)anId;	// 0x315dc2d9
- (void)_startImageProcessingTaskForImage:(id)image;	// 0x315dcce5
- (void)_updateContents;	// 0x315dc065
- (void)cancelImageLoading;	// 0x315dc831
- (void)controlWasActivated;	// 0x315dc769
- (void)controlWasDeactivated;	// 0x315dc72d
- (void)dealloc;	// 0x315dc979
// converted property getter: - (id)image;	// 0x315dc005
// converted property getter: - (id)imageProxy;	// 0x315dc911
- (void)layoutSubcontrols;	// 0x315dcf49
- (void)loadImage;	// 0x315dc8d9
- (void)setCropAndFill:(BOOL)fill;	// 0x315dc035
- (void)setDefaultImage:(id)image;	// 0x315dc6e5
// converted property setter: - (void)setImage:(id)image;	// 0x315dc8fd
// converted property setter: - (void)setImageProxy:(id)proxy;	// 0x315dc941
- (void)setLoadImageOnActivation:(BOOL)activation;	// 0x315dc015
- (void)setUseFadeInAnimation:(BOOL)animation;	// 0x315dc055
- (void)setUseLoadFailureImage:(BOOL)image;	// 0x315dc045
- (void)setUseLoadingSpinner:(BOOL)spinner;	// 0x315dc025
@end
