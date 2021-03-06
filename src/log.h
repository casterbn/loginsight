#ifndef LOG_H
#define LOG_H

#include <QString>
#include <QVector>
#include <QFile>
#include <QTextDocument>
#include "LongtimeOperation.h"
#include <QTextCodec>

struct SearchResult {
    int line;
    int pos;
};

class SubLog;
class Log {
public:
    virtual QString getLine(int from, int to) = 0;
    virtual int lineCount() = 0;

public:
    void setCodec(QTextCodec* codec){mCodec=codec;}
    QTextCodec* getCodec(){return mCodec;}
protected:
    QTextCodec* mCodec;

public:
    virtual ~Log();
    virtual QString getLine(int num);
    virtual SearchResult search(const QString& text,
                                QTextDocument::FindFlags flag,
                                int fromLine, int fromPos,
                                LongtimeOperation& op);
    virtual SubLog* createSubLog(const QString& text, bool caseSensitive, LongtimeOperation& op);
};

class FileLog : public Log
{
public:
    FileLog();
    ~FileLog() override;

public:
    bool open(const QString& path, LongtimeOperation& op);
    void close();
    bool isOpened(){return mFile != nullptr;}

public:
    QString getLine(int from, int to) override;
    int lineCount() override {return mLineCnt;}
    SubLog* createSubLog(const QString& text, bool caseSensitive, LongtimeOperation& op) override;
    SearchResult search(const QString& text,
                        QTextDocument::FindFlags flag,
                        int fromLine, int fromPos,
                        LongtimeOperation& op) override;

private:
    qint64 getLineStart(int num);
    void detectTextCodec();
    int lineFromPos(qint64 pos, int from = 1);

    void splitLines(LongtimeOperation& op);
    void splitLine(LongtimeOperation &op, QVector<qint64> *enters, char* ptr, bool progress);

private:
    char* mMem{nullptr};
    qint64 mSize{0};
    QVector<qint64> mEnters;
    int mLineCnt{0};
    QFile* mFile{nullptr};
    int mEnterCharOffset;
};

class SubLog: public Log
{
public:
    SubLog(Log* parent);

public:
    void addLine(int lineNum);

public:
    QString getLine(int from, int to) override;
    int lineCount() override {return mLines.size();}
    int toParentLine(int lineNum);
    int fromParentLine(int lineNum);

private:
    QVector<int> mLines;
    Log* mParent;
};

#endif // LOG_H
