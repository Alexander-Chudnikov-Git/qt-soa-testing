#ifndef SETTINGS_MANAGER_HPP
#define SETTINGS_MANAGER_HPP

#include "settings_defaults.hpp"

#include <QCoreApplication>
#include <QMap>
#include <QMutex>
#include <QObject>
#include <QPair>
#include <QSettings>
#include <QVariant>

namespace UTILS
{
class SettingsManager : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY_MOVE(SettingsManager)
public:
	enum class Setting
	{
		WINDOW_RECT,
		LAST_OPEN_PANEL,

		TRANSLATION_LANG,

		TEST_TIME_LIMIT,
		TEST_QUESTIONS,

		EASTER_EGG,

		COUNT = DEFAULTS::d_project_enum_invalid
	};

	enum class Group
	{
		GENERIC,
		APPLICATION,
		LANGUAGE,
		TEST,

		EASTER_EGG,

		DEFAULT = GENERIC,

		COUNT = DEFAULTS::d_project_enum_invalid
	};

signals:
	void settingChanged(Setting setting, const QVariant &value);
	void settingsChanged();
	void settingsLoaded();
	void settingsSaved();

public:
	static SettingsManager *instance();

	QVariant getValue(Setting setting, const QVariant &defaultValue = QVariant()) const;
	void	 setValue(Setting setting, const QVariant &value);
	void	 setDirectValue(Setting setting, const QVariant &value);

	bool isChanged(Setting setting) const;

	void clearChanges();
	void saveChanges();
	void discardChanges();

public slots:
	void onApplicationAboutToQuit();

private:
	explicit SettingsManager(QObject *parent = nullptr);
	~SettingsManager() override;

	void populateSettings();

	void populateGroup(Group group, QString group_string);
	void populateSetting(Setting setting, QString setting_string, QVariant default_value, Group group = Group::DEFAULT);

	void loadSettings();
	void saveSettings();

	QString findGroupName(Setting setting) const;

private:
	static SettingsManager *m_instance;
	static QMutex			m_mutex;

	/**
	 *  Yeah, go fuck yourself with this one
	 *
	 *  So, m_group_lookup contains translation of Group enum to it's string
	 *  representation.
	 *
	 *  Then we have m_setting_lookup, it contains translation of Settings enum
	 *  to it's string representation.
	 *
	 *  Also there is m_setting_group, it contains information about to which
	 *  group does setting belong.
	 *
	 *  m_setting_defaults on the other hand stores default values for specific
	 *  setting, from here QSettings will be populated if option is not
	 *  populated.
	 *
	 *  m_changed_settings contains changed settings, they will not be saved
	 *  until saveChanges is called or discarded or discardChanges.
	 **/

	QMap<Group, QString>   m_group_lookup;
	QMap<Setting, QString> m_setting_lookup;

	QMap<Group, QVector<Setting>> m_setting_group;

	QMap<Setting, QVariant> m_setting_defaults;
	QMap<Setting, QVariant> m_changed_settings;

	QSettings *m_settings;
};
} // namespace UTILS

#endif // SETTINGS_MANAGER_HPP
