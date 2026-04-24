#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QWidget>
#include <QPlainTextEdit>

#include <QMap>
#include <QString>

QT_BEGIN_NAMESPACE
class QRect;
QT_END_NAMESPACE

struct HighlightRule {
    QRegularExpression pattern;
    QTextCharFormat format;
};

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    CodeEditor(QWidget *parent, QString filePath);

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
    QString getSyntaxConfig(QString filePath);
    QVector <HighlightRule> parseJsonToRules(QString path);

    QWidget *lineNumberArea;

    QMap<QString, QString> officialSyntaxes = {
        {".py", "python.strl"}
    };
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