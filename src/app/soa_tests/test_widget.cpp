#include "test_widget.hpp"

#include <QButtonGroup>
#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QRandomGenerator>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QVBoxLayout>

namespace APP
{

TestWidget::TestWidget(TestGeneric test, QWidget *parent) : QWidget(parent), m_test(std::move(test)), m_is_checked(false)
{
	determineQuestionType();
	initialize();
}

TestWidget::~TestWidget()
{}

void TestWidget::determineQuestionType()
{
	if (QRandomGenerator::global()->bounded(2) == 0)
		m_question_type = QUESTION_TYPE::INPUT;
	else
		m_question_type = QUESTION_TYPE::SELECTOR;
}

void TestWidget::initialize()
{
	setupUi();
	setupStyle();
	setupConnections();
}

void TestWidget::setupUi()
{
	setAutoFillBackground(true);
	setAttribute(Qt::WA_StyledBackground);

	m_main_layout	 = new QVBoxLayout();
	m_button_layout	 = new QHBoxLayout();
	m_submit_button	 = new QPushButton("Сохранить ответ");
	m_question_label = new QLabel(m_test.question());

	m_main_layout->setContentsMargins(32, 10, 32, 10);

	m_main_layout->addStretch(1);

	QFont title_font;
	title_font.setPointSize(18);
	title_font.setBold(true);

	QFont footnote_font;
	footnote_font.setPointSize(12);
	footnote_font.setBold(true);

	m_question_label->setAlignment(Qt::AlignCenter);
	m_question_label->setAlignment(Qt::AlignCenter);
	m_question_label->setWordWrap(true);
	m_question_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	m_question_label->setFont(title_font);

	m_main_layout->addWidget(m_question_label);

	if (m_question_type == QUESTION_TYPE::SELECTOR)
	{
		m_answers_group			  = new QGroupBox("Выберете правильный ответ:");
		QVBoxLayout *radio_layout = new QVBoxLayout(m_answers_group);
		m_button_group			  = new QButtonGroup();

		m_answers_group->setFont(footnote_font);

		m_answers_group->setLayout(radio_layout);

		const QStringList answers = m_test.possibleAnswers();
		m_radio_buttons.reserve(answers.size());

		for (const QString &answer : answers)
		{
			QRadioButton *radioButton = new QRadioButton(answer, m_answers_group);
			m_radio_buttons.append(radioButton);
			radio_layout->addWidget(radioButton);
			m_button_group->addButton(radioButton);
		}
		m_main_layout->addWidget(m_answers_group);
	}
	else
	{
		m_input_line_edit = new QLineEdit();

		m_input_line_edit->setFont(footnote_font);
		m_input_line_edit->setPlaceholderText("Введите ответ...");
		m_main_layout->addWidget(m_input_line_edit);
	}

	m_button_layout->addStretch(1);
	m_button_layout->addWidget(m_submit_button);
	m_button_layout->addStretch(1);

	m_main_layout->addLayout(m_button_layout);

	m_main_layout->addStretch(1);
	this->setLayout(m_main_layout);
}

void TestWidget::setupStyle()
{
	setStyleSheet("* { background-color: #181818; color: #ffffff; }"
				  "QPushButton { background-color: #333333; color: #ffffff; border: 1px solid #555555; border-radius: 5px; "
				  "padding: 5px; }"
				  "QPushButton:hover { background-color: #444444; }"
				  "QPushButton:pressed { background-color: #555555; }"
				  "QPushButton:disabled { background-color: #181818; color: #555555; }");
}

void TestWidget::setupConnections()
{
	connect(m_submit_button, &QPushButton::clicked, this, &TestWidget::onSubmitClicked);
}

void TestWidget::onSubmitClicked()
{
	if (m_is_checked)
	{
		emit taskSubmitted();
	}
	else
	{
		// add handler for different QUESTION_TYPE
		m_is_checked = true;
		m_submit_button->setText("Перейти к следующему заданию");

		for (auto button : m_radio_buttons)
		{
			button->setDisabled(true);
			int current_index = -(m_button_group->id(button) + 1);

			if (current_index == m_test.correctAnswer())
			{
				button->setStyleSheet("background-color: #FFD50D;");
			}

			if (button->isChecked())
			{

				if (current_index != m_test.correctAnswer())
				{
					button->setStyleSheet("background-color: #EB0100;");
				}
				else
				{
					button->setStyleSheet("background-color: #71D800;");
				}
			}
		}
	}
}

TestAnswer TestWidget::getAnswer()
{
	switch (m_question_type)
	{
		case QUESTION_TYPE::SELECTOR: {
			return TestAnswer(m_test, QUESTION_TYPE::SELECTOR, QString::number(-(m_button_group->checkedId() + 1)));
		}
		case QUESTION_TYPE::INPUT: {
			return TestAnswer(m_test, QUESTION_TYPE::INPUT, m_input_line_edit->text());
		}
		default: {
			return TestAnswer(m_test, QUESTION_TYPE::NONE, "");
		}
	}
}
} // namespace APP
