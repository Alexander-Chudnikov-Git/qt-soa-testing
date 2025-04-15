#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <QCloseEvent>
#include <QEvent>
#include <QFuture>
#include <QGridLayout>
#include <QMainWindow>
#include <QMoveEvent>
#include <QObject>
#include <QPushButton>
#include <QResizeEvent>
#include <QTimer>
#include <qlabel.h>

namespace APP
{
class UserPanelWidget;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow() override;

protected:
	void moveEvent(QMoveEvent *event) override;
	void resizeEvent(QResizeEvent *event) override;
	bool event(QEvent *event) override;
	bool eventFilter(QObject *watched, QEvent *event) override;
	void closeEvent(QCloseEvent *event) override;

private slots:
	void onMoveResizeTimerTimeout();

private:
	void initialize();
	void setupUi();
	void setupConnections();
	void setupStyle();

	static void	  executeProcessShellMethod(const QString &command);
	QFuture<void> runShellCommandAsync(const QString &command);

	QTimer			*m_move_resize_timer;
	QGridLayout		*m_main_layout;
	UserPanelWidget *m_user_panel;

	QPushButton *m_close_button;

	bool m_unlock_quit;

	QMap<QString, QPair<QString, QString>> m_original_keybinds;

	QTimer *m_test_timer;
	QTimer *m_test_timer_updater;
	QLabel *m_test_timer_label;
};

} // namespace APP

#endif // MAIN_WINDOW_HPP
