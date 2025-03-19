#ifndef TEST_RESULT_WIDGET_HPP
#define TEST_RESULT_WIDGET_HPP

#include <QLabel>
#include <QWidget>

class QGridLayout;

namespace APP
{
class TestResultWidget : public QWidget
{
	Q_OBJECT

public:
	explicit TestResultWidget(QWidget *parent = nullptr);
	explicit TestResultWidget(QMap<QString, bool>	 result,
							  QMap<QString, QString> input,
							  QMap<QString, int>	 invalid,
							  QWidget				*parent = nullptr);
	~TestResultWidget();

public:
	void setTestResult(QMap<QString, bool> result, QMap<QString, QString> input, QMap<QString, int> invalid);

private:
	void initialize();
	void setupUi();
	void setupStyle();
	void setupConnections();
	void updateResults();

	QString resolveTestName(const QString &test_name);

private:
	QMap<QString, bool>	   m_result;
	QMap<QString, QString> m_input;
	QMap<QString, int>	   m_invalid;

	QGridLayout *m_main_layout;

	QLabel *m_title_label;
};

} // namespace APP

#endif // TEST_RESULT_WIDGET_HPP
