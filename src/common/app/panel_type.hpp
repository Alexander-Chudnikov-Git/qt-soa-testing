#ifndef PANEL_TYPE_HPP
#define PANEL_TYPE_HPP

#include <QMetaType>

namespace APP
{
Q_NAMESPACE

enum class PanelType : quint32
{
	NONE,
	TEST_INTRODUCTION,
	TEST_ONE,
	TEST_TWO,
	TEST_RESULT,
	COUNT
};
Q_ENUM_NS(PanelType)
} // namespace APP
Q_DECLARE_METATYPE(APP::PanelType)

#endif // PANEL_TYPE_HPP
