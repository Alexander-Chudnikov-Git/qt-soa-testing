#include "test_two_widget.hpp"

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
TestTwoWidget::TestTwoWidget(QWidget *parent) :
	QWidget(parent),
	m_is_validated(false),
	m_result_map({{"test_2_var_valid", false},
				  {"test_2_group_valid", false},
				  {"test_2_single_valid", false},
				  {"test_2_multi_valid", false},
				  {"test_2_any_valid", false},
				  {"test_2_variable_valid", false},
				  {"test_2_negation_valid", false}})
{
	initialize();
}

TestTwoWidget::~TestTwoWidget()
{}

void TestTwoWidget::initialize()
{
	setupUi();
	setupStyle();
	setupConnections();
}

void TestTwoWidget::setupUi()
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

	m_title_label->setText("Тест 2: Настройка групп портов");
	m_subtitle_label->setText("С выданной схемы указать все порты, задействовав не менее 5ти разных способов их "
							  "записи.\nПосле нажатия кнопки \"Проверить\" пути назад не будет.");
	m_check_button->setText("Проверить");

	m_main_layout->addWidget(m_title_label, 0, 0);
	m_main_layout->addWidget(m_subtitle_label, 1, 0);
	m_main_layout->addWidget(m_input_text_edit, 2, 0);
	m_main_layout->addWidget(m_check_button, 3, 0);

	setLayout(m_main_layout);
}

void TestTwoWidget::setupStyle()
{
	m_title_label->setStyleSheet("font-weight: bold; font-size: 16px;");
	m_subtitle_label->setStyleSheet("font-size: 14px;");
}

void TestTwoWidget::setupConnections()
{
	connect(m_check_button, &QPushButton::clicked, this, &TestTwoWidget::onCheckButtonClicked);
}

bool TestTwoWidget::validateInput()
{
	QString input = m_input_text_edit->toPlainText();

	m_result_map["test_2_var_valid"]	  = true;
	m_result_map["test_2_group_valid"]	  = true;
	m_result_map["test_2_single_valid"]	  = false;
	m_result_map["test_2_multi_valid"]	  = false;
	m_result_map["test_2_any_valid"]	  = false;
	m_result_map["test_2_variable_valid"] = false;
	m_result_map["test_2_negation_valid"] = false;

	bool vars_section_found		   = false;
	bool port_groups_section_found = false;
	bool overall_valid			   = true;

	QRegularExpression single_port_regex(
		R"(^\s*([a-zA-Z0-9_-]+)\s*:\s*"?(!?(?:6553[0-5]|655[0-2][0-9]|65[0-4][0-9]{2}|6[0-4][0-9]{3}|[1-5][0-9]{4}|[1-9][0-9]{0,3}|0|any))\s*"?\s*$)");
	QRegularExpression multi_port_regex(
		R"(^\s*([a-zA-Z0-9_-]+)\s*:\s*"?\[\s*((?:(?:!?6553[0-5]|!?655[0-2][0-9]|65[0-4][0-9]{2}|6[0-4][0-9]{3}|!?[1-5][0-9]{4}|!?[1-9][0-9]{0,3}|!?0|!any|any|!\$[a-zA-Z0-9_-]+|\$[a-zA-Z0-9_-]+)\s*,\s*)+(?:!?6553[0-5]|!?655[0-2][0-9]|!?65[0-4][0-9]{2}|!?6[0-4][0-9]{3}|!?[1-5][0-9]{4}|!?[1-9][0-9]{0,3}|!?0|!any|any|!\$[a-zA-Z0-9_-]+|\$[a-zA-Z0-9_-]+))\s*\]\s*"?\s*$)");
	QRegularExpression any_port_regex(R"(^\s*([a-zA-Z0-9_-]+)\s*:\s*"\s*?(!?any)\s*"?\s*$)");
	QRegularExpression variable_port_regex(R"(^\s*([a-zA-Z0-9_-]+)\s*:\s*"?(\$[a-zA-Z0-9_-]+)\"?\s*$)");
	QRegularExpression negation_port_regex(R"(^\s*([a-zA-Z0-9_-]+)\s*:\s*"?(!\$[a-zA-Z0-9_-]+)\"?\s*$)");
	QStringList		   lines = input.split('\n', Qt::KeepEmptyParts);

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

		if (!port_groups_section_found && match_count == 0)
		{
			if (trimmed_line.startsWith("port-groups:"))
			{
				port_groups_section_found = true;
				continue;
			}
		}

		bool line_matched = false;
		if (any_port_regex.match(trimmed_line).hasMatch())
		{
			match_count += 1;
			m_result_map["test_2_any_valid"] = true;
			line_matched					 = true;
		}
		else if (single_port_regex.match(trimmed_line).hasMatch())
		{
			match_count += 1;
			m_result_map["test_2_single_valid"] = true;
			line_matched						= true;
		}
		else if (multi_port_regex.match(trimmed_line).hasMatch())
		{
			match_count += 1;
			m_result_map["test_2_multi_valid"] = true;
			line_matched					   = true;
		}
		else if (variable_port_regex.match(trimmed_line).hasMatch())
		{
			match_count += 1;
			m_result_map["test_2_variable_valid"] = true;
			line_matched						  = true;
		}
		else if (negation_port_regex.match(trimmed_line).hasMatch())
		{
			match_count += 1;
			m_result_map["test_2_negation_valid"] = true;
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
		m_result_map["test_2_var_valid"] = false;
		overall_valid					 = false;
		SPD_WARN_CLASS(UTILS::DEFAULTS::d_settings_group_application, "vars section not found");
	}
	if (!port_groups_section_found)
	{
		m_result_map["test_2_group_valid"] = false;
		overall_valid					   = false;
		SPD_WARN_CLASS(UTILS::DEFAULTS::d_settings_group_application, "port-groups section not found");
	}

	int true_count = 0;
	for (const QString &key : m_result_map.keys())
	{
		if (m_result_map.value(key))
		{
			true_count++;
		}
	}

	if (overall_valid && true_count < 7)
	{
		SPD_WARN_CLASS(UTILS::DEFAULTS::d_settings_group_application,
					   "Max score is 7. Current score is: " + QString::number(true_count));
	}

	return true;
}

void TestTwoWidget::onCheckButtonClicked()
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
		SPD_INFO_CLASS(UTILS::DEFAULTS::d_settings_group_application, "Test two passed.");
		emit testResult(m_result_map, m_input_text_edit->toPlainText(), m_invalid_count);
	}
}
} // namespace APP
