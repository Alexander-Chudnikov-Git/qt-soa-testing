#include "main_window.hpp"

#include "settings_manager.hpp"
#include "spdlog_wrapper.hpp"
#include "user_panel_widget.hpp"

#include <QApplication>
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
#include <qlogging.h>

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
	this->m_move_resize_timer->setInterval(100000);
	this->m_move_resize_timer->setSingleShot(false);
	this->m_move_resize_timer->start();
	this->setupUi();
	this->setupConnections();
	this->setupStyle();
	qApp->installEventFilter(this);
}

void MainWindow::setupUi()
{
	setAutoFillBackground(true);
	setAttribute(Qt::WA_StyledBackground);

	auto layout_wrapper = new QWidget();
	this->m_main_layout = new QGridLayout();
	this->m_user_panel	= new UserPanelWidget();
	QPushButton *button = new QPushButton("Close Application");
	button->setEnabled(false);
	connect(button, &QPushButton::clicked, this, [this]() {
		restoreAllGSettingsKeybinds();
		m_unlock_quit = true;
		close();
		qApp->quit();
	});
	connect(this, &MainWindow::keybindsDisabled, this, [this, button]() {
		button->setEnabled(true);
	});
	this->m_main_layout->addWidget(button, 0, 0);
	this->m_main_layout->addWidget(this->m_user_panel, 1, 0);
	layout_wrapper->setLayout(this->m_main_layout);
	this->setCentralWidget(layout_wrapper);
	this->setFocus();
	this->activateWindow();
	this->raise();
}

void MainWindow::setupConnections()
{
	connect(m_move_resize_timer, &QTimer::timeout, this, &MainWindow::onMoveResizeTimerTimeout);
	connect(m_user_panel, &UserPanelWidget::testStarted, this, [this]() {
		disableAllGSettingsKeybinds();
	});
	connect(m_user_panel, &UserPanelWidget::testFinished, this, [this]() {
		restoreAllGSettingsKeybinds();
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
	this->setFocus();
	this->activateWindow();
	this->raise();
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
	process.start("bash", QStringList() << "-c" << command << "> /dev/null 2>&");
	process.waitForFinished();
}

QFuture<void> MainWindow::runShellCommandAsync(const QString &command)
{
	return QtConcurrent::run(executeProcessShellMethod, command);
}

void MainWindow::disableAllGSettingsKeybinds()
{
	QFuture<void> future =
		runShellCommandAsync("gsettings set org.gnome.shell.extensions.dash-to-dock autohide-in-fullscreen true");

	SPD_WARN_CLASS(UTILS::DEFAULTS::d_settings_group_application,
				   "Disabling all GSettings keybinds\nIf application crashed, you can restore them manually by running "
				   "'gsettings list-schemas | xargs -n 1 gsettings reset-recursively'");

	QProcess process;
	process.start("bash",
				  QStringList() << "-c" << "gsettings list-recursively | grep -E \"<[a-zA-Z]*>|(Super|Alt|Control|Meta|Key)\"");
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

			future = runShellCommandAsync(QString("gsettings set %1 %2 %3").arg(schema, key, new_value));
			SPD_WARN_CLASS(UTILS::DEFAULTS::d_settings_group_application,
						   "\tTemporary removing keybind: " + schema + " " + key + " " + value + " " + new_value);
		}
	}

	future.waitForFinished();

	emit keybindsDisabled();
}

void MainWindow::restoreAllGSettingsKeybinds()
{
	SPD_WARN_CLASS(UTILS::DEFAULTS::d_settings_group_application, "Restoring all GSettings keybinds");

	QFuture<void>  future;
	QList<QString> keys_to_remove;

	for (auto it = m_original_keybinds.begin(); it != m_original_keybinds.end(); ++it)
	{
		QString key	   = it.key();
		QString value  = it.value().second;
		QString schema = it.value().first;

		future = runShellCommandAsync(QString("gsettings set %1 %2 %3").arg(schema, key, value));
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
