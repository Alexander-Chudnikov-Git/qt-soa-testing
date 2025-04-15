#ifndef TEST_ANSWER_WIDGET_HPP
#define TEST_ANSWER_WIDGET_HPP

#include "test_answer.hpp"

#include <QWidget>

class QGridLayout;
class QLabel;

namespace APP
{
class TestAnswerWidget : public QWidget
{
	Q_OBJECT
public:
	TestAnswerWidget(const TestAnswer &answer, int index, QWidget *parent = nullptr);
	~TestAnswerWidget();

private:
	void initialize();
	void setupUi();
	void setupStyle();
	void setupConnections();

	void updateAnswer();

private:
	QGridLayout *m_main_layout;
	QLabel		*m_title_label;
	QLabel		*m_correct_label;
	QLabel		*m_real_label;
	QLabel		*m_comment_label;

	TestAnswer m_answer;

	int m_index;
};
} // namespace APP
#endif // TEST_ANSWER_HPP
