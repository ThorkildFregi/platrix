#ifndef SYNTAXHIGHLIGHTER_H
#define SYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>

QT_BEGIN_NAMESPACE
class QTextCharFormat;
class QRegularExpression;
QT_END_NAMESPACE

struct HighlightRule {
    QRegularExpression pattern;
    QTextCharFormat format;
};

class SyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    SyntaxHighlighter(QTextDocument *parent = nullptr);

    void setRules(const QVector<HighlightingRule> &rules);

protected:
    void HighlightBlock(const QString &text) override;

private:
    QVector<HighlightingRule> highlightongRules;
};

#endif