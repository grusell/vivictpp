// SPDX-FileCopyrightText: 2022 Sveriges Television AB
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef APPLICATION_HH
#define APPLICATION_HH

#include "gtkmm/application.h"
#include "gtkmm/window.h"

#include "gtk/ApplicationWindow.hh"
#include "Controller.hh"

#include <memory>


namespace vivictpp {
namespace gtk {

class Application : public Gtk::Application {
protected:
  Application();

public:
  static Glib::RefPtr<Application> create();

protected:
  // Override default signal handlers:
  void on_activate() override;
  void on_open(const Gio::Application::type_vec_files& files,
    const Glib::ustring& hint) override;

private:
  ApplicationWindow* createAppwindow();
  void onHideWindow(Gtk::Window* window);

  std::shared_ptr<vivictpp::Controller> controller;
};

}  // namespace gtk
}  // namespace vivictpp

#endif  // APPLICATION_HH
