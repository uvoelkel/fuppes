/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            PlaylistParser.cpp
 *
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2007-2008 Ulrich Völkel <u-voelkel@users.sourceforge.net>
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
 
#include "PlaylistParser.h"
#include "../Common/RegEx.h"
#include "../Common/XMLParser.h"

#include <fstream>
#include <iostream>

using namespace std;

BasePlaylistParser::BasePlaylistParser(std::string p_sFileName)
{
  m_bEof = true;
  m_nPosition = 0;
  m_sListPath = ExtractFilePath(p_sFileName);
}

BasePlaylistParser::~BasePlaylistParser()
{
  while (!m_lEntries.empty()) {
    delete m_lEntries.front();
    m_lEntries.pop_front();
  }
}
    
std::string ReadFile(std::string p_sFileName)
{
  fstream fsPlaylist;
  char*   szBuf;
  long    nSize;  
  string  sResult;
  
  fsPlaylist.open(p_sFileName.c_str(), ios::in);
  if(fsPlaylist.fail())
    return ""; 
   
  fsPlaylist.seekg(0, ios::end); 
  nSize = streamoff(fsPlaylist.tellg()); 
  fsPlaylist.seekg(0, ios::beg);
  szBuf = new char[nSize + 1];  
  fsPlaylist.read(szBuf, nSize); 
  szBuf[nSize] = '\0';
  fsPlaylist.close();
    
  sResult = szBuf;
  delete[] szBuf;  
   
  return sResult;
}

BasePlaylistParser* BasePlaylistParser::Load(std::string p_sFileName)
{
  string sResult = ReadFile(p_sFileName);
  if (sResult.compare("") == 0) {
    return NULL;
  }
  
  BasePlaylistParser* playlistResult = NULL;
  
  if(sResult.length() > strlen("#EXTM3U") && sResult.substr(0, strlen("#EXTM3U")).compare("#EXTM3U") == 0) {
    //cout << "M3U" << endl;
    playlistResult = new M3U_PlaylistParser(p_sFileName);
  }
  else if(sResult.length() > strlen("[playlist]") && sResult.substr(0, strlen("[playlist]")).compare("[playlist]") == 0) {
    //cout << "PLS" << endl;
    playlistResult = new PLS_PlaylistParser(p_sFileName);
  }
	else if(ToLower(ExtractFileExt(p_sFileName)).compare("rss") == 0) {
    //cout << "RSS" << endl;
    playlistResult = new RSS_PlaylistParser(p_sFileName);
	}

  if (playlistResult != NULL) {
    playlistResult->Parse(sResult);
  }
  
  return playlistResult;
}

PlaylistEntry_t* BasePlaylistParser::Entry()
{
  return (PlaylistEntry_t*)*m_lEntriesIterator;
}

void BasePlaylistParser::Next()
{
  if(m_lEntriesIterator == m_lEntries.end()) {
    m_bEof = true;
  } else if (m_lEntries.size() > 0) {
    ++m_lEntriesIterator;
  }
}

/*
Sample M3U File:

#EXTM3U

#EXTINF:123,Sample title
C:\Documents and Settings\I\My Music\Sample.mp3

#EXTINF:321,Example title
C:\Documents and Settings\I\My Music\Greatest Hits\Example.ogg
*/

bool M3U_PlaylistParser::Parse(std::string p_sContent)
{
  PlaylistEntry_t* pEntry = NULL;
  
  RegEx rxLines("(.*)\n", PCRE_CASELESS);
  RegEx rxFile("^[#EXTM3U]", PCRE_CASELESS);  
  //RegEx rxData("^[#EXTINF]", PCRE_CASELESS);
  RegEx rxData("^[#EXTINF]:([0-9]+),(.*)\n", PCRE_CASELESS);
  if(!rxLines.Search(p_sContent.c_str())) {
    return false;
  }
  
  playlistLength = 0;
  do
  {
    if (rxFile.Search(rxLines.Match(1))) {
      continue;
    }
    
    if (rxData.Search(rxLines.Match(1))) {
      if (pEntry != NULL) {
        // TODO Log error for two EXTINF lines in a row
        return false;
      } else {
        pEntry = new PlaylistEntry_t();
      }

      //  #EXTINF:123,Sample title
      pEntry->length = atoi(rxData.Match(1).c_str());
      playlistLength += pEntry->length;

      pEntry->sTitle = rxData.Match(2);
      continue;
    }
        
    if (pEntry == NULL) {
      // TODO Log error for two EXTINF lines in a row
      return false;
    }
    if(IsURL(rxLines.Match(1))) {       
      pEntry->sFileName = rxLines.Match(1);
      pEntry->bIsLocalFile = false;
    }
    else {
      pEntry->sFileName = FormatFileName(rxLines.Match(1));
      pEntry->bIsLocalFile = true;
    }
    m_lEntries.push_back(pEntry);
    pEntry = NULL;  // allow us to catch errors
  } while(rxLines.SearchAgain());  
    
  if(!m_lEntries.empty()) {
    m_bEof = false;
    m_lEntriesIterator = m_lEntries.begin();
  }
  return true;
}

