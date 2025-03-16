#include "main_window.hpp"

#include "settings_manager.hpp"
#include "user_panel_widget.hpp"

#include <QGridLayout>
#include <QLabel>
#include <QPixmap>
#include <QSizePolicy>
#include <QSplashScreen>
#include <QSplitter>
#include <QTimer>

namespace APP
{
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), m_move_resize_timer(new QTimer(this))
{
	this->initialize();
}

MainWindow::~MainWindow()
{}

void MainWindow::initialize()
{
	QRect window_rect = UTILS::SettingsManager::instance()->getValue(UTILS::SettingsManager::Setting::WINDOW_RECT).toRect();

	setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint | Qt::FramelessWindowHint |
				   Qt::SplashScreen | Qt::MSWindowsFixedSizeDialogHint | Qt::BypassWindowManagerHint | Qt::MSWindowsOwnDC |
				   Qt::WindowOverridesSystemGestures);

	move(window_rect.x(), window_rect.y());
	setFixedSize(window_rect.width(), window_rect.height());
	showMaximized();
	showFullScreen();

	this->m_move_resize_timer->setInterval(100);
	this->m_move_resize_timer->setSingleShot(false);
	this->m_move_resize_timer->start();

	this->setupUi();
	this->setupConnections();
	this->setupStyle();
}

void MainWindow::setupUi()
{
	auto layout_wrapper = new QWidget();
	this->m_main_layout = new QGridLayout();
	this->m_user_panel	= new UserPanelWidget();

	this->m_main_layout->addWidget(this->m_user_panel);

	layout_wrapper->setLayout(this->m_main_layout);
	this->setCentralWidget(layout_wrapper);
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
	showFullScreen();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
	showFullScreen();
}

void MainWindow::onMoveResizeTimerTimeout()
{
	showFullScreen();
	raise();
	activateWindow();
	setFocus();
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

} // namespace APP
