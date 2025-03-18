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
TestOneWidget::TestOneWidget(QWidget *parent) : QWidget(parent), m_is_validated(false)
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

	m_title_label->setText("Тест 1: Настройка групп адресов");
	m_subtitle_label->setText("С выданной схемы указать все IP адреса, задействовав не менее 6ти разных способов их записи.");
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

	input = input.trimmed();

	if (input.isEmpty())
	{
		SPD_WARN_CLASS(UTILS::DEFAULTS::d_settings_group_application, "Test one input is empty.");

		return false;
	}

	if (!input.startsWith("vars:"))
	{
		SPD_WARN_CLASS(UTILS::DEFAULTS::d_settings_group_application, "Invalid section, config should start with 'vars:'.");

		return false;
	}

	input = input.mid(5).trimmed();

	if (input.isEmpty())
	{
		SPD_WARN_CLASS(UTILS::DEFAULTS::d_settings_group_application, "No address groups provided.");
		return false;
	}

	if (!input.startsWith("address-groups:"))
	{
		SPD_WARN_CLASS(UTILS::DEFAULTS::d_settings_group_application,
					   "Invalid section, config address should start with 'address-groups:'.");
		return false;
	}

	input = input.mid(15).trimmed();
	if (input.isEmpty())
	{
		SPD_WARN_CLASS(UTILS::DEFAULTS::d_settings_group_application, "Address groups are empty.");
		return false;
	}

	QStringList lines = input.split('\n', Qt::SkipEmptyParts);

	int		  validFormatCount = 0;
	QSet<int> usedFormats;

	QRegularExpression singleIpRegex(
		R"(^\s*([a-zA-Z0-9_-]+)\s*:\s*"?((?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?))\"?\s*$)");
	QRegularExpression rangeIpRegex(
		R"(^\s*([a-zA-Z0-9_-]+)\s*:\s*"?((?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)/(?:[0-9]|[12][0-9]|3[0-2]))\"?\s*$)");
	QRegularExpression multiIpRegex(R"(^\s*([a-zA-Z0-9_-]+)\s*:\s*"?\[(.*)\]"?\s*$)");
	QRegularExpression anyIpRegex(R"(^\s*([a-zA-Z0-9_-]+)\s*:\s*"?any"?\s*$)");
	QRegularExpression variableRegex(R"(^\s*([a-zA-Z0-9_-]+)\s*:\s*"?(\$[a-zA-Z0-9_-]+)\"?\s*$)");
	QRegularExpression negationRegex(R"(^\s*([a-zA-Z0-9_-]+)\s*:\s*"?(!\$[a-zA-Z0-9_-]+)\"?\s*$)");
	QRegularExpression copyVarRegex(R"(^\s*([a-zA-Z0-9_-]+)\s*:\s*"?([a-zA-Z0-9_-]+)\"?\s*$)");

	for (const QString &line : lines)
	{
		if (singleIpRegex.match(line).hasMatch())
		{
			if (usedFormats.find(0) == usedFormats.end())
			{
				validFormatCount++;
			}
			usedFormats.insert(0);
		}
		else if (rangeIpRegex.match(line).hasMatch())
		{
			if (usedFormats.find(1) == usedFormats.end())
			{
				validFormatCount++;
			}
			usedFormats.insert(1);
		}
		else if (multiIpRegex.match(line).hasMatch())
		{
			if (usedFormats.find(2) == usedFormats.end())
			{
				validFormatCount++;
			}
			usedFormats.insert(2);
		}
		else if (anyIpRegex.match(line).hasMatch())
		{
			if (usedFormats.find(3) == usedFormats.end())
			{
				validFormatCount++;
			}
			usedFormats.insert(3);
		}
		else if (variableRegex.match(line).hasMatch())
		{
			if (usedFormats.find(4) == usedFormats.end())
			{
				validFormatCount++;
			}
			usedFormats.insert(4);
		}
		else if (negationRegex.match(line).hasMatch())
		{
			if (usedFormats.find(5) == usedFormats.end())
			{
				validFormatCount++;
			}
			usedFormats.insert(5);
		}
		else if (copyVarRegex.match(line).hasMatch())
		{
			if (usedFormats.find(4) == usedFormats.end())
			{
				validFormatCount++;
			}
			usedFormats.insert(4);
		}
		else
		{
			SPD_WARN_CLASS(UTILS::DEFAULTS::d_settings_group_application, "Invalid line format: " + line);
			return false;
		}
	}

	if (validFormatCount < 6)
	{
		SPD_WARN_CLASS(UTILS::DEFAULTS::d_settings_group_application,
					   "Less than 6 different IP address formats used. Provided only: " + QString::number(validFormatCount));
		return false;
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
			SPD_INFO_CLASS(UTILS::DEFAULTS::d_settings_group_application, "Validation passed successfully.");
		}
	}
	else
	{
		SPD_INFO_CLASS(UTILS::DEFAULTS::d_settings_group_application, "Test one passed.");
	}
}
} // namespace APP
