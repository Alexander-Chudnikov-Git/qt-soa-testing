#ifndef TEST_ONE_WIDGET_HPP
#define TEST_ONE_WIDGET_HPP

#include <QPushButton>
#include <QTextEdit>
#include <QWidget>

class QGridLayout;
class QLabel;

namespace APP
{
class TestOneWidget : public QWidget
{
	Q_OBJECT

public:
	explicit TestOneWidget(QWidget *parent = nullptr);
	~TestOneWidget();

signals:
	void testResult(QMap<QString, bool> result, const QString &user_input, int m_invalid_count);

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

#endif // TEST_ONE_WIDGET_HPP
