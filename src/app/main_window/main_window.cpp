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
#include <qlabel.h>
#include <qlogging.h>

namespace APP
{
MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	m_move_resize_timer(new QTimer(this)),
	m_test_timer(new QTimer(this)),
	m_test_timer_updater(new QTimer(this)),
	m_unlock_quit(false)
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

	this->setupUi();
	this->setupConnections();
	this->setupStyle();

	this->m_move_resize_timer->setInterval(500);
	this->m_move_resize_timer->setSingleShot(false);
	this->m_move_resize_timer->start();

	auto test_time = UTILS::SettingsManager::instance()->getValue(UTILS::SettingsManager::Setting::TEST_TIME_LIMIT).toInt();
	this->m_test_timer->setInterval(test_time * 1000 * 60);
	this->m_test_timer->setSingleShot(true);

	this->m_test_timer_updater->setInterval(1000);
	this->m_test_timer_updater->setSingleShot(false);
	this->m_test_timer_updater->start();
	// qApp->installEventFilter(this);
}

void MainWindow::setupUi()
{
	setAutoFillBackground(true);
	setAttribute(Qt::WA_StyledBackground);

	auto layout_wrapper		 = new QWidget();
	this->m_main_layout		 = new QGridLayout();
	this->m_user_panel		 = new UserPanelWidget();
	this->m_close_button	 = new QPushButton("Close Application");
	this->m_test_timer_label = new QLabel("00:00:00");

	this->m_close_button->setEnabled(true);
	this->m_main_layout->addWidget(this->m_close_button, 0, 0);
	this->m_main_layout->addWidget(this->m_test_timer_label, 0, 1);
	this->m_main_layout->addWidget(this->m_user_panel, 1, 0, 1, 2);

	layout_wrapper->setLayout(this->m_main_layout);
	this->setCentralWidget(layout_wrapper);
	// this->setFocus();
	// this->activateWindow();
	// this->raise();
}

void MainWindow::setupConnections()
{
	connect(this->m_close_button, &QPushButton::clicked, this, [this]() {
		this->m_unlock_quit = true;
		close();
		qApp->quit();
	});
	connect(this->m_move_resize_timer, &QTimer::timeout, this, &MainWindow::onMoveResizeTimerTimeout);
	connect(this->m_user_panel, &UserPanelWidget::testStarted, this, [this]() {
		this->m_close_button->setEnabled(false);
		this->m_test_timer->start();
	});
	connect(this->m_test_timer_updater, &QTimer::timeout, this, [this]() {
		int current_time = this->m_test_timer->remainingTime();

		int hours	= current_time / 1000 / 3600;
		int minutes = (current_time / 1000 % 3600) / 60;
		int seconds = current_time / 1000 % 60;

		QString timeString =
			QString("%1:%2:%3").arg(hours, 2, 10, QChar('0')).arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'));

		this->m_test_timer_label->setText(timeString);
	});
	connect(this->m_user_panel, &UserPanelWidget::testFinished, this, [this]() {
		this->m_close_button->setEnabled(true);
		this->m_test_timer->stop();
		auto test_time = UTILS::SettingsManager::instance()->getValue(UTILS::SettingsManager::Setting::TEST_TIME_LIMIT).toInt();
		this->m_test_timer->setInterval(test_time * 1000 * 60);
	});

	connect(this->m_test_timer, &QTimer::timeout, this, [this]() {
		this->m_user_panel->finishPrematurely();
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
} // namespace APP
