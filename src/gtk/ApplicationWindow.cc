#include "gtk/ApplicationWindow.hh"
#include <gtkmm.h>


vivictpp::gtk::ApplicationWindow::ApplicationWindow():
  Gtk::ApplicationWindow(),
  eventListener(std::make_shared<EventListenerProxy>()),
  logger(vivictpp::logging::getOrCreateLogger("ApplicationWindow"))
{

  set_title("Vivict++");
  set_default_size(1280, 720);
//  set_border_width(10);

  add_events(Gdk::KEY_PRESS_MASK);
  add_events(Gdk::POINTER_MOTION_MASK);

  add(overlay);
  overlay.add(drawingArea);
  drawingArea.set_vexpand(true);


  drawingArea.add_events(Gdk::POINTER_MOTION_MASK);
  drawingArea.signal_motion_notify_event()
    .connect(sigc::mem_fun(*this, &vivictpp::gtk::ApplicationWindow::on_mouse_motion));

  signal_window_state_event()
    .connect(sigc::mem_fun(*this, &vivictpp::gtk::ApplicationWindow::onWindowStateEvent));
  signal_key_press_event()
    .connect(sigc::mem_fun(*this, &vivictpp::gtk::ApplicationWindow::on_key_press_event), false);
  drawingArea.show();
  overlay.show();

}

void vivictpp::gtk::ApplicationWindow::setFullscreen(bool _fullscreen) {
  if (_fullscreen) {
    fullscreen();
  } else {
    unfullscreen();
  }
}

bool vivictpp::gtk::ApplicationWindow::on_key_press_event(GdkEventKey* keyEvent) {
//  guint unicodeKey = gdk_keyval_to_unicode(key_event->keyval);

  vivictpp::KeyModifiers modifiers{
    !!(keyEvent->state & GDK_SHIFT_MASK),
    !!(keyEvent->state & GDK_CONTROL_MASK),
    !!(keyEvent->state & GDK_META_MASK)
  };
  // gdk_keymap_translate_keyboard_state

  char utf8string[10];
  guint32 unichar = gdk_keyval_to_unicode(keyEvent->keyval);
  int ll = g_unichar_to_utf8(unichar, utf8string);
  utf8string[ll] = '\0';
  std::string key(utf8string);
  std::cout << "key pressed: " << key << std::endl;
  logger->debug("key pressed: {}", key);
  /*
  if (key == "f") {
    if (this->isFullscreen) {
      unfullscreen();
    } else {
      fullscreen();
    }
  } else if (key == "q") {
    hide();
  } else if (key == "l") {

  } else if (key == "k") {

  }
  */
  if (key == " ") {
    key = "Space";
  } else {
    for (auto & c: key) c = toupper(c);
  }
  eventListener->keyPressed(key, modifiers);
  //   Glib::signal_timeout().connect( sigc::mem_fun(*this, &IdleExample::on_timer), 50 );

  return true;
}


bool vivictpp::gtk::ApplicationWindow::onWindowStateEvent(GdkEventWindowState* pEvent) {
    this->isFullscreen = (pEvent->new_window_state &
GDK_WINDOW_STATE_FULLSCREEN);

    return true; // we want to keep getting this signal
}

bool vivictpp::gtk::ApplicationWindow::on_mouse_motion(GdkEventMotion *motion_event) {
  logger->trace("onMouseMotion x={} y={}", motion_event->x, motion_event->y);
  eventListener->mouseMotion(motion_event->x, motion_event->y);
  return true;
}

// Our new improved signal handler.  The data passed to this method is
// printed to stdout.
void vivictpp::gtk::ApplicationWindow::on_button_clicked(Glib::ustring data)
{
  (void) data;
  logger->debug("Hello World - something was pressed");
}

bool vivictpp::gtk::ApplicationWindow::onTimeout(int timerId) {
  logger->debug("vivictpp::gtk::ApplicationWindow::onTimeout timerId={}", timerId);
  (void) timerId;
  switch (timerId) {
  case advanceFrameTimerId:
    eventListener->advanceFrame();
    break;
  case refreshDisplayTimerId:
    eventListener->refreshDisplay();
    break;
  default:
    break;
  }
  return false;
}

