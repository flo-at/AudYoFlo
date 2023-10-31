#ifndef _IJVXPROPERTYEXTENDERNFTFPROVIDER_H_
#define _IJVXPROPERTYEXTENDERNFTFPROVIDER_H_

JVX_INTERFACE IjvxPropertyExtenderNftfProvider;

JVX_INTERFACE IjvxPropertyExtenderNftfProvider_report
{
public:
	virtual ~IjvxPropertyExtenderNftfProvider_report() {};
	virtual jvxErrorType report_position(IjvxPropertyExtenderNftfProvider* origin, jvxData* xyzPosiLeftEar_3, jvxData* xyzPosiRightEar_3, jvxData* rxryrzRotationHead_3) = 0;
	virtual jvxErrorType report_nftf_fname(IjvxPropertyExtenderNftfProvider* origin, const char* fNameNftf) = 0;
	virtual jvxErrorType report_quality(IjvxPropertyExtenderNftfProvider* origin, jvxRemoteConnectionQuality qual, jvxData qualValue = -1) = 0;
};

JVX_INTERFACE IjvxPropertyExtenderNftfProvider
{
public:
	virtual ~IjvxPropertyExtenderNftfProvider() {};
	virtual jvxErrorType register_listener(IjvxPropertyExtenderNftfProvider_report* ptr, const char* tag) = 0;
	virtual jvxErrorType unregister_listener(IjvxPropertyExtenderNftfProvider_report* ptr) = 0;
};

#endif