#ifndef _TTSACAPELA_H_
#define _TTSACAPELA_H_

#include <QHttp>
#include <QMultiMap>
#include <QTextStream>
#include <QThread>
#include "ttsinterface.h"
	
class TTSresponsivevoice : public TTSInterface
{
	Q_OBJECT
	Q_INTERFACES(TTSInterface)
	
public:
	TTSresponsivevoice();
	virtual ~TTSresponsivevoice();
	QByteArray CreateNewSound(QString, QString, bool);

private:
};

#endif
