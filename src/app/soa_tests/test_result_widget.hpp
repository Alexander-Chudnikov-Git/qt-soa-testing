#ifndef TEST_RESULT_WIDGET_HPP
#define TEST_RESULT_WIDGET_HPP

#include "test_answer.hpp" // Assuming this defines the TestAnswer class

#include <QVector> // Include QVector
#include <QWidget>

// Forward declarations
class QGridLayout;
class QVBoxLayout;
class QLabel;
class QPushButton;
class QScrollArea;
class QFrame; // For separators

namespace APP
{

class ResultWidget : public QWidget
{
	Q_OBJECT
public:
	explicit ResultWidget(QWidget *parent = nullptr);
	~ResultWidget();

	void setTestAnswers(const QVector<TestAnswer> &answers);

signals:
	void onStartTestClicked();

private:
	void initialize();
	void setupUi();
	void setupStyle();
	void setupConnections();
	void clearLayout(QLayout *layout);

private:
	QGridLayout *m_main_layout = nullptr;

	QLabel		*m_title_label		   = nullptr;
	QLabel		*m_score_label		   = nullptr;
	QPushButton *m_restart_test_button = nullptr;

	QScrollArea *m_scroll_area			  = nullptr;
	QWidget		*m_results_content_widget = nullptr;
	QVBoxLayout *m_results_layout		  = nullptr;

	QVector<TestAnswer> m_answers;
};

} // namespace APP

#endif // TEST_RESULT_WIDGET_HPP
