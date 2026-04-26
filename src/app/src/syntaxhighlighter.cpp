#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>

#include "syntaxhighlighter.h"

SyntaxHighlighter::SyntaxHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent)
{
}

void SyntaxHighlighter::setRules(const QVector<HighlightRule> &rules)
{
    highlightingRules = rules;
    rehighlight();
}

void SyntaxHighlighter::highlightBlock(const QString &text)
{
    for (const HighlightRule &rule : highlightingRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);

        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();

            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}