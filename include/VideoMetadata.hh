// SPDX-FileCopyrightText: 2020 Sveriges Television AB
// SPDX-FileCopyrightText: 2021 Sveriges Television AB
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef VIDEOMETADATA_HH_
#define VIDEOMETADATA_HH_

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/pixdesc.h>
}

#include <iomanip>
#include <sstream>
#include <string>

#include "Resolution.hh"
#include "time/Time.hh"

class VideoMetadata {
public:
  VideoMetadata():
    resolution(0,0) {};
  VideoMetadata(std::string source, AVFormatContext *formatContext,
                AVStream *videoStream);
  std::string source;
  int streamIndex;
  std::string pixelFormat;
  int width;
  int height;
  Resolution resolution;
  int bitrate;
  double frameRate;
  vivictpp::time::Time frameDuration;
  vivictpp::time::Time startTime;
  vivictpp::time::Time duration;
  vivictpp::time::Time endTime;
  std::string codec;

  std::string toString() const;
};

#endif // VIDEOMETADATA_HH_
