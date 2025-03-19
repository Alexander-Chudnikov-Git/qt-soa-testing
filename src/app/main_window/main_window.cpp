#include "main_window.hpp"

#include "settings_manager.hpp"
#include "spdlog_wrapper.hpp"
#include "user_panel_widget.hpp"

#include <QApplication>
#include <QClipboard>
#include <QGridLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QPixmap>
#include <QProcess>
#include <QPushButton>
#include <QSizePolicy>
#include <QSplashScreen>
#include <QSplitter>
#include <QTimer>
#include <QtConcurrent>
#include <pwd.h>
#include <unistd.h>
#include <utmp.h>

namespace APP
{
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), m_move_resize_timer(new QTimer(this)), m_unlock_quit(false)
{
	this->initialize();
}

MainWindow::~MainWindow()
{}

void MainWindow::initialize()
{
	QRect window_rect = UTILS::SettingsManager::instance()->getValue(UTILS::SettingsManager::Setting::WINDOW_RECT).toRect();
	this->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint | Qt::FramelessWindowHint |
						 Qt::MSWindowsFixedSizeDialogHint | Qt::BypassWindowManagerHint | Qt::MSWindowsOwnDC |
						 Qt::WindowOverridesSystemGestures | Qt::Widget);
	this->move(window_rect.x(), window_rect.y());
	this->setFixedSize(window_rect.width(), window_rect.height());
	this->showFullScreen();
	this->m_move_resize_timer->setInterval(500);
	this->m_move_resize_timer->setSingleShot(false);
	this->m_move_resize_timer->start();
	this->setupUi();
	this->setupConnections();
	this->setupStyle();
	// qApp->installEventFilter(this);
}

void MainWindow::setupUi()
{
	setAutoFillBackground(true);
	setAttribute(Qt::WA_StyledBackground);

	auto layout_wrapper = new QWidget();
	this->m_main_layout = new QGridLayout();
	this->m_user_panel	= new UserPanelWidget();
	m_close_button		= new QPushButton("Close Application");
	m_close_button->setEnabled(false);
	connect(m_close_button, &QPushButton::clicked, this, [this]() {
		restoreAllGSettingsKeybinds();
		m_unlock_quit = true;
		close();
		qApp->quit();
	});
	connect(this, &MainWindow::keybindsDisabled, this, [this]() {

	});
	connect(m_user_panel, &UserPanelWidget::validationFinished, this, [this]() {
		m_close_button->setEnabled(true);
	});
	this->m_main_layout->addWidget(m_close_button, 0, 0);
	this->m_main_layout->addWidget(this->m_user_panel, 1, 0);
	layout_wrapper->setLayout(this->m_main_layout);
	this->setCentralWidget(layout_wrapper);
	// this->setFocus();
	// this->activateWindow();
	// this->raise();
}

void MainWindow::setupConnections()
{
	connect(m_move_resize_timer, &QTimer::timeout, this, &MainWindow::onMoveResizeTimerTimeout);
	connect(m_user_panel, &UserPanelWidget::testStarted, this, [this]() {
		disableAllGSettingsKeybinds();
		m_close_button->setEnabled(false);
	});
	connect(m_user_panel, &UserPanelWidget::testFinished, this, [this]() {
		restoreAllGSettingsKeybinds();
		m_close_button->setEnabled(true);
	});
}

void MainWindow::setupStyle()
{
	setStyleSheet("* { background-color: #181818; color: #ffffff; }");
}

void MainWindow::moveEvent(QMoveEvent *event)
{
	onMoveResizeTimerTimeout();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
	onMoveResizeTimerTimeout();
}

void MainWindow::onMoveResizeTimerTimeout()
{
	QRect window_rect = UTILS::SettingsManager::instance()->getValue(UTILS::SettingsManager::Setting::WINDOW_RECT).toRect();
	this->move(window_rect.x(), window_rect.y());
	this->setFixedSize(window_rect.width(), window_rect.height());
	this->showFullScreen();
	// this->setFocus();
	// this->activateWindow();
	// this->raise();
}

