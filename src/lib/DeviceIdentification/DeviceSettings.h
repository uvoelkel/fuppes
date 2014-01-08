/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            DeviceSettings.h
 * 
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2007-2010 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _DEVICESETTINGS_H
#define _DEVICESETTINGS_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string>
#include <list>
#include <map>

#include "../UPnP/UPnPObjectTypes.h"
#include "../Common/Common.h"

struct CImageSettings {
  
  friend class CDeviceConfigFile;
  
  CImageSettings();
  CImageSettings(CImageSettings* pImageSettings);
  
  std::string  sExt;
  std::string  sMimeType;  
  
  // dcraw
  bool    bDcraw;
  std::string  sDcrawParams;

  std::string Extension() { return sExt; }
  std::string MimeType() { return sMimeType; }
  
  bool Enabled() { return bEnabled; }
  
  // ImageMagick
  bool bGreater;
	bool bLess;
	int  nWidth;
	int  nHeight;
	enum { resize, scale } nResizeMethod; // resize = better quality (lower) | scale = lower quality (faster)

  bool Greater() { return bGreater; }
  bool Less() { return bLess; }
  int  Width() { return nWidth; }
  int  Height() { return nHeight; }
  
  private:
    bool bEnabled;
};

typedef struct {
  bool bShowChildCountInTitle;
  int  nMaxFileNameLength;
} DisplaySettings_t;

typedef enum TRANSCODING_TYPE {
  TT_NONE,
  TT_RENAME,
  TT_THREADED_DECODER_ENCODER,
  TT_TRANSCODER,
  TT_THREADED_TRANSCODER  
} TRANSCODING_TYPE;

/*
typedef enum TRANSCODER_TYPE {
  TTYP_NONE,
  TTYP_IMAGE_MAGICK,
  //TTYP_FFMPEG,
	TTYP_EXTERNAL_CMD
} TRANSCODER_TYPE;
*/

/*
typedef enum ENCODER_TYPE {
  ET_NONE,
  ET_LAME,
  ET_TWOLAME,
  ET_WAV,
  ET_PCM
} ENCODER_TYPE;
*/
/*
typedef enum DECODER_TYPE {
  DT_NONE ,
  DT_OGG_VORBIS,
  DT_FLAC,
  DT_MUSEPACK,
  DT_FAAD,
	DT_MAD
} DECODER_TYPE;
*/

typedef enum TRANSCODING_HTTP_RESPONSE {
  RESPONSE_STREAM,
  RESPONSE_CHUNKED
} TRANSCODING_HTTP_RESPONSE;

/*struct CFFmpegSettings {
  
  private:
    std::string   sVcodec;
    std::string   sAcodec;
    int           nVBitRate;
    int           nVFrameRate;
    int           nABitRate;
    int           nASampleRate;
};*/

struct CTranscodingSettings {
  
    friend class CDeviceConfigFile;
  
    CTranscodingSettings();  
    CTranscodingSettings(CTranscodingSettings* pTranscodingSettings);
  
    std::string   sExt;
    std::string   sMimeType;
    //std::string   sDLNA;
  
    /*std::string   sDecoder;     // vorbis | flac | mpc
    std::string   sEncoder;     // lame | twolame | pcm | wav
    std::string   sTranscoder;  // ffmpeg*/
  
    std::string   sOutParams;
  
    std::string MimeType() { return sMimeType; }
    //std::string DLNA() { return sDLNA; }
    bool Enabled() { return bEnabled; }
  
    unsigned int AudioBitRate() { return nAudioBitRate; }
    unsigned int AudioSampleRate() { return nAudioSampleRate; }
    //unsigned int VideoBitRate() { return nVideoBitRate; }
  
    std::string  AudioCodec(std::string p_sACodec = "");
    //std::string  VideoCodec(std::string p_sVCodec = "");
  
    //std::string  FFmpegParams() { return sFFmpegParams; }
  
    int LameQuality() { return nLameQuality; }
  
    std::string  Extension() { return sExt; }
    TRANSCODING_HTTP_RESPONSE   TranscodingHTTPResponse() { return nTranscodingResponse; }
  
    int ReleaseDelay() { return nReleaseDelay; }
  
