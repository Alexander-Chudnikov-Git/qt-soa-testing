#ifndef SURICATA_VALIDATOR_WIDGET_HPP
#define SURICATA_VALIDATOR_WIDGET_HPP

#include <QDir>
#include <QFuture>
#include <QString>
#include <QStringList>
#include <QWidget>

class QLabel;
class QVBoxLayout;

namespace APP
{
enum class ValidationStatus
{
	Checking,
	Success,
	Failure
};

class SuricataValidatorWidget : public QWidget
{
	Q_OBJECT

signals:
	void validationFinished(ValidationStatus status);
	void validationStatusChanged(ValidationStatus status);

public:
	explicit SuricataValidatorWidget(QWidget* parent = nullptr);
	~SuricataValidatorWidget() override;

	void setSuricataPaths(const QStringList& paths);
	void setSuricataConfDirs(const QStringList& dirs);
	void setSuricataConfFiles(const QStringList& files);
	void setReasonLabelText(const QString& text);

	ValidationStatus getCurrentStatus() const;
	QString			 getSuricataPath() const;
	QStringList		 getConfigFilePaths() const;

private:
	void initialize();
	void setupUi();
	void setupStyle();
	void setupConnections();
	void startValidation();
	void updateStatusDisplay();

	bool		  checkSuricata();
	void		  searchDirectoryRecursive(const QDir& dir);
	static void	  executeProcessShellMethod(const QString& command);
	QFuture<void> runShellCommandAsync(const QString& command);

private:
	QLabel*			 m_status_label;
	QLabel*			 m_reason_label;
	ValidationStatus m_current_status;
	QVBoxLayout*	 m_main_layout;

	QStringList m_config_file_paths;
	QStringList m_active_interfaces;
	QString		m_suricata_path;
	QString		m_suricata_config_path;
	QString		m_suricata_log_path;
	QString		m_suricata_log_dir;

private:
	QStringList m_suricata_paths	  = {"/usr/bin/suricata", "/usr/local/bin/suricata", "/sbin/suricata", "/usr/sbin/suricata",
										 "/opt/suricata/bin/suricata"};
	QStringList m_suricata_conf_dirs  = {"/etc/suricata", "/home", "/usr/local/etc/suricata", "/opt/suricata/etc"};
	QStringList m_suricata_conf_files = {"suricata.yaml", "suricata.conf"};
};
} // namespace APP

#endif // SURICATA_VALIDATOR_WIDGET_HPP
