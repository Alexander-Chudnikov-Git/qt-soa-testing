#include "test_one_widget.hpp"

#include "settings_manager.hpp"
#include "spdlog_wrapper.hpp"

#include <QDebug>
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QRegularExpression>
#include <QStringList>
#include <QTextEdit>

namespace APP
{
TestOneWidget::TestOneWidget(QWidget *parent) :
	QWidget(parent),
	m_is_validated(false),
	m_result_map({{"test_1_var_valid", false},
				  {"test_1_group_valid", false},
				  {"test_1_single_valid", false},
				  {"test_1_range_valid", false},
				  {"test_1_multi_valid", false},
				  {"test_1_any_valid", false},
				  {"test_1_variable_valid", false},
				  {"test_1_negation_valid", false}})
{
	initialize();
}

TestOneWidget::~TestOneWidget()
{}

void TestOneWidget::initialize()
{
	setupUi();
	setupStyle();
	setupConnections();
}

void TestOneWidget::setupUi()
{
	m_main_layout = new QGridLayout(this);

	m_title_label	  = new QLabel(this);
	m_subtitle_label  = new QLabel(this);
	m_input_text_edit = new QTextEdit(this);
	m_check_button	  = new QPushButton(this);

	m_input_text_edit->setEnabled(true);
	m_input_text_edit->setReadOnly(false);
	m_input_text_edit->setFocus();
	m_input_text_edit->raise();
	m_input_text_edit->setPlaceholderText("example\n\tsome settings");
	m_input_text_edit->setTabStopDistance(4);

	m_title_label->setText("Тест 1: Настройка групп адресов");
	m_subtitle_label->setText("С выданной схемы указать все IP адреса, задействовав не менее 6ти разных способов их "
							  "записи.\nПосле нажатия кнопки \"Проверить\" пути назад не будет.");
	m_check_button->setText("Проверить");

	m_main_layout->addWidget(m_title_label, 0, 0);
	m_main_layout->addWidget(m_subtitle_label, 1, 0);
	m_main_layout->addWidget(m_input_text_edit, 2, 0);
	m_main_layout->addWidget(m_check_button, 3, 0);

	setLayout(m_main_layout);
}

void TestOneWidget::setupStyle()
{
	m_title_label->setStyleSheet("font-weight: bold; font-size: 16px;");
	m_subtitle_label->setStyleSheet("font-size: 14px;");
}

void TestOneWidget::setupConnections()
{
	connect(m_check_button, &QPushButton::clicked, this, &TestOneWidget::onCheckButtonClicked);
}

bool TestOneWidget::validateInput()
{
	QString input = m_input_text_edit->toPlainText();

	m_result_map["test_1_var_valid"]	  = true;
	m_result_map["test_1_group_valid"]	  = true;
	m_result_map["test_1_single_valid"]	  = false;
	m_result_map["test_1_range_valid"]	  = false;
	m_result_map["test_1_multi_valid"]	  = false;
	m_result_map["test_1_any_valid"]	  = false;
	m_result_map["test_1_variable_valid"] = false;
	m_result_map["test_1_negation_valid"] = false;

	bool vars_section_found			  = false;
	bool address_groups_section_found = false;
	bool overall_valid				  = true;

	QRegularExpression single_ip_regex(
		R"(^\s*([a-zA-Z0-9_-]+)\s*:\s*"?(!?(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)|any)\s*"?\s*$)");
	QRegularExpression range_ip_regex(
		R"(^\s*([a-zA-Z0-9_-]+)\s*:\s*"?(!?(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)/(?:[0-9]|[12][0-9]|3[0-2]))\s*"?\s*$)");
	QRegularExpression multi_ip_regex(
		R"(^\s*([a-zA-Z0-9_-]+)\s*:\s*"??\[(?:\s*(!?\s*(?:\$[a-zA-Z0-9_-]+|(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)(?:\/(?:[0-9]|[12][0-9]|3[0-2]))?|any))\s*,?\s*)+\]"?\s*$)");
	QRegularExpression any_ip_regex(R"(^\s*([a-zA-Z0-9_-]+)\s*:\s*"\s*?(!?any)\s*"?\s*$)");
	QRegularExpression variable_regex(R"(^\s*([a-zA-Z0-9_-]+)\s*:\s*"?(\$[a-zA-Z0-9_-]+)\"?\s*$)");
	QRegularExpression negation_regex(R"(^\s*([a-zA-Z0-9_-]+)\s*:\s*"?(!\$[a-zA-Z0-9_-]+)\"?\s*$)");

	QStringList lines = input.split('\n', Qt::KeepEmptyParts);

	int match_count = 0;
	m_invalid_count = 0;

	for (const QString &line : lines)
	{
		QString trimmed_line = line.trimmed();

		if (trimmed_line.isEmpty() || trimmed_line.startsWith("#"))
		{
			continue;
		}

		if (!vars_section_found && match_count == 0)
		{
			if (trimmed_line.startsWith("vars:"))
			{
				vars_section_found = true;
				continue;
			}
		}

		if (!address_groups_section_found && match_count == 0)
		{
			if (trimmed_line.startsWith("address-groups:"))
			{
				address_groups_section_found = true;
				continue;
			}
		}

		bool line_matched = false;
		if (any_ip_regex.match(trimmed_line).hasMatch())
		{
			match_count += 1;
			m_result_map["test_1_any_valid"] = true;
			line_matched					 = true;
		}
		else if (single_ip_regex.match(trimmed_line).hasMatch())
		{
			match_count += 1;
			m_result_map["test_1_single_valid"] = true;
			line_matched						= true;
		}
		else if (range_ip_regex.match(trimmed_line).hasMatch())
		{
			match_count += 1;
			m_result_map["test_1_range_valid"] = true;
			line_matched					   = true;
		}
		else if (multi_ip_regex.match(trimmed_line).hasMatch())
		{
			match_count += 1;
			m_result_map["test_1_multi_valid"] = true;
			line_matched					   = true;
		}
		else if (variable_regex.match(trimmed_line).hasMatch())
		{
			match_count += 1;
			m_result_map["test_1_variable_valid"] = true;
			line_matched						  = true;
		}
		else if (negation_regex.match(trimmed_line).hasMatch())
		{
			match_count += 1;
			m_result_map["test_1_negation_valid"] = true;
			line_matched						  = true;
		}

		if (!line_matched)
		{
			m_invalid_count += 1;
			overall_valid = false;
			SPD_WARN_CLASS(UTILS::DEFAULTS::d_settings_group_application, "Invalid line format: " + line);
		}
	}

	if (!vars_section_found)
	{
		m_result_map["test_1_var_valid"] = false;
		overall_valid					 = false;
		SPD_WARN_CLASS(UTILS::DEFAULTS::d_settings_group_application, "vars section not found");
	}

	if (!address_groups_section_found)
	{
		m_result_map["test_1_group_valid"] = false;
		overall_valid					   = false;
		SPD_WARN_CLASS(UTILS::DEFAULTS::d_settings_group_application, "address-groups section not found");
	}

	int true_count = 0;
	for (const QString &key : m_result_map.keys())
	{
		if (m_result_map.value(key))
		{
			true_count++;
		}
	}

	if (overall_valid && true_count < 8)
	{
		SPD_WARN_CLASS(UTILS::DEFAULTS::d_settings_group_application,
					   "Max score is 8. Current score is: " + QString::number(true_count));
	}

	return true;
}

void TestOneWidget::onCheckButtonClicked()
{
	if (!m_is_validated)
	{
		if (validateInput())
		{
			m_is_validated = true;
			m_check_button->setText("Перейти к следующему тесту");
			m_input_text_edit->setEnabled(false);
			SPD_INFO_CLASS(UTILS::DEFAULTS::d_settings_group_application, "Validation passed successfully.");
		}
	}
	else
	{
		SPD_INFO_CLASS(UTILS::DEFAULTS::d_settings_group_application, "Test one passed.");
		emit testResult(m_result_map, m_input_text_edit->toPlainText(), m_invalid_count);
	}
}
} // namespace APP