bool PLS_PlaylistParser::Parse(std::string p_sContent)
{
	PlaylistEntry_t* pEntry;
	RegEx rxNumEntries("NumberOfEntries=(\\d+)", PCRE_CASELESS);
		
	int numEntries = 0;
		
	if(rxNumEntries.Search(p_sContent.c_str())) {
		numEntries = atoi(rxNumEntries.Match(1).c_str());
	}
		
	if(numEntries == 0)
		return false;
		
	for(int i = 0; i < numEntries; i++) {
		
		stringstream sExpr;
    sExpr << "File" << (i + 1) << "=(.+)\\n";    
    RegEx rxFile(sExpr.str().c_str(), PCRE_CASELESS);
    if(!rxFile.Search(p_sContent.c_str())) {
      // TODO Log the incorrect format
      continue;
    }

    pEntry = new PlaylistEntry_t();
    if(IsURL(rxFile.Match(1))) {       
      pEntry->sFileName = rxFile.Match(1);
      pEntry->bIsLocalFile = false;
    }
    else {
      pEntry->sFileName = FormatFileName(rxFile.Match(1));
      pEntry->bIsLocalFile = true;
    }
    m_lEntries.push_back(pEntry);

	}

  // TODO If the Version is not 2 then Log an error message (read PLS spec)

  if(!m_lEntries.empty()) {
    m_bEof = false;
    m_lEntriesIterator = m_lEntries.begin();
  }
		
  return true;
}

bool RSS_PlaylistParser::Parse(std::string p_sContent)
{
		
	CXMLDocument* pDoc = new CXMLDocument();
	if(!pDoc->LoadFromString(p_sContent)) {
      // TODO Log the incorrect format
		delete pDoc;
		return false;
	}
	
	CXMLNode* pTmp;
	pTmp = pDoc->RootNode()->FindNodeByName("channel");
	if(!pTmp) {
      // TODO Log the incorrect format
		delete pDoc;
		return false;
	}
	
	for(int i = 0; i < pTmp->ChildCount(); i++) {
		if(pTmp->ChildNode(i)->Name().compare("item") != 0) {
      // TODO Log the incorrect format
			continue;
		}
		
		CXMLNode* pEnc = pTmp->ChildNode(i)->FindNodeByName("enclosure");
		if(!pEnc) {
      // TODO Log the incorrect format
			continue;
		}
		
		PlaylistEntry_t* pEntry = new PlaylistEntry_t();	
			
		pEntry->sFileName = pEnc->Attribute("url");
		pEntry->nSize = pEnc->AttributeAsUInt("length");
		pEntry->sMimeType = pEnc->Attribute("type");
		
		pEntry->bIsLocalFile = false;
			
		if(pTmp->ChildNode(i)->FindNodeByName("title")) {
			pEntry->sTitle = pTmp->ChildNode(i)->FindNodeByName("title")->Value();
		}
			
		m_lEntries.push_back(pEntry);
		
	}
		
/*		<channel>
	<title>Marillion Online Podcast</title>
	<link>http://www.marillion.com</link>
	<language>en</language>
	<copyright>&#x2117; &amp; &#xA9; 2006 Marillion</copyright>
	<pubDate>Wed, 13 Dec 2006 15:00:00 GMT</pubDate>
	<itunes:subtitle>Find a Better Way of Life at marillion.com</itunes:subtitle>
	<itunes:author>Marillion</itunes:author>
	<itunes:summary>Official insider information for British rock band Marillion. Whether writing in the studio, recording new material, preparing for a live show, or on the road - get the scoop DIRECT from the band members themselves. Find a Better Way of Life at marillion.com</itunes:summary>
	<description>Official insider information for British rock band Marillion. Whether writing in the studio, recording new material, preparing for a live show, or on the road - get the scoop DIRECT from the band members themselves. Find a Better Way of Life at marillion.com</description>
				
		
		<item>
		<title>Album Number 14 Begins</title>
		<itunes:author>Marillion</itunes:author>
		<itunes:subtitle>Recording is set to begin on the next studio album</itunes:subtitle>
		<itunes:summary>26 May 2006. Marillion have been jamming song ideas since the beginning of 2006 and have now come up with a short-list of contenders for the next studio album. Coming to you direct from the live room at the Racket Club studio with Mark Kelly, Ian Mosley, Pete Trewavas, Mike Hunter, and the Racket Records Peanut Gallery.</itunes:summary>
		<enclosure url="http://media.marillion.com/podcast/20060526.mp3" length="3361560" type="audio/mpeg" />
		<guid>http://media.marillion.com/podcast/20060526.mp3</guid>
		<pubDate>Fri, 26 May 2006 09:00:00 GMT</pubDate>
		<itunes:duration>4:00</itunes:duration>
		<itunes:explicit>yes</itunes:explicit>
		<itunes:keywords>marillion, studio, new, album, recording, update, racket, hogarth, kelly, mosley, trewavas, rothery</itunes:keywords>
	</item>	*/
		
	delete pDoc;
		
  if(!m_lEntries.empty()) {
    m_bEof = false;
    m_lEntriesIterator = m_lEntries.begin();
  }

	return true;
}