    //std::list<CFFmpegSettings*>   pFFmpegSettings;
  
    TRANSCODING_TYPE TranscodingType() { return nTranscodingType; }
    /*TRANSCODER_TYPE TranscoderType() { return TTYP_NONE; }
    DECODER_TYPE    DecoderType() { return DT_NONE; }
    ENCODER_TYPE    EncoderType() { return ET_NONE; }*/

    std::string    getTranscoderName() { return m_transcoderName; }
    std::string    getDecoderName() { return m_decoderName; }
    std::string    getEncoderName() { return m_encoderName; }
   
    bool  DoTranscode(std::string p_sACodec, std::string p_sVCodec);
  
		std::string ExternalCmd() { return sExternalCmd; }
		
  private:
    bool          bEnabled;
  
    TRANSCODING_HTTP_RESPONSE   nTranscodingResponse;
    TRANSCODING_TYPE            nTranscodingType;
    /* 
    TRANSCODER_TYPE             nTranscoderType;
    DECODER_TYPE                nDecoderType;
    ENCODER_TYPE                nEncoderType;
    */
   
    std::string                 m_transcoderName;
    std::string                 m_decoderName;
    std::string                 m_encoderName;
    int                         nReleaseDelay;
  
    unsigned int  nAudioBitRate;
    unsigned int  nAudioSampleRate;
  
    //unsigned int  nVideoBitRate;
  
    int           nLameQuality;
  
    std::string     sACodecCondition;
    std::string     sVCodecCondition;
    std::string     sACodec;
    //std::string     sVCodec;
  
    //std::string     sFFmpegParams;
		std::string			sExternalCmd;
};

struct CFileSettings {
  
  friend class CDeviceConfigFile;
  
  CFileSettings();
  CFileSettings(CFileSettings* pFileSettings);
  ~CFileSettings();

  /*
   * returns the original or the target mime type depending on whether transcoding is enabled
   */
  std::string   MimeType(std::string p_sACodec = "", std::string p_sVCodec = "");
  /*
   * always returns the original mime type
   */
  std::string   originalMimeType() { return sMimeType; }
  //std::string   DLNA();
  
  unsigned int  TargetAudioSampleRate();
  unsigned int  TargetAudioBitRate();
  
  std::string   Extension(std::string p_sACodec = "", std::string p_sVCodec = "");
  
  TRANSCODING_HTTP_RESPONSE   TranscodingHTTPResponse();

  
  bool Enabled() { return bEnabled; }  
  void setEnabled(bool enabled) { bEnabled = enabled; }
  bool ExtractMetadata() { return bExtractMetadata; }
  
  CTranscodingSettings* pTranscodingSettings;
  CImageSettings*       pImageSettings;
  
  object_type_t   ObjectType() { return nType; }
  std::string   ObjectTypeAsStr();
  
  int ReleaseDelay();
  
  private:
    bool  bEnabled;
    bool  bExtractMetadata;
  
    std::string   sExt;
    object_type_t   nType;
    std::string   sMimeType;
    //std::string   sDLNA;  
};

typedef std::map<std::string, CFileSettings*>::iterator FileSettingsIterator_t;

struct CMediaServerSettings
{
	std::string		FriendlyName;
	std::string 	Manufacturer;
	std::string 	ManufacturerURL;
	std::string 	ModelName;
	std::string 	ModelNumber;
	std::string 	ModelURL;
	std::string		ModelDescription;
	bool					UseModelDescription;
	std::string 	SerialNumber;
	bool					UseSerialNumber;
	std::string		UPC;
	bool					UseUPC;

  
  enum DlnaVersion_t {
    dlna_none,
    dlna_1_0,
    dlna_1_5
  };
  
	//bool					UseDLNA;
  DlnaVersion_t DlnaVersion;

	bool					UseURLBase;
	bool					UseXMSMediaReceiverRegistrar;

  bool          rewriteUrls;
};

class CDeviceSettings
{
  // TODO using friends seems messy; but it also seems nessecary for now. 
  // If somebody can think of a better way then go ahead
  friend class CDeviceConfigFile;
  friend class CDeviceIdentificationMgr;
  
  public:

