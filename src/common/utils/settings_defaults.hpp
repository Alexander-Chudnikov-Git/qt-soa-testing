/**
 * @file default.hpp
 * @brief Defines default settings for the application configuration.
 *
 * This file provides a set of default values that are used by the `SettingsManager`
 * to populate the application's configuration file upon its initial startup or when
 * the configuration file is missing.  These defaults act as a fallback and ensure
 * a consistent application behavior even before a user-defined configuration is loaded.
 *
 * The default values are organized within the `UTILS::DEFAULTS` namespace. They cover various
 * aspects of the application, including project information, window dimensions,
 * logger configurations, translation settings, and OpenCV interface options.
 *
 * @note The constants defined here are intended to be overwritten by user-defined settings
 *       stored in the configuration file. They serve as the initial state.
 */
#ifndef DEFAULT_HPP
#define DEFAULT_HPP

#include "panel_type.hpp"

namespace UTILS
{
namespace DEFAULTS
{
	constexpr auto d_project_name		  = PROJECT_NAME;
	constexpr auto d_project_version	  = PROJECT_VERSION;
	constexpr auto d_application_name	  = PROJECT_APPLICATION_NAME;
	constexpr auto d_organization_name	  = PROJECT_ORGANIZATION_NAME;
	constexpr auto d_organization_website = PROJECT_ORGANIZATION_WEBSITE;
	constexpr auto d_developer_name		  = PROJECT_DEVELOPER_NAME;
	constexpr auto d_developer_email	  = PROJECT_DEVELOPER_EMAIL;
	constexpr auto d_compile_time		  = COMPILE_TIME;
	constexpr auto d_compile_id			  = COMPILER_ID;
	constexpr auto d_version			  = COMPILER_VERSION;

	constexpr auto d_project_enum_invalid = 255;

	constexpr auto d_window_width  = 640;
	constexpr auto d_window_height = 480;

	constexpr auto d_settings_group_generic		= "Generic";
	constexpr auto d_settings_group_application = "Application";
	constexpr auto d_settings_group_language	= "Language";

	constexpr auto d_settings_setting_window_rect	   = "window_rect";
	constexpr auto d_settings_setting_translation_lang = "translation_lang";
	constexpr auto d_settings_setting_last_open_panel  = "last_open_panel";

	constexpr auto d_application_default_panel = APP::PanelType::TEST_INTRODUCTION;

	constexpr auto d_logger_name				= "global_logger";
	constexpr auto d_logger_settings_manager	= "settings";
	constexpr auto d_logger_translation_manager = "language";

	constexpr auto d_translator_base_name	= "lang_";
	constexpr auto d_translator_base_dir	= ":/i18n/";
	constexpr auto d_translator_base_locale = "en_GB";
	constexpr auto d_translator_base_format = ".qm";

	constexpr auto d_opencv_interface_default = "";
} // namespace DEFAULTS
} // namespace UTILS
#endif // DEFAULT_HPP
