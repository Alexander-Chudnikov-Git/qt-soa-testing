#include "settings_manager.hpp"

#include "settings_defaults.hpp"
#include "spdlog_wrapper.hpp"

#include <QApplication>
#include <qfontdatabase.h>
#include <qvariant.h>

namespace UTILS
{
SettingsManager *SettingsManager::m_instance = nullptr;
QMutex			 SettingsManager::m_mutex;

SettingsManager::SettingsManager(QObject *parent) : QObject(parent)
{
	QMutexLocker locker(&this->m_mutex);

	populateSettings();

	QCoreApplication::setOrganizationName(DEFAULTS::d_organization_name);
	QCoreApplication::setOrganizationDomain(DEFAULTS::d_organization_website);
	QCoreApplication::setApplicationName(DEFAULTS::d_project_name);

	this->m_settings = new QSettings(this);
	this->m_settings->setDefaultFormat(QSettings::NativeFormat);

	this->loadSettings();

	connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, this, &SettingsManager::onApplicationAboutToQuit);
}

SettingsManager::~SettingsManager()
{
	this->saveChanges();
}

void SettingsManager::populateSettings()
{
	populateGroup(Group::GENERIC, DEFAULTS::d_settings_group_generic);
	populateGroup(Group::APPLICATION, DEFAULTS::d_settings_group_application);
	populateGroup(Group::LANGUAGE, DEFAULTS::d_settings_group_language);

	// [Application defaults]
	populateSetting(Setting::WINDOW_RECT, DEFAULTS::d_settings_setting_window_rect,
					QRect(-16, -16, QApplication::primaryScreen()->geometry().width() + 32,
						  QApplication::primaryScreen()->geometry().height() + 32),
					Group::APPLICATION);
	populateSetting(Setting::LAST_OPEN_PANEL, DEFAULTS::d_settings_setting_last_open_panel,
					QVariant::fromValue(DEFAULTS::d_application_default_panel), Group::APPLICATION);

	// [Language defaults]
	populateSetting(Setting::TRANSLATION_LANG, DEFAULTS::d_settings_setting_translation_lang,
					DEFAULTS::d_translator_base_locale, Group::LANGUAGE);
}

void SettingsManager::populateGroup(SettingsManager::Group group, QString group_string)
{
	if (static_cast<int>(group) == DEFAULTS::d_project_enum_invalid)
	{
		SPD_WARN_CLASS(DEFAULTS::d_logger_settings_manager, "Populating group with invalid enum");
		return;
	}

	if (group_string.isEmpty())
	{
		SPD_WARN_CLASS(DEFAULTS::d_logger_settings_manager, "Populating group with invalid string");
		return;
	}

	this->m_group_lookup.insert(group, group_string);
}

void SettingsManager::populateSetting(Setting				 setting,
									  QString				 setting_string,
									  QVariant				 default_value,
									  SettingsManager::Group group)
{
	if (static_cast<int>(setting) == DEFAULTS::d_project_enum_invalid)
	{
		SPD_WARN_CLASS(DEFAULTS::d_logger_settings_manager, "Populating setting with invalid enum");
		return;
	}

	if (setting_string.isEmpty())
	{
		SPD_WARN_CLASS(DEFAULTS::d_logger_settings_manager, "Populating setting with invalid string");
		return;
	}

	if (static_cast<int>(group) == DEFAULTS::d_project_enum_invalid)
	{
		SPD_WARN_CLASS(DEFAULTS::d_logger_settings_manager, "Populating setting with invalid group");
		return;
	}

	this->m_setting_lookup.insert(setting, setting_string);
	this->m_setting_defaults.insert(setting, default_value);

	if (!m_setting_group.contains(group))
	{
		this->m_setting_group.insert(group, QVector<Setting>());
	}

	m_setting_group[group].append(setting);
}

