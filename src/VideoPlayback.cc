#include "VideoPlayback.hh"

int vivictpp::VideoPlayback::SeekState::seekStart(vivictpp::time::Time seekTarget) {
  std::lock_guard<std::mutex> lg(m);
  currentSeekId++;
  seekDone = false;
  error = false;
  this->seekTarget = seekTarget;
  return currentSeekId;
};

void vivictpp::VideoPlayback::SeekState::seekFinished(int seekId, vivictpp::time::Time pos, bool err) {
  std::lock_guard<std::mutex> lg(m);
  if (seekId != currentSeekId) {
    return;
  }
  seekDone = true;
  seekEndPos = pos;
  error = err;
}

vivictpp::VideoPlayback::VideoPlayback(VivictPPConfig vivictPPConfig):
  videoInputs(vivictPPConfig),
  frameDuration(vivictPPConfig.sourceConfigs.size() == 1 ?
                videoInputs.metadata()[0][0].frameDuration :
                std::min(videoInputs.metadata()[0][0].frameDuration,
                         videoInputs.metadata()[1][0].frameDuration)),
  logger(vivictpp::logging::getOrCreateLogger("vivictpp::VideoPlayback"))
{
  playbackState.pts = videoInputs.startTime();
  playbackState.duration = videoInputs.duration();
}

void vivictpp::VideoPlayback::togglePlaying() {
  if (playbackState.seeking) {
    return;
  }
  if (!playbackState.playing) {
    play();
  } else {
    pause();
  }
}

void vivictpp::VideoPlayback::play() {
  t0 = vivictpp::time::relativeTimeMicros();
  playbackStartPts = playbackState.pts;
  playbackState.playing = true;
}

void vivictpp::VideoPlayback::pause() { playbackState.playing = false; }

void vivictpp::VideoPlayback::seek(vivictpp::time::Time seekPts) {
  seekPts = std::max(seekPts, videoInputs.minPts());
  if (videoInputs.hasMaxPts()) {
    seekPts = std::min(seekPts, videoInputs.maxPts());
  }
  if (!playbackState.seeking && videoInputs.ptsInRange(seekPts)) {
    advanceFrame(seekPts);
    stepped = true;
    if (playbackState.playing) {
      playbackStartPts = seekPts;
      t0 = vivictpp::time::relativeTimeMicros();
    }
    // TODO: Make make special method for this
    int seekId = seekState.seekStart(seekPts);
    seekState.seekFinished(seekId, seekPts, false);
  } else {
    playbackState.seeking = true;
    int seekId = seekState.seekStart(seekPts);
    videoInputs.seek(seekPts, [this, seekId](vivictpp::time::Time pos, bool error) {
      this->seekState.seekFinished(seekId, pos, error);
    });
  }
}

void vivictpp::VideoPlayback::seekRelative(vivictpp::time::Time deltaPts) {
  if (playbackState.seeking) {
    seek(seekState.seekTarget + deltaPts);
  } else {
    seek(playbackState.pts + deltaPts);
  }
}

void vivictpp::VideoPlayback::seekRelativeFrame(int distance) {
  if (distance == 0) return;
  if (playbackState.seeking) {
    seek(seekState.seekTarget + distance * frameDuration);
  } else {
    vivictpp::time::Time seekPts;
    if (distance == 1) seekPts = videoInputs.nextPts();
    else if (distance == -1) seekPts = videoInputs.previousPts();
    else seekPts = playbackState.pts + distance * frameDuration;
    if (vivictpp::time::isNoPts(seekPts)) {
      seekPts = playbackState.pts + distance * frameDuration;
    }
    logger->info("seekRelativeFrame  seeking to {}", seekPts);
    seek(seekPts);
  }
}

bool vivictpp::VideoPlayback::checkAdvanceFrame(int64_t nextPresent) {
  if (playbackState.seeking && !seekState.seekDone) {
    // videoInputs.dropIfFullAndNextOutOfRange(nextPts, 1);
    return false;
  }
  if (playbackState.seeking) {
    logger->info("checkAdvanceFrame playbackState.seeking=true");
    advanceFrame(seekState.seekEndPos);
    playbackState.seeking = false;
    if (playbackState.playing) {
      playbackStartPts = seekState.seekEndPos;
      t0 = vivictpp::time::relativeTimeMicros();
    }
    return true;
  }
  if (!playbackState.playing) {
    if (stepped) {
      stepped = false;
      return true;
    }
    return false;
  }
  vivictpp::time::Time nextPts = videoInputs.nextPts();
  if (videoInputs.ptsInRange(nextPts)) {
    if ((nextPresent - t0) >= (nextPts - playbackStartPts)) {
      advanceFrame(nextPts);
      return true;
    } else {
      return false;
    }
  }
  videoInputs.dropIfFullAndNextOutOfRange(nextPts, 1);
  return false;
};

void vivictpp::VideoPlayback::advanceFrame(vivictpp::time::Time nextPts) {
  logger->info("advanceFrame nextPts={}", nextPts);
  bool forward = nextPts > playbackState.pts;
  playbackState.pts = nextPts;
  if (forward) {
    videoInputs.stepForward(playbackState.pts);
  } else {
    videoInputs.stepBackward(playbackState.pts);
  }
}
