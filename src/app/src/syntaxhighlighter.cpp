#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>

#include "syntaxhighlighter.h"

void SyntaxHightlighter::setRules(const QVector<HighlightingRule> &rules)
{
    highlightingRules = rules;
    rehighlight();
}

void SyntaxHighlighter::highlightBlock(const QString &text)
{
    for (const HiglightingRule &rule : highlightingRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);

        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();

            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}