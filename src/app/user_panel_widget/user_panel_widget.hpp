#ifndef USER_PANEL_WIDGET_HPP
#define USER_PANEL_WIDGET_HPP

#include "panel_type.hpp"
#include "test_answer.hpp"
#include "test_generator.hpp"
#include "test_widget.hpp"

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

	void cleanupQuestions();
	void generateQuestions();

	void addScreen(const ScreenInfo &screen_info);
	void switchScreen(PanelType type);
	void nextScreen();

	void finishPrematurely();

signals:
	void testStarted();
	void testFinished();

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

	QMap<QString, bool>	   m_result_map;
	QMap<QString, QString> m_result_input_map;
	QMap<QString, int>	   m_result_invalid_map;

	TestGenerator		*m_test_generator;
	QVector<TestGeneric> m_questions;
	QVector<TestAnswer>	 m_answers;

	int m_current_test_index;
};
} // namespace APP
#endif // USER_PANEL_WIDGET_HPP
