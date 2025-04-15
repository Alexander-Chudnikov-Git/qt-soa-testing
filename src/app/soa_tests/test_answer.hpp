#ifndef TEST_ANSWER_HPP
#define TEST_ANSWER_HPP

#include "panel_type.hpp"
#include "test_generic.hpp"

#include <QStringList>

namespace APP
{
class TestAnswer
{
public:
	TestAnswer();
	TestAnswer(const TestGeneric& question, QUESTION_TYPE question_type, const QString& answer);
	~TestAnswer();

	void setQuestion(const TestGeneric& question);
	void setQuestionType(QUESTION_TYPE question_type);
	void setAnswer(const QString& answer);

	TestGeneric	  getQuestion() const;
	QUESTION_TYPE getQuestionType() const;
	QString		  getAnswer() const;

	bool isCorrect() const;

private:
	TestGeneric	  m_question;
	QUESTION_TYPE m_question_type;
	QString		  m_answer;
};
} // namespace APP
#endif // TEST_ANSWER_HPP
