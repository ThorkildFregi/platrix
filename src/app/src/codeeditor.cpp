#include <QWidget>
#include <QTextEdit>
#include <QPlainTextEdit>

#include <QPainter>
#include <QTextBlock>

#include "codeeditor.h"

#include "settingsmanager.h"

CodeEditor::CodeEditor(QWidget *parent)
{
    lineNumberArea = new LineNumberArea(this);

    setLineWrapMode(QPlainTextEdit::NoWrap);

    connect(this, &CodeEditor::cursorPositionChanged, this, &CodeEditor::highlightCurrentLine);
    connect(this, &CodeEditor::blockCountChanged, this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &CodeEditor::updateRequest, this, &CodeEditor::updateLineNumberArea);
    connect(this, &CodeEditor::cursorPositionChanged, this, [this]() {
        lineNumberArea->update();
    });

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    auto &manager = SettingsManager::instance();

    QString rectColor;
    QString numColor;
    if (manager.get("darkTheme").toBool()) {
        numColor = "#59288d";
        rectColor = "#12061F";
    } else {
        numColor = "#8DD5F2";
        rectColor = "#FFFFFF";
    }

    painter.fillRect(event->rect(), QColor(rectColor));

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();

    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);            

            if (blockNumber == textCursor().blockNumber()) {
                painter.setPen(QColor(manager.get("darkTheme").toBool() ? "#F2D194" : "#293d46"));
            } else {
                painter.setPen(QColor(numColor));
            }

            painter.drawText(0, top, lineNumberArea->width() - 5, fontMetrics().height(), Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

int CodeEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int maxLines = qMax(1, blockCount());

    while (maxLines >= 10) {
        maxLines /= 10;
        ++digits;
    }

    int charWidth = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9'));
    int space = 10 + charWidth * digits;

    return space;
}

void CodeEditor::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {

        insertPlainText("\n");
        return;
    }

    if (event->key() == Qt::Key_Tab) {
        auto &manager = SettingsManager::instance();

        QString spaces = QString(manager.get("tabSize").toInt(), ' ');

        insertPlainText(spaces);

        return;
    }

    QPlainTextEdit::keyPressEvent(event);
}

void CodeEditor::updateFontSize()
{
    auto &manager = SettingsManager::instance();

    QFont f = font();

    f.setPointSize(manager.get("fontSize").toInt());

    setFont(f);
}

void CodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        auto &manager = SettingsManager::instance();

        QColor lineColor = QColor(manager.get("darkTheme").toBool() ? "#4b2f70" : "#beb8c5");

        selection.format.setBackground(lineColor);
        
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

void CodeEditor::updateLineNumberAreaWidth(int)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy) lineNumberArea->scroll(0, dy);
    else lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect())) updateLineNumberAreaWidth(0);
}


void LineNumberArea::paintEvent(QPaintEvent *event)
{
    editor->lineNumberAreaPaintEvent(event);
}