#include "gtk/Application.hh"
#include <gtkmm.h>

#include "SourceConfig.hh"

vivictpp::gtk::Application::Application()
: Gtk::Application("org.gtkmm.examples.application", Gio::APPLICATION_HANDLES_OPEN)
{
}

Glib::RefPtr<vivictpp::gtk::Application> vivictpp::gtk::Application::create()
{
  return Glib::RefPtr<vivictpp::gtk::Application>(new vivictpp::gtk::Application());
}

vivictpp::gtk::ApplicationWindow* vivictpp::gtk::Application::createAppwindow()
{
  auto appwindow = new ApplicationWindow();

  // Make sure that the application runs for as long this window is still open.
  add_window(*appwindow);

  // Gtk::Application::add_window() connects a signal handler to the window's
  // signal_hide(). That handler removes the window from the application.
  // If it's the last window to be removed, the application stops running.
  // Gtk::Window::set_application() does not connect a signal handler, but is
  // otherwise equivalent to Gtk::Application::add_window().

  // Delete the window when it is hidden.
  appwindow->signal_hide().connect(sigc::bind<Gtk::Window*>(sigc::mem_fun(*this,
    &vivictpp::gtk::Application::onHideWindow), appwindow));

  return appwindow;
}

void vivictpp::gtk::Application::on_activate()
{
  // The application has been started, so let's show a window.
  auto appwindow = createAppwindow();
  appwindow->present();
}

void vivictpp::gtk::Application::on_open(const Gio::Application::type_vec_files& files,
  const Glib::ustring& /* hint */)
{
  (void) files;
  // The application has been asked to open some files,
  // so let's open a new view for each one.
  ApplicationWindow* appwindow = nullptr;
  auto windows = get_windows();
  if (windows.size() > 0)
    appwindow = dynamic_cast<ApplicationWindow*>(windows[0]);

  if (!appwindow)
    appwindow = createAppwindow();

  std::vector<SourceConfig> sourceConfigs;
  for (const auto& file : files)
    sourceConfigs.push_back(SourceConfig(file->get_path()));
  VivictPPConfig vivictPPConfig(sourceConfigs, true);
  controller = std::make_shared<vivictpp::Controller>(appwindow, appwindow, vivictPPConfig);
  controller->run();
  appwindow->present();

}

void vivictpp::gtk::Application::onHideWindow(Gtk::Window* window)
{
  controller.reset();
  delete window;
}
