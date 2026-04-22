#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QWidget>
#include <QPlainTextEdit>

QT_BEGIN_NAMESPACE
class QRect;
QT_END_NAMESPACE

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    CodeEditor(QWidget *parent = nullptr);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void updateFontSize();
    void updateSaveState(QWidget *parent, bool changed);

    void highlightCurrentLine();

    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(const QRect &rect, int dy);

private:
    QWidget *lineNumberArea;
};

class LineNumberArea : public QWidget
{
    Q_OBJECT

public: 
    LineNumberArea(CodeEditor *editor) : QWidget(editor), editor(editor) {}

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    CodeEditor *editor;
};

#endif