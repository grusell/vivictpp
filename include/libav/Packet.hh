// SPDX-FileCopyrightText: 2021 Sveriges Television AB
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef PACKET_HH_
#define PACKET_HH_

#include <memory>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

namespace vivictpp {
namespace libav {

class Packet {
public:
  Packet();
  Packet(AVPacket *pkt);
  ~Packet() = default;
  AVPacket* avPacket();
  bool empty() { return !packet; }
private:
  std::shared_ptr<AVPacket> packet;
};

}  // namespace libav
}  // namespace vivictpp

#endif  // PACKET_HH_
