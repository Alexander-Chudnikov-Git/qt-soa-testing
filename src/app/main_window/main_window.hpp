#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QEvent>
#include <QMainWindow>
#include <QMoveEvent>
#include <QResizeEvent>

class QGridLayout;
class QSplitter;
class QTimer;

namespace APP
{
class ControlPanelWidget;
class UserPanelWidget;
} // namespace APP

namespace APP
{
class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow();

private:
	void initialize();
	void setupUi();
	void setupStyle();
	void setupConnections();

private slots:
	void onMoveResizeTimerTimeout();

private:
	void moveEvent(QMoveEvent *event) override;
	void resizeEvent(QResizeEvent *event) override;

protected:
	bool event(QEvent *event) override;

private:
	QGridLayout *m_main_layout;

	UserPanelWidget *m_user_panel;

	QTimer *m_move_resize_timer;
};
} // namespace APP
#endif // MAINWINDOW_HPP
