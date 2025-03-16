#include "main_window.hpp"

#include "settings_manager.hpp"
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

	this->setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint | Qt::FramelessWindowHint |
						 Qt::SplashScreen | Qt::MSWindowsFixedSizeDialogHint | Qt::BypassWindowManagerHint |
						 Qt::MSWindowsOwnDC | Qt::WindowOverridesSystemGestures);

	this->move(window_rect.x(), window_rect.y());
	this->setFixedSize(window_rect.width(), window_rect.height());
	this->showFullScreen();

	this->m_move_resize_timer->setInterval(200);
	this->m_move_resize_timer->setSingleShot(false);
	this->m_move_resize_timer->start();

	this->setupUi();
	this->setupConnections();
	this->setupStyle();

	qApp->installEventFilter(this);

	disableAllGSettingsKeybinds();
}

void MainWindow::setupUi()
{
	auto layout_wrapper = new QWidget();
	this->m_main_layout = new QGridLayout();
	this->m_user_panel	= new UserPanelWidget();

	// Debug close button
	QPushButton *button = new QPushButton("Close Application");
	connect(button, &QPushButton::clicked, this, [this]() {
		restoreAllGSettingsKeybinds();
		m_unlock_quit = true;
		close();
		qApp->quit();
	});
	this->m_main_layout->addWidget(button, 0, 0);

	this->m_main_layout->addWidget(this->m_user_panel, 0, 1);

	layout_wrapper->setLayout(this->m_main_layout);
	this->setCentralWidget(layout_wrapper);

	this->setFocus();
	this->activateWindow();
	this->raise();
}

void MainWindow::setupConnections()
{
	connect(m_move_resize_timer, &QTimer::timeout, this, &MainWindow::onMoveResizeTimerTimeout);
}

void MainWindow::setupStyle()
{
	setStyleSheet("MainWindow { background: #181818; }");
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
	if (QMainWindow::event(event))
		return true;

	return false;
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

		return false;
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

void MainWindow::disableAllGSettingsKeybinds()
{
	QProcess process;
	process.start("bash",
				  QStringList() << "-c" << "gsettings list-recursively | grep -E \"<[a-zA-Z]*>|(Super|Alt|Control|Meta|Key)\"");
	process.waitForFinished();
	QString output = process.readAllStandardOutput();

	QStringList lines = output.split("\n", Qt::SkipEmptyParts);
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

			QProcess::execute("gsettings", {"set", schema, key, new_value});
			qDebug() << "gsettings" << "set" << schema << key << new_value;
		}
	}
}

void MainWindow::restoreAllGSettingsKeybinds()
{
	for (auto it = m_original_keybinds.begin(); it != m_original_keybinds.end(); ++it)
	{
		QString key	   = it.key();
		QString value  = it.value().second;
		QString schema = it.value().first;
		QProcess::execute("gsettings", {"set", schema, key, value});
	}
	m_original_keybinds.clear();
}

} // namespace APP
