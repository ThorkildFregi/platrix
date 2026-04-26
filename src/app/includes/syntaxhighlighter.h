#ifndef SYNTAXHIGHLIGHTER_H
#define SYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>

#include <QRegularExpression>

QT_BEGIN_NAMESPACE
class QTextCharFormat;
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

    void setRules(const QVector<HighlightRule> &rules);

protected:
    void highlightBlock(const QString &text) override;

private:
    QVector<HighlightRule> highlightingRules;
};

#endif