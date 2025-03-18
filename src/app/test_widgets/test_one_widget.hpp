#ifndef TEST_ONE_WIDGET_HPP
#define TEST_ONE_WIDGET_HPP

#include <QWidget>
#include <QPushButton>
#include <QTextEdit> // Добавили QTextEdit для ввода

class QGridLayout;
class QLabel;

namespace APP
{
class TestOneWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TestOneWidget(QWidget *parent = nullptr);
    ~TestOneWidget();

private:
    void initialize();
    void setupUi();
    void setupStyle();
    void setupConnections();
    bool validateInput();

private slots:
    void onCheckButtonClicked(); 

private:
    QGridLayout *m_main_layout;

    QLabel *m_title_label;
    QLabel *m_subtitle_label;
    
    QTextEdit *m_input_text_edit; 
    QPushButton *m_check_button; 
    
    bool m_is_validated;
};
} // namespace APP

#endif // TEST_ONE_WIDGET_HPP