#include "user_panel_widget.hpp"

#include "settings_manager.hpp"
#include "spdlog_wrapper.hpp"
#include "test_introduction_widget.hpp"
#include "test_one_widget.hpp"

#include <QStackedWidget>
#include <QVBoxLayout>
#include <qlogging.h>
#include <qvariant.h>
#include <qwidget.h>

namespace APP
{
UserPanelWidget::UserPanelWidget(QWidget *parent) : QWidget(parent)
{
	this->initialize();
}

UserPanelWidget::~UserPanelWidget()
{}

void UserPanelWidget::addScreen(const ScreenInfo &screen_info)
{
	if (screen_info.type >= PanelType::COUNT || screen_info.type == PanelType::NONE)
	{
		spdlog::error(QObject::tr("pebkac_error_invalid_screen"));
		return;
	}

	auto index = this->m_main_layout->addWidget(screen_info.widget);

	this->m_screens.insert(index, screen_info);
}

void UserPanelWidget::switchScreen(PanelType type)
{
	int index = -1;
	for (auto it = this->m_screens.begin(); it != this->m_screens.end(); ++it)
	{
		if (it.value().type == type)
		{
			if (this->m_main_layout->currentIndex() == it.key())
			{
				return;
			}

			this->m_main_layout->setCurrentIndex(it.key());
			UTILS::SettingsManager::instance()->setDirectValue(UTILS::SettingsManager::Setting::LAST_OPEN_PANEL,
															   QVariant::fromValue(it.value().type));
			return;
		}
	}
	spdlog::error(QObject::tr("pebkac_error_invalid_screen"));
	return;
}

void UserPanelWidget::nextScreen()
{
	int index = this->m_main_layout->currentIndex();
	if (index == this->m_screens.size() - 1)
	{
		return;
	}

	this->m_main_layout->setCurrentIndex(index + 1);
	UTILS::SettingsManager::instance()->setDirectValue(UTILS::SettingsManager::Setting::LAST_OPEN_PANEL,
													   QVariant::fromValue(this->m_screens[index + 1].type));
	return;
}

void UserPanelWidget::initialize()
{
	this->setupUi();
	this->setupStyle();
	this->setupConnections();
}

void UserPanelWidget::setupUi()
{
	this->m_main_layout = new QStackedWidget();
	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(this->m_main_layout);
	this->setLayout(layout);

	ScreenInfo introduction_screen = {PanelType::TEST_INTRODUCTION, this->resolveScreenText(PanelType::TEST_INTRODUCTION),
									  this->resolveScreenWidget(PanelType::TEST_INTRODUCTION)};
	ScreenInfo test_one_screen	   = {PanelType::TEST_ONE, this->resolveScreenText(PanelType::TEST_ONE),
									  this->resolveScreenWidget(PanelType::TEST_ONE)};
	ScreenInfo test_two_screen	   = {PanelType::TEST_TWO, this->resolveScreenText(PanelType::TEST_TWO),
									  this->resolveScreenWidget(PanelType::TEST_TWO)};
	ScreenInfo test_result_screen  = {PanelType::TEST_RESULT, this->resolveScreenText(PanelType::TEST_RESULT),
									  this->resolveScreenWidget(PanelType::TEST_RESULT)};

	this->addScreen(introduction_screen);
	this->addScreen(test_one_screen);
	this->addScreen(test_two_screen);
	this->addScreen(test_result_screen);

	// switchScreen(
	//	UTILS::SettingsManager::instance()->getValue(UTILS::SettingsManager::Setting::LAST_OPEN_PANEL).value<PanelType>());
	switchScreen(PanelType::TEST_INTRODUCTION);
}

void UserPanelWidget::setupConnections()
{}

void UserPanelWidget::setupStyle()
{
	setStyleSheet("UserPanelWidget { background: #181818; }"
				  "QLabel { color: #EEEEEE; }");
}

QWidget *UserPanelWidget::resolveScreenWidget(PanelType type)
{
	switch (type)
	{
		case PanelType::TEST_INTRODUCTION: {
			IntroductionWidget *widget = new IntroductionWidget();
			connect(widget, &IntroductionWidget::onStartTestClicked, this, [this]() {
				switchScreen(PanelType::TEST_ONE);
				emit testStarted();
			});
			connect(widget, &IntroductionWidget::onValidationDone, this, [this]() {
				emit validationFinished();
			});
			return std::move(widget);
		}
		case PanelType::TEST_ONE:
			return new TestOneWidget();
		case PanelType::TEST_TWO:
			return new QWidget();
		case PanelType::TEST_RESULT:
			return new QWidget();
		default:
			return new QWidget();
	}
}

QString UserPanelWidget::resolveScreenText(PanelType type) const
{
	switch (type)
	{
		default:
			return "";
	}
}
} // namespace APP
