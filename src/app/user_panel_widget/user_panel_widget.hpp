#ifndef USER_PANEL_WIDGET_HPP
#define USER_PANEL_WIDGET_HPP

#include "panel_type.hpp"

#include <QMap>
#include <QWidget>

class QStackedWidget;
class QLabel;
class QLineEdit;
class QWidget;

namespace APP
{
class UserPanelWidget : public QWidget
{
	Q_OBJECT
public:
	struct ScreenInfo
	{
		PanelType type = PanelType::NONE;
		QString	  text;
		QWidget	 *widget;
	};

public:
	explicit UserPanelWidget(QWidget *parent = nullptr);
	~UserPanelWidget();

	void addScreen(const ScreenInfo &screen_info);
	void switchScreen(PanelType type);
	void nextScreen();

signals:
	void testStarted();
	void testFinished();
	void validationFinished();

private:
	void initialize();
	void setupUi();
	void setupStyle();
	void setupConnections();

	QWidget *resolveScreenWidget(PanelType type);
	QString	 resolveScreenText(PanelType type) const;

private:
	QStackedWidget *m_main_layout;

	QMap<int, ScreenInfo> m_screens;
};
} // namespace APP
#endif // USER_PANEL_WIDGET_HPP
