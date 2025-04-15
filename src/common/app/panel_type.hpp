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
	TEST_RESULT,
	TEST_RESERVED, // Used to indicate pages that should not be reset
	
	TEST_PAGE,	   // or TEST_ZERO. There might be more of them, but i don't really care, but the limit is somewhere at 254, or
				   // maximum number of hardcoded tests
	TEST_ONE,
	TEST_TWO,
	TEST_THREE,
	TEST_FOUR,
	TEST_FIVE,
	TEST_SIX,
	TEST_SEVEN,
	TEST_EIGHT,
	TEST_NINE,
	TEST_TEN,
	TEST_ELEVEN,
	TEST_TWELVE,
	TEST_THIRTEEN,
	TEST_FOURTEEN,
	TEST_FIFTEEN,
	COUNT = 255
};
Q_ENUM_NS(PanelType)

enum class QUESTION_TYPE
{
	NONE,
	SELECTOR,
	INPUT
};

} // namespace APP
Q_DECLARE_METATYPE(APP::PanelType)

#endif // PANEL_TYPE_HPP
