#include "suricata_validator.hpp"

#include "settings_manager.hpp"
#include "spdlog_wrapper.hpp"

#include <QFutureWatcher>
#include <QLabel>
#include <QNetworkInterface>
#include <QProcess>
#include <QTimer>
#include <QVBoxLayout>
#include <QtConcurrent>

namespace APP
{
SuricataValidatorWidget::SuricataValidatorWidget(QWidget *parent) :
	QWidget(parent),
	m_current_status(ValidationStatus::Checking),
	m_suricata_path("")
{
	initialize();
	startValidation();
}

SuricataValidatorWidget::~SuricataValidatorWidget()
{}

void SuricataValidatorWidget::initialize()
{
	setupUi();
	setupStyle();
	setupConnections();
}

void SuricataValidatorWidget::setupUi()
{
	QFont title_font;
	title_font.setPointSize(18);
	title_font.setBold(true);

	QFont subtitle_font;
	subtitle_font.setPointSize(14);
	subtitle_font.setBold(true);

	m_reason_label = new QLabel("", this);
	m_reason_label->setAlignment(Qt::AlignCenter);
	m_reason_label->setFont(subtitle_font);

	m_status_label = new QLabel("Проверка...", this);
	m_status_label->setAlignment(Qt::AlignCenter);
	m_status_label->setFont(title_font);

	m_main_layout = new QVBoxLayout(this);
	m_main_layout->addWidget(m_status_label);
	m_main_layout->addWidget(m_reason_label);
	setLayout(m_main_layout);

	setMinimumSize(400, 100);
}

void SuricataValidatorWidget::setupStyle()
{
	setAutoFillBackground(true);
	setAttribute(Qt::WA_StyledBackground);

	setStyleSheet("* {"
				  "   background-color: #f0f0f0;"
				  "   border: 1px solid #c0c0c0;"
				  "   border-radius: 8px;"
				  "   padding: 0px;"
				  "}"
				  "QLabel {"
				  "   font-size: 16px;"
				  "   border: none;"
				  "}");
	updateStatusDisplay();
}

void SuricataValidatorWidget::updateStatusDisplay()
{
	switch (m_current_status)
	{
		case ValidationStatus::Checking:
			m_status_label->setText("Проверка...");
			setStyleSheet("* {"
						  "   background-color: #e0e0e0;"
						  "   border: 1px solid #c0c0c0;"
						  "   border-radius: 8px;"
						  "   padding: 0px;"
						  "}"
						  "QLabel {"
						  "   color: black;"
						  "   border: none;"
						  "}");
			break;
		case ValidationStatus::Success:
			m_status_label->setText("Проверка успешна");
			setStyleSheet("* {"
						  "   background-color: #c0f0c0;"
						  "   border: 1px solid #00c000;"
						  "   border-radius: 8px;"
						  "   padding: 0px;"
						  "}"
						  "QLabel {"
						  "   color: darkgreen;"
						  "   border: none;"
						  "}");
			break;
		case ValidationStatus::Failure:
			m_status_label->setText("Работа не прошла проверку");
			setStyleSheet("* {"
						  "   background-color: #f0c0c0;"
						  "   border: 1px solid #c00000;"
						  "   border-radius: 8px;"
						  "   padding: 0px;"
						  "}"
						  "QLabel {"
						  "   color: darkred;"
						  "   border: none;"
						  "}");
			break;
	}
	emit validationStatusChanged(m_current_status);
}

void SuricataValidatorWidget::setupConnections()
{}

void SuricataValidatorWidget::startValidation()
{
	m_current_status = ValidationStatus::Checking;
	updateStatusDisplay();

	QFutureWatcher<bool> *watcher = new QFutureWatcher<bool>(this);
	connect(watcher, &QFutureWatcher<bool>::finished, this, [this, watcher]() {
		bool result = watcher->result();
		if (result)
		{
			m_current_status = ValidationStatus::Success;
		}
		else
		{
			m_current_status = ValidationStatus::Failure;
		}
		updateStatusDisplay();
		watcher->deleteLater();
		emit validationFinished(m_current_status);
	});

	QFuture<bool> future = QtConcurrent::run([this]() {
		return checkSuricata();
	});
	watcher->setFuture(future);
}

bool SuricataValidatorWidget::checkSuricata()
{
	// Check suricata executable
	for (const QString &path : m_suricata_paths)
	{
		if (QFile::exists(path) && QFileInfo(path).isExecutable())
		{
			m_suricata_path = path;
			SPD_INFO_CLASS(UTILS::DEFAULTS::d_settings_group_application, "Found suricata executable: " + path);
			break;
		}
	}

	if (m_suricata_path.isEmpty())
	{
		m_reason_label->setText("Suricata не установлен");
		return false;
	}

	// Validate suricata is disabled
	QProcess suricata_process;
	suricata_process.start("pidof", {"suricata"});
	suricata_process.waitForFinished();

	if (!suricata_process.readAllStandardOutput().trimmed().isEmpty())
	{
		m_reason_label->setText("Suricata уже запущен");
		return false;
	}

	// Check suricata configuration files
	for (const QString &dir_path : m_suricata_conf_dirs)
	{
		QDir dir(dir_path);
		if (dir.exists())
		{
			searchDirectoryRecursive(dir);
		}
	}

	SPD_INFO_CLASS(UTILS::DEFAULTS::d_settings_group_application,
				   "Found: " + QString::number(m_config_file_paths.count()) + " config files");

	if (m_config_file_paths.isEmpty())
	{
		m_reason_label->setText("Файл конфигурации Suricata не найден");
		return false;
	}

	// Validate suricata configuration files
	for (const QString &config_path : m_config_file_paths)
	{
		QFile config_file(config_path);
		if (!config_file.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			SPD_ERROR_CLASS(UTILS::DEFAULTS::d_settings_group_application,
							QString("Unable to open configuration file: %1").arg(config_path));
			m_reason_label->setText(QString("Не удалось прочитать файл конфигурации Suricata: %1").arg(config_path));
			continue;
		}

		QTextStream in(&config_file);
		bool		fast_log_enabled = false;
		bool		in_fast_section	 = false;

		while (!in.atEnd())
		{
			QString line = in.readLine().trimmed();

			if (line.startsWith("#") || line.isEmpty())
			{
				continue;
			}

			if (line.contains("default-log-dir:"))
			{
				m_suricata_log_dir = line.split(":").last().trimmed();
				SPD_INFO_CLASS(UTILS::DEFAULTS::d_settings_group_application,
							   QString("Fast log dir found: %1").arg(m_suricata_log_dir));
			}

			if (line.contains("- fast:"))
			{
				in_fast_section	 = true;
				fast_log_enabled = false;
			}
			else if (in_fast_section)
			{
				if (line.contains("enabled:"))
				{
					QString enabled_value = line.split(":").last().trimmed();
					if (enabled_value == "yes")
					{
						fast_log_enabled = true;
					}
					else if (enabled_value == "no")
					{
						fast_log_enabled = false;
						in_fast_section	 = false;
					}
					else
					{
						fast_log_enabled = false;
						in_fast_section	 = false;
					}
				}
				else if (fast_log_enabled && line.contains("filename:"))
				{
					m_suricata_log_path = line.split(":").last().trimmed();
					in_fast_section		= false;
					break;
				}
				else if (line.startsWith("- "))
				{
					in_fast_section	 = false;
					fast_log_enabled = false;
				}
			}
		}

		config_file.close();

		if (!fast_log_enabled || m_suricata_log_path.isEmpty())
		{
			SPD_ERROR_CLASS(UTILS::DEFAULTS::d_settings_group_application, "Fast log not enabled or log path is missing");
			m_reason_label->setText(QString("В файле конфигурации Suricata: %1 выключен fast лог").arg(config_path));
			continue;
		}

		SPD_INFO_CLASS(UTILS::DEFAULTS::d_settings_group_application,
					   QString("Fast log enabled for: %1").arg(m_suricata_log_path));

		QProcess	process;
		QString		program = m_suricata_path;
		QStringList arguments;
		arguments << "-T" << "-c" << config_path;
		process.start(program, arguments);
		process.waitForFinished();

		QString		output		= process.readAllStandardError();
		int			error_count = 0;
		QStringList lines		= output.split('\n');

		for (const QString &line : lines)
		{
			if (line.startsWith("E:") || line.startsWith("Error:"))
			{
				error_count++;
			}
		}

		if (error_count > 0)
		{
			SPD_WARN_CLASS(UTILS::DEFAULTS::d_settings_group_application,
						   QString("Suricata config error count: %1 in file %2").arg(error_count).arg(config_path));
			m_reason_label->setText(
				QString("Обнаружено %1 ошибок в файле конфигурации Suricata: %2").arg(error_count).arg(config_path));
			return false;
		}
		else
		{
			SPD_INFO_CLASS(UTILS::DEFAULTS::d_settings_group_application,
						   QString("Suricata config test passed for file: %1").arg(config_path));
			m_reason_label->clear();
			m_suricata_config_path = config_path;
		}
	}

	if (m_suricata_config_path.isEmpty() || m_suricata_log_path.isEmpty())
	{
		SPD_WARN_CLASS(UTILS::DEFAULTS::d_settings_group_application, QString("Suricata config not found"));
		return false;
	}

	m_active_interfaces.clear();
	const auto interfaces = QNetworkInterface::allInterfaces();

	for (const QNetworkInterface &iface : interfaces)
	{
		if (iface.flags().testFlag(QNetworkInterface::IsUp) && iface.flags().testFlag(QNetworkInterface::IsRunning) &&
			!iface.flags().testFlag(QNetworkInterface::IsLoopBack))
		{
			m_active_interfaces.append(iface.humanReadableName());
			SPD_INFO_CLASS(UTILS::DEFAULTS::d_settings_group_application,
						   QString("Found interface: %1").arg(iface.humanReadableName()));
		}
	}

	if (m_active_interfaces.isEmpty())
	{
		SPD_ERROR_CLASS(UTILS::DEFAULTS::d_settings_group_application, QString("Unable to find any active interfaces"));
		m_reason_label->setText(QString("Нет подключенных интерфейсов"));
		return false;
	}

	QStringList arguments;
	arguments << "-c" << m_suricata_config_path << "-i" << m_active_interfaces[0];

	QProcess final_suricata_process;
	final_suricata_process.start(m_suricata_path, arguments);

	if (!final_suricata_process.waitForStarted())
	{
		SPD_ERROR_CLASS(UTILS::DEFAULTS::d_settings_group_application, QString("Unable to start Suricata"));
		m_reason_label->setText(QString("Невозможно запустить Suricata"));
		final_suricata_process.kill();
		return false;
	}

	QThread::msleep(2500);

	QStringList possible_log_paths = {m_suricata_log_path, m_suricata_log_dir + m_suricata_log_path,
									  m_suricata_log_dir + "/" + m_suricata_log_path};

	for (const QString &path : possible_log_paths)
	{
		QFile file(path);
		if (file.exists())
		{
			m_suricata_log_path = path;

			if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
			{
				file.close();
				SPD_INFO_CLASS(UTILS::DEFAULTS::d_settings_group_application,
							   QString("Cleared log file: %1").arg(m_suricata_log_path));
			}
			else
			{
				SPD_ERROR_CLASS(UTILS::DEFAULTS::d_settings_group_application,
								QString("Failed to clear log file: %1").arg(m_suricata_log_path));
			}
			break;
		}
	}

	if (m_suricata_log_path.isEmpty())
	{
		SPD_ERROR_CLASS(UTILS::DEFAULTS::d_settings_group_application, QString("Unable to find Suricata log file"));
		m_reason_label->setText(QString("Невозможно найти файл журнала Suricata"));
		final_suricata_process.terminate();
		return false;
	}

	QProcess ping_process;
	ping_process.start("ping", QStringList() << "-c" << "1" << "1.1.1.1");
	ping_process.waitForFinished();

	QThread::msleep(500);

	QFile file(m_suricata_log_path);
	if (file.exists() && file.size() > 0)
	{
		SPD_INFO_CLASS(UTILS::DEFAULTS::d_settings_group_application, QString("File has content: %1").arg(m_suricata_log_path));
	}
	else
	{
		SPD_ERROR_CLASS(UTILS::DEFAULTS::d_settings_group_application,
						QString("File is empty or does not exist: %1").arg(m_suricata_log_path));
		m_reason_label->setText(QString("ICMP запрос не перехвачен Suricata"));
		final_suricata_process.terminate();
		return false;
	}

	final_suricata_process.terminate();

	return true;
}

void SuricataValidatorWidget::searchDirectoryRecursive(const QDir &dir)
{
	QStringList entries = dir.entryList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot | QDir::Readable | QDir::NoSymLinks);

