#ifndef __FFMPEG_HELPERS_H__
#define __FFMPEG_HELPERS_H__

#include "jvx.h"

extern "C"
{
#include "libavformat/avformat.h"
#include "libavutil/opt.h"
#include "libavcodec/avcodec.h"
}

extern const char* optionsWav[];
extern const char* optionsMp3M4a[];
extern const char* fileFormatPostfixTranslator[];
#define JVX_OPTION_INIT_WAV 0
#define JVX_OPTION_INIT_MP3M4A 4

int check_stream_specifier(AVFormatContext* s, AVStream* st, const char* spec);

AVDictionary* filter_codec_opts(AVDictionary* opts, enum AVCodecID codec_id,
	AVFormatContext* s, AVStream* st, const AVCodec* codec);

AVDictionary** setup_find_stream_info_opts(AVFormatContext* s,
	AVDictionary* codec_opts);

struct jvxFfmpegAudioParameter
{
	std::string fFormatTag;
	jvxSize nChans = 0;
	jvxSize sRate = 0;
	jvxSize frameSize = 0;
	jvxSize frameSizeMax = 0;
	jvxSize bitRate = 0;
	jvxSize bitsPerRaw = 0;
	jvxSize bitsPerCoded = 0;
	jvxBool isFloat = false;
	jvxSize bSizeAudio = 0;

	enum AVCodecID idCodec = AV_CODEC_ID_NONE;
	AVMediaType tpCodec = AVMEDIA_TYPE_AUDIO;
	AVChannelLayout chanLayout;
	AVSampleFormat sFormatId = AV_SAMPLE_FMT_NONE;
	std::string codec_selection ;

	void reset()
	{
		fFormatTag.clear();
		nChans = 0;
		sRate = 0;
		frameSize = 0;
		bitRate = 0;
		bitsPerRaw = 0;
		bitsPerCoded = 0;
		isFloat = false;
		bSizeAudio = 0;
		
		chanLayout.nb_channels = 0;
		chanLayout.order = AV_CHANNEL_ORDER_UNSPEC;
		chanLayout.u.mask = 0;
		tpCodec = AVMEDIA_TYPE_UNKNOWN;
		sFormatId = AV_SAMPLE_FMT_NONE;

		codec_selection.clear();
	};
};

struct jvxFfmpegEncoderAudioParameter: public jvxFfmpegAudioParameter
{
	const AVCodec* enc = nullptr;
	AVCodecContext* enc_ctx = nullptr;
	AVFrame* frame = nullptr;
	int64_t pts_current = 0;
	void reset()
	{
		enc = nullptr;
		enc_ctx = nullptr;
		frame = nullptr;
	}
};

struct jvxFfmpegFileAudioParameter : public jvxFfmpegAudioParameter
{
	std::string fName;
	AVStream* st = nullptr;
	void reset()
	{
		jvxFfmpegAudioParameter::reset();
		fName.clear();
		st = nullptr;
	};
};

struct jvxFfmpegInputFileAudioParameter : public jvxFfmpegFileAudioParameter
{

	AVFormatContext* ic = nullptr;
	// AVPacket* pkt = nullptr;
	AVInputFormat* iformat = nullptr;
	
	jvxSize lengthSamples = 0;
	jvxData lengthSecs = 0;
	jvxSize numFrames = 0;

	void reset()
	{
		jvxFfmpegFileAudioParameter::reset();
		ic = nullptr;
		// pkt = nullptr;
		iformat = nullptr;
		// bSizeMax = 0;
		// sizePerSample = 0;

		lengthSamples = 0;
		lengthSecs = 0;
		numFrames = 0;
	};
};

struct jvxFfmpegOutputFileParameter : public jvxFfmpegFileAudioParameter
{
	const AVOutputFormat* fmt = nullptr;
	AVFormatContext* oc = nullptr;
	AVCodecContext* enc_ctx = nullptr;
	AVStream* st = nullptr;
	std::string subTypeSpec;
	jvxSize idSub = JVX_SIZE_UNSELECTED;
	std::string fPrefix;
	std::string fSuffix;

	void reset()
	{
		jvxFfmpegFileAudioParameter::reset();
		idSub = JVX_SIZE_UNSELECTED;
		st = nullptr;
		enc_ctx = nullptr;
		fmt = nullptr;
		oc = nullptr;
		st = nullptr;
		subTypeSpec.clear();
		idSub = JVX_SIZE_UNSELECTED;
		fPrefix.clear();
		fSuffix.clear();
	};
};

struct jvxFfmpegAudioParameterDerived
{
	std::string sFormat;
	std::string chanLayoutTag;
	
	std::string codecIdTag;
	std::string codecTypeTag;

	void reset()
	{
		sFormat.clear();
		chanLayoutTag.clear();

		codecIdTag.clear();
		codecTypeTag.clear();
	}
};

std::string jvx_ffmpeg_parameter_2_codec_token(const jvxFfmpegAudioParameter& params);
jvxErrorType jvx_ffmpeg_codec_token_2_parameter(const char* tokenArg, jvxFfmpegAudioParameter& params);

void jvx_ffmpeg_verify_correct_codec_settings(jvxFfmpegAudioParameter& params);
void jvx_ffmpeg_update_derived(const jvxFfmpegAudioParameter& params, jvxFfmpegAudioParameterDerived& derived);
void jvx_ffmpeg_wav_params(jvxFfmpegAudioParameter& params);

void jvx_ffmpeg_update_format_settings_wav(jvxFfmpegAudioParameter& params, jvxSize idSubType);
void jvx_ffmpeg_update_format_settings_mp3(jvxFfmpegAudioParameter& params, jvxSize idSubType);

void jvx_ffmpeg_printout_params(jvxFfmpegAudioParameter& params);
void jvx_ffmpeg_printout_file_params(jvxFfmpegFileAudioParameter& params);
void jvx_ffmpeg_printout_input_file_params(jvxFfmpegInputFileAudioParameter& params);

#endif
