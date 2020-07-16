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

/**
 * Gets a widget from a builder.
 * @tparam T The type of widget.
 * @param builder The builder that contains the widget.
 * @param name The name of the widget.
 * @warning Exits if widget is not found.
 * @return A reference to the widget.
 */
template <typename T>
T& GetWidget(const Glib::RefPtr<Gtk::Builder>& builder, const std::string& name)
{
  T* widget;
  builder->get_widget(name, widget);
  detail::AssertWidgetIsValid(widget);
  return *widget;
}

/**
 * Gets a derived widget from a builder.
 * @tparam T The derived type of widget.
 * @tparam Args Type of args passed to constructor.
 * @param builder The builder that contains the widget.
 * @param name The name of the widget.
 * @param args Parameter pack of args passed to constructor.
 * @return A reference to the widget.
 */
template <typename T, typename... Args>
T& GetWidgetDerived(const Glib::RefPtr<Gtk::Builder>& builder, const std::string& name, Args&&... args)
{
  T* widget;
  builder->get_widget_derived(name, widget, args...);
  detail::AssertWidgetIsValid(widget);
  return *widget;
}

}  // namespace inv::widget
