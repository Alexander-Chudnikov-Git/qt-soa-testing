#include "test_answer.hpp"

#include <QRegularExpression>

namespace APP
{
TestAnswer::TestAnswer()
{}

TestAnswer::TestAnswer(const TestGeneric& question, QUESTION_TYPE question_type, const QString& answer)
{
	m_question		= question;
	m_question_type = question_type;
	m_answer		= answer;
}

TestAnswer::~TestAnswer()
{}

void TestAnswer::setQuestion(const TestGeneric& question)
{
	m_question = question;
}

void TestAnswer::setQuestionType(QUESTION_TYPE question_type)
{
	m_question_type = question_type;
}

void TestAnswer::setAnswer(const QString& answer)
{
	m_answer = answer;
}

TestGeneric TestAnswer::getQuestion() const
{
	return m_question;
}

QUESTION_TYPE TestAnswer::getQuestionType() const
{
	return m_question_type;
}

QString TestAnswer::getAnswer() const
{
	return m_answer;
}

bool TestAnswer::isCorrect() const
{
	bool is_correct = false;

	if (m_answer.isEmpty())
	{
		return is_correct;
	}

	if (m_question_type == QUESTION_TYPE::SELECTOR)
	{
		is_correct = m_answer.toInt() == m_question.correctAnswer();
	}
	else if (m_question_type == QUESTION_TYPE::INPUT)
	{
		QRegularExpression		regex(m_question.inputValidator());
		QRegularExpressionMatch match = regex.match(m_answer);

		is_correct = match.hasMatch();
	}

	return is_correct;
}

} // namespace APP
