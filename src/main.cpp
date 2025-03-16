#include "main_window.hpp"
#include "settings_manager.hpp"
#include "spdlog_wrapper.hpp"
#include "translation_manager.hpp"

#include <QApplication>
#include <QDirIterator>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	UTILS::SettingsManager::instance();
	UTILS::TranslationManager::instance();

	spdlog::info(QObject::tr("application_init_message"));

	app.setQuitOnLastWindowClosed(true);

	auto window = std::make_unique<APP::MainWindow>(nullptr);
	window->show();

	return app.exec();
}
