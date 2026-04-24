#include <QWidget>
#include <QTextEdit>
#include <QTabWidget>
#include <QTextDocument>
#include <QPlainTextEdit>

#include <QPixmap>
#include <QPainter>
#include <QTextBlock>

#include <QJsonValue>
#include <QJsonObject>
#include <QJsonDocument>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDirIterator>
#include <QStandardPaths>

#include "codeeditor.h"
#include "syntaxhighlighter.h"

#include "settingsmanager.h"

CodeEditor::CodeEditor(QWidget *parent, QString filePath)
{
    lineNumberArea = new LineNumberArea(this);

    highlighter = new SyntaxHighlighter(this->document());

    QString syntaxRulePath = getSyntaxConfig(filePath);
    QVector<HighlightingRule> rules = parseJsonToRules(syntaxRulePath);

    highlighter->setRules(rules);

    setLineWrapMode(QPlainTextEdit::NoWrap);
    installEventFilter(this);

    connect(document(), &QTextDocument::modificationChanged, this, [this, parent](bool changed) {
        this->updateSaveState(parent, changed);
    });

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

    if (event->key() == Qt::Key_Backspace && !textCursor().hasSelection()) {
        QTextCursor cursor = textCursor();

        QString lineText = cursor.block().text();
        int positionInLine = cursor.positionInBlock();
        QString textBeforeCursor = lineText.left(positionInLine);

        int tabSize = SettingsManager::instance().get("tabSize").toInt();
        if (textBeforeCursor.endsWith(QString(tabSize, ' '))) {
            cursor.beginEditBlock();

            for (int i = 0; i < tabSize; ++i) {
                cursor.deletePreviousChar();
            }

            cursor.endEditBlock();

            return;
        }
    }

    if (event->key() == Qt::Key_Backtab) {
        QTextCursor cursor = textCursor();
        int tabSize = SettingsManager::instance().get("tabSize").toInt();

        int start = cursor.selectionStart();
        int end = cursor.selectionEnd();

        QTextCursor worker(cursor.document());
        worker.setPosition(start);
        int startBlock = worker.blockNumber();

        worker.setPosition(end);
        int endBlock = worker.blockNumber();

        cursor.beginEditBlock();

        for (int i = startBlock; i <= endBlock; ++i) {
            QTextBlock block = cursor.document()->findBlockByNumber(i);
            QTextCursor lineCursor(block);
            
            QString text = block.text();
            if (text.startsWith(' ')) {
                int toRemove = 0;
                while (toRemove < tabSize && toRemove < text.length() && text[toRemove] == ' ') {
                    toRemove++;
                }
                
                lineCursor.movePosition(QTextCursor::StartOfBlock);
                for (int j = 0; j < toRemove; ++j) {
                    lineCursor.deleteChar();
                }
            }
        }

        cursor.endEditBlock();
        return;
    }

    if (event->key() == Qt::Key_Tab) {
        QTextCursor cursor = textCursor();
        int tabSize = SettingsManager::instance().get("tabSize").toInt();

        if (cursor.hasSelection()) {
            int start = cursor.selectionStart();
            int end = cursor.selectionEnd();

            QTextCursor worker = cursor;

            worker.setPosition(start);
            int startBlock = worker.blockNumber();

            worker.setPosition(end);
            int endBlock = (worker.columnNumber() == 0) ? worker.blockNumber() - 1 : worker.blockNumber();

            cursor.beginEditBlock();

            for (int i = startBlock; i <= endBlock; ++i) {
                QTextCursor lineCursor(document()->findBlockByNumber(i));
                lineCursor.movePosition(QTextCursor::StartOfBlock);
                lineCursor.insertText(QString(tabSize, ' '));
            }

            cursor.endEditBlock();
        } else {
            insertPlainText(QString(tabSize, ' '));
        }

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

void CodeEditor::updateSaveState(QWidget *parent, bool changed)
{
    if (changed) {
        QPixmap px(12, 12);
        px.fill(Qt::transparent);

        QPainter painter(&px);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setBrush(Qt::white);
        painter.setPen(Qt::NoPen);

        painter.drawEllipse(1, 1, 10, 10);
        painter.end();

        QIcon icon(px);

        QTabWidget *tabs = qobject_cast<QTabWidget*>(parent);
        if (tabs) {
        tabs->setTabIcon(tabs->currentIndex(), icon);
        }
    } else {
        QTabWidget *tabs = qobject_cast<QTabWidget*>(parent);
        if (tabs) {
        tabs->setTabIcon(tabs->currentIndex(), QIcon());
        }
    }
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

QString CodeEditor::getSyntaxConfig(QString filePath)
{
    QString userSyntaxPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/syntax/";

    QFileInfo file(filePath);
    QString ext = "." + file.suffix();

    QStringList filters;
    filters << "*.strl";

    QDirIterator it(userSyntaxPath, filters, QDir::Files, QDirIterator::Subdirectories);
    while(it.hasNext()) {
        it.next();

        QFile file(it.filePath());
        file.open(QIODevice::ReadOnly)
        
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonObject json = doc.object();

        if (json["extension"] == ext) {
            return it.filePath();
        }
    }

    if (officialSyntaxes.contains(ext)) {
        return ":/syntaxes/" + officialSyntaxes[ext] + ".strl";
    }

    return ""
}

QVector<HighlightingRule> CodeEditor::parseJsonToRule(QString path)
{
    QVector<HighlightingRule> rules;

    
}


void LineNumberArea::paintEvent(QPaintEvent *event)
{
    editor->lineNumberAreaPaintEvent(event);
}