    enum PlaylistStyle {
      container,    // send playlists as container
      file,         // use playlist type of the original file
      pls,          // PLS
      m3u,          // m3u
      wpl,          // wpl
      xspf          // xspf
    };
    
	  CDeviceSettings(std::string p_sDeviceName);
    CDeviceSettings(std::string p_sDeviceName, CDeviceSettings* pSettings);
    ~CDeviceSettings();

    std::string       name() { return m_sDeviceName; }
    
    bool HasIP(std::string p_sIPAddress); 
		
    object_type_t       ObjectType(std::string p_sExt);
    std::string       ObjectTypeAsStr(std::string p_sExt);

    bool              DoTranscode(std::string p_sExt, std::string p_sACodec = "", std::string p_sVCodec = "");
    TRANSCODING_TYPE  GetTranscodingType(std::string p_sExt);
    std::string       getTranscoderName(std::string p_sExt, std::string p_sACodec = "", std::string p_sVCodec = "");
    std::string       getTranscoderCommand(std::string p_sExt, std::string p_sACodec = "", std::string p_sVCodec = "");
    std::string       getDecoderName(std::string p_sExt);
    std::string       getEncoderName(std::string p_sExt);
  
    std::string   MimeType(std::string p_sExt, std::string p_sACodec = "", std::string p_sVCodec = "");
    std::string   extensionByMimeType(std::string mimeType);
    //std::string   DLNA(std::string p_sExt);
  
    unsigned int  TargetAudioSampleRate(std::string p_sExt);
    unsigned int  TargetAudioBitRate(std::string p_sExt);
    
    bool          Exists(std::string p_sExt);
    std::string   Extension(std::string p_sExt, std::string p_sACodec = "", std::string p_sVCodec = "");
    TRANSCODING_HTTP_RESPONSE TranscodingHTTPResponse(std::string p_sExt);
  
    int ReleaseDelay(std::string p_sExt);
    
    DisplaySettings_t* DisplaySettings() { return &m_DisplaySettings; }
    CFileSettings* FileSettings(std::string p_sExt);
    void AddExt(CFileSettings* pFileSettings, std::string p_sExt);
    
		CMediaServerSettings* MediaServerSettings() { return &m_MediaServerSettings; }
		
    bool        EnableDeviceIcon() { return m_bEnableDeviceIcon; }  
    bool        Xbox360Support() { return m_bXBox360Support; }    
		//bool        ShowPlaylistAsContainer() { return m_bShowPlaylistAsContainer; }		
    PlaylistStyle playlistStyle() { return m_playlistStyle; }
    //bool        DLNAEnabled() { return MediaServerSettings()->UseDLNA; }
    CMediaServerSettings::DlnaVersion_t dlnaVersion() { return MediaServerSettings()->DlnaVersion; }
    bool				ShowEmptyResolution() { return m_bShowEmptyResolution; }
		
    /*std::string  virtualFolderLayout() { return m_virtualFolderLayout; }
    void  setVirtualFolderLayout(std::string layout) { m_virtualFolderLayout = layout; }*/

    std::string   protocolInfo();

    std::string   getSupportedObjectTypes();
    std::string   getSupportedFileExtensions();
    std::string   getHttpServerSignature() { return m_httpServerSignature; }
    void          setHttpServerSignature(std::string httpServerSignature) { m_httpServerSignature = httpServerSignature; }
    
  private:
    std::string m_sDeviceName;
    //std::string m_virtualFolderLayout;
    
    DisplaySettings_t m_DisplaySettings;
		CMediaServerSettings m_MediaServerSettings;
		
		//bool m_bShowPlaylistAsContainer;
    PlaylistStyle m_playlistStyle;
		bool m_bXBox360Support;
    //bool m_bDLNAEnabled;  
    bool m_bEnableDeviceIcon;
		bool m_bShowEmptyResolution;

    std::string   m_protocolInfo;
    
    std::string               m_cachedSupportedObjectTypes;
    std::string               m_cachedSupportedFileExtensions;
    std::string               m_httpServerSignature;
		
    std::map<std::string, CFileSettings*> m_FileSettings;
    std::map<std::string, CFileSettings*>::iterator m_FileSettingsIterator;
  
    int nDefaultReleaseDelay;
};

#endif // _DEVICESETTINGS_H
