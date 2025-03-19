#ifndef TEST_TWO_WIDGET_HPP
#define TEST_TWO_WIDGET_HPP

#include <QPushButton>
#include <QTextEdit>
#include <QWidget>

class QGridLayout;
class QLabel;

namespace APP
{
class TestTwoWidget : public QWidget
{
	Q_OBJECT

public:
	explicit TestTwoWidget(QWidget *parent = nullptr);
	~TestTwoWidget();

signals:
	void testResult(QMap<QString, bool> result, const QString &user_input, int invalid_count);

private:
	void initialize();
	void setupUi();
	void setupStyle();
	void setupConnections();
	bool validateInput();

private slots:
	void onCheckButtonClicked();

private:
	QGridLayout *m_main_layout;

	QLabel *m_title_label;
	QLabel *m_subtitle_label;

	QTextEdit	*m_input_text_edit;
	QPushButton *m_check_button;

	bool m_is_validated;
	int	 m_invalid_count;

	QMap<QString, bool> m_result_map;
};
} // namespace APP

#endif // TEST_TWO_WIDGET_HPP
