#include "translation_manager.hpp"

#include "settings_defaults.hpp"
#include "settings_manager.hpp"
#include "spdlog_wrapper.hpp"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QLocale>
#include <QMutex>
#include <QMutexLocker>
#include <QStringList>
#include <QTranslator>

namespace UTILS
{
TranslationManager* TranslationManager::m_instance = nullptr;
QMutex				TranslationManager::m_mutex;

TranslationManager::TranslationManager() :
	m_base_name(DEFAULTS::d_translator_base_name),
	m_translations_dir(DEFAULTS::d_translator_base_dir),
	m_default_locale(DEFAULTS::d_translator_base_locale),
	m_default_format(DEFAULTS::d_translator_base_format),
	m_current_translator(nullptr),
	m_current_locale("")
{
	auto current_lang = SettingsManager::instance()->getValue(SettingsManager::Setting::TRANSLATION_LANG).toString();
	if (current_lang.isEmpty() || current_lang.isNull())
	{
		this->loadSystemTranslation();
	}
	else
	{
		this->loadTranslation(current_lang);
	}
}

TranslationManager::~TranslationManager()
{
	QMutexLocker locker(&this->m_mutex);
	if (this->m_current_translator)
	{
		QCoreApplication::instance()->removeTranslator(this->m_current_translator);
		delete this->m_current_translator;
		this->m_current_translator = nullptr;
	}
}

TranslationManager* TranslationManager::instance()
{
	if (m_instance == nullptr)
	{
		m_instance = new TranslationManager();
		SPD_INFO_CLASS(DEFAULTS::d_logger_translation_manager, tr("generic_initialize_message"));
	}
	return m_instance;
}

bool TranslationManager::loadTranslation(const QString& locale)
{
	QMutexLocker locker(&this->m_mutex);

	QTranslator* translator = new QTranslator();
	QString		 file_path	= this->m_translations_dir + this->m_base_name + locale + this->m_default_format;

	if (QFile::exists(file_path) && translator->load(file_path, this->m_translations_dir))
	{
		SPD_INFO_CLASS(DEFAULTS::d_logger_translation_manager,
					   fmt::format("Loaded translation for: {}", file_path.toStdString()));
	}
	else
	{
		QString app_dir_file_path = QDir::currentPath() + "/" + this->m_base_name + locale + this->m_default_format;

		if (QFile::exists(app_dir_file_path) && translator->load(app_dir_file_path))
		{
			SPD_INFO_CLASS(DEFAULTS::d_logger_translation_manager,
						   fmt::format("Loaded translation from app dir for: {}", app_dir_file_path.toStdString()));
		}
		else
		{
			SPD_ERROR_CLASS(DEFAULTS::d_logger_translation_manager,
							fmt::format("Failed to load translation for: {}", app_dir_file_path.toStdString()));
			delete translator;
			return false;
		}
	}

	if (this->m_current_translator)
	{
		QCoreApplication::instance()->removeTranslator(this->m_current_translator);
		delete this->m_current_translator;
	}

	this->m_current_translator = translator;
	QCoreApplication::instance()->installTranslator(this->m_current_translator);

	this->m_current_locale = locale;
	SettingsManager::instance()->setValue(SettingsManager::Setting::TRANSLATION_LANG,
										  QVariant::fromValue(this->m_current_locale));
	return true;
}

void TranslationManager::loadSystemTranslation()
{
	bool translation_loaded = false;

	for (const QString& locale : QLocale::system().uiLanguages())
	{
		SPD_INFO_CLASS(DEFAULTS::d_logger_translation_manager,
					   fmt::format("Trying to load system translation: {}", locale.toStdString()));
		if (this->loadTranslation(QLocale(locale).name()))
		{
			translation_loaded = true;
			break;
		}
	}

	if (!translation_loaded)
	{
		loadDefaultTranslation();
	}
}
void TranslationManager::loadDefaultTranslation()
{
	if (this->loadTranslation(this->m_default_locale))
	{
		SPD_INFO_CLASS(DEFAULTS::d_logger_translation_manager,
					   fmt::format("Loaded default translation: {}", this->m_default_locale.toStdString()));
	}
	else
	{
		spdlog::warn("");
		SPD_INFO_CLASS(DEFAULTS::d_logger_translation_manager, "Failed to load default translation");
	}
}

QStringList TranslationManager::getAvailableTranslations() const
{
	QMutexLocker locker(&this->m_mutex);

	QStringList available_translations;
	QDir		translations_directory(this->m_translations_dir);
	QStringList name_filters;
	name_filters << this->m_base_name + "*" + this->m_default_format;
	QStringList file_names = translations_directory.entryList(name_filters, QDir::Files);

	for (const QString& file_name : file_names)
	{
		QString locale = file_name.mid(m_base_name.length());
		locale.chop(3);
		available_translations.append(locale);
	}

	QDir		app_translations_directory(QDir::currentPath());
	QStringList app_file_names = app_translations_directory.entryList(name_filters, QDir::Files);

	for (const QString& file_name : app_file_names)
	{
		QString locale = file_name.mid(m_base_name.length());
		locale.chop(3);

		if (!available_translations.contains(locale))
		{
			available_translations.append(locale);
		}
	}

	return available_translations;
}

QString TranslationManager::getCurrentLocale() const
{
	QMutexLocker locker(&this->m_mutex);
	return this->m_current_locale;
}

} // namespace UTILS
