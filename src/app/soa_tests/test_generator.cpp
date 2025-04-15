#include "test_generator.hpp"

#include <QRandomGenerator>

namespace APP
{
TestGenerator::TestGenerator(QObject *parent) : QObject(parent)
{}

TestGenerator::~TestGenerator()
{
	m_questions.clear();
}

void TestGenerator::addQuestion(const TestGeneric &question)
{
	m_questions.push_back(question);
}

QVector<TestGeneric> TestGenerator::getQuestions(int number) const
{
	QVector<TestGeneric> selectedQuestions;
	const int			 availableCount = m_questions.size();

	if (number <= 0 || availableCount == 0)
	{
		return selectedQuestions;
	}

	const int countToSelect = std::min(number, availableCount);

	std::vector<int> indices(availableCount);
	std::iota(indices.begin(), indices.end(), 0);

	std::shuffle(indices.begin(), indices.end(), *QRandomGenerator::global());

	selectedQuestions.reserve(countToSelect);
	for (int i = 0; i < countToSelect; ++i)
	{
		selectedQuestions.append(m_questions[indices[i]]);
	}

	return selectedQuestions;
}
} // namespace APP
