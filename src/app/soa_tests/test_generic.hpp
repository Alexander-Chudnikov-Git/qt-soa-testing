#ifndef TEST_GENERIC_HPP
#define TEST_GENERIC_HPP

#include <QStringList>

namespace APP
{
class TestGeneric
{
public:
	TestGeneric();
	TestGeneric(const QString	  &question,
				const QStringList &possible_answers,
				int				   correct_answer,
				const QString	  &input_answer,
				const QString	  &input_validator,
				const QString	  &explanation);
	~TestGeneric();

	void setQuestion(const QString &question);
	void setPossibleAnswers(const QStringList &answers);
	void setCorrectAnswer(int answer);
	void setInputAnswer(const QString &input_answer);
	void setInputValidator(const QString &input_validator);
	void setExplanation(const QString &explanation);

	QString		question() const;
	QStringList possibleAnswers() const;
	int			correctAnswer() const;
	QString		inputAnswer() const;
	QString		inputValidator() const;
	QString		explanation() const;

private:
	QString m_question;

	// Given Choice questions
	QStringList m_possible_answers;
	int			m_correct_answer;

	// Input questions
	QString m_input_answer;
	QString m_input_validator;

	QString m_explanation;
};
} // namespace APP
#endif // TEST_GENERIC_HPP
