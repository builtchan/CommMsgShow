#ifndef CMYTEXTEDIT_H
#define CMYTEXTEDIT_H
#include <QTextEdit>
#include <QDragEnterEvent>
#include <QMimeData>
class CMyTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    CMyTextEdit(QWidget *p = NULL) :QTextEdit(p)
    {
        this->setAcceptDrops(true);
    }
    ~CMyTextEdit()
    {

    }

protected:
    void dragEnterEvent(QDragEnterEvent *e)
    {
        e->acceptProposedAction();
    }

    void dropEvent(QDropEvent *e)
    {
        emit dropFile(e->mimeData()->urls().first().toLocalFile());
    }

signals:
    void dropFile(QString strFile);
};



#endif // CMYTEXTEDIT_H
