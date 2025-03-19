#include "test_result_widget.hpp"

#include <QDebug> // For debugging, you can remove it later.
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMap>
#include <QPalette>
#include <QScrollArea>
#include <QTextEdit>
#include <QVBoxLayout>

namespace APP
{

TestResultWidget::TestResultWidget(QWidget* parent) : QWidget(parent)
{
	initialize();
}

TestResultWidget::TestResultWidget(QMap<QString, bool>	  result,
								   QMap<QString, QString> input,
								   QMap<QString, int>	  invalid,
								   QWidget*				  parent) :
	QWidget(parent)
{
	initialize();
	setTestResult(result, input, invalid);
}

TestResultWidget::~TestResultWidget()
{}

void TestResultWidget::setTestResult(QMap<QString, bool> result, QMap<QString, QString> input, QMap<QString, int> invalid)
{
	m_result  = result;
	m_input	  = input;
	m_invalid = invalid;
	updateResults();
}

void TestResultWidget::initialize()
{
	setupUi();
	setupStyle();
	setupConnections();
}

void TestResultWidget::setupUi()
{
	m_main_layout = new QGridLayout();

	setLayout(m_main_layout);
}

void TestResultWidget::setupStyle()
{}

void TestResultWidget::setupConnections()
{}

void TestResultWidget::updateResults()
{
	while (m_main_layout->count() > 0)
	{
		QLayoutItem* item = m_main_layout->takeAt(0);
		if (item)
		{
			if (QWidget* widget = item->widget())
			{
				widget->deleteLater();
			}
			delete item;
		}
	}

	m_title_label = new QLabel();

	QFont title_font;
	title_font.setPointSize(18);
	title_font.setBold(true);

	m_title_label->setText("Результаты теста");
	m_title_label->setFont(title_font);

	m_main_layout->addWidget(m_title_label, 0, 0);

	int row = 1;
	for (auto it = m_input.begin(); it != m_input.end(); ++it)
	{
		QString test_name  = it.key();
		QString test_input = it.value();

		QHBoxLayout* test_layout				= new QHBoxLayout();
		QVBoxLayout* test_wrapper_layout		= new QVBoxLayout();
		QVBoxLayout* test_edit_wrapper_layout	= new QVBoxLayout();
		QVBoxLayout* test_result_wrapper_layout = new QVBoxLayout();

		QLabel*	   label	   = new QLabel(resolveTestName(test_name));
		QLabel*	   input_label = new QLabel("Ответ пользователя:");
		QTextEdit* input_edit  = new QTextEdit();

		int result_number  = 0;
		int result_correct = 0;

		for (auto rt = m_result.begin(); rt != m_result.end(); ++rt)
		{
			if (!rt.key().contains(test_name.section('_', 0, -2)))
			{
				continue;
			}

			QString sub_test_name	= rt.key();
			bool	sub_test_result = rt.value();

			QLabel* result_label = new QLabel(resolveTestName(sub_test_name));
			result_label->setAlignment(Qt::AlignLeft);

			result_number += 1;
			if (sub_test_result)
			{
				result_label->setStyleSheet("background-color: green;");
				result_correct += 1;
			}
			else
			{
				result_label->setStyleSheet("background-color: red;");
			}

			test_result_wrapper_layout->addWidget(result_label);
		}

		QLabel* total_result_label =
			new QLabel(QString("Всего: %1/%2 - %3").arg(result_correct).arg(result_number).arg(m_invalid[test_name]));
		total_result_label->setAlignment(Qt::AlignLeft);
		test_result_wrapper_layout->addWidget(total_result_label);

		input_edit->setText(test_input);
		input_edit->setReadOnly(true);

		test_edit_wrapper_layout->addWidget(input_label);
		test_edit_wrapper_layout->addWidget(input_edit);

		test_wrapper_layout->addWidget(label);
		test_wrapper_layout->addLayout(test_layout);

		QWidget* test_edit_wrapper_widget	= new QWidget();
		QWidget* test_result_wrapper_widget = new QWidget();

		test_edit_wrapper_widget->setLayout(test_edit_wrapper_layout);
		test_result_wrapper_widget->setLayout(test_result_wrapper_layout);

		test_layout->addWidget(test_edit_wrapper_widget);
		test_layout->addWidget(test_result_wrapper_widget);

		m_main_layout->addLayout(test_wrapper_layout, row++, 0);
	}

	m_main_layout->setRowStretch(row, 1);
}

QString TestResultWidget::resolveTestName(const QString& test_name)
{
	QString human_name;

	if (test_name == "test_1_var_valid")
	{
		human_name = "Проверка 1: Проверка var:";
	}
	else if (test_name == "test_1_group_valid")
	{
		human_name = "Проверка 2: Проверка address-groups:";
	}
	else if (test_name == "test_1_single_valid")
	{
		human_name = "Проверка 3: Наличие обычного ip";
	}
	else if (test_name == "test_1_range_valid")
	{
		human_name = "Проверка 4: Наличие диапазона ip";
	}
	else if (test_name == "test_1_multi_valid")
	{
		human_name = "Проверка 5: Наличие набора ip";
	}
	else if (test_name == "test_1_any_valid")
	{
		human_name = "Проверка 6: Наличие any ip";
	}
	else if (test_name == "test_1_variable_valid")
	{
		human_name = "Проверка 7: Наличие переменной ip";
	}
	else if (test_name == "test_1_negation_valid")
	{
		human_name = "Проверка 8: Наличие обратного ip";
	}
	else if (test_name == "test_2_var_valid")
	{
		human_name = "Проверка 1: Проверка var:";
	}
	else if (test_name == "test_2_group_valid")
	{
		human_name = "Проверка 2: Проверка address-groups:";
	}
	else if (test_name == "test_2_single_valid")
	{
		human_name = "Проверка 3: Наличие обычного порта";
	}
	else if (test_name == "test_2_multi_valid")
	{
		human_name = "Проверка 4: Наличие набора портов";
	}
	else if (test_name == "test_2_any_valid")
	{
		human_name = "Проверка 5: Наличие any порта";
	}
	else if (test_name == "test_2_variable_valid")
	{
		human_name = "Проверка 6: Наличие переменной порта";
	}
	else if (test_name == "test_2_negation_valid")
	{
		human_name = "Проверка 7: Наличие обратного порта";
	}
	else if (test_name == "test_1_input")
	{
		human_name = "Входные данные для теста 1";
	}
	else if (test_name == "test_2_input")
	{
		human_name = "Входные данные для теста 2";
	}
	else
	{
		human_name = test_name;
	}

	return human_name;
}

} // namespace APP
