#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <QCloseEvent>
#include <QEvent>
#include <QFuture>
#include <QGridLayout>
#include <QMainWindow>
#include <QMoveEvent>
#include <QObject>
#include <QResizeEvent>
#include <QTimer>

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
	void disableAllGSettingsKeybinds();
	void restoreAllGSettingsKeybinds();

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

	bool m_unlock_quit;

	QMap<QString, QPair<QString, QString>> m_original_keybinds;
};

} // namespace APP

#endif // MAIN_WINDOW_HPP
