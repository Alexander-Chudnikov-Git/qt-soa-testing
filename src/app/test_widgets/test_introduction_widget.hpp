#ifndef TEST_INTRODUCTION_WIDGET_HPP
#define TEST_INTRODUCTION_WIDGET_HPP

#include <QWidget>
#include <qpushbutton.h>

class QGridLayout;
class QLabel;

namespace APP
{
class SuricataValidatorWidget;
enum class ValidationStatus;
class IntroductionWidget : public QWidget
{
	Q_OBJECT
public:
	explicit IntroductionWidget(QWidget *parent = nullptr);
	~IntroductionWidget();

signals:
	void onStartTestClicked();

private:
	void initialize();
	void setupUi();
	void setupStyle();
	void setupConnections();

private slots:
	void onValidationFinished(ValidationStatus status);

private:
	QGridLayout *m_main_layout;

	QLabel *m_title_label;
	QLabel *m_footnote_label;

	SuricataValidatorWidget *m_suricata_validator_widget;

	QPushButton *m_start_test_button;
};
} // namespace APP

#endif // TEST_INTRODUCTION_WIDGET_HPP
