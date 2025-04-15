#include "test_result_widget.hpp"

#include "panel_type.hpp"
#include "test_answer.hpp"
#include "test_answer_widget.hpp"

#include <QFont>
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

namespace APP
{
ResultWidget::ResultWidget(QWidget *parent) : QWidget(parent)
{
	initialize();
}

ResultWidget::~ResultWidget()
{}

void ResultWidget::initialize()
{
	setupUi();
	setupConnections();
	setupStyle();
}

void ResultWidget::setupUi()
{
	setAutoFillBackground(true);
	setAttribute(Qt::WA_StyledBackground);

	m_main_layout = new QGridLayout(this);

	QFont title_font;
	title_font.setPointSize(18);
	title_font.setBold(true);

	m_title_label = new QLabel("Результаты теста");
	m_title_label->setAlignment(Qt::AlignCenter);
	m_title_label->setWordWrap(true);
	// m_title_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	m_title_label->setFont(title_font);

	m_score_label = new QLabel(this);
	m_score_label->setAlignment(Qt::AlignCenter);
	m_score_label->setWordWrap(false);

	m_scroll_area			 = new QScrollArea(this);
	m_results_content_widget = new QWidget();
	m_results_layout		 = new QVBoxLayout(m_results_content_widget);

	m_restart_test_button = new QPushButton(tr("Начать новый тест"), this);

	m_scroll_area->setWidgetResizable(true);
	m_scroll_area->setWidget(m_results_content_widget);
	m_scroll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_scroll_area->setFrameShape(QFrame::NoFrame);

	m_results_layout->setContentsMargins(10, 5, 10, 15);
	m_results_layout->setSpacing(0);
	m_results_layout->addStretch(1);

	m_main_layout->addWidget(m_title_label, 0, 0, 1, 1, Qt::AlignCenter);
	m_main_layout->addWidget(m_score_label, 1, 0, 1, 1, Qt::AlignCenter);
	m_main_layout->addWidget(m_scroll_area, 2, 0, 1, 1);
	m_main_layout->addWidget(m_restart_test_button, 3, 0, 1, 1, Qt::AlignCenter);

	m_main_layout->setRowStretch(0, 0);
	m_main_layout->setRowStretch(1, 0);
	m_main_layout->setRowStretch(2, 1);
	m_main_layout->setRowStretch(3, 0);

	setMinimumSize(450, 400);
}

void ResultWidget::setupStyle()
{
	setStyleSheet(
		"* { background-color: #181818; color: #ffffff; border: none; }"
		"QLabel { background-color: transparent; }"
		"QScrollArea { background-color: #202020; border: 1px solid #333333; }"
		"QWidget#resultsContentWidget { background-color: #202020; }"
		"QScrollBar:vertical { border: none; background: #252525; width: 10px; margin: 0px 0px 0px 0px; }"
		"QScrollBar::handle:vertical { background: #555555; min-height: 20px; border-radius: 5px; }"
		"QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { border: none; background: none; height: 0px; }"
		"QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical { background: none; }"
		"QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: none; }"
		"QPushButton { background-color: #333333; color: #ffffff; border: 1px solid #555555; border-radius: 5px; "
		"padding: 8px 15px; margin-top: 10px; }"
		"QPushButton:hover { background-color: #444444; }"
		"QPushButton:pressed { background-color: #555555; }"
		"QPushButton:disabled { background-color: #282828; color: #555555; border-color: #444444; }");

	this->setObjectName("resultWidget");
	m_title_label->setObjectName("titleLabel");
	m_score_label->setObjectName("scoreLabel");
	m_restart_test_button->setObjectName("restartButton");
	m_scroll_area->setObjectName("resultsScrollArea");
	m_results_content_widget->setObjectName("resultsContentWidget");

	QFont scoreFont = m_score_label->font();
	scoreFont.setPointSize(12);
	m_score_label->setFont(scoreFont);
}

void ResultWidget::setupConnections()
{
	connect(m_restart_test_button, &QPushButton::clicked, this, &ResultWidget::onStartTestClicked);
}

void ResultWidget::setTestAnswers(const QVector<TestAnswer> &answers)
{
	m_answers = answers;

	clearLayout(m_results_layout);

	int correctCount   = 0;
	int totalQuestions = m_answers.size();

	if (totalQuestions == 0)
	{
		m_score_label->setText(tr("Нет ответов для отображения."));
		QLabel *placeholder = new QLabel(tr("Нет результатов."), m_results_content_widget);
		placeholder->setAlignment(Qt::AlignCenter);
		placeholder->setStyleSheet("color: #aaaaaa;");
		m_results_layout->insertWidget(0, placeholder);
		return;
	}

	for (int i = 0; i < totalQuestions; ++i)
	{
		auto answer_widget = new TestAnswerWidget(m_answers[i], i + 1);

		if (m_answers[i].isCorrect())
		{
			correctCount++;
		}

		m_results_layout->addWidget(answer_widget);

		if (i < totalQuestions - 1)
		{
			QFrame *separator = new QFrame(m_results_content_widget);
			separator->setFrameShape(QFrame::HLine);
			separator->setFrameShadow(QFrame::Sunken);
			separator->setStyleSheet("border: none; border-top: 1px solid #444444; margin-top: 8px; margin-bottom: 8px;");
			m_results_layout->addWidget(separator);
		}
	}

	m_results_layout->addStretch(1);

	m_score_label->setText(tr("<b>Результат: %1 из %2 (%3%)</b>")
							   .arg(QString::number(correctCount))
							   .arg(QString::number(totalQuestions))
							   .arg(QString::number(correctCount * 100 / totalQuestions)));

	m_results_content_widget->adjustSize();
	m_results_layout->activate();
}

void ResultWidget::clearLayout(QLayout *layout)
{
	if (!layout)
		return;

	QLayoutItem *stretch = nullptr;
	if (layout->count() > 0)
	{
		stretch = layout->itemAt(layout->count() - 1);
		if (stretch && stretch->spacerItem())
		{
			layout->removeItem(stretch);
			// delete stretch;
		}
		else
		{
			stretch = nullptr;
		}
	}

	QLayoutItem *item;
	while ((item = layout->takeAt(0)) != nullptr)
	{
		if (QWidget *widget = item->widget())
		{
			widget->deleteLater();
		}
		if (QLayout *childLayout = item->layout())
		{
			clearLayout(childLayout);
			delete childLayout;
		}
		delete item;
	}

	layout->setSpacing(1); // addStretch(1);
}

} // namespace APP