	for (const QString &entry : entries)
	{
		QFileInfo fileInfo(dir.filePath(entry));

		if (fileInfo.isFile())
		{
			for (const QString &filter : m_suricata_conf_files)
			{
				if (QDir::match(filter, entry))
				{
					SPD_INFO_CLASS(UTILS::DEFAULTS::d_settings_group_application,
								   "Found suricata configuration file: " + fileInfo.absoluteFilePath());
					m_config_file_paths.append(fileInfo.absoluteFilePath());
					break;
				}
			}
		}
		else if (fileInfo.isDir())
		{
			searchDirectoryRecursive(QDir(fileInfo.absoluteFilePath()));
		}
	}
}

void SuricataValidatorWidget::executeProcessShellMethod(const QString &command)
{
	QProcess process;
	process.start("bash", QStringList() << "-c" << command << "> /dev/null 2>&");
	process.waitForFinished();
}

QFuture<void> SuricataValidatorWidget::runShellCommandAsync(const QString &command)
{
	return QtConcurrent::run(executeProcessShellMethod, command);
}

void SuricataValidatorWidget::setSuricataPaths(const QStringList &paths)
{
	m_suricata_paths = paths;
}

void SuricataValidatorWidget::setSuricataConfDirs(const QStringList &dirs)
{
	m_suricata_conf_dirs = dirs;
}

void SuricataValidatorWidget::setSuricataConfFiles(const QStringList &files)
{
	m_suricata_conf_files = files;
}

void SuricataValidatorWidget::setReasonLabelText(const QString &text)
{
	m_reason_label->setText(text);
}

ValidationStatus SuricataValidatorWidget::getCurrentStatus() const
{
	return m_current_status;
}

QString SuricataValidatorWidget::getSuricataPath() const
{
	return m_suricata_path;
}

QStringList SuricataValidatorWidget::getConfigFilePaths() const
{
	return m_config_file_paths;
}

} // namespace APP