bool MainWindow::event(QEvent *event)
{
	if (event->type() == QEvent::FocusOut)
	{
		onMoveResizeTimerTimeout();
	}
	else if (event->type() == QEvent::FocusIn)
	{
		onMoveResizeTimerTimeout();
	}
	return QMainWindow::event(event);
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
	if (event->type() == QEvent::FocusOut)
	{
		onMoveResizeTimerTimeout();
	}
	if (event->type() == QEvent::FocusIn)
	{
		onMoveResizeTimerTimeout();
	}
	if (event->type() == QEvent::KeyPress)
	{
		QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
		if (keyEvent->modifiers() & Qt::AltModifier)
		{
			return true;
		}
		if (keyEvent->modifiers() & Qt::ControlModifier)
		{
			return true;
		}
		if (keyEvent->modifiers() & Qt::MetaModifier)
		{
			return true;
		}
		if (keyEvent->key() & Qt::Key_Super_L)
		{
			return true;
		}
		if (keyEvent->key() & Qt::Key_Super_R)
		{
			return true;
		}
	}
	return QMainWindow::eventFilter(watched, event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	if (m_unlock_quit)
	{
		event->accept();
		return;
	}
	event->ignore();
}

void MainWindow::executeProcessShellMethod(const QString &command)
{
	QProcess process;
	// process.start("bash", QStringList() << "-c" << command << "> /dev/null 2>&");
	process.start("bash", {"-c", command});
	// process.start(command);
	process.waitForFinished();
}

QFuture<void> MainWindow::runShellCommandAsync(const QString &command)
{
	return QtConcurrent::run(executeProcessShellMethod, command);
}

void MainWindow::disableAllGSettingsKeybinds()
{
	QClipboard *clipboard = QGuiApplication::clipboard();
	clipboard->clear();
	clipboard->setText(":)", QClipboard::Clipboard);

	QStringList	 users;
	struct utmp *ut;

	setutent();
	while ((ut = getutent()) != nullptr)
	{
		if (ut->ut_type == USER_PROCESS)
		{
			if (ut->ut_user[0] != '\0')
			{
				QString username(ut->ut_user);
				if (!users.contains(username))
				{
					users << username;
				}
			}
		}
	}
	endutent();

	struct passwd *pwd = getpwnam(users[0].toUtf8().constData());

	QString user_uid = QString::number(pwd->pw_uid);

	QFuture<void> future =
		runShellCommandAsync(QString("sudo -Hu %1 DISPLAY=:0 DBUS_SESSION_BUS_ADDRESS=unix:path=/run/user/%2/bus gsettings set "
									 "org.gnome.shell.extensions.dash-to-dock autohide-in-fullscreen true")
								 .arg(users[0], user_uid));

	SPD_WARN_CLASS(UTILS::DEFAULTS::d_settings_group_application,
				   "Disabling all GSettings keybinds\nIf application crashed, you can restore them manually by running "
				   "'gsettings list-schemas | xargs -n 1 gsettings reset-recursively'");

	QProcess process;
	process.start("bash", {"-c", QString("sudo -Hu %1 DISPLAY=:0 DBUS_SESSION_BUS_ADDRESS=unix:path=/run/user/%2/bus gsettings "
										 "list-recursively | grep -E \"<[a-zA-Z]*>|(Super|Alt|Control|Meta|Key)\"")
									 .arg(users[0], user_uid)});
	process.waitForFinished();
	QString		output = process.readAllStandardOutput();
	QStringList lines  = output.split("\n", Qt::SkipEmptyParts);

	for (const QString &line : lines)
	{
		QStringList parts = line.split(" ", Qt::SkipEmptyParts);
		if (parts.size() >= 3)
		{
			QString schema = parts[0];
			QString key	   = parts[1];
			QString value  = parts.mid(2).join(" ");
			QString new_value;
			m_original_keybinds[key] = {schema, value};

			if (value.count("["))
			{
				new_value = "['']";
			}
			else
			{
				new_value = "''";
			}

			future = runShellCommandAsync(
				QString("sudo -Hu %1 DISPLAY=:0 DBUS_SESSION_BUS_ADDRESS=unix:path=/run/user/%2/bus gsettings set %3 %4 %5")
					.arg(users[0], user_uid, schema, key, new_value));
			SPD_WARN_CLASS(UTILS::DEFAULTS::d_settings_group_application,
						   "\tTemporary removing keybind: " + schema + " " + key + " " + value + " " + new_value);
		}
	}

	future.waitForFinished();

	emit keybindsDisabled();
}

void MainWindow::restoreAllGSettingsKeybinds()
{
	QClipboard *clipboard = QGuiApplication::clipboard();
	clipboard->clear();

	QStringList	 users;
	struct utmp *ut;

	setutent();
	while ((ut = getutent()) != nullptr)
	{
		if (ut->ut_type == USER_PROCESS)
		{
			if (ut->ut_user[0] != '\0')
			{
				QString username(ut->ut_user);
				if (!users.contains(username))
				{
					users << username;
				}
			}
		}
	}
	endutent();

	struct passwd *pwd = getpwnam(users[0].toUtf8().constData());

	QString user_uid = QString::number(pwd->pw_uid);

	SPD_WARN_CLASS(UTILS::DEFAULTS::d_settings_group_application, "Restoring all GSettings keybinds");

	QFuture<void>  future;
	QList<QString> keys_to_remove;

	for (auto it = m_original_keybinds.begin(); it != m_original_keybinds.end(); ++it)
	{
		QString key	   = it.key();
		QString value  = it.value().second;
		QString schema = it.value().first;

		future = runShellCommandAsync(
			QString("sudo -Hu %1 DISPLAY=:0 DBUS_SESSION_BUS_ADDRESS=unix:path=/run/user/%2/bus gsettings set %3 %4 %5")
				.arg(users[0], user_uid, schema, key, value));
		SPD_WARN_CLASS(UTILS::DEFAULTS::d_settings_group_application,
					   "\tRestoring keybind: " + schema + " " + key + " " + value);

		keys_to_remove.append(key);
	}

	future.waitForFinished();

	for (const auto &key : keys_to_remove)
	{
		m_original_keybinds.remove(key);
	}
}

} // namespace APP
