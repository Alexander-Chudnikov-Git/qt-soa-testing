#include "test_generic.hpp"

namespace APP
{
TestGeneric::TestGeneric()
{}

TestGeneric::TestGeneric(const QString	   &question,
						 const QStringList &possible_answers,
						 int				correct_answer,
						 const QString	   &input_answer,
						 const QString	   &input_validator,
						 const QString	   &explanation)
{
	this->m_question		 = question;
	this->m_possible_answers = possible_answers;
	this->m_correct_answer	 = correct_answer;
	this->m_input_answer	 = input_answer;
	this->m_input_validator	 = input_validator;
	this->m_explanation		 = explanation;
}

TestGeneric::~TestGeneric()
{}

void TestGeneric::setQuestion(const QString &question)
{
	this->m_question = question;
}

void TestGeneric::setPossibleAnswers(const QStringList &answers)
{
	this->m_possible_answers = answers;
}

void TestGeneric::setCorrectAnswer(int answer)
{
	this->m_correct_answer = answer;
}

void TestGeneric::setInputAnswer(const QString &input_answer)
{
	this->m_input_answer = input_answer;
}

void TestGeneric::setInputValidator(const QString &input_validator)
{
	this->m_input_validator = input_validator;
}

void TestGeneric::setExplanation(const QString &explanation)
{
	this->m_explanation = explanation;
}

QString TestGeneric::question() const
{
	return this->m_question;
}

QStringList TestGeneric::possibleAnswers() const
{
	return this->m_possible_answers;
}

int TestGeneric::correctAnswer() const
{
	return this->m_correct_answer;
}

QString TestGeneric::inputAnswer() const
{
	return this->m_input_answer;
}

QString TestGeneric::inputValidator() const
{
	return this->m_input_validator;
}

QString TestGeneric::explanation() const
{
	return this->m_explanation;
}
} // namespace APP
