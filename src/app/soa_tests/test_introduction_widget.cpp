#include "test_introduction_widget.hpp"

#include "settings_manager.hpp"

#include <QGridLayout>
#include <QLabel>

namespace APP
{
IntroductionWidget::IntroductionWidget(QWidget *parent) : QWidget(parent)
{
	initialize();
}

IntroductionWidget::~IntroductionWidget()
{}

void IntroductionWidget::initialize()
{
	setupUi();
	setupStyle();
	setupConnections();
}

void IntroductionWidget::setupUi()
{
	setAutoFillBackground(true);
	setAttribute(Qt::WA_StyledBackground);

	m_main_layout = new QGridLayout(this);

	QFont title_font;
	title_font.setPointSize(18);
	title_font.setBold(true);

	QFont footnote_font;
	footnote_font.setPointSize(12);
	footnote_font.setBold(true);

	m_title_label = new QLabel("Лабораторная работа № 2 по дисциплине «Системы Обнаружения Атак»");
	m_title_label->setAlignment(Qt::AlignCenter);
	m_title_label->setWordWrap(true);
	m_title_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	m_title_label->setFont(title_font);

	auto test_time = UTILS::SettingsManager::instance()->getValue(UTILS::SettingsManager::Setting::TEST_TIME_LIMIT).toString();
	auto test_questions =
		UTILS::SettingsManager::instance()->getValue(UTILS::SettingsManager::Setting::TEST_QUESTIONS).toString();

	m_footnote_label = new QLabel(QString("После начала теста, у вас будет ровно %1 минут на его прохождение. \n"
										  "Тест будет состоять из %2 вопросов.")
									  .arg(test_time)
									  .arg(test_questions));
	m_footnote_label->setAlignment(Qt::AlignCenter);
	m_footnote_label->setWordWrap(true);
	m_footnote_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	m_footnote_label->setFont(footnote_font);

	m_start_test_button = new QPushButton(" Начать тест ");
	m_start_test_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	m_start_test_button->setFont(title_font);
	m_start_test_button->setDisabled(false);

	m_main_layout->addWidget(m_title_label, 0, 0);
	m_main_layout->setRowStretch(2, 0);
	m_main_layout->addWidget(m_start_test_button, 3, 0, Qt::AlignCenter);
	m_main_layout->addWidget(m_footnote_label, 5, 0);

	setLayout(m_main_layout);
}

void IntroductionWidget::setupStyle()
{
	setStyleSheet("* { background-color: #181818; color: #ffffff; }"
				  "QPushButton { background-color: #333333; color: #ffffff; border: 1px solid #555555; border-radius: 5px; "
				  "padding: 5px; }"
				  "QPushButton:hover { background-color: #444444; }"
				  "QPushButton:pressed { background-color: #555555; }"
				  "QPushButton:disabled { background-color: #181818; color: #555555; }");
}

void IntroductionWidget::setupConnections()
{
	connect(m_start_test_button, &QPushButton::clicked, this, &IntroductionWidget::onStartTestClicked);
}
} // namespace APP
