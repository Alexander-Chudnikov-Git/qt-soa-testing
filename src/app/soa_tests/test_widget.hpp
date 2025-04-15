#ifndef TEST_WIDGET_HPP
#define TEST_WIDGET_HPP

#include "panel_type.hpp"
#include "test_answer.hpp"
#include "test_generic.hpp"

#include <QString>
#include <QWidget>

class QLabel;
class QVBoxLayout;
class QHBoxLayout;
class QRadioButton;
class QLineEdit;
class QButtonGroup;
class QRegularExpressionValidator;
class QCheckBox;
class QGroupBox;
class QPushButton;

namespace APP
{

class TestWidget : public QWidget
{
	Q_OBJECT
signals:
	void taskSubmitted();

public:
	explicit TestWidget(TestGeneric test, QWidget* parent = nullptr);
	~TestWidget() override;

	TestAnswer getAnswer();

private:
	void initialize();
	void setupUi();
	void setupStyle();
	void setupConnections();

	void determineQuestionType();

private slots:
	void onSubmitClicked();

private:
	TestGeneric m_test;

	QVBoxLayout* m_main_layout	  = nullptr;
	QLabel*		 m_question_label = nullptr;

	QGroupBox*			   m_answers_group = nullptr;
	QVector<QRadioButton*> m_radio_buttons;
	QButtonGroup*		   m_button_group = nullptr;

	QLineEdit*					 m_input_line_edit = nullptr;
	QRegularExpressionValidator* m_validator	   = nullptr;

	QHBoxLayout* m_button_layout = nullptr;
	QPushButton* m_submit_button = nullptr;

	QUESTION_TYPE m_question_type;

	bool m_is_checked;
};

} // namespace APP

#endif // TEST_WIDGET_HPP
