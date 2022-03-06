// SPDX-FileCopyrightText: 2022 Sveriges Television AB
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef APPLICATION_WINDOW_HH
#define APPLICATION_WINDOW_HH

#include <gtkmm/applicationwindow.h>
#include <gtkmm/window.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm/overlay.h>


#include "libav/Frame.hh"
#include "ui/VivictPPUI.hh"
#include "ui/DisplayState.hh"
#include "VideoMetadata.hh"
#include "logging/Logging.hh"



namespace vivictpp {
namespace gtk {

class DrawingArea : public Gtk::DrawingArea {
public:
  DrawingArea();
  virtual ~DrawingArea() {};
  void setData(const std::array<vivictpp::libav::Frame, 2> &frames,
               const vivictpp::ui::DisplayState &displayState){
    this->frames = frames;
    this->displayState = displayState;
  };
  friend class ApplicationWindow;
  
protected:
  bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;

private:
  std::array<vivictpp::libav::Frame, 2> frames;
  vivictpp::ui::DisplayState displayState;
  VideoMetadata leftVideoMetadata;
  VideoMetadata rightVideoMetadata;
  vivictpp::logging::Logger logger;
};

class EventListenerProxy: vivictpp::EventListener {
public:
  EventListenerProxy():
    eventListener(nullptr) {}
  void mouseDrag(const int xrel, const int yrel) override {
    if (eventListener) eventListener->mouseDrag(xrel, yrel);
  };
  void mouseMotion(const int x, const int y) override {
    if (eventListener) eventListener->mouseMotion(x,y);
  };
  void mouseWheel(const int x, const int y) override {
    if (eventListener) eventListener->mouseWheel(x,y);
  };
  void mouseClick(const int x, const int y) override {
    if (eventListener) eventListener->mouseClick(x,y);
  };
  void keyPressed(const std::string &key, const KeyModifiers &modifiers) override {
    if (eventListener) eventListener->keyPressed(key, modifiers);
  };
  void advanceFrame() override {
    if (eventListener) eventListener->advanceFrame();
  };
  void queueAudio() override {
    if (eventListener) eventListener->queueAudio();
  };
  void refreshDisplay() override {
    if (eventListener) eventListener->refreshDisplay();
  };
  void fade() override {
    if (eventListener) eventListener->fade();
  };
  void set(vivictpp::EventListener *eventListener) {
    this->eventListener = eventListener;
  }
  void reset() {
    this->eventListener = nullptr;
  }

private:
  vivictpp::EventListener *eventListener;

};

const int advanceFrameTimerId = 1;
const int refreshDisplayTimerId = 2;
const int queueAudioTimerId = 3;
const int fadeTimerId = 4;

class ApplicationWindow : public Gtk::ApplicationWindow, public vivictpp::ui::VivictPPUI {
public:
  ApplicationWindow();
  virtual ~ApplicationWindow() = default;
  void displayFrame(const std::array<vivictpp::libav::Frame, 2> &frames,
                    const vivictpp::ui::DisplayState &displayState) override;
  int getWidth() override { return get_width(); };
  int getHeight() override { return get_height(); };
  void setFullscreen(bool _fullscreen) override;
//  void setCursorHand();
//  void setCursorDefault();
  void setLeftMetadata(const VideoMetadata &metadata) override {
    drawingArea.leftVideoMetadata = metadata;
  };
  void setRightMetadata(const VideoMetadata &metadata) override {
    drawingArea.rightVideoMetadata = metadata;
  };

  void scheduleAdvanceFrame(int delay) override;
  void scheduleRefreshDisplay(int delay) override;
  void scheduleQueueAudio(int delay) override {
    (void) delay;
  };
  void scheduleFade(int delay) override {
    (void) delay;
  };
  void clearAdvanceFrame() override { };
  void start(EventListener *eventListener) override {
    this->eventListener->set(eventListener);
  };
  void stop() override {
    hide();
    this->eventListener->reset();
  };


protected:
  void on_button_clicked(Glib::ustring data);
  bool on_mouse_motion(GdkEventMotion *motion_event);
  bool on_key_press_event(GdkEventKey* key_event) override;
  bool onWindowStateEvent(GdkEventWindowState* pEvent);
  bool onTimeout(int timer_number);
  sigc::connection setTimer(int delay, int timerId);

  Gtk::Overlay overlay;
  DrawingArea drawingArea;
  sigc::connection advanceFrameConnection;
  bool isFullscreen = false;
  std::shared_ptr<EventListenerProxy> eventListener;
  vivictpp::logging::Logger logger;
};


}  // namespace gtk
}  // namespace vivictpp


#endif  // GTK_APPLICATION_HH
