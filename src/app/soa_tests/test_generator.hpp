#ifndef TEST_GENERATOR_HPP
#define TEST_GENERATOR_HPP

#include "test_generic.hpp"

#include <QObject>
#include <QVector>

namespace APP
{
class TestGenerator : public QObject
{
	Q_OBJECT
public:
	TestGenerator(QObject *parent = nullptr);
	~TestGenerator();

	void addQuestion(const TestGeneric &question);

	QVector<TestGeneric> getQuestions(int number) const;

private:
	QVector<TestGeneric> m_questions;
};
} // namespace APP
#endif // TEST_GENERATOR_HPP
