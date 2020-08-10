/**
 * @file main.cc
 * @author author
 * @copyright 2020 author
 */

#include <gtkmm.h>
#include <iex/detail/singleton.h>
#include <spdlog/spdlog.h>

#include "invport/detail/keychain.h"
#include "invport/detail/transaction_history.h"
#include "invport/widget/key_selector.h"
#include "invport/widget/transaction_creator.h"
#include "invport/widget/util.h"

int main(int argc, char **argv)
{
  spdlog::set_pattern("%Y-%m-%d %H:%M:%S.%e %l : %v");
  spdlog::info("Starting invport.");

  // Create runnable application
  Glib::RefPtr<Gtk::Application> application = Gtk::Application::create(argc, argv, "com.aokellermann.invport");
  if (!application)
  {
    spdlog::error("Gtk::Application::create() failed.");
    return EXIT_FAILURE;
  }

  // Create builder (used for glade)
  Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create();
  if (!builder)
  {
    spdlog::error("Gtk::Builder::create() failed.");
    return EXIT_FAILURE;
  }

  // Parse glade file
  try
  {
    builder->add_from_file("../share/invport/invport.glade");
  }
  catch (const Glib::FileError &ex)
  {
    spdlog::error("File error: {}.", ex.what().c_str());
    return EXIT_FAILURE;
  }
  catch (const Glib::MarkupError &ex)
  {
    spdlog::error("Markup error: {}.", ex.what().c_str());
    return EXIT_FAILURE;
  }
  catch (const Gtk::BuilderError &ex)
  {
    spdlog::error("Builder error: {}.", ex.what().c_str());
    return EXIT_FAILURE;
  }

  spdlog::info("Loading API keys.");

  // Load keys if they exist
  auto &keychain = iex::singleton::GetInstance<inv::key::Keychain>();
  if (keychain.KeychainValidity().Failure())
  {
    spdlog::error("Keychain error: {}.", keychain.KeychainValidity());
    return EXIT_FAILURE;
  }

  // If keys don't exist, run dialog to get from user
  if (!keychain.Populated())
  {
    spdlog::info("Running KeySelector dialog.");

    int status = application->run(
        inv::widget::GetWidgetDerived<inv::widget::KeySelector>(builder, "key_selector_dialog", keychain));

    spdlog::info("KeySelector dialog exited with status {}.", status);

    if (status)
    {
      return status;
    }

    if (!keychain.Populated())
    {
      spdlog::error("API keys are not populated.");
      return EXIT_FAILURE;
    }
  }

  spdlog::info("API keys are now populated.");

  auto th = inv::TransactionHistory::Factory();

  auto status = application->run(
      inv::widget::GetWidgetDerived<inv::widget::TransactionCreator>(builder, "transaction_creator_dialog", th));
  return status;
}