void vivictpp::gtk::ApplicationWindow::displayFrame(const std::array<vivictpp::libav::Frame, 2> &frames,
                                                const vivictpp::ui::DisplayState &displayState) {
  logger->debug("vivictpp::gtk::ApplicationWindow::displayFrame");
  drawingArea.setData(frames, displayState);
  drawingArea.queue_draw();
}

void vivictpp::gtk::ApplicationWindow::scheduleAdvanceFrame(int delay) {
  advanceFrameConnection = setTimer(delay, advanceFrameTimerId);
}

void vivictpp::gtk::ApplicationWindow::scheduleRefreshDisplay(int delay) {
  setTimer(delay, refreshDisplayTimerId);
}

sigc::connection vivictpp::gtk::ApplicationWindow::setTimer(int delay, int timerId) {
        sigc::slot<bool> my_slot = sigc::bind(sigc::mem_fun(*this,
                                                            &vivictpp::gtk::ApplicationWindow::onTimeout), timerId);
        return Glib::signal_timeout().connect(my_slot, delay);
}



vivictpp::gtk::DrawingArea::DrawingArea():
  logger(vivictpp::logging::getOrCreateLogger("DrawingArea")){

}

Glib::RefPtr<Gdk::Pixbuf> createImage(AVFrame *frame, const VideoMetadata &metadata) {
  return Gdk::Pixbuf::create_from_data(frame->data[0],
                                       Gdk::COLORSPACE_RGB,
                                       false,
                                       8,
                                       metadata.width,
                                       metadata.height,
                                       frame->linesize[0]);
}

bool vivictpp::gtk::DrawingArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr) {
  logger->debug("vivictpp::gtk::DrawingArea::on_draw");
//  logger->debug("linesize={}", frame->linesize[0]);
  Gtk::Allocation allocation = get_allocation();

  const int width = allocation.get_width();
  const int height = allocation.get_height();
  double scale = std::min((double) width / leftVideoMetadata.width, (double) height / leftVideoMetadata.height);
  cr->save();
  cr->scale(scale, scale);
  (void) scale;
  logger->debug("width={}, height={}, img-width={}, image-height={}, scale={}", width, height, leftVideoMetadata.width, leftVideoMetadata.height, scale);

  AVFrame* frame = frames[0].avFrame();
  auto image = createImage(frame, leftVideoMetadata);
  Gdk::Cairo::set_source_pixbuf(cr, image);
//  cr->rectangle(0, 0, std::min(width, image->get_width()), std::min(height, image->get_height()));
  double w1 = frames[1].empty() ? image->get_width() : image->get_width() * displayState.splitPercent / 100.0;
  cr->rectangle(0, 0, w1, image->get_height());
  cr->fill();
  cr->restore();
  if (!frames[1].empty()) {
    cr->save();
    cr->begin_new_path();
    logger->debug("drawing right frame");
    scale = std::min((double) width / rightVideoMetadata.width, (double) height / rightVideoMetadata.height);
    cr->scale(scale, scale);
    image = createImage(frames[1].avFrame(), rightVideoMetadata);
    double x = image->get_width() * displayState.splitPercent / 100.0;
    Gdk::Cairo::set_source_pixbuf(cr, image);
    double w = image->get_width() - x;
    logger->debug("x={}, w={}", x, w);
    cr->rectangle(x, 0, w, image->get_height());
    cr->fill();

    cr->begin_new_path();
    cr->move_to(x,0);
    cr->line_to(x, image->get_height());
    cr->set_source_rgba(255, 255, 255, 50);
    cr->set_line_width(1);
    cr->stroke();

    cr->restore();
  }
  



/*  
  const int rectangle_width = width;
  const int rectangle_height = height / 2;

  // Draw a black rectangle
  cr->set_source_rgb(0.3, 0.3, 0.3);
  cr->rectangle(0, 0, rectangle_width, rectangle_height);
  cr->fill();
*/
  return true;
}