SettingsManager *SettingsManager::instance()
{
	if (m_instance == nullptr)
	{
		m_instance = new SettingsManager();
		SPD_INFO_CLASS(DEFAULTS::d_logger_settings_manager, tr("generic_initialize_message"));
	}
	return m_instance;
}

void SettingsManager::loadSettings()
{
	SPD_INFO_CLASS(DEFAULTS::d_logger_settings_manager, "Loading settings");

	QString	 setting_name;
	QString	 group_name;
	QVariant setting_default_value;

	this->m_settings->beginGroup(DEFAULTS::d_settings_group_generic);

	for (auto it = m_setting_group.constBegin(); it != m_setting_group.constEnd(); ++it)
	{
		const Group			   &group	 = it.key();
		const QVector<Setting> &settings = it.value();
		group_name						 = this->m_group_lookup.value(group);

		if (this->m_settings->group() != group_name)
		{
			this->m_settings->endGroup();
			this->m_settings->beginGroup(group_name);
		}

		for (const Setting &setting : settings)
		{
			setting_name		  = this->m_setting_lookup.value(setting);
			setting_default_value = this->m_setting_defaults.value(setting);

			if (setting_name.isEmpty())
			{
				continue;
			}

			SPD_INFO_CLASS(DEFAULTS::d_logger_settings_manager, QString("| Loading setting: %1").arg(setting_name));

			if (!this->m_settings->contains(setting_name))
			{
				if (setting_default_value.isValid())
				{
					this->m_settings->setValue(setting_name, setting_default_value);

					SPD_INFO_CLASS(
						DEFAULTS::d_logger_settings_manager,
						QString("| Setting default value for %1: %2").arg(setting_name).arg(setting_default_value.toString()));
				}
				else
				{
					SPD_WARN_CLASS(DEFAULTS::d_logger_settings_manager,
								   QString("| No default value defined for setting: %1").arg(setting_name));
				}
			}
			else
			{
				SPD_INFO_CLASS(
					DEFAULTS::d_logger_settings_manager,
					QString("| Setting loaded %1: %2").arg(setting_name).arg(this->m_settings->value(setting_name).toString()));
			}
		}
	}

	this->m_settings->endGroup();
	this->m_changed_settings.clear();

	SPD_INFO_CLASS(DEFAULTS::d_logger_settings_manager, "Settings loaded");

	emit settingsLoaded();
}

void SettingsManager::saveSettings()
{
	QMutexLocker locker(&this->m_mutex);

	this->m_settings->sync();

	SPD_INFO_CLASS(DEFAULTS::d_logger_settings_manager, "Settings saved");

	emit settingsSaved();
}

QVariant SettingsManager::getValue(Setting setting, const QVariant &default_value) const
{
	QMutexLocker locker(&m_mutex);

	if (m_changed_settings.contains(setting))
	{
		return m_changed_settings.value(setting);
	}

	QString setting_name = m_setting_lookup.value(setting);
	if (setting_name.isEmpty())
	{
		SPD_WARN_CLASS(DEFAULTS::d_logger_settings_manager,
					   QString("Unable to retrieve setting, setting name is not defined: %1").arg(static_cast<int>(setting)));
		return default_value;
	}

	Group group = Group::COUNT;
	for (auto it = m_setting_group.constBegin(); it != m_setting_group.constEnd(); ++it)
	{
		if (it.value().contains(setting))
		{
			group = it.key();
			break;
		}
	}

	if (group == Group::COUNT)
	{
		SPD_WARN_CLASS(DEFAULTS::d_logger_settings_manager,
					   QString("Unable to retrieve setting, setting group is not defined: %1").arg(static_cast<int>(setting)));
		return default_value;
	}

	QString group_name = m_group_lookup.value(group);

	m_settings->beginGroup(group_name);
	QVariant extracted_value = m_settings->value(setting_name, default_value);
	m_settings->endGroup();

	return extracted_value;
}

