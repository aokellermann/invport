/**
 * @file base.h
 * @author Antony Kellermann
 * @copyright 2020 Antony Kellermann
 */

#pragma once

#include <gtkmm.h>

#include <utility>

namespace inv::widget
{
/**
 * All widgets should derive from this. It contains access to the Gtk Builder, which can be used to access other UI
 * elements by name.
 */
struct WidgetBase
{
  explicit WidgetBase(Glib::RefPtr<Gtk::Builder> bldr) : builder(std::move(bldr)) {}

  Glib::RefPtr<Gtk::Builder> builder;
};
}  // namespace inv::widget