bool WPL_PlaylistParser::Parse(std::string p_sContent)
{
	CXMLDocument* pDoc = new CXMLDocument();
	if(!pDoc->LoadFromString(p_sContent)) {
		delete pDoc;
		return false;
	}

  // Parse <head> data
  CXMLNode* pHead;
  pHead = pDoc->RootNode()->FindNodeByName("head");
  if (!pHead) {
    delete pDoc;
    return false;
  }
  
  // parse <body> data
  CXMLNode* pSeq;
  pSeq = pDoc->RootNode()->FindNodeByName("body");
  if (!pSeq) pSeq = pSeq->FindNodeByName("seq");
  if (!pSeq) {
    delete pDoc;
    return false;
  }
  
  int children = pSeq->ChildCount();
  CXMLNode* child;
  PlaylistEntry_t* pEntry;
  for (int i = 0; i < children; ++i) {
    child = pSeq->ChildNode(i);
		if(child->Name().compare("media") != 0) {
      continue;
    }
    
    pEntry = new PlaylistEntry_t();
    if(IsURL(child->Value())) {       
      pEntry->sFileName = child->Value();
      pEntry->bIsLocalFile = false;
    }
    else {
      pEntry->sFileName = FormatFileName(child->Value());
      pEntry->bIsLocalFile = true;
    }
    m_lEntries.push_back(pEntry);
  }

  if(!m_lEntries.empty()) {
    m_bEof = false;
    m_lEntriesIterator = m_lEntries.begin();
  }

  delete pDoc;
  return true;
}

/*
Example Windows Media Playlist file from wikipedia:

<?wpl version="1.0"?>
<smil>
    <head>
        <meta name="Generator" content="Microsoft Windows Media Player -- 11.0.5721.5145"/>
        <meta name="AverageRating" content="33"/>
        <meta name="TotalDuration" content="1102"/>
        <meta name="ItemCount" content="3"/>
        <author/>
        <title>Bach Organ Works</title>
    </head>
    <body>
        <seq>
            <media src="\\server\vol\music\Classical\Bach\OrganWorks\cd03\track01.mp3"/>
            <media src="\\server\vol\music\Classical\Bach\OrganWorks\cd03\track02.mp3"/>
            <media src="\\server\vol\music\Classical\Bach\OrganWorks\cd03\track03.mp3"/>
        </seq>
    </body>
</smil>
*/

//
// Helper Functions
//

std::string BasePlaylistParser::FormatFileName(std::string p_sValue)
{
  if(p_sValue.length() <= 2)
    return p_sValue;
  
  //bool bRelative = false;
  
  #ifdef WIN32  
  if(p_sValue.substr(0, 2).compare(".\\") == 0) {
    p_sValue = p_sValue.substr(2, p_sValue.length());    
  }
  
  if(p_sValue.substr(1, 2).compare(":") != 0) {
    p_sValue = m_sListPath + p_sValue;
  }
  #else
  if(p_sValue.substr(0, 2).compare("./") == 0) {
    p_sValue = p_sValue.substr(2, p_sValue.length());    
  }
  
  if(p_sValue.substr(0, 1).compare("/") != 0) {
    p_sValue = m_sListPath + p_sValue;
  }
  #endif
  
  return p_sValue;
}

bool BasePlaylistParser::IsURL(std::string p_sValue)
{
  RegEx rxUrl("\\w+://", PCRE_CASELESS);
  return rxUrl.Search(p_sValue.c_str());
}