void SettingsManager::setValue(Setting setting, const QVariant &value)
{
	QMutexLocker locker(&this->m_mutex);

	QString setting_name = this->m_setting_lookup.value(setting);

	if (setting_name.isEmpty())
	{
		SPD_WARN_CLASS(DEFAULTS::d_logger_settings_manager,
					   QString("Unable to set setting, setting name is not defined: %1").arg(static_cast<int>(setting)));
		return;
	}

	QVariant old_value = this->m_settings->value(setting_name);

	if (old_value != value)
	{
		this->m_changed_settings[setting] = value;
		emit settingChanged(setting, value);
		emit settingsChanged();
	}
}

void SettingsManager::setDirectValue(Setting setting, const QVariant &value)
{
	QMutexLocker locker(&m_mutex);

	QString setting_name = m_setting_lookup.value(setting);
	if (setting_name.isEmpty())
	{
		SPD_WARN_CLASS(DEFAULTS::d_logger_settings_manager,
					   QString("Unable to set setting, setting name is not defined: %1").arg(static_cast<int>(setting)));
		return;
	}

	QString group_name = findGroupName(setting);
	if (group_name.isEmpty())
	{
		SPD_WARN_CLASS(DEFAULTS::d_logger_settings_manager,
					   QString("Unable to set setting, setting group is not defined: %1").arg(static_cast<int>(setting)));
		return;
	}

	m_settings->beginGroup(group_name);
	QVariant old_value = m_settings->value(setting_name);

	if (old_value != value)
	{
		SPD_INFO_CLASS(DEFAULTS::d_logger_settings_manager,
					   QString("Saving setting: %1, With value: %2").arg(setting_name).arg(value.toString()));

		m_settings->setValue(setting_name, value);
		m_settings->sync();

		emit settingChanged(setting, value);
		emit settingsChanged();
	}

	m_settings->endGroup();
}

bool SettingsManager::isChanged(Setting setting) const
{
	QMutexLocker locker(&this->m_mutex);

	return this->m_changed_settings.contains(setting);
}

void SettingsManager::clearChanges()
{
	QMutexLocker locker(&this->m_mutex);

	this->m_changed_settings.clear();
}
void SettingsManager::saveChanges()
{
	QMutexLocker locker(&m_mutex);

	for (auto it = m_changed_settings.constBegin(); it != m_changed_settings.constEnd(); ++it)
	{
		Setting	 setting = it.key();
		QVariant value	 = it.value();

		QString setting_name = m_setting_lookup.value(setting);
		if (setting_name.isEmpty())
		{
			SPD_WARN_CLASS(DEFAULTS::d_logger_settings_manager,
						   QString("Unable to save setting, setting name is not defined: %1").arg(static_cast<int>(setting)));
			continue;
		}

		QString group_name = findGroupName(setting);
		if (group_name.isEmpty())
		{
			SPD_WARN_CLASS(DEFAULTS::d_logger_settings_manager,
						   QString("Unable to save setting, group name is not defined: %1").arg(setting_name));
			continue;
		}

		m_settings->beginGroup(group_name);
		m_settings->setValue(setting_name, value);
		m_settings->endGroup();
	}

	m_settings->sync();

	m_changed_settings.clear();
	SPD_INFO_CLASS(DEFAULTS::d_logger_settings_manager, "Settings Saved");
}

void SettingsManager::discardChanges()
{
	QMutexLocker locker(&this->m_mutex);

	this->m_changed_settings.clear();
	this->loadSettings();
}

void SettingsManager::onApplicationAboutToQuit()
{
	this->saveChanges();
}

QString SettingsManager::findGroupName(Setting setting) const
{
	for (auto it = m_setting_group.constBegin(); it != m_setting_group.constEnd(); ++it)
	{
		if (it.value().contains(setting))
		{
			return m_group_lookup.value(it.key());
		}
	}
	return QString();
}
} // namespace UTILS
