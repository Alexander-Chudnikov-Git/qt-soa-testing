#include "test_answer_widget.hpp"

#include <QGridLayout>
#include <QLabel>
#include <QString>

namespace APP
{
TestAnswerWidget::TestAnswerWidget(const TestAnswer &answer, int index, QWidget *parent) : QWidget(parent)
{
	m_answer = answer;
	m_index	 = index;

	initialize();
	updateAnswer();
}

TestAnswerWidget::~TestAnswerWidget()
{}

void TestAnswerWidget::initialize()
{
	setupUi();
	setupStyle();
	setupConnections();
}

void TestAnswerWidget::setupUi()
{
	m_main_layout	= new QGridLayout(this);
	m_title_label	= new QLabel("test");
	m_correct_label = new QLabel("correct");
	m_real_label	= new QLabel("answer");
	m_comment_label = new QLabel("comment");

	m_title_label->setWordWrap(true);
	m_correct_label->setWordWrap(true);
	m_real_label->setWordWrap(true);
	m_comment_label->setWordWrap(true);

	m_main_layout->addWidget(m_title_label, 0, 0, 1, 2);
	m_main_layout->addWidget(m_correct_label, 1, 0, 1, 1);
	m_main_layout->addWidget(m_real_label, 2, 0, 1, 1);
	m_main_layout->addWidget(m_comment_label, 1, 1, 1, 2);

	setLayout(m_main_layout);
}

void TestAnswerWidget::setupStyle()
{
	setAutoFillBackground(true);
	setAttribute(Qt::WA_StyledBackground);
}

void TestAnswerWidget::setupConnections()
{}

void TestAnswerWidget::updateAnswer()
{
	m_title_label->setText(QString("<b>Вопрос %1:</b> %2").arg(m_index).arg(m_answer.getQuestion().question()));

	if (m_answer.getQuestionType() == QUESTION_TYPE::SELECTOR)
	{
		QString real_answer;
		QString correct_answer;

		if (m_answer.getQuestion().possibleAnswers().size() < m_answer.getAnswer().toInt() ||
			m_answer.getQuestion().possibleAnswers().size() < m_answer.getQuestion().correctAnswer())
		{
			real_answer	   = "[Ошибка]";
			correct_answer = "Ошибка";
		}
		else if (m_answer.getAnswer().toInt() == 0)
		{
			real_answer	   = "[Нет ответа]";
			correct_answer = m_answer.getQuestion().possibleAnswers().at(m_answer.getQuestion().correctAnswer() - 1);
		}
		else
		{
			real_answer	   = m_answer.getQuestion().possibleAnswers().at(m_answer.getAnswer().toInt() - 1);
			correct_answer = m_answer.getQuestion().possibleAnswers().at(m_answer.getQuestion().correctAnswer() - 1);
		}

		m_real_label->setText(QString("Ваш ответ: %1").arg(real_answer.isEmpty() ? tr("[Нет ответа]") : real_answer));
		m_correct_label->setText(
			QString("Правильный ответ: %1").arg(correct_answer.isEmpty() ? tr("[Нет ответа]") : correct_answer));
	}
	else if (m_answer.getQuestionType() == QUESTION_TYPE::INPUT)
	{
		m_real_label->setText(
			QString("Ваш ответ: %1").arg(m_answer.getAnswer().isEmpty() ? tr("[Нет ответа]") : m_answer.getAnswer()));
		m_correct_label->setText(QString("Правильный ответ: %1")
									 .arg(m_answer.getQuestion().inputAnswer().isEmpty() ?
											  tr("[Нет ответа]") :
											  m_answer.getQuestion().inputAnswer()));
	}
	m_comment_label->setText(QString("<i>Пояснение:</i> %1").arg(m_answer.getQuestion().explanation()));

	if (m_answer.isCorrect())
	{
		setStyleSheet("background-color: #71D800;");
	}
	else
	{
		setStyleSheet("background-color: #EB0100;");
	}
}

} // namespace APP
