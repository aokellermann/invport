/**
 * @file util.h
 * @author Antony Kellermann
 * @copyright 2020 Antony Kellermann
 */

#pragma once

#include <gtkmm.h>

#include <string>
#include <type_traits>

namespace inv::widget
{
namespace detail
{
inline void AssertWidgetIsValid(const Gtk::Widget* widget)
{
  // This is fatal. builder->get_widget prints error message, so we don't have to.

  if (!widget)
  {
    exit(EXIT_FAILURE);
  }
}
}  // namespace detail

template <typename T>
static T& GetWidget(const Glib::RefPtr<Gtk::Builder>& builder, const std::string& name)
{
  T* widget;
  builder->get_widget(name, widget);
  detail::AssertWidgetIsValid(widget);
  return *widget;
}

template <typename T, typename... Args>
static T& GetWidgetDerived(const Glib::RefPtr<Gtk::Builder>& builder, const std::string& name, Args&&... args)
{
  T* widget;
  builder->get_widget_derived(name, widget, args...);
  detail::AssertWidgetIsValid(widget);
  return *widget;
}

}  // namespace inv::widget